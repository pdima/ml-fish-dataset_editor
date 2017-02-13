#include "maskview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QBitmap>
#include <QApplication>

MaskView::MaskView(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void MaskView::setModel(SelectionModel *model)
{
    m_model = model;
    connect(model, SIGNAL(changed()), SLOT(updateCrop()));
}

void MaskView::updateCrop()
{
    if (m_model->isEmpty())
    {
        m_crop = QImage();
    }
    else
    {
        m_selection = m_model->currentSelection();
        m_crop = m_model->m_fullImage.copy(m_selection.r.toRect());
//        m_mask = m_selection.mask;
    }

    update();
}

void MaskView::mousePressEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);

    paintOnMask(ev, m_brushSize/imgScale());
    update();
}

void MaskView::mouseMoveEvent(QMouseEvent *ev)
{
    QWidget::mouseMoveEvent(ev);

    if (ev->buttons() != Qt::NoButton)
        paintOnMask(ev, m_brushSize/imgScale());

    m_mousePos = ev->pos();

    update();
}

void MaskView::mouseReleaseEvent(QMouseEvent *)
{
    m_model->currentSelection().mask = m_selection.mask;
    m_model->save();
    m_model->update();
}

void MaskView::wheelEvent(QWheelEvent *ev)
{
    if (QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ControlModifier))
    {
        if (ev->angleDelta().y() < 0)
            m_brushSize /= 1.1;

        if (ev->angleDelta().y() > 0)
            m_brushSize *= 1.1;

        update();
    }
    else
    {
        if (ev->angleDelta().y() < 0)
            m_model->selectNextCrop();

        if (ev->angleDelta().y() > 0)
            m_model->selectPrevCrop();
    }
}

void MaskView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.translate(m_border, m_border);

    if (!m_crop.isNull())
    {
        double scale = imgScale();

        QRectF scaledRect = QRectF(0, 0, m_crop.width()*scale, m_crop.height()*scale);
        p.drawImage(scaledRect, m_crop);

        p.setPen(Qt::lightGray);
        p.drawEllipse(m_mousePos-QPointF(m_border, m_border).toPoint(), int(m_brushSize), int(m_brushSize));

        p.setClipRegion(QRegion(m_selection.mask.scaled(scaledRect.size().toSize())));
        p.fillRect(scaledRect, QColor(0,0,0,128));
    }
}

double MaskView::imgScale() const
{
    if (m_crop.isNull())
        return 1.0;

    return qMin(double(width()-m_border*2)/m_crop.width(), double(height()-m_border*2)/m_crop.height());
}

QPointF MaskView::configToWidget(const QPointF &p) const
{
    double s = imgScale();
    return QPointF(p.x()*s+m_border, p.y()*s+m_border);
}

QPointF MaskView::widgetToConfig(const QPointF &p) const
{
    double s = imgScale();
    return QPointF((p.x()-m_border)/s, (p.y()-m_border)/s);
}

void MaskView::paintOnMask(QMouseEvent *ev, int size)
{
    QPainter p(&m_selection.mask);
    if (ev->buttons() & Qt::MouseButton::RightButton) {
        p.setPen(Qt::black);
        p.setBrush(Qt::black);
    } else if (ev->buttons() & Qt::MouseButton::LeftButton) {
        p.setPen(Qt::white);
        p.setBrush(Qt::white);
    } else
        return;

    QPoint pos = widgetToConfig(ev->pos()).toPoint();
    p.drawEllipse(pos, size, size);
}
