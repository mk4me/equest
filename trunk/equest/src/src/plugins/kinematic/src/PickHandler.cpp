#include "PCH.h"
#include "PickHandler.h"
#include <QtWidgets/QMessageBox>
#include <set>
PickHandler::PickHandler( HandlerFunction callback ) :
    handlerFunction(callback)
{

}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
    case(osgGA::GUIEventAdapter::PUSH):
        {
            osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
            if (view) {
                pick(view,ea);
            }
            return false;
        }    
    case(osgGA::GUIEventAdapter::KEYDOWN):
        {
            if (ea.getKey()=='c')
            {        
                osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
                osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
                event->setX((ea.getXmin()+ea.getXmax())*0.5);
                event->setY((ea.getYmin()+ea.getYmax())*0.5);
                if (view) pick(view,*event);
            }
            return false;
        }    
    default:
        return false;
    }
}

void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
    osgUtil::LineSegmentIntersector::Intersections intersections;

    float x = ea.getX();
    float y = ea.getY();

    
    osg::Vec3 eye, center, up;
    view->getCamera()->getViewMatrixAsLookAt( eye, center, up );

    if (view->computeIntersections(x,y,intersections))
    {
        PickerList list;
        std::set<GeodePtr> geodes;
        for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin(); hitr != intersections.end(); ++hitr) {
            GeodePtr geode = hitr->nodePath.empty() ? GeodePtr() : dynamic_cast<osg::Geode*>(hitr->nodePath.back());
            if (!geode || geodes.find(geode) != geodes.end()) {
                continue;
            }
            geodes.insert(geode);
            DrawablePtr drawable = hitr->drawable.valid() ? hitr->drawable : DrawablePtr();
            
            list.push_back(boost::make_tuple(geode, drawable, (eye - hitr->getWorldIntersectPoint()).length()));
        }

        list.sort([&](const PickerTuple& left, const PickerTuple& right) { return left.get<2>() < right.get<2>(); });
        handlerFunction(list);
    }
}
