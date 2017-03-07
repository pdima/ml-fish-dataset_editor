#ifndef SELECTIONMODEL_H
#define SELECTIONMODEL_H

#include <QObject>
#include <QFileInfo>
#include <QImage>
#include <QRectF>
#include <QBitmap>

struct SelectionInfo
{
    QRectF r;
    QPointF head;
    QPointF tail;

    QBitmap mask;

    bool wrongSpecies {false};
    bool smallPart {false};
    bool lowQuality {false};
    bool ignored {false};
    bool unsure {false};

    QString species;
};

class SelectionModel: public QObject
{
    Q_OBJECT
public:
    SelectionModel();
    ~SelectionModel();

    static QString selectionsPath(const QFileInfo& imgFile);
    static QString maskPath(const QFileInfo& imgFile, int selIdx);
    static QString generatedMaskPath(const QFileInfo& imgFile);

    QFileInfo m_imgPath;
    QImage m_fullImage;
    QList<SelectionInfo> m_selections;
    int m_currentSelection {-1};

    QString m_species;

    bool isEmpty() const { return m_selections.empty(); }
    int size() const { return m_selections.size(); }
    int currentSelectionIdx() const { return m_currentSelection; }

    SelectionInfo currentSelection() const;
    SelectionInfo& currentSelection() { return m_selections[m_currentSelection]; }

    void selectNextCrop();
    void selectPrevCrop();

    void appendSelection(const QRectF& r);

    void load(const QFileInfo& imgPath);
    void save();

    int missingWrongSpeciesSelectionIdx() const;

public slots:
    void update();
    void setCurrentSelection(int selection);

    void clearSelections();
    void clearCurrentSelection();

signals:
    void changed();
    void nextImageRequested();
    void prevImageRequested();

private:
    int boundSelIndex(int idx) const;
};

#endif // SELECTIONMODEL_H
