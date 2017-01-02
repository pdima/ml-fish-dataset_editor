#include "selectionmodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

SelectionModel::SelectionModel()
{
}

SelectionModel::~SelectionModel()
{

}

QString SelectionModel::selectionsPath(const QFileInfo &imgFile)
{
    return imgFile.filePath() + ".sel.json";
}

QString SelectionModel::maskPath(const QFileInfo &imgFile, int selIdx)
{
    QDir dir = imgFile.dir();
    dir.cd("masks");
    QString baseName = imgFile.baseName();
    return dir.absoluteFilePath(QString("%2_%3.png").arg(baseName).arg(selIdx));
}

QString SelectionModel::generatedMaskPath(const QFileInfo &imgFile)
{
    QDir dir = imgFile.dir();
    dir.cd("generated_masks");
    QString baseName = imgFile.baseName();
    return dir.absoluteFilePath(QString("%2.png").arg(baseName));
}

SelectionInfo SelectionModel::currentSelection() const
{
    if (m_currentSelection >= 0 && m_currentSelection < m_selections.size())
        return m_selections[m_currentSelection];

    return SelectionInfo();
}

void SelectionModel::selectNextCrop()
{
    if (m_currentSelection < m_selections.size()-1)
    {
        setCurrentSelection(m_currentSelection+1);
    }
    else
    {
        emit nextImageRequested();
    }
}

void SelectionModel::selectPrevCrop()
{
    if (m_currentSelection > 0)
    {
        setCurrentSelection(m_currentSelection-1);
    }
    else
    {
        emit prevImageRequested();
    }
}

void SelectionModel::appendSelection(const QRectF &r)
{
    SelectionInfo info;
    info.r = r;
    m_selections.append(info);
    m_currentSelection = m_selections.size() - 1;
    save();
    emit changed();
}

static QPointF pointFromJson(const QJsonValue& value)
{
    return QPointF(value.toObject().value("x").toDouble(0.0),
                   value.toObject().value("y").toDouble(0.0));
}

static QJsonObject pointToJson(const QPointF& value)
{
    QJsonObject res;
    res["x"] = value.x();
    res["y"] = value.y();
    return res;
}


void SelectionModel::load(const QFileInfo &imgPath)
{
    m_imgPath = imgPath;
    m_fullImage = QImage(imgPath.filePath());

    m_currentSelection = -1;
    m_selections.clear();

    QFile selectionsFile(selectionsPath(m_imgPath));
    if (selectionsFile.open(QFile::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(selectionsFile.readAll());
        QJsonObject root = doc.object();
        m_species = root["species"].toString();
        int selIdx = 0;
        for (QJsonValue selValue: root["selections"].toArray())
        {
            QJsonObject sel = selValue.toObject();
            QJsonObject rect = sel["rect"].toObject();

            SelectionInfo info;
            info.wrongSpecies = sel.value("wrong_species").toBool(false);
            info.smallPart = sel.value("small_part").toBool(false);
            info.lowQuality = sel.value("low_quality").toBool(false);

            info.r = QRectF(rect["x"].toDouble(), rect["y"].toDouble(), rect["w"].toDouble(), rect["h"].toDouble());
            info.head = pointFromJson(sel.value("head"));
            info.tail = pointFromJson(sel.value("tail"));

            selIdx++;
            QString maskFilePath = maskPath(imgPath, selIdx);
            if (QFileInfo(maskFilePath).exists())
            {
                info.mask.load(maskFilePath);
            }

            if (info.mask.isNull())
            {
//                info.mask.load(generatedMaskPath(imgPath));
//                info.mask = info.mask.scaled(m_fullImage.size(), Qt::IgnoreAspectRatio, Qt::FastTransformation);
//                info.mask = info.mask.copy(info.r.toRect());
                info.mask = QBitmap(info.r.toRect().size());
                info.mask.fill(Qt::black);
            }

            m_selections.append(info);
        }
    }

    if (!m_selections.empty())
        m_currentSelection = 0;

    update();
}

void SelectionModel::save()
{
    QFile selectionsFile(selectionsPath(m_imgPath));
    if (selectionsFile.open(QFile::WriteOnly))
    {
        QJsonObject root;
        root["species"] = m_species;
        root["file"] = m_imgPath.fileName();
        root["w"] = m_fullImage.width();
        root["h"] = m_fullImage.height();

        QJsonArray selections;
        int selIdx = 0;
        for (const SelectionInfo& info: m_selections)
        {
            QJsonObject sel;

            if (info.wrongSpecies) sel["wrong_species"] = true;
            if (info.smallPart) sel["small_part"] = true;
            if (info.lowQuality) sel["low_quality"] = true;

            QJsonObject rect;
            rect["x"] = info.r.x();
            rect["y"] = info.r.y();
            rect["w"] = info.r.width();
            rect["h"] = info.r.height();
            sel["rect"] = rect;

            sel["head"] = pointToJson(info.head);
            sel["tail"] = pointToJson(info.tail);

            selections.append(sel);

            selIdx++;
            if (!info.mask.isNull())
            {
                info.mask.save(maskPath(m_imgPath, selIdx));
            }
        }

        root["selections"] = selections;

        QJsonDocument doc(root);
        selectionsFile.write(doc.toJson());
    }
}

void SelectionModel::update()
{
    emit changed();
}

void SelectionModel::setCurrentSelection(int selection)
{
    if (boundSelIndex(selection) != m_currentSelection)
    {
        m_currentSelection = boundSelIndex(selection);
        emit changed();
    }
}

void SelectionModel::clearSelections()
{
    m_selections.clear();
    m_currentSelection = -1;
    save();
    emit changed();
}

void SelectionModel::clearCurrentSelection()
{
    if (m_currentSelection >= 0 && m_currentSelection < m_selections.size())
    {
        m_selections.removeAt(m_currentSelection);

        if (m_currentSelection >= m_selections.size())
            m_currentSelection--;

        save();
        emit changed();
    }
}

int SelectionModel::boundSelIndex(int idx) const
{
    if (isEmpty())
        return -1;

    if (idx < 0)
        return 0;

    if (idx > m_selections.size()-1)
        return m_selections.size()-1;

    return idx;
}


