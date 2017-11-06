/********************************************************************
    created:  2011/05/16
    created:  16:5:2011   11:48
    filename: osgVDFRectangleSelectionFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VDF__OSGVDFRECTANGLESELECTIONFACTORY_H__
#define HEADER_GUARD_VDF__OSGVDFRECTANGLESELECTIONFACTORY_H__

#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgWidget/Widget>

class osgVDFRectangleSelectionFactory
{
public:

    osgVDFRectangleSelectionFactory() {}
    virtual ~osgVDFRectangleSelectionFactory() {}

    void refreshSelection(osg::Geode * selection){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
        refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
    }

    osg::LineWidth * getSelectionBorderWidth(osg::Geode * selection){
        return dynamic_cast<osg::LineWidth*>(selection->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
    }

    void setSelectionBorderWidth(osg::Geode * selection, float width){
        getSelectionBorderWidth(selection)->setWidth(width);
    }

    void setSelectionBorderWidthAndRefresh(osg::Geode * selection, float width){
        setSelectionBorderWidth(selection,width);
        refreshSelection(selection);
    }

    osgWidget::Color getSelectionBorderColor(osg::Geode * selection){
        return *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(selection->getDrawable(0))->getColorArray())->begin());
    }

    osgWidget::Color getSelectionBodyColor(osg::Geode * selection){
        //TODO
        //implement
        return osgWidget::Color();
    }

    void setSelectionBodyColor(osg::Geode * selection, const osgWidget::Color & color){
        //TODO
        //implement
    }

    void setSelectionBodyColorAndRefresh(osg::Geode * selection, const osgWidget::Color & color){
        setSelectionBodyColor(selection, color);
        refreshSelection(selection);
    }

    void setSelectionBorderColor(osg::Geode * selection, const osgWidget::Color & color){
        *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(selection->getDrawable(0))->getColorArray())->begin()) = color;
    }

    void setSelectionBorderColorAndRefresh(osg::Geode * selection, const osgWidget::Color & color){
        setSelectionBorderColor(selection, color);
        refreshSelection(selection);
    }

    osg::Geode * createSelection(const osgWidget::Color & color = osgWidget::Color(0.0,0.58,0.58,1), float width = 1){

        osg::Geode * geode = new osg::Geode();
        geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(width), osg::StateAttribute::ON);

        osg::Geometry* geom = new osg::Geometry();
        geom->setUseDisplayList(false);
        osg::DrawArrays* drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP); 
        geom->addPrimitiveSet(drawArrayLines); 
        osg::Vec3Array* vertexData = new osg::Vec3Array(4);
        geom->setVertexArray(vertexData); 

        osg::Vec4Array* c = new osg::Vec4Array();
        c->push_back( color );
        geom->setColorArray( c );
        geom->setColorBinding( osg::Geometry::BIND_OVERALL );

        geode->addDrawable( geom );
        return geode;
    }

    void setSelectionStart(osg::Geode * selection, const osgWidget::XYCoord & startPos){
        setSelectionControlPoint(selection, 0, startPos);
    }

    void setSelectionStartAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & startPos){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        setControlPoint((*vertexData)[0], startPos);
        regenerateOtherPoints(selection);
        refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
    }

    osgWidget::XYCoord getSelectionStart(osg::Geode * selection){
        return getSelectionControlPoint(selection,0);
    }

    osgWidget::XYCoord getSelectionEnd(osg::Geode * selection){
        return getSelectionControlPoint(selection,2);
    }

    std::pair<osgWidget::XYCoord, osgWidget::XYCoord> getSelectionEndings(osg::Geode * selection){
        std::pair<osgWidget::XYCoord, osgWidget::XYCoord> ret;

        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        ret.first.x() = (*vertexData)[0].x();
        ret.first.y() = (*vertexData)[0].y();

        ret.second.x() = (*vertexData)[2].x();
        ret.second.y() = (*vertexData)[2].y();

        return ret;
    }

    void setSelectionEnd(osg::Geode * selection, const osgWidget::XYCoord & endPos){
        setSelectionControlPoint(selection, 1, endPos);
    }

    void setSelectionEndAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & endPos){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        setControlPoint((*vertexData)[2], endPos);
        regenerateOtherPoints(selection);
        refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
    }

    void setSelectionEndings(osg::Geode * selection, const osgWidget::XYCoord & startPos,
        const osgWidget::XYCoord & endPos){

            osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
            osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

            setControlPoint((*vertexData)[0], startPos);
            setControlPoint((*vertexData)[2], endPos);
    }

    void setSelectionEndingsAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & startPos,
        const osgWidget::XYCoord & endPos){

            osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
            osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

            setControlPoint((*vertexData)[0], startPos);
            setControlPoint((*vertexData)[2], endPos);
            regenerateOtherPoints(selection);

            refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
    }

    void setSelectionEndingsAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & startPos,
        const osgWidget::XYCoord & endPos, osgWidget::point_type z){

            osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
            osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

            setControlPoint((*vertexData)[0], startPos);
            setControlPoint((*vertexData)[2], endPos);

            regenerateOtherPoints(selection);

            for(unsigned int i = 0; i < vertexData->size(); ++i){
                (*vertexData)[0][2] = z;
            }

            refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
    }

protected:

    void regenerateOtherPoints(osg::Geode * selection){
        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        float xDiff = (*vertexData)[2][0] - (*vertexData)[0][0];
        float yDiff = (*vertexData)[2][1] - (*vertexData)[0][1];

        (*vertexData)[1][0] = (*vertexData)[0][0] + xDiff;
        (*vertexData)[1][1] = (*vertexData)[2][1] - yDiff;

        (*vertexData)[3][0] = (*vertexData)[2][0] - xDiff;
        (*vertexData)[3][1] = (*vertexData)[0][1] + yDiff;
    }

    void setSelectionControlPoint(osg::Geode * selection, unsigned int controlPointIdx,
        const osgWidget::XYCoord & controlPointPos){

            osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
            osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

            setControlPoint((*vertexData)[controlPointIdx], controlPointPos);
    }

    osgWidget::XYCoord getSelectionControlPoint(osg::Geode * selection, unsigned int controlPointIdx){

        osgWidget::XYCoord ret;

        osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
        osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

        ret.x() = (*vertexData)[controlPointIdx].x();
        ret.y() = (*vertexData)[controlPointIdx].y();

        return ret;
    }

    void setControlPoint(osg::Vec3f & cp, const osgWidget::XYCoord & pos){
        cp[0] = pos.x();
        cp[1] = pos.y();
    }

    void refreshSelection(osg::DrawArrays * drArrys){
        drArrys->setFirst(0); 
        drArrys->setCount(4);
        drArrys->dirty();
    }
};

#endif 
