#ifndef SELECTIONONIMAGE_H
#define SELECTIONONIMAGE_H

#include <QWidget>
#include <QFileInfo>

class SelectionModel;

class FullImageView : public QWidget
{
    Q_OBJECT
public:
    FullImageView(QWidget *parent);
    virtual ~FullImageView();

    void setModel(SelectionModel* model);

signals:
    void nextImageRequested();
    void prevImageRequested();

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

    SelectionModel* m_model {nullptr};

    bool m_selecting;
    QPointF m_startSelection;
    QPointF m_endSelection;
};

#endif // SELECTIONONIMAGE_H
