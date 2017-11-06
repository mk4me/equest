/********************************************************************
    created:  2011/05/16
    created:  16:5:2011   13:07
    filename: osgVDFLinearConnectionFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VDF__OSGVDFLINEARCONNECTIONFACTORY_H__
#define HEADER_GUARD_VDF__OSGVDFLINEARCONNECTIONFACTORY_H__

#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgWidget/Widget>

class osgVDFLinearConnectionFactory
{
public:
	
    static void refreshConnection(osg::Geode * connection)
    {
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void finalizeConnection(osg::Geode * connection, float width = 1)
    {
		setConnectionWidthAndRefresh(connection, width);
	}

	static osg::LineWidth * getConnectionWidth(osg::Geode * connection)
    {
		return dynamic_cast<osg::LineWidth*>(connection->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
	}

	static void setConnectionWidth(osg::Geode * connection, float width)
    {
		getConnectionWidth(connection)->setWidth(width);
	}

	static void setConnectionWidthAndRefresh(osg::Geode * connection, float width)
    {
		getConnectionWidth(connection)->setWidth(width);
		refreshConnection(connection);
	}

	static osgWidget::Color getConnectionColor(osg::Geode * connection)
    {
		return *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin());
	}

	static void setConnectionColor(osg::Geode * connection, const osgWidget::Color & color)
    {
		*(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin()) = color;
	}

	static void setConnectionColorAndRefresh(osg::Geode * connection, const osgWidget::Color & color)
    {
		setConnectionColor(connection, color);
		refreshConnection(connection);
	}

	static osg::Geode * createVisualConnection(const osgWidget::Color & color = osgWidget::Color(0.38,0.76,0.82,1), float width = 1, float z = 0)
    {
		osg::Geode * geode = new osg::Geode();
		geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(width), osg::StateAttribute::ON);
				
		osg::Geometry* geom = new osg::Geometry();
		geom->setUseDisplayList(false);
		osg::DrawArrays* drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP); 
		geom->addPrimitiveSet(drawArrayLines); 
		osg::Vec3Array* vertexData = new osg::Vec3Array(2, &osg::Vec3(0,0,z));
		geom->setVertexArray(vertexData); 

		osg::Vec4Array* c = new osg::Vec4Array();
		c->push_back( color );
		geom->setColorArray( c );
		geom->setColorBinding( osg::Geometry::BIND_OVERALL );

		geode->addDrawable( geom );
		return geode;
	}

	static void setConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos)
    {
		setConnectionControlPoint(connection, 0, startPos);
	}

	static void setConnectionStartAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos)
    {
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static osgWidget::XYCoord getConnectionStart(osg::Geode * connection)
    {
		return getConnectionControlPoint(connection,0);
	}

	static osgWidget::XYCoord getConnectionEnd(osg::Geode * connection)
    {
		return getConnectionControlPoint(connection,1);
	}

	static std::pair<osgWidget::XYCoord, osgWidget::XYCoord> getConnectionEndings(osg::Geode * connection)
    {
		std::pair<osgWidget::XYCoord, osgWidget::XYCoord> ret;

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		ret.first.x() = (*vertexData)[0].x();
		ret.first.y() = (*vertexData)[0].y();

		ret.second.x() = (*vertexData)[1].x();
		ret.second.y() = (*vertexData)[1].y();

		return ret;
	}

	static void setConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos)
    {
		setConnectionControlPoint(connection, 1, endPos);
	}

	static void setConnectionEndAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & endPos)
    {
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[1], endPos);

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void setConnectionEndings(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos)
    {
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);
		setControlPoint((*vertexData)[1], endPos);
	}

	static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos)
    {
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);
		setControlPoint((*vertexData)[1], endPos);

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos, osgWidget::point_type z)
    {

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);
		setControlPoint((*vertexData)[1], endPos);

		for(unsigned int i = 0; i < vertexData->size(); ++i){
			(*vertexData)[0][3] = z;
		}

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

protected:

	static void setConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx,
		const osgWidget::XYCoord & controlPointPos)
    {

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[controlPointIdx], controlPointPos);
	}

	static osgWidget::XYCoord getConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx)
    {

		osgWidget::XYCoord ret;

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		ret.x() = (*vertexData)[controlPointIdx].x();
		ret.y() = (*vertexData)[controlPointIdx].y();

		return ret;
	}

	static void setControlPoint(osg::Vec3f & cp, const osgWidget::XYCoord & pos)
    {
		cp[0] = pos.x();
		cp[1] = pos.y();
	}

	static void refreshConnection(osg::DrawArrays * drArrys)
    {
		drArrys->setFirst(0); 
		drArrys->setCount(2);
		drArrys->dirty();
	}
};

#endif 

