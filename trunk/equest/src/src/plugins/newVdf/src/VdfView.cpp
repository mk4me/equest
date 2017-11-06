//#include "NewVdfPCH.h"
//#include "VdfView.h"
//#include "SimpleItem.h"
//#include <QtWidgets/QGraphicsScene>
//#include <QtWidgets/QGraphicsView>
//#include <QtWidgets/QHBoxLayout>
//
//using namespace vdf;
//
//VdfView::VdfView( QGraphicsScene* scene ) : 
//    QGraphicsView(scene), 
//    viewScale(1.0)
//{
//    setAcceptDrops(true);
//}
//
//void VdfView::wheelEvent( QWheelEvent *event )
//{
//    if (event->modifiers() == Qt::ControlModifier)
//    {
//        // parametry dobrane empirycznie
//        qreal sc = std::pow(4.0 / 3.0, (-event->delta() / 240.0));
//        viewScale += event->delta() * 0.0001;
//        scale(sc, sc);
//    } else {
//        QGraphicsView::wheelEvent(event);
//    }
//}
//
//
