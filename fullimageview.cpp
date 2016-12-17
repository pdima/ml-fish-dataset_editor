#include "fullimageview.h"
#include "selectionmodel.h"

#include <QPainter>
#include <QMouseEvent>
#include <QTextStream>
#include <QDebug>

FullImageView::FullImageView(QWidget *parent)
    : QWidget(parent)
{
}

FullImageView::~FullImageView()
{
}

void FullImageView::setModel(SelectionModel *model)
{
    m_model = model;
    connect(model, SIGNAL(changed()), SLOT(update()));
}

void FullImageView::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        m_selecting = true;
        m_startSelection = ev->pos();
        m_endSelection = ev->pos();
    }
}

void FullImageView::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        m_endSelection = ev->pos();
        m_selecting = false;
        QRectF imgSelection = widgetToImg(QRectF(m_startSelection, m_endSelection));
        if (imgSelection.width() > 10 && imgSelection.height() > 10)
            m_model->appendSelection(imgSelection);
    }
}

void FullImageView::mouseMoveEvent(QMouseEvent *ev)
{
    if (m_selecting)
    {
        m_endSelection = ev->pos();
        update();
    }
}

void FullImageView::wheelEvent(QWheelEvent *ev)
{
    if (ev->angleDelta().y() < 0)
        emit nextImageRequested();

    if (ev->angleDelta().y() > 0)
        emit prevImageRequested();
}

void FullImageView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QColor normalSelectionColor(40, 40, 70);
    QColor currentSelectionColor(40, 40, 250);

    if (!m_model->m_fullImage.isNull())
    {
        double scale = imgScale();

        QRectF scaledRect = QRectF(0, 0, m_model->m_fullImage.width()*scale, m_model->m_fullImage.height()*scale);
        p.drawImage(scaledRect, m_model->m_fullImage);

        for (int i=0; i<m_model->size(); i++)
        {
            if (i == m_model->currentSelectionIdx())
                p.setPen(currentSelectionColor);
            else
                p.setPen(normalSelectionColor);

            QRectF selectionInWidgetSpace = imgToWidget(m_model->m_selections[i].r);

            p.fillRect(selectionInWidgetSpace, QColor(40, 40, 100, 50));
            p.drawRect(selectionInWidgetSpace);
        }

        if (m_selecting)
        {
            p.setPen(normalSelectionColor);
            p.fillRect(QRectF(m_startSelection, m_endSelection), QColor(40, 40, 100, 50));
            p.drawRect(QRectF(m_startSelection, m_endSelection));
        }
    }
}

double FullImageView::imgScale() const
{
    if (m_model->m_fullImage.isNull())
        return 1.0;

    return qMin(double(width())/m_model->m_fullImage.width(), double(height())/m_model->m_fullImage.height());
}

QRectF FullImageView::imgToWidget(const QRectF &r)
{
    double s = imgScale();
    return QRectF(r.topLeft()*s, r.bottomRight()*s).normalized();
}

QRectF FullImageView::widgetToImg(const QRectF &r)
{
    double s = imgScale();
    return QRectF(r.topLeft()/s, r.bottomRight()/s).normalized();
}

