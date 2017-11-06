#ifndef HEADER_GUARD__EXTKEYBOARDHANDLER_H__
#define HEADER_GUARD__EXTKEYBOARDHANDLER_H__

#include <set>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgWidget/WindowManager>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

//! Handler klawiszy wspierający mapowanie przy użyciu osgui::KeyboardMapper
class KeyboardMapperHandler : public osgGA::GUIEventHandler
{
private:
    osg::observer_ptr<osgWidget::WindowManager> wm;

public:
    //! \param wm Window manager. Musi być rzutowalny na KeyboardMappera.
	KeyboardMapperHandler(osgWidget::WindowManager* wm);

    //! Handler przekazujący zdarzenia KeyboardMapperowi.
	virtual bool handle(const osgGA::GUIEventAdapter& gea,
        osgGA::GUIActionAdapter& gaa, osg::Object* obj, osg::NodeVisitor* nv);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////

#endif
