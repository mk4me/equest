#ifndef HEADER_GUARD__VDFNODE_H__
#define HEADER_GUARD__VDFNODE_H__

//#include <osgText/Text>
#include <osgDB/ReadFile>
#include <iterator>
#include <osgWidget/EventInterface>
#include <stdio.h>

#include <vdfmlib/osgVDFBaseNode.h>
#include <osgui/Grid.h>
#include <osgui/AspectRatioKeeper.h>
#include <osgui/EmbeddedWindow.h>
#include <dfmlib/DFNode.h>
#include <osgui/Borderized.h>
#include <osgui/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFNode : public osgVDFBaseNode
{
public:
	META_Window(osgWidget::Window, osgVDFNode);

	osgVDFNode(const std::string& nodeName = "", osg::ref_ptr<osg::Image> img = osg::ref_ptr<osg::Image>());
	osgVDFNode(const osgVDFNode& node, const osg::CopyOp& co) {};

	~osgVDFNode(void);

	virtual void managed(osgWidget::WindowManager* wm);

	//for debug only!!
	virtual void loggZCoordinates() const;

	virtual void setName(const std::string & name);
	virtual std::string getName() const;

protected:
	typedef osgui::Borderized<osgWidget::Label> BLabel;

protected:

	virtual void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight);
	virtual Sizes _getWidthImplementation  () const{
		return Sizes(m_pNodeLayoutGridEmbedded->getWidth(), m_pNodeLayoutGridEmbedded->getMinWidth());
	}

    virtual Sizes _getHeightImplementation () const{
		return Sizes(m_pNodeLayoutGridEmbedded->getHeight(), m_pNodeLayoutGridEmbedded->getMinHeight());
	}

	virtual void graphAddInPin(osgVDFBasePin * inPin, const std::string & pinName);
	virtual void graphAddOutPin(osgVDFBasePin * outPin, const std::string & pinName);

	void repositionOutPins();
	void repositionInPins();

	osgWidget::point_type getTotalPinsHeight();
	osgWidget::point_type getBodyHeight();

	virtual void graphSetStatus(VisualStatus nodeVisualStatus);

    virtual void lockForEditing(bool lock);

	bool deleteNode(osgWidget::Event& ev);

	bool configureNode(osgWidget::Event& ev);

protected:

	//image for grid body, characteristic for node type
	osg::ref_ptr<osg::Image> m_pBodyLabelImage;

	//label in top part of node with node name
	osg::ref_ptr<BLabel> m_pNodeNameLabel;
	//osg::ref_ptr<osgui::AspectRatioKeeper> m_pNodeNameLabelEmbedded;

	//node itself
	osg::ref_ptr<osgui::Grid> m_pMainGrid;
	osg::ref_ptr<osgui::Embedded<osgui::Grid> > m_pMainGridEmbedded;

	//node with borders as bitmaps
	osg::ref_ptr<osgui::Grid> m_pNodeLayoutGrid;
	osg::ref_ptr<osgui::Embedded<osgui::Grid> > m_pNodeLayoutGridEmbedded;

	//top of node - name of node and controls: delete, configure, ...
	osg::ref_ptr<osgui::Grid> m_pTopGrid;
	osg::ref_ptr<osgui::Embedded<osgui::Grid> > m_pTopGridEmbedded;

	//top center of node - name of node and controls: delete, configure, ...
	//osg::ref_ptr<osgui::Grid> m_pTopCenterGrid;
	//osg::ref_ptr<osgui::Embedded<osgui::Grid> > m_pTopCenterGridEmbedded;

	//bottom of node - round corners as bitmaps, eventually resize control
	osg::ref_ptr<osgui::Grid> m_pBottomGrid;
	osg::ref_ptr<osgui::Embedded<osgui::Grid> > m_pBottomGridEmbedded;

	//grid for input pins
	osg::ref_ptr<osgui::Grid> m_pInPinsGrid;
	osg::ref_ptr<osgui::Embedded<osgui::Grid> > m_pInPinsGridEmbedded;

	//grid for output pins
	osg::ref_ptr<osgui::Grid> m_pOutPinsGrid;
	osg::ref_ptr<osgui::Embedded<osgui::Grid> > m_pOutPinsGridEmbedded;

	//label for image characteristic for node type
	osg::ref_ptr<BLabel> m_pBodyLabel;
	//osg::ref_ptr<osgui::AspectRatioKeeper> m_pBodyLabelEmbedded;

	osg::ref_ptr<osgWidget::Widget> m_pConfigure;
	osg::ref_ptr<osgui::AspectRatioKeeper> m_pConfigureARKeeper;

	osg::ref_ptr<osgWidget::Widget> m_pDelete;
	osg::ref_ptr<osgui::AspectRatioKeeper> m_pDeleteARKeeper;

	//empty widget
	osg::ref_ptr<osgWidget::Widget> m_pEmptyWidgetProto;
};

}

#endif
