#include "selectiononimage.h"

#include <QPainter>
#include <QMouseEvent>
#include <QTextStream>
#include <QDebug>

SelectionOnImage::SelectionOnImage(QWidget *parent)
    : QWidget(parent)
{
}

SelectionOnImage::~SelectionOnImage()
{

}

QString SelectionOnImage::selectionsPath(const QFileInfo &imgFile)
{
    return imgFile.filePath() + ".sel";;
}

void SelectionOnImage::clearSelections()
{
    m_selections.clear();
    update();
    saveSelections();
}

void SelectionOnImage::clearLastSelection()
{
    if (!m_selections.empty())
        m_selections.removeLast();
    update();
    saveSelections();
}

void SelectionOnImage::load(const QFileInfo &f)
{
    m_imgPath = f;
    m_img = QImage(f.filePath());
    loadSelections(selectionsPath(m_imgPath));
    update();
}

void SelectionOnImage::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        m_selecting = true;
        m_startSelection = ev->pos();
        m_endSelection = ev->pos();
    }
}

void SelectionOnImage::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        m_endSelection = ev->pos();
        m_selecting = false;

        m_selections.append(widgetToImg(QRectF(m_startSelection, m_endSelection)));
        saveSelections();
        update();
    }
}

void SelectionOnImage::mouseMoveEvent(QMouseEvent *ev)
{
    if (m_selecting)
    {
        m_endSelection = ev->pos();
        update();
    }
}

void SelectionOnImage::wheelEvent(QWheelEvent *ev)
{
    if (ev->angleDelta().y() < 0)
        emit nextImageRequested();

    if (ev->angleDelta().y() > 0)
        emit prevImageRequested();
}

void SelectionOnImage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    if (!m_img.isNull())
    {
        double scale = imgScale();

        QRectF scaledRect = QRectF(0, 0, m_img.width()*scale, m_img.height()*scale);
        p.drawImage(scaledRect, m_img);

        p.setPen(QColor(40, 40, 70));

        for (const QRectF& selection: m_selections)
        {
            p.fillRect(imgToWidget(selection), QColor(40, 40, 100, 50));
            p.drawRect(imgToWidget(selection));
        }

        if (m_selecting)
        {
            p.fillRect(QRectF(m_startSelection, m_endSelection), QColor(40, 40, 100, 50));
            p.drawRect(QRectF(m_startSelection, m_endSelection));
        }
    }
}

double SelectionOnImage::imgScale() const
{
    if (m_img.isNull())
        return 1.0;

    return qMin(double(width())/m_img.width(), double(height())/m_img.height());
}

QRectF SelectionOnImage::imgToWidget(const QRectF &r)
{
    double s = imgScale();
    return QRectF(r.topLeft()*s, r.bottomRight()*s).normalized();
}

QRectF SelectionOnImage::widgetToImg(const QRectF &r)
{
    double s = imgScale();
    return QRectF(r.topLeft()/s, r.bottomRight()/s).normalized();
}

void SelectionOnImage::saveSelections()
{
    saveSelections(selectionsPath(m_imgPath));
}

void SelectionOnImage::loadSelections(const QString &path)
{
    m_selections.clear();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&f);
    QString line = in.readLine();
    while (!line.isNull()) {
        QStringList items = line.split(' ');
        QRectF r(items[0].toInt(), items[1].toInt(), items[2].toInt(), items[3].toInt());
        m_selections.append(r.normalized());
        line = in.readLine();
    }
}

void SelectionOnImage::saveSelections(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&f);

    for (QRectF r: m_selections)
    {
        out << QString("%1 %2 %3 %4\n")
               .arg(qRound(r.left()))
               .arg(qRound(r.top()))
               .arg(qRound(r.width()))
               .arg(qRound(r.height()));
    }
}
