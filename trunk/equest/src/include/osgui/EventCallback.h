/********************************************************************
    created:  2011/03/24
    created:  24:3:2011   16:23
    filename: EventCallback.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_OSGUI__EVENTCALLBACK_H__
#define HEADER_GUARD_OSGUI__EVENTCALLBACK_H__

UTILS_PUSH_WARNINGS
#include <osg/NodeCallback>
#include <osgGA/GUIEventAdapter>
#include <osgGA/EventVisitor>
UTILS_POP_WARNINGS

namespace osgui 
{
    //! Uniwersalny event callback dla węzłów OSG.
    template <class Func>
    class EventCallback : public osg::NodeCallback
    {
    private:
        Func func;
        const osgGA::GUIEventAdapter::EventType eventType;

    public:
        //! \param func
        EventCallback(osgGA::GUIEventAdapter::EventType eventType, const Func& func) :
        eventType(eventType), func(func)
        {}

        //! \param node
        //! \param nv
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        { 
            osgGA::EventVisitor* ev = dynamic_cast<osgGA::EventVisitor*>(nv);
            if ( ev ) {
                typedef osgGA::EventQueue::Events Events;
                const Events& events = ev->getEvents();
                for ( Events::const_iterator it = events.begin(), last = events.end(); it != last; ++it ) {
                    auto event = *it;
					if (event->asGUIEventAdapter()->getEventType() == eventType) {
						func(node, event->asGUIEventAdapter());
                    }
                }
            }
            traverse(node, nv);
        }
    };

    //! Metoda fabryczna ułatwiająca tworzenie callbacków.
    template <class Func>
    EventCallback<Func>* createEventCallback(osgGA::GUIEventAdapter::EventType eventType, const Func& func)
    {
        return new EventCallback<Func>(eventType, func);
    }

} // namespace osgui


#endif  // HEADER_GUARD_OSGUI__EVENTCALLBACK_H__
