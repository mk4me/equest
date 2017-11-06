/********************************************************************
    created:  2011/05/16
    created:  16:5:2011   13:13
    filename: osgVDFBezieerConnectionFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VDF__OSGVDFBEZIEERCONNECTIONFACTORY_H__
#define HEADER_GUARD_VDF__OSGVDFBEZIEERCONNECTIONFACTORY_H__

#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgWidget/Widget>
#include <osgAnimation/Interpolator>

namespace osgVDF{

class osgVDFBezieerConnectionFactory{
public:

    static void refreshConnection(osg::Geode * connection){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));

        //recalculate points
        regenerateInterPoints(connection);
        refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
    }

    static void setConnectionZ(osg::Geode * connection, float z){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        for(unsigned int i = 0; i < vertexData->size(); ++i){
            (*vertexData)[i][2] = z;
        }
    }

    static void setConnectionZAndRefresh(osg::Geode * connection, float z){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        for(unsigned int i = 0; i < vertexData->size(); ++i){
            (*vertexData)[i][2] = z;
        }

		refreshConnection(connection);
    }

    static void finalizeConnection(osg::Geode * connection, float z){

        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        for(unsigned int i = 0; i < vertexData->size(); ++i){
            (*vertexData)[i][2] = z;
        }

        setConnectionWidthAndRefresh(connection, connectedWidth);
    }

    static void setOutDir(const osgWidget::XYCoord & outDir){
        osgVDFBezieerConnectionFactory::dirOut = outDir;
    }

    static const osgWidget::XYCoord & getOutDir(){
        return osgVDFBezieerConnectionFactory::dirOut;
    }

    static void setOnDir(const osgWidget::XYCoord & inDir){
        osgVDFBezieerConnectionFactory::dirIn = inDir;
    }

    static const osgWidget::XYCoord & getInDir(){
        return osgVDFBezieerConnectionFactory::dirIn;
    }

    static void setInterpSteps(unsigned int interpSteps){
        osgVDFBezieerConnectionFactory::interpCount = interpSteps;
    }

    static unsigned int getInterpSteps(){
        return osgVDFBezieerConnectionFactory::interpCount;
    }

    static void setCoeffMul(float coeffMul){
        osgVDFBezieerConnectionFactory::coeffMul = coeffMul;
    }

    static float getCoeffMul(){
        return osgVDFBezieerConnectionFactory::coeffMul;
    }

    static osg::LineWidth * getConnectionWidth(osg::Geode * connection){
        return dynamic_cast<osg::LineWidth*>(connection->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
    }

    static void setConnectionWidth(osg::Geode * connection, float width){
        getConnectionWidth(connection)->setWidth(width);
    }

    static void setConnectionWidthAndRefresh(osg::Geode * connection, float width){
        setConnectionWidth(connection,width);
        refreshConnection(connection);
    }

    static osgWidget::Color getConnectionColor(osg::Geode * connection){
        return *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin());
    }

    static void setConnectionColor(osg::Geode * connection, const osgWidget::Color & color){
        *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin()) = color;
    }

    static void setConnectionColorAndRefresh(osg::Geode * connection, const osgWidget::Color & color){
        setConnectionColor(connection, color);
        refreshConnection(connection);
    }

    static osg::Geode * createVisualConnection(const osgWidget::Color & color = osgWidget::Color(0.1,0.1,0.1,1)/*osgWidget::Color(0.38,0.76,0.82,1)*/, float width = connectingWidth, float z = 0){
        static osg::Vec3 defCoords(0,0,0);

        osg::Geode * geode = new osg::Geode();
        geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(width), osg::StateAttribute::ON);

        osg::Geometry* geom = new osg::Geometry();
        geom->setUseDisplayList(false);
        osg::DrawArrays* drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP); 
        geom->addPrimitiveSet(drawArrayLines);
        defCoords.z() = z;
        osg::Vec3Array* vertexData = new osg::Vec3Array();
        vertexData->resize((unsigned int)interpCount, defCoords);
        geom->setVertexArray(vertexData); 

        osg::Vec4Array* c = new osg::Vec4Array();
        c->push_back( color );
        geom->setColorArray( c );
        geom->setColorBinding( osg::Geometry::BIND_OVERALL );

        geode->addDrawable( geom );
        return geode;
    }

    static void setConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos){
        setConnectionControlPoint(connection, 0, startPos);
    }

    static void setConnectionStartAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));

        setConnectionControlPoint(connection, 0, startPos);

        refreshConnection(connection);
    }

    static osgWidget::XYCoord getConnectionStart(osg::Geode * connection){
        return getConnectionControlPoint(connection,0);
    }

    static osgWidget::XYCoord getConnectionEnd(osg::Geode * connection){
        return getConnectionControlPoint(connection,interpCount-1);
    }

    static std::pair<osgWidget::XYCoord, osgWidget::XYCoord> getConnectionEndings(osg::Geode * connection){
        std::pair<osgWidget::XYCoord, osgWidget::XYCoord> ret;

        ret.first = getConnectionStart(connection);
        ret.second = getConnectionEnd(connection);

        return ret;
    }

    static void setConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos){
        setConnectionControlPoint(connection, interpCount-1, endPos);
    }

    static void setConnectionEndAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & endPos){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));

        setConnectionControlPoint(connection, interpCount-1, endPos);

        refreshConnection(connection);
    }

    static void setConnectionEndings(osg::Geode * connection, const osgWidget::XYCoord & startPos,
        const osgWidget::XYCoord & endPos){

            setConnectionStart(connection, startPos);
            setConnectionEnd(connection, endPos);
    }

    static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
        const osgWidget::XYCoord & endPos){

            setConnectionEndings(connection, startPos, endPos);

            refreshConnection(connection);
    }

    static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
        const osgWidget::XYCoord & endPos, osgWidget::point_type z){

            setConnectionEndings(connection, startPos, endPos);

            regenerateInterPoints(connection);

            osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
            osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

            for(unsigned int i = 0; i < vertexData->size(); ++i){
                (*vertexData)[i][2] = z;
            }

            refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
    }

protected:

    static void regenerateInterPoints(osg::Geode * connection){
        //initialize interpolator
        osgAnimation::Vec2CubicBezierInterpolator interp;

        osgWidget::XYCoord start = getConnectionStart(connection);
        osgWidget::XYCoord end = getConnectionEnd(connection);

        float dist = std::sqrt(std::pow(start.x() - end.x(), 2) + std::pow(start.y() - end.y(), 2));
        float coeff = coeffMul;

        if(dist < coeffMul){
            coeff /= 2.0;
        }

        osgWidget::XYCoord out = dirOut * coeff + start;
        osgWidget::XYCoord in = dirIn * coeff + end;

        osgAnimation::TemplateKeyframeContainer<osgAnimation::Vec2CubicBezier> keyFrames;
        keyFrames.push_back(osgAnimation::Vec2CubicBezierKeyframe(0, osgAnimation::Vec2CubicBezier(start, out, in)));
        keyFrames.push_back(osgAnimation::Vec2CubicBezierKeyframe(1, osgAnimation::Vec2CubicBezier(end)));

        //initialize control points
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        osg::Vec2 res;

        for(unsigned int i = 1; i < interpCount-1; ++i){
            interp.getValue(keyFrames, (float)i / (float)interpCount, res);
            (*vertexData)[i].x() = res.x();
            (*vertexData)[i].y() = res.y();
        }
    }

    static void setConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx,
        const osgWidget::XYCoord & controlPointPos){

            osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
            osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

            setControlPoint((*vertexData)[controlPointIdx], controlPointPos);
    }

    static osgWidget::XYCoord getConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx){

        osgWidget::XYCoord ret;

        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        ret.x() = (*vertexData)[controlPointIdx].x();
        ret.y() = (*vertexData)[controlPointIdx].y();

        return ret;
    }

    static void setControlPoint(osg::Vec3f & cp, const osgWidget::XYCoord & pos){
        cp[0] = pos.x();
        cp[1] = pos.y();
    }

    static void refreshConnection(osg::DrawArrays * drArrys){
        drArrys->setFirst(0); 
        drArrys->setCount(interpCount);
        drArrys->dirty();
    }

    static osgWidget::XYCoord dirOut;
    static osgWidget::XYCoord dirIn;
    static unsigned int interpCount;
    static float coeffMul;

    static float connectingWidth;
    static float connectedWidth;
};

}

#endif  //  HEADER_GUARD_VDF__OSGVDFBEZIEERCONNECTIONFACTORY_H__
