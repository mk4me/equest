#include "PCH.h"
#include <osgui/AspectRatioKeeper.h>
#include <utils/Debug.h>

#undef min
#undef max

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class AspectRatioKeeperWindow : public osgWidget::Window
{

private:

    typedef osgWidget::Window Base;

private:

    osgWidget::XYCoord size;
    osgWidget::point_type aspectRatio;

    //! Widget poddany zmianie rozmiaru.
    osg::observer_ptr<osgWidget::Widget> target;

public:
    //META_Object(osgui, AspectRatioKeeperWindow);
	META_Window(osgui, AspectRatioKeeperWindow);
    //!
    AspectRatioKeeperWindow(const std::string& name = "", osgWidget::Widget* target = NULL, osgWidget::point_type aspectRatio = 1);
    //!
    AspectRatioKeeperWindow(const AspectRatioKeeperWindow& window, const osg::CopyOp& copyop);
    //! \return
    osgWidget::Widget* getTarget();
    //! \return
    const osgWidget::Widget* getTarget() const;
    //! \param target
    void setTarget(osgWidget::Widget* target);
    //! \return
    osgWidget::point_type getAspectRatio() const;
    //! \param aspectRatio
    void setAspectRatio(osgWidget::point_type aspectRatio);
    //!
    //! \param target
    //! \param aspectRatio
    static EmbeddedWindow* createWrapper(osgWidget::Widget* target, osgWidget::point_type aspectRatio);

protected:

    virtual void _resizeImplementation(osgWidget::point_type diffWidth, osgWidget::point_type diffHeight);
    virtual osgWidget::Window::Sizes _getWidthImplementation() const;
    virtual osgWidget::Window::Sizes _getHeightImplementation() const;

    osgWidget::XYCoord applyAspectRatio( osgWidget::XYCoord size );

};

AspectRatioKeeperWindow::AspectRatioKeeperWindow(const std::string& name, osgWidget::Widget* target, osgWidget::point_type aspectRatio) :
Base(name), aspectRatio(aspectRatio)
{
    setTarget(target);
    getBackground()->setColor(0, 0, 0, 0);
    setEventMask(0);
}

AspectRatioKeeperWindow::AspectRatioKeeperWindow( const AspectRatioKeeperWindow& window, const osg::CopyOp& copyop ) :
Base(window, copyop), size(window.size), aspectRatio(window.aspectRatio)
{
    osg::ref_ptr<osgWidget::Widget> windowTarget;
    if ( window.target.lock(windowTarget) ) {
        const std::string& name = windowTarget->getName();
        target = getByName(name);
    }
}

//! \return
osgWidget::Widget* AspectRatioKeeperWindow::getTarget()
{
    return target.get();
}

const osgWidget::Widget* AspectRatioKeeperWindow::getTarget() const
{
    return target.get();
}

//! \param target
void AspectRatioKeeperWindow::setTarget(osgWidget::Widget* target)
{
    osg::ref_ptr<osgWidget::Widget> strongTarget;
    this->target.lock(strongTarget);
    if ( strongTarget != target ) {
        if ( strongTarget ) {
            UTILS_ASSERT(strongTarget->getParent() == this);
            removeWidget(strongTarget);
        }
        this->target = target;
        if ( target ) {
            UTILS_ASSERT(!target->getParent());
            addWidget(target);
        }
        resizeAdd();
    }
}
//! \return
osgWidget::point_type AspectRatioKeeperWindow::getAspectRatio() const
{
    return aspectRatio;
}
//! \param aspectRatio
void AspectRatioKeeperWindow::setAspectRatio(osgWidget::point_type aspectRatio)
{
    this->aspectRatio = aspectRatio;
}

osgWidget::Window::EmbeddedWindow* AspectRatioKeeperWindow::createWrapper(osgWidget::Widget* target, osgWidget::point_type aspectRatio)
{
    if ( !target ) {
        return NULL;
    }
    EmbeddedWindow* result = (new AspectRatioKeeperWindow(target->getName()+"ARK", target, aspectRatio))->embed(target->getName());
    result->setSize(target->getWidthTotal(), target->getHeightTotal());
    result->setColor(0, 0, 0, 0);
    return result;
}

void AspectRatioKeeperWindow::_resizeImplementation(osgWidget::point_type diffWidth, osgWidget::point_type diffHeight)
{
    size = getSize() + osgWidget::XYCoord(diffWidth, diffHeight);
    osg::ref_ptr<osgWidget::Widget> target;
    if ( this->target.lock(target) ) {
        osgWidget::XYCoord targetSize = applyAspectRatio(size - osgWidget::XYCoord(target->getPadHorizontal(), target->getPadVertical()));
        target->setOrigin(0, 0);
        targetSize.x() = osg::round(targetSize.x());
        targetSize.y() = osg::round(targetSize.y());
        target->setSize( targetSize );
        _positionWidget( target, size.x(), size.y() );
    }
}

osgWidget::Window::Sizes AspectRatioKeeperWindow::_getWidthImplementation() const
{
    osgWidget::Window::Sizes result;
    osg::ref_ptr<osgWidget::Widget> target;
    if ( this->target.lock(target) ) {
        result.minimum = target->getMinWidthTotal();
    }
    result.current = std::max( size.x(), result.minimum );
    return result;
}

osgWidget::Window::Sizes AspectRatioKeeperWindow::_getHeightImplementation() const
{
    osgWidget::Window::Sizes result;
    osg::ref_ptr<osgWidget::Widget> target;
    if ( this->target.lock(target) ) {
        result.minimum = target->getMinHeightTotal();
    }
    result.current = std::max( size.y(), result.minimum );
    return result;
}

osgWidget::XYCoord AspectRatioKeeperWindow::applyAspectRatio( osgWidget::XYCoord size )
{
    osgWidget::point_type ratio = size.x() / size.y();
    if ( aspectRatio > ratio ) {
        size.y() = size.x() / aspectRatio;
    } else {
        size.x() = size.y() * aspectRatio;
    }
    return size;
}


AspectRatioKeeper::AspectRatioKeeper( osgWidget::Widget* widget, osgWidget::point_type aspectRatio ) :
osgWidget::Window::EmbeddedWindow()
{
    UTILS_ASSERT(widget);
    setWindow(new AspectRatioKeeperWindow(widget->getName(), widget, aspectRatio));
    getWindow()->getBackground()->setColor(0, 0, 0, 0);

    setLayer( static_cast<osgWidget::Widget::Layer>(widget->getLayer()) );
    setName( widget->getName() );
    setSize( widget->getWidthTotal(), widget->getHeightTotal() );
    setColor(0, 0, 0, 0);
    setCanFill(true);
}

AspectRatioKeeper::AspectRatioKeeper( const AspectRatioKeeper& keeper, const osg::CopyOp& copyop ) :
osgWidget::Window::EmbeddedWindow(keeper, copyop)
{
}

osgWidget::Widget* AspectRatioKeeper::getTarget()
{
    return dynamic_cast<AspectRatioKeeperWindow*>(getWindow())->getTarget();
}

const osgWidget::Widget* AspectRatioKeeper::getTarget() const
{
    return dynamic_cast<const AspectRatioKeeperWindow*>(getWindow())->getTarget();
}

void AspectRatioKeeper::setTarget( osgWidget::Widget* target )
{
    dynamic_cast<AspectRatioKeeperWindow*>(getWindow())->setTarget(target);
}

osgWidget::point_type AspectRatioKeeper::getAspectRatio() const
{
    return dynamic_cast<const AspectRatioKeeperWindow*>(getWindow())->getAspectRatio();
}

void AspectRatioKeeper::setAspectRatio( osgWidget::point_type aspectRatio )
{
    dynamic_cast<AspectRatioKeeperWindow*>(getWindow())->setAspectRatio(aspectRatio);
}


////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
