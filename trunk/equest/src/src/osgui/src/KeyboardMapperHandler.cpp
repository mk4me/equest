#include "PCH.h"
#include <osgui/KeyboardMapperHandler.h>
#include <osgui/KeyboardMapper.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

KeyboardMapperHandler::KeyboardMapperHandler(osgWidget::WindowManager* wm) :
wm(wm)
{
    UTILS_ASSERT( dynamic_cast<KeyboardMapper*>(wm), "Przekazany obiekt nie rozszerza KeyboardMappera" );
}

bool KeyboardMapperHandler::handle(const osgGA::GUIEventAdapter& gea, osgGA::GUIActionAdapter& gaa,
	osg::Object* obj, osg::NodeVisitor* nv)
{
    using namespace osgGA;

    // pobranie klawisza
	GUIEventAdapter::KeySymbol key = (GUIEventAdapter::KeySymbol)gea.getKey();
	GUIEventAdapter::EventType ev = gea.getEventType();
	if (key == -1 || (ev != GUIEventAdapter::KEYDOWN && ev != GUIEventAdapter::KEYUP)) {
		return false;
	}

    // czy to mapper?
    if ( wm.valid() ) {
	    KeyboardMapper * mapper = dynamic_cast<KeyboardMapper*>(wm.get());
        UTILS_ASSERT(mapper, "Przekazany obiekt nie rozszerza KeyboardMappera");
	    if (ev == GUIEventAdapter::KEYDOWN) {
		    mapper->keyPressed(key);
	    } else if(ev == GUIEventAdapter::KEYUP) {
		    mapper->keyReleased(key);
	    }
    }

    // nie blokujemy innych handlerów
	return false;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
