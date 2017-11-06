#include "PCH.h"
#include <osgui/EmbeddedWindow.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

EmbeddedWindow::EmbeddedWindow( const std::string& name /*= ""*/, osgWidget::point_type w /*= 0*/, osgWidget::point_type h /*= 0 */ ) : 
osgWidget::Window::EmbeddedWindow(name, w, h)
{
    setColor(0,0,0,0);
    setCanFill(true);
    setDimensions(0, 0, w, h);
}

EmbeddedWindow::EmbeddedWindow( const std::string& name, osgWidget::Window* window ):
osgWidget::Window::EmbeddedWindow(name)
{
    setColor(0,0,0,0);
    setCanFill(true);
    if ( window ) {
        setSize(window->getWidth(), window->getHeight());
        setWindow(window);
    }
}

EmbeddedWindow::EmbeddedWindow( const EmbeddedWindow& widget, const osg::CopyOp& copyop ) :
osgWidget::Window::EmbeddedWindow(widget, copyop)
{
    if (widget.getWindow()) {
        setWindow( dynamic_cast<osgWidget::Window*>(copyop(widget.getWindow())) );
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
