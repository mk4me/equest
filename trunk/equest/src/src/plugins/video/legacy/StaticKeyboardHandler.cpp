#include "VideoPCH.h"
#include <osgWidget/ViewerEventHandlers>
#include <osgGA/GUIEventAdapter>
#include "StaticKeyboardHandler.h"

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

bool StaticKeyboardHandler::ctrlPressed = false;
bool StaticKeyboardHandler::shiftPressed = false;
bool StaticKeyboardHandler::altPressed = false;

bool StaticKeyboardHandler::handle(const osgGA::GUIEventAdapter& gea, osgGA::GUIActionAdapter& gaa,
                                   osg::Object* obj,  osg::NodeVisitor* nv)
{
  osgGA::GUIEventAdapter::EventType ev = gea.getEventType();
  if(ev != osgGA::GUIEventAdapter::KEYDOWN && ev != osgGA::GUIEventAdapter::KEYUP) {
    return false;
  }
  if ( gea.getKey() == osgGA::GUIEventAdapter::KEY_Control_L ) {
    ctrlPressed = (ev == osgGA::GUIEventAdapter::KEYDOWN);
  } else if ( gea.getKey() == osgGA::GUIEventAdapter::KEY_Alt_L ) {
    altPressed = (ev == osgGA::GUIEventAdapter::KEYDOWN);
  } else if ( gea.getKey() == osgGA::GUIEventAdapter::KEY_Shift_L ) {
    shiftPressed = (ev == osgGA::GUIEventAdapter::KEYDOWN);
  }
  return osgWidget::KeyboardHandler::handle(gea, gaa, obj, nv);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////
