#ifndef HEADER_GUARD__UTILS2D_H__
#define HEADER_GUARD__UTILS2D_H__

#include <osgWidget/Window>
#include <osgWidget/WindowManager>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class Utils2D{
public:

	typedef std::set<osgWidget::Window*> WINDOWS_SET;

public:


	static osgWidget::XYCoord findFreeSpaceInNearby(const osgWidget::XYCoord & pos,
		osgWidget::point_type width, osgWidget::point_type height, const osgWidget::WindowManager * wm);
	
	static bool collidesWithOthers(const osgWidget::Window * ww, const osgWidget::WindowManager * wm,
		const WINDOWS_SET & exclude = WINDOWS_SET());

	static bool collidesWithOthers(const osg::BoundingBox & bb, const osgWidget::WindowManager * wm,
		const WINDOWS_SET & exclude = WINDOWS_SET());

	static bool checkWindowsCollision(const osgWidget::Window * w1, const osgWidget::Window * w2);
	
	static osg::BoundingBox generateBox(const osgWidget::Window * ww);
	static osg::BoundingBox generateBox(const osgWidget::Widget * w);
	static osg::BoundingBox generateBox(const osgWidget::XYCoord & pos, osgWidget::point_type width,
		osgWidget::point_type height);

	static bool boxFullyEnclosedInBox(const osg::BoundingBox & bb1, const osg::BoundingBox & bb2);
	static osg::BoundingBox translateBoundingBox(const osg::BoundingBox & bb, const osgWidget::XYCoord & t);
	static osg::BoundingBox rotateBoundingBox(const osg::BoundingBox & bb, const osgWidget::XYCoord & rotCenter, float rotAngle);

	static osgWidget::point_type calcAbsZ(const osgWidget::Window * window);
	static osgWidget::point_type calcAbsZ(const osgWidget::Widget * widget);
};

}

#endif


