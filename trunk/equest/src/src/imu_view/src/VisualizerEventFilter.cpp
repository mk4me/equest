#include "MdePCH.h"
#include "VisualizerEventFilter.h"
#include "ContextEventFilter.h"

VisualizerEventFilter::VisualizerEventFilter( ContextEventFilterWeakPtr contextFilter ) :
    contextEventFilter(contextFilter)
{
}

bool VisualizerEventFilter::eventFilter( QObject *obj, QEvent *event )
{
    auto w = (QWidget*)obj;
    auto eType = event->type();
    switch(event->type())
    {
    case QEvent::FocusIn:
    case QEvent::MouseButtonPress:
        emit focusOn(w);
        break;
    }
    auto cf = contextEventFilter.lock();
    if (cf) {
        return cf->eventFilter(obj, event);
    } else {
        return QObject::eventFilter(obj, event);
    }
}
