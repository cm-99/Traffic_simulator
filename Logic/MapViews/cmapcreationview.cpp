#include "cmapcreationview.h"
#include <QMouseEvent>

CMapCreationView::CMapCreationView(CReadOnlyMap *map_model, CMapCreationController *map_creation_controller) :
    CBaseView(map_model),
    m_map_creation_controller(map_creation_controller)
{
    setMouseTracking(true);
}

void CMapCreationView::mouseMoveEvent(QMouseEvent *event)
{
    m_map_creation_controller->process_mouse_move_event(event);
    QGraphicsView::mouseMoveEvent(event);
}

void CMapCreationView::mousePressEvent(QMouseEvent *event)
{
    m_map_creation_controller->process_mouse_press_event(event);
    event->accept();
}

void CMapCreationView::process_zooming(QWheelEvent *event)
{
    if(event->angleDelta().y() < 0){
        QRectF scene_rect = scene()->itemsBoundingRect();
        QRectF viewport_rect = viewport()->rect();
        qreal max_zoom_out = qMin(viewport_rect.width() / scene_rect.width(),
                                  viewport_rect.height() / scene_rect.height());
        if (transform().m11() <= max_zoom_out || transform().m22() <= max_zoom_out) {
            return;
        }
    }

    qreal scale_factor = qPow(2, event->angleDelta().y() / 240.0);
    scale(scale_factor, scale_factor);
}

void CMapCreationView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        process_zooming(event);
    }
    else{
        bool event_handled = m_map_creation_controller -> process_wheel_event(event);
        if(!event_handled)
            QGraphicsView::wheelEvent(event);
    }

    event->accept();
}
