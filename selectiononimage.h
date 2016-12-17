#ifndef SELECTIONONIMAGE_H
#define SELECTIONONIMAGE_H

#include <QWidget>
#include <QFileInfo>

class SelectionOnImage : public QWidget
{
    Q_OBJECT
public:
    SelectionOnImage(QWidget *parent);
    virtual ~SelectionOnImage();

    static QString selectionsPath(const QFileInfo& imgFile);

signals:
    void nextImageRequested();
    void prevImageRequested();

public slots:
    void clearSelections();
    void clearLastSelection();
    void load(const QFileInfo& f);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void paintEvent(QPaintEvent *) override;

private:
    double imgScale() const;
    QRectF imgToWidget(const QRectF& r);
    QRectF widgetToImg(const QRectF& r);


    void saveSelections();
    void loadSelections(const QString& path);
    void saveSelections(const QString& path);

    QFileInfo m_imgPath;
    QImage m_img;
    QList<QRectF> m_selections;

    bool m_selecting;
    QPointF m_startSelection;
    QPointF m_endSelection;
};

#endif // SELECTIONONIMAGE_H
