#include "CoreUiPCH.h"
#include <coreui/WheelGraphicsView.h>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QWheelEvent>

using namespace coreUI;

WheelGraphicsView::WheelGraphicsView( QGraphicsScene* scene, Qt::KeyboardModifier modifier ) : 
    QGraphicsView(scene), 
    viewScale(1.0),
    modifier(modifier)
{
    //setAcceptDrops(true);
}


#ifndef QT_NO_WHEELEVENT
void WheelGraphicsView::wheelEvent( QWheelEvent *event )
{
    if (event->modifiers() == modifier)
    {
        // parametry dobrane empirycznie
        qreal sc = std::pow(4.0 / 3.0, (-event->delta() / 240.0));
        viewScale += event->delta() * 0.0001;
        scale(sc, sc);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}
#endif

