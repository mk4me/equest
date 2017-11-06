#include "PCH.h"
#include <osgui/Utils2D.h>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

osgWidget::XYCoord Utils2D::findFreeSpaceInNearby(const osgWidget::XYCoord & pos,
		osgWidget::point_type width, osgWidget::point_type height, const osgWidget::WindowManager * wm){

	if(width == 0){
		return pos;
	}

	osgWidget::XYCoord ret(pos);

	osg::BoundingBox wbb(0, 0, 0, width, height, 0);
	osg::BoundingBox wmbb(0, 0, 0, wm->getWidth(), wm->getHeight(), 0);
	
	unsigned int outsideScreen = 0;
	unsigned int counter = 0;

	float fi = osg::PI / 4.0;
	float dFI = fi;
	float a = width / 4.0;
	
	while(outsideScreen < 8){
		//calc sipral parameters
		float r = a * fi;

		//return from polar to cartesian
		osgWidget::XYCoord posLoc(std::cos(fi)*r, std::sin(fi)*r);

		posLoc += pos;

		osg::BoundingBox tmpWBB(posLoc.x(), posLoc.y(), 0, wbb.xMax() + posLoc.x(), wbb.yMax() + posLoc.y(), 0);

		//check if new pos is ok
		//if pos is in screen
		if(boxFullyEnclosedInBox(tmpWBB, wmbb) == false){
			++outsideScreen;
		}else if(collidesWithOthers(tmpWBB,wm) == false){
			ret = posLoc;
			break;
		}
		
		fi += dFI;
		if(dFI > osg::PI / 180.0 && (++counter %= 8) == 0){
			dFI /= 2.0;
		}
	}

	return ret;
}

bool Utils2D::collidesWithOthers(const osgWidget::Window * ww,
	const osgWidget::WindowManager * wm,
	const std::set<osgWidget::Window*> & exclude){

	std::set<osgWidget::Window*> tmpExclude(exclude);
	tmpExclude.insert(const_cast<osgWidget::Window*>(ww));
	return collidesWithOthers(generateBox(ww), wm, tmpExclude);
}

bool Utils2D::collidesWithOthers(const osg::BoundingBox & bb, const osgWidget::WindowManager * wm,
	const std::set<osgWidget::Window*> & exclude){

	bool ret = false;
	for(osgWidget::UIObjectParent<osgWidget::Window>::ConstIterator it = wm->begin();
		it != wm->end(); ++it){

		if(exclude.find(it->get()) == exclude.end() &&
			bb.intersects(generateBox(it->get())) == true){
					
			ret = true;
			break;
		}
	}

	return ret;
}

bool Utils2D::checkWindowsCollision(const osgWidget::Window * w1, const osgWidget::Window * w2){
	return generateBox(w1).intersects(generateBox(w2));
}
	
osg::BoundingBox Utils2D::generateBox(const osgWidget::Window * ww){
	return generateBox(ww->getAbsoluteOrigin(), ww->getWidth(),
		ww->getHeight());
}

osg::BoundingBox Utils2D::generateBox(const osgWidget::Widget * w){
	return w->getParent() == 0 ? generateBox(osgWidget::XYCoord(0,0), w->getWidth(), w->getHeight()) :
		generateBox(w->getParent()->getAbsoluteOrigin() + w->getOrigin(), w->getWidth(),
		w->getHeight());
}

osg::BoundingBox Utils2D::generateBox(const osgWidget::XYCoord & pos, osgWidget::point_type width,
	osgWidget::point_type height){
	
	return osg::BoundingBox(pos.x(), pos.y(),0,
		pos.x()+width,pos.y()+height,0);
}

bool Utils2D::boxFullyEnclosedInBox(const osg::BoundingBox & bb1, const osg::BoundingBox & bb2){
	return bb1.intersect(bb2).radius2() == bb1.radius2();
}

osg::BoundingBox Utils2D::translateBoundingBox(const osg::BoundingBox & bb,
	const osgWidget::XYCoord & t){

	osg::BoundingBox ret(bb);
	ret.xMin() += t.x();
	ret.xMax() += t.x();
	ret.yMin() += t.y();
	ret.yMax() += t.y();
	return ret;
}

osg::BoundingBox Utils2D::rotateBoundingBox(const osg::BoundingBox & bb,
	const osgWidget::XYCoord & rotCenter, float rotAngle){

	//TODO
	//implement
	return bb;
}

osgWidget::point_type Utils2D::calcAbsZ(const osgWidget::Window * window){
	if(window == 0){
		return 0;
	}

	osgWidget::point_type ret = window->getZ();

	window = window->getParent();
	while(window !=0 ){
		ret += window->getZ();
		window = window->getParent();
	}

	return ret;
}

osgWidget::point_type Utils2D::calcAbsZ(const osgWidget::Widget * widget){
	osgWidget::point_type ret = widget->getZ();

	return ret + calcAbsZ(widget->getParent());
}

}
