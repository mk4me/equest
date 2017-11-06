/********************************************************************
	created:  2010/07/13
	created:  13:7:2010   15:24
	filename: StaticKeyboardHandler.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__STATICKEYBOARDHANDLER_H__
#define __HEADER_GUARD__STATICKEYBOARDHANDLER_H__

#include <osgWidget/ViewerEventHandlers>
#include <osgGA/GUIEventAdapter>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class StaticKeyboardHandler : public osgWidget::KeyboardHandler
{
private:
  static bool ctrlPressed;
  static bool altPressed;
  static bool shiftPressed;

public:
  StaticKeyboardHandler(osgWidget::WindowManager* wm) :
    osgWidget::KeyboardHandler(wm)
  {}

  //! \return
  inline static bool isCtrlPressed()
  { 
    return ctrlPressed;
  }
  //! \return
  inline static bool isAltPressed()
  { 
    return altPressed;
  }
  //! \return
  inline static bool isShiftPressed()
  { 
    return shiftPressed;
  }

  //! 
  //! \param gea
  //! \param gaa
  //! \param obj
  //! \param nv
  bool handle(const osgGA::GUIEventAdapter& gea, osgGA::GUIActionAdapter& gaa,
    osg::Object* obj,  osg::NodeVisitor* nv);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__STATICKEYBOARDHANDLER_H__
