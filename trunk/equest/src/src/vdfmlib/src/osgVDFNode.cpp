#include "StdAfx.h"
#include <vdfmlib/osgVDFNode.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

const unsigned int nodeTotalHeight = 112;
const unsigned int nodeTotalWidth = 92;
const unsigned int nodeBorderHeight = 6;
const unsigned int nodeBorderWidth = 6;

const unsigned int nodeTopHeight = 20;
const unsigned int nodeBottomHeight = 3;

const unsigned int nodeImgWidth = 30;
const unsigned int nodeImgHeight = 30;

const unsigned int nodeCtrlImgHeight = 12;
const unsigned int nodeCtrlImgWidth = 12;

const unsigned int pinWidth = 13;
const unsigned int pinHeight = 12;
const unsigned int pinToPinOffset = 6;

const int pinsToVerticalBorderOffset = -1;
const unsigned int pinsToTopOffset = 8;
const unsigned int pinsToBottomOffset = pinsToTopOffset + nodeBorderHeight;
const unsigned int pinsToPinsOffset = pinHeight * 2;
const unsigned int maxPinDescSigns = 5;
const unsigned int maxNodeDescSigns = 5;

const unsigned int pinFontSize = 10;
const unsigned int nodeFontSize = 13;

osgVDFNode::osgVDFNode(const std::string& name,	osg::ref_ptr<osg::Image> img) 
	: osgVDFBaseNode(name), m_pBodyLabelImage(img),
	m_pNodeLayoutGrid(new osgui::Grid("layoutGrid",3,3)),
	m_pNodeLayoutGridEmbedded(new osgui::Embedded<osgui::Grid>("layoutEmbedded",m_pNodeLayoutGrid)),
	m_pMainGrid(new osgui::Grid("mainGrid",3,1)),
	m_pMainGridEmbedded(new osgui::Embedded<osgui::Grid>("mainEmbedded",m_pMainGrid)),
	m_pTopGrid(new osgui::Grid("topGrid",1,5)),
	m_pTopGridEmbedded(new osgui::Embedded<osgui::Grid>("topEmbedded",m_pTopGrid)),
	m_pConfigure(new osgWidget::Widget("cfgWidget")),
	m_pConfigureARKeeper(new osgui::AspectRatioKeeper(m_pConfigure, (double)pinWidth / (double)pinHeight)),
	m_pDelete(new osgWidget::Widget("delWidget")),
	m_pDeleteARKeeper(new osgui::AspectRatioKeeper(m_pDelete, (double)pinWidth / (double)pinHeight)),
	m_pBottomGrid(new osgui::Grid("bottomGrid",1,3)),
	m_pBottomGridEmbedded(new osgui::Embedded<osgui::Grid>("bottomEmbedded",m_pBottomGrid)),
	m_pInPinsGrid(new osgui::Grid("inputPins",0,0)),
	m_pInPinsGridEmbedded(new osgui::Embedded<osgui::Grid>("inputPinsEmbedded",m_pInPinsGrid)),
	m_pOutPinsGrid(new osgui::Grid("outputPins",0,0)),
	m_pOutPinsGridEmbedded(new osgui::Embedded<osgui::Grid>("outputPinsEmbedded",m_pOutPinsGrid)),
	m_pNodeNameLabel(new BLabel(name+"/nameLabel",name)),
	m_pBodyLabel(new BLabel(name+"/bodyLabel","")),
	m_pEmptyWidgetProto(new osgWidget::Widget("emptyWidget"))
{
	osgWidget::Color defaultColor(0,0,0,0);

	m_pEmptyWidgetProto->setMinimumSize(nodeBorderWidth, nodeBorderHeight);
	m_pEmptyWidgetProto->setSize(nodeBorderWidth, nodeBorderHeight);
	m_pEmptyWidgetProto->setCanFill(true);
	m_pEmptyWidgetProto->setColor(1,1,1,1);

	m_pInPinsGridEmbedded->setLayer(osgWidget::Widget::LAYER_HIGH);
	m_pOutPinsGridEmbedded->setLayer(osgWidget::Widget::LAYER_HIGH);

	m_pBodyLabel->setColor(defaultColor);
	m_pBodyLabel->setCanFill(true);

	setName(name);
	m_pNodeNameLabel->setAlignHorizontal(osgWidget::Widget::HA_LEFT);
	m_pNodeNameLabel->setSize(0,0);
	m_pNodeNameLabel->setFontSize(nodeFontSize);
	//m_pNodeNameLabel->setSize(m_pNodeNameLabel->getTextSize());
	m_pNodeNameLabel->setColor(defaultColor);
	m_pNodeNameLabel->setCanFill(true);

	//all must be transparent
	this->getBackground()->setColor(defaultColor);
	//grids
	m_pNodeLayoutGrid->getBackground()->setColor(defaultColor);
	m_pNodeLayoutGridEmbedded->setColor(defaultColor);
	m_pTopGrid->getBackground()->setColor(defaultColor);
	m_pTopGridEmbedded->setColor(defaultColor);
	m_pMainGrid->getBackground()->setColor(defaultColor);
	m_pMainGridEmbedded->setColor(defaultColor);
	m_pBottomGrid->getBackground()->setColor(defaultColor);
	m_pBottomGridEmbedded->setColor(defaultColor);

	//pins
	m_pInPinsGrid->getBackground()->setColor(defaultColor);
	m_pInPinsGridEmbedded->setColor(defaultColor);
	m_pOutPinsGrid->getBackground()->setColor(defaultColor);
	m_pOutPinsGridEmbedded->setColor(defaultColor);


	//set grids fillable
	m_pNodeLayoutGridEmbedded->setCanFill(true);
	m_pTopGridEmbedded->setCanFill(true);
	m_pMainGridEmbedded->setCanFill(true);
	m_pBottomGridEmbedded->setCanFill(true);

	unsigned int nodeWidth = nodeTotalWidth - 2 * nodeBorderWidth;
	unsigned int nodeHeight = nodeTotalHeight - 2 * nodeBorderHeight;

	this->_width.minimum = nodeTotalWidth;
	this->_height.minimum = nodeTotalHeight;

	m_pNodeLayoutGridEmbedded->setSize(nodeTotalWidth, nodeTotalHeight);
	m_pNodeLayoutGridEmbedded->setMinimumSize(nodeTotalWidth, nodeTotalHeight);

	m_pMainGridEmbedded->setSize(nodeWidth, nodeHeight);
	//m_pMainGridEmbedded->setMinimumSize(nodeWidth, nodeHeight - nodeBottomHeight - nodeTopHeight);

	m_pTopGridEmbedded->setSize(nodeWidth, nodeTopHeight);
	//m_pTopGridEmbedded->setMinimumSize(nodeWidth, nodeTopHeight);

	m_pBottomGridEmbedded->setSize(nodeWidth, nodeBottomHeight);
	//m_pBottomGridEmbedded->setMinimumSize(nodeWidth, nodeBottomHeight);

	m_pBodyLabel->setSize(nodeWidth, nodeHeight - nodeBottomHeight - nodeTopHeight);

	//SET CONTENT!!!
	m_pNodeLayoutGrid->setColumnWeight(2,0);
	m_pNodeLayoutGrid->setColumnWeight(1,1);
	m_pNodeLayoutGrid->setColumnWeight(0,0);

	m_pNodeLayoutGrid->setRowWeight(2,0);
	m_pNodeLayoutGrid->setRowWeight(1,1);
	m_pNodeLayoutGrid->setRowWeight(0,0);


	//main
	m_pMainGrid->setRowWeight(2,0);
	m_pMainGrid->setRowWeight(1,1);
	m_pMainGrid->setRowWeight(0,0);

	//top
	m_pTopGrid->setColumnWeight(4,0);
	m_pTopGrid->setColumnWeight(3,0);
	m_pTopGrid->setColumnWeight(2,0);
	m_pTopGrid->setColumnWeight(1,1);
	m_pTopGrid->setColumnWeight(0,0);

	//bottom
	m_pBottomGrid->setColumnWeight(0,0);
	m_pBottomGrid->setColumnWeight(1,1);
	m_pBottomGrid->setColumnWeight(2,0);

	m_pNodeLayoutGrid->fillEmpty(m_pEmptyWidgetProto);
	m_pNodeLayoutGrid->resetFillables();

	m_pMainGrid->fillEmpty(m_pEmptyWidgetProto);
	m_pMainGrid->resetFillables();

	m_pTopGrid->addWidget(m_pNodeNameLabel,0,1);

	m_pConfigure->setMinimumSize(pinWidth, pinHeight);
	m_pConfigure->setSize(pinWidth, pinHeight);
	m_pConfigure->setEventMask(osgWidget::EVENT_MOUSE_PUSH);
	m_pConfigure->addCallback(new osgWidget::Callback(&osgVDFNode::configureNode, this, osgWidget::EVENT_MOUSE_PUSH));
	m_pConfigure->setCanFill(false);

	m_pDelete->setMinimumSize(pinWidth, pinHeight);
	m_pDelete->setSize(pinWidth, pinHeight);
	m_pDelete->setEventMask(osgWidget::EVENT_MOUSE_PUSH);
	m_pDelete->addCallback(new osgWidget::Callback(&osgVDFNode::deleteNode, this, osgWidget::EVENT_MOUSE_PUSH));
	m_pDelete->setCanFill(false);

	m_pConfigureARKeeper->setMinimumSize(pinWidth, pinHeight);
	m_pConfigureARKeeper->setSize(pinWidth, pinHeight);
	m_pConfigureARKeeper->setColor(defaultColor);

	m_pDeleteARKeeper->setMinimumSize(pinWidth, pinHeight);
	m_pDeleteARKeeper->setSize(pinWidth, pinHeight);
	m_pDeleteARKeeper->setColor(defaultColor);

	m_pTopGrid->addWidget(m_pConfigureARKeeper,0,2);
	m_pTopGrid->addWidget(m_pDeleteARKeeper,0,3);

	m_pEmptyWidgetProto->setMinimumSize(nodeBottomHeight,nodeBottomHeight);
	m_pEmptyWidgetProto->setSize(nodeBottomHeight,nodeBottomHeight);

	m_pTopGrid->fillEmpty(m_pEmptyWidgetProto);
	m_pTopGrid->resetFillables();

	m_pBottomGrid->fillEmpty(m_pEmptyWidgetProto);
	m_pBottomGrid->resetFillables();

	m_pNodeLayoutGrid->addWidget(m_pMainGridEmbedded,1,1);

	m_pMainGrid->addWidget(m_pTopGridEmbedded,2,0);
	m_pMainGrid->addWidget(m_pBodyLabel,1,0);
	m_pMainGrid->addWidget(m_pBottomGridEmbedded,0,0);

	m_pEmptyWidgetProto->setMinimumSize(0,pinToPinOffset);
	m_pEmptyWidgetProto->setSize(0,pinToPinOffset);
	m_pEmptyWidgetProto->setColor(0,0,0,0);

	//set events
	m_pNodeLayoutGrid->setEventMask(0);
	m_pNodeLayoutGrid->setEventMask(0);
	m_pNodeLayoutGridEmbedded->setEventMask(0);
	m_pTopGrid->setEventMask(0);
	m_pTopGridEmbedded->setEventMask(0);
	m_pMainGrid->setEventMask(0);
	m_pMainGridEmbedded->setEventMask(0);
	m_pBottomGrid->setEventMask(0);
	m_pBottomGridEmbedded->setEventMask(0);

	//pins
	m_pInPinsGrid->setEventMask(0);
	m_pInPinsGridEmbedded->setEventMask(0);
	m_pOutPinsGrid->setEventMask(0);
	m_pOutPinsGridEmbedded->setEventMask(0);
}

osgVDFNode::~osgVDFNode(void)
{
}

bool osgVDFNode::configureNode(osgWidget::Event& ev){
	if(ev.getWindowManager()->isLeftMouseButtonDown() == true){
		getModelNode()->configure();
	}

	return false;
}

void osgVDFNode::lockForEditing(bool lock)
{
    if(lock == true){
        m_pConfigure->setEventMask(osgWidget::EVENT_NONE);
        m_pDelete->setEventMask(osgWidget::EVENT_NONE);

        //TODO
        //load new textures!!
        //albo podepnij inne eventy z info że to teraz nie działa
    }else{
        m_pConfigure->setEventMask(osgWidget::EVENT_MOUSE_PUSH);
        m_pDelete->setEventMask(osgWidget::EVENT_MOUSE_PUSH);

        //TODO
        //load new textures!!
        //ewentualnie podepnij właściwe eventy
    }
}

void osgVDFNode::loggZCoordinates() const{
	osgVDFBaseNode::loggZCoordinates();

	std::cout << "m_pNodeNameLabel:\t" << osgui::Utils2D::calcAbsZ(m_pNodeNameLabel) << std::endl;
	std::cout << "m_pMainGrid:\t" << osgui::Utils2D::calcAbsZ(m_pMainGrid) << std::endl;
	std::cout << "m_pMainGridEmbedded:\t" << osgui::Utils2D::calcAbsZ(m_pMainGridEmbedded) << std::endl;
	std::cout << "m_pNodeLayoutGrid:\t" << osgui::Utils2D::calcAbsZ(m_pNodeLayoutGrid) << std::endl;
	std::cout << "m_pNodeLayoutGridEmbedded:\t" << osgui::Utils2D::calcAbsZ(m_pNodeLayoutGridEmbedded) << std::endl;
	std::cout << "m_pTopGrid:\t" << osgui::Utils2D::calcAbsZ(m_pTopGrid) << std::endl;
	std::cout << "m_pTopGridEmbedded:\t" << osgui::Utils2D::calcAbsZ(m_pTopGridEmbedded) << std::endl;
	std::cout << "m_pBottomGrid:\t" << osgui::Utils2D::calcAbsZ(m_pBottomGrid) << std::endl;
	std::cout << "m_pBottomGridEmbedded:\t" << osgui::Utils2D::calcAbsZ(m_pBottomGridEmbedded) << std::endl;
	std::cout << "m_pInPinsGrid:\t" << osgui::Utils2D::calcAbsZ(m_pInPinsGrid) << std::endl;
	std::cout << "m_pInPinsGridEmbedded:\t" << osgui::Utils2D::calcAbsZ(m_pInPinsGridEmbedded) << std::endl;
	std::cout << "m_pOutPinsGrid:\t" << osgui::Utils2D::calcAbsZ(m_pOutPinsGrid) << std::endl;
	std::cout << "m_pOutPinsGridEmbedded:\t" << osgui::Utils2D::calcAbsZ(m_pOutPinsGridEmbedded) << std::endl;
	std::cout << "m_pBodyLabel:\t" << osgui::Utils2D::calcAbsZ(m_pBodyLabel) << std::endl;
	std::cout << "m_pConfigure:\t" << osgui::Utils2D::calcAbsZ(m_pConfigure) << std::endl;
	std::cout << "m_pConfigureARKeeper:\t" << osgui::Utils2D::calcAbsZ(m_pConfigureARKeeper) << std::endl;
	std::cout << "m_pDelete:\t" << osgui::Utils2D::calcAbsZ(m_pDelete) << std::endl;
	std::cout << "m_pDeleteARKeeper:\t" << osgui::Utils2D::calcAbsZ(m_pDeleteARKeeper) << std::endl;
	std::cout << "m_pNodeNameLabel:\t" << osgui::Utils2D::calcAbsZ(m_pNodeNameLabel) << std::endl;

	std::cout << "IN PINS:" << std::endl;
	for(osgVDFBaseNode::Pins::const_iterator it = this->getInPins().begin(); it != this->getInPins().end(); ++it){

			std::cout << (*it)->getName() << ":\t" << osgui::Utils2D::calcAbsZ(*it) << std::endl;
	}

	std::cout << "OUT PINS:" << std::endl;
	for(osgVDFBaseNode::Pins::const_iterator it = this->getOutPins().begin(); it != this->getOutPins().end(); ++it){

			std::cout << (*it)->getName() << ":\t" << osgui::Utils2D::calcAbsZ(*it) << std::endl;
	}
}

void osgVDFNode::graphSetStatus(osgVDFBaseNode::VisualStatus nodeVisualStatus) {
		
	std::string status;

	switch(nodeVisualStatus){
	case osgVDFBaseNode::ACTIVE:
		status = "active";
		break;
	case osgVDFBaseNode::OK:
		status = "normal";
		break;
	case osgVDFBaseNode::COLLISION:
		status = "collision";
		break;
	}

	m_pNodeLayoutGrid->getByRowCol(2,0)->setStyle("vdf.node.border.TL." + status);
	m_pNodeLayoutGrid->getByRowCol(2,1)->setStyle("vdf.node.border.TM." + status);
	m_pNodeLayoutGrid->getByRowCol(2,2)->setStyle("vdf.node.border.TR." + status);
	m_pNodeLayoutGrid->getByRowCol(1,0)->setStyle("vdf.node.border.ML." + status);
	m_pNodeLayoutGrid->getByRowCol(1,2)->setStyle("vdf.node.border.MR." + status);
	m_pNodeLayoutGrid->getByRowCol(0,0)->setStyle("vdf.node.border.BL." + status);
	m_pNodeLayoutGrid->getByRowCol(0,1)->setStyle("vdf.node.border.BM." + status);
	m_pNodeLayoutGrid->getByRowCol(0,2)->setStyle("vdf.node.border.BR." + status);
		
	m_pMainGrid->getByRowCol(1,0)->setStyle("vdf.node.body.Center." + status);
	m_pTopGrid->getByRowCol(0,0)->setStyle("vdf.node.body.TL." + status);	
	m_pTopGrid->getByRowCol(0,1)->setStyle("vdf.node.body.TNameLabel." + status);
	//m_pNodeNameLabel->setStyle("vdf.node.body.nameLabel." + status);
	m_pTopGrid->getByRowCol(0,2)->setStyle("vdf.node.body.TConfigBCG." + status);
	m_pTopGrid->getByRowCol(0,3)->setStyle("vdf.node.body.TDeleteBCG." + status);
	m_pTopGrid->getByRowCol(0,4)->setStyle("vdf.node.body.TR." + status);	

	m_pConfigure->setStyle("vdf.node.body.config." + status);
	m_pDelete->setStyle("vdf.node.body.delete." + status);

	m_pBottomGrid->getByRowCol(0,0)->setStyle("vdf.node.body.BL." + status);
	m_pBottomGrid->getByRowCol(0,1)->setStyle("vdf.node.body.BM." + status);
	m_pBottomGrid->getByRowCol(0,2)->setStyle("vdf.node.body.BR." + status);

	osgWidget::WindowManager* wm = this->getWindowManager();
	if(wm != 0){
		osgWidget::StyleManager * sm = wm->getStyleManager();
		if(sm != 0){
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(0,0));
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(0,1));
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(0,2));
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(1,0));
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(1,2));
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(2,0));
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(2,1));
			sm->applyStyles(m_pNodeLayoutGrid->getByRowCol(2,2));


			sm->applyStyles(m_pTopGrid->getByRowCol(0,0));
			sm->applyStyles(m_pTopGrid->getByRowCol(0,1));
			sm->applyStyles(m_pTopGrid->getByRowCol(0,2));
			sm->applyStyles(m_pTopGrid->getByRowCol(0,3));
			sm->applyStyles(m_pTopGrid->getByRowCol(0,4));

			sm->applyStyles(m_pDelete);
			sm->applyStyles(m_pConfigure);
			sm->applyStyles(m_pMainGrid->getByRowCol(1,0));
				
			sm->applyStyles(m_pBottomGrid->getByRowCol(0,0));
			sm->applyStyles(m_pBottomGrid->getByRowCol(0,1));
			sm->applyStyles(m_pBottomGrid->getByRowCol(0,2));

			
			for(int i = 0; i < m_pOutPinsGrid->getNumRows(); ++i){
				sm->applyStyles(m_pOutPinsGrid->getByRowCol(i,0));
			}

			for(int i = 0; i < m_pInPinsGrid->getNumRows(); ++i){
				sm->applyStyles(m_pInPinsGrid->getByRowCol(i,1));
			}
		}
	}
}

bool osgVDFNode::deleteNode(osgWidget::Event& ev){
	if(ev.getWindowManager()->isLeftMouseButtonDown() == true){
		loggZCoordinates();
		osgVDFBaseNode::deleteNode();
        return true;
	}

    return false;
}

osgWidget::point_type osgVDFNode::getBodyHeight(){
	return (this->_height.current < this->_height.minimum ? this->_height.minimum : this->_height.current) - nodeTopHeight - nodeBottomHeight;
}


void osgVDFNode::managed(osgWidget::WindowManager* wm){
	osgVDFBaseNode::managed(wm);

	this->addWidget(m_pNodeLayoutGridEmbedded);

	m_pInPinsGridEmbedded->setSize(m_pInPinsGrid->getSize());
	this->addWidget(m_pInPinsGridEmbedded);
	/*for(unsigned int i = 0; i < m_pInPinsGrid->getNumRows(); i++){
		osgWidget::Label * ll = dynamic_cast<osgWidget::Label*>(m_pInPinsGrid->getByRowCol(i,1));
		if(ll != 0){
			ll->setSize(ll->getTextSize());
		}
	}
	m_pInPinsGrid->resize();*/
	
	repositionInPins();

	m_pOutPinsGridEmbedded->setSize(m_pOutPinsGrid->getSize());
	this->addWidget(m_pOutPinsGridEmbedded);
	/*for(unsigned int i = 0; i < m_pOutPinsGrid->getNumRows(); i++){
		osgWidget::Label * ll = dynamic_cast<osgWidget::Label*>(m_pOutPinsGrid->getByRowCol(i,0));
		if(ll != 0){
			ll->setSize(ll->getTextSize());
		}
	}
	m_pOutPinsGrid->resize();*/
	
	repositionOutPins();

	graphSetStatus(getVisualStatus());
	//set grids configurations
	//layout
}

void osgVDFNode::graphAddInPin(osgVDFBasePin * inPin, const std::string & pinName){

	//add pin
	m_pInPinsGrid->setNumRows(inPins.size() * 2 - 1);
	if(m_pInPinsGrid->getNumColumns() < 2){
		m_pInPinsGrid->setNumColumns(2);
	}
	m_pInPinsGrid->addWidget(inPin,(inPins.size() - 1) * 2, 0);
	if(pinName.empty() == false){
		//add label with description
		std::string name(pinName);
		if(name.size() > maxPinDescSigns){
			//too long desc - make it shorter
			name.resize(maxPinDescSigns);
		}

		//create label
		osgWidget::Label * label = new osgWidget::Label("",name);
		label->setSize(0,0);
		label->setFontSize(pinFontSize);
		//label->setSize(label->getTextSize());

		label->setAlignHorizontal(osgWidget::Widget::HA_LEFT);
		label->setAlignVertical(osgWidget::Widget::VA_CENTER);
		label->setStyle("vdf.node.pin.text");
		
		m_pInPinsGrid->addWidget(label,(inPins.size() - 1) * 2, 1);

		if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
			getWindowManager()->getStyleManager()->applyStyles(label);
		}
	}

	m_pInPinsGrid->fillEmpty(m_pEmptyWidgetProto);
	m_pInPinsGrid->resetFillables();

	m_pInPinsGridEmbedded->setSize(m_pInPinsGrid->getSize());

	//check if pins fit node
	osgWidget::point_type pinsTotalHeight = getTotalPinsHeight();
	osgWidget::point_type heigth = getBodyHeight();
	if(pinsTotalHeight > heigth){
		this->_height.current = this->_height.minimum += pinsTotalHeight - heigth;

		m_pNodeLayoutGridEmbedded->setMinimumSize(this->_width.minimum, this->_height.minimum);
		m_pNodeLayoutGridEmbedded->setSize(this->_width.current < this->_width.minimum ? this->_width.minimum : this->_width.current,
			this->_height.current < this->_height.minimum ? this->_height.minimum : this->_height.current);

		if(_wm != 0){
			this->resize();
		}
	}else{
		repositionInPins();
	}
}


osgWidget::point_type osgVDFNode::getTotalPinsHeight(){
	osgWidget::point_type ret = 0;

	if(getInPins().empty() == false){
		ret += pinsToTopOffset + m_pInPinsGridEmbedded->getSize().y();
	}

	if(getOutPins().empty() == false){
		ret += pinsToBottomOffset - m_pBottomGridEmbedded->getHeight() + m_pOutPinsGridEmbedded->getSize().y();
	}

	if(getInPins().empty() == false && getOutPins().empty() == false){
		ret += pinsToPinsOffset;
	}

	return ret;
}

void osgVDFNode::repositionOutPins(){
	osgWidget::XYCoord pos(nodeBorderWidth, nodeBorderHeight);
	pos.x() += m_pMainGridEmbedded->getWidth() - m_pOutPinsGridEmbedded->getWidth() - pinsToVerticalBorderOffset + pinWidth / 2;
	pos.y() += pinsToBottomOffset - m_pBottomGridEmbedded->getHeight();
	m_pOutPinsGridEmbedded->setOrigin(pos);
}


void osgVDFNode::repositionInPins(){
	osgWidget::XYCoord pos;
	pos.y() = m_pTopGrid->getOrigin().y() - pinsToTopOffset - m_pInPinsGridEmbedded->getSize().y();
	pos.x() = m_pMainGrid->getOrigin().x() + pinsToVerticalBorderOffset - pinWidth / 2;
	m_pInPinsGridEmbedded->setOrigin(pos);
}


void osgVDFNode::graphAddOutPin(osgVDFBasePin * outPin, const std::string & pinName){	

	//add pin
	bool repositionRequired = false;

	m_pOutPinsGrid->setNumRows(outPins.size() * 2 - 1);
	if(m_pOutPinsGrid->getNumColumns() < 2){
		m_pOutPinsGrid->setNumColumns(2);
	}
	m_pOutPinsGrid->addWidget(outPin,(outPins.size() - 1) * 2, 1);
	if(pinName.empty() == false){
		//add label with description
		std::string name(pinName);
		if(name.size() > maxPinDescSigns){
			//too long desc - make it shorter
			name.resize(maxPinDescSigns);
		}

		//create label
		osgWidget::Label * label = new osgWidget::Label("",name);
		label->setSize(0,0);
		label->setFontSize(pinFontSize);
		//label->setSize(label->getTextSize());
		/*osgWidget::Table::CellSizes cs;

		m_pOutPinsGrid->getColumnWidths(cs);
		if(*(cs.begin()) < label->getWidth()){
			repositionRequired = true;
		}*/

		label->setAlignHorizontal(osgWidget::Widget::HA_RIGHT);
		label->setAlignVertical(osgWidget::Widget::VA_CENTER);
		label->setStyle("vdf.node.pin.text");

		m_pOutPinsGrid->addWidget(label,(outPins.size() - 1) * 2, 0);

		if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
			getWindowManager()->getStyleManager()->applyStyles(label);
		}
	}

	m_pOutPinsGrid->fillEmpty(m_pEmptyWidgetProto);
	m_pOutPinsGrid->resetFillables();

	m_pOutPinsGridEmbedded->setSize(m_pOutPinsGrid->getSize());

	

	//check if pins fit node
	osgWidget::point_type pinsTotalHeight = getTotalPinsHeight();
	osgWidget::point_type heigth = getBodyHeight();
	if(pinsTotalHeight > heigth){
		this->_height.current = this->_height.minimum += pinsTotalHeight - heigth;

		m_pNodeLayoutGridEmbedded->setMinimumSize(this->_width.minimum, this->_height.minimum);
		m_pNodeLayoutGridEmbedded->setSize(this->_width.current < this->_width.minimum ? this->_width.minimum : this->_width.current,
			this->_height.current < this->_height.minimum ? this->_height.minimum : this->_height.current);

		if(_wm != 0){
			this->resize();
		}
	}else if(repositionRequired == true){
		repositionOutPins();
	}
}


void osgVDFNode::setName(const std::string & name){
	std::string tmpName(name);
	if(tmpName.size() > maxNodeDescSigns){
		tmpName.resize(maxNodeDescSigns);
	}

	m_pNodeNameLabel->setLabel(tmpName);
}


std::string osgVDFNode::getName() const{
	return m_pNodeNameLabel->getLabel();
}


void osgVDFNode::_resizeImplementation(osgWidget::point_type diffWidth, osgWidget::point_type diffHeight){

	//resize layoutGrid
	m_pNodeLayoutGridEmbedded->addSize(diffWidth, diffHeight);

	if(diffWidth != 0 || diffHeight != 0){
		if(inPins.empty() == false){
			repositionInPins();
		}

		//update pozycji out pins jeśli są
		if(outPins.empty() == false){
			repositionOutPins();
		}

		graphSetStatus(this->getVisualStatus());
	}
}

}
