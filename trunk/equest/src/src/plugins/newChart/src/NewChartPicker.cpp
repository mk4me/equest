#include "NewChartPCH.h"
#include <QtWidgets/QApplication>
#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <QtGui/QCloseEvent>
#include "NewChartPicker.h"
#include "NewChartVisualizer.h"

NewChartPicker::NewChartPicker( NewChartVisualizer* visualizer):
    NewChartState( visualizer ),
    pixelTolerance(20), currentCurve(nullptr),
    move(false)
{
    canvas = qobject_cast<QwtPlotCanvas *>(visualizer->getPlot()->canvas());
    canvas->installEventFilter( this );
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();
}

void NewChartPicker::setCurrentCurve(QwtPlotCurve * curve)
{
    currentCurve = curve;
}

bool NewChartPicker::stateEventFilter( QObject *object, QEvent *event )
{
    if (object != canvas ) {
        return false;
    }

    switch( event->type() ) {
        case QEvent::Paint: {
            QApplication::postEvent( this, new QEvent( QEvent::User ) );
            break;
        }
        case QEvent::MouseMove:
            move = true;
            break;

        case QEvent::MouseButtonRelease: {
            if(move == false){
                QMouseEvent * mouseEvent = (QMouseEvent*)event;

                if(mouseEvent->button() == Qt::LeftButton){
                    if(select( mouseEvent->pos() ) == true){
                        mouseEvent->accept();
                        return true;
                    }
                }
            }
            
            move = false;
            break;
        }
        default:
            break;
    }

    return QObject::eventFilter( object, event );
}

bool NewChartPicker::select( const QPoint &pos )
{
    bool ret = false;
    QwtPlotCurve* curve = nullptr;
    double dist = 10e10;
    const QwtPlotItemList& itmList = visualizer->getPlot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it ) {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = ( QwtPlotCurve* )( *it );

            double d;
            c->closestPoint( pos, &d );
            if ( d < dist )
            {
                curve = c;
                dist = d;
            }
        }
    }

    if ( curve && curve != currentCurve && dist < pixelTolerance ) {
        ret = true;
        emit serieSelected(static_cast<QwtPlotItem*>(curve));
    }

    return ret;
}

void NewChartPicker::stateBegin()
{
    visualizer->setManipulation(true);
    canvas->setCursor(Qt::DragLinkCursor);
}
