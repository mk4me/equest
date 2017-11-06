#ifndef HEADER_GUARD__VDFMODEL_H__
#define HEADER_GUARD__VDFMODEL_H__

#include <vdfmlib/osgVDFBaseModel.h>
#include <vdfmlib/osgVDFPin.h>
#include <vdfmlib/osgVDFNode.h>

#include <osg/LineWidth>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgDB/ReadFile>
#include <type_traits>
#include <algorithm>
#include <cmath>

#include <dfmlib/DFModel.h>
#include <osgui/Utils2D.h>

#include <vdfmlib/osgVDFBezieerConnectionFactory.h>
#include <vdfmlib/osgVDFRectangleSelectionFactory.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

template<class VNode = osgVDFNode, class VPin = osgVDFPin, class ConnectionFactory = osgVDFBezieerConnectionFactory,
	class SelectionFactory = osgVDFRectangleSelectionFactory>
class osgVDFModel :	public osgVDFBaseModel, public SelectionFactory
{
	BOOST_STATIC_ASSERT((boost::is_base_of<osgVDFBaseNode, VNode>::value));
	BOOST_STATIC_ASSERT((boost::is_base_of<osgVDFBasePin, VPin>::value));

public:
	osgVDFModel(osgViewer::View* view = 0, osgWidget::point_type width = 0.0f, osgWidget::point_type height = 0.0f,
		unsigned int nodeMask = 0, unsigned int flags = 0);

	~osgVDFModel(void);


private:

	virtual osgVDFBaseNode* createVisualNode(osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
		const std::string &  name = std::string());
    virtual osgVDFBasePin * createVisualPin(const std::string &  name = std::string());

	//user interaction - connection managment
	virtual osg::Geode* createVisualConnection();
	virtual void setVisualConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos);
	virtual void setVisualConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos);
	virtual void setVisualConnectionZ(osg::Geode * connection, osgWidget::point_type z);
	virtual void setVisualConnectionColor(osg::Geode * connection, const osgWidget::Color & color);

	//user interaction - selection managment
	virtual osg::Geode* createVisualSelection(const osgWidget::XYCoord & startPos);
	virtual void continueVisulaSelection(osg::Geode * selection, const osgWidget::XYCoord & continuePos);		
};

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::osgVDFModel(osgViewer::View* view, osgWidget::point_type width,
	osgWidget::point_type height, unsigned int nodeMask, unsigned int flags)
	: osgVDFBaseModel(view, width, height, nodeMask, flags, dflm::DFMPtr(new dflm::DFModel())){

		BOOST_ASSERT((view != 0));
		view->getCamera()->setClearColor(osg::Vec4(0.3,0.3,0.3,1));
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::~osgVDFModel(void){

}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osgVDFBaseNode* osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::createVisualNode(osg::ref_ptr<osg::Image> image,
	const std::string & name){

		osgVDFBaseNode* ret = new VNode(name, image);

		return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osgVDFBasePin* osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::createVisualPin(const std::string & name){

        osgVDFBasePin* ret = new VPin(name);

        return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osg::Geode* osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::createVisualConnection(){
	osg::Geode* ret = ConnectionFactory::createVisualConnection();
	return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos){
	ConnectionFactory::setConnectionEndAndRefresh(connection, endPos);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos){
	ConnectionFactory::setConnectionStartAndRefresh(connection, startPos);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionZ(osg::Geode * connection, osgWidget::point_type z){
	ConnectionFactory::setConnectionZAndRefresh(connection, z);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionColor(osg::Geode * connection, const osgWidget::Color & color){
	ConnectionFactory::setConnectionColorAndRefresh(connection, color);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osg::Geode* osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::createVisualSelection(const osgWidget::XYCoord & startPos){
	osg::Geode* ret = createSelection();
	setSelectionStart(ret, startPos);
	return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::continueVisulaSelection(osg::Geode * selection, const osgWidget::XYCoord & continuePos){
	setSelectionEndAndRefresh(selection, continuePos);
}

typedef osgVDFModel<osgVDFNode, osgVDFPin, osgVDFBezieerConnectionFactory, osgVDFRectangleSelectionFactory> osgVDFDefaultModel; 

}

#endif
