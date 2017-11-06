#include "PCH.h"
#include <osgui/Toolbar.h>
#include <osgWidget/StyleManager>
#include <osgWidget/WindowManager>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

const osgWidget::point_type defaultToolbarHeight = 28;

Toolbar::Toolbar(const std::string & name) : osgui::Grid(name,2,1), m_pActiveTab(0),
	m_pNullWidth(new osgWidget::Widget("nullWidth")) {

	m_sTabNormalStyleDefault = m_sTabNormalStyle = "osgui.toolbar.tab.normal";
	m_sTabActiveStyleDefault = m_sTabActiveStyle = "osgui.toolbar.tab.active";
	m_sTabDisableStyleDefault = m_sTabDisableStyle = "osgui.toolbar.tab.disable";

	m_sToolbarUpperHalfStyleDefault = m_sToolbarUpperHalfStyle = "osgui.toolbar.upperHalf";
	m_sToolbarLowerHalfStyleDefault = m_sToolbarLowerHalfStyle = "osgui.toolbar.lowerHalf";

	m_pTabPushEvent = new osgWidget::Callback(&Toolbar::tabPush, this, osgWidget::EVENT_MOUSE_PUSH);

	m_pNullWidth->setMinimumSize(0,0);
	m_pNullWidth->setWidth(0);
	m_pNullWidth->setHeight(0);
	m_pNullWidth->setCanFill(false);
	m_pNullWidth->setColor(0,0,0,0);

	m_pTabsGrid.first = new osgui::Grid(name + "\\tabsGrid",1,1);
	m_pTabsGrid.first->setStyle(m_sToolbarUpperHalfStyle);
	m_pTabsGrid.first->fillEmpty();
	m_pTabsGrid.first->resetFillables();

	m_pTabsGrid.second = new osgui::Embedded<osgui::Grid>(name + "\\tabsGridEmbedded", m_pTabsGrid.first);
	m_pTabsGrid.second->setCanFill(true);
	m_pTabsGrid.second->setColor(0,0,0,0);
	m_pTabsGrid.second->setMinimumSize(0,defaultToolbarHeight/2);
	this->getBackground()->setColor(0,0,0,0);
}

Toolbar::~Toolbar(void){

}

void Toolbar::managed(osgWidget::WindowManager* wm){
	osgui::Grid::managed(wm);

	addWidget(m_pTabsGrid.second,1,0);

	this->fillEmpty();
	this->resetFillables();
	this->getByRowCol(0,0)->setMinimumSize(0,defaultToolbarHeight/2);
	setStyle(this->getByRowCol(0,0), m_sToolbarLowerHalfStyle);

	osgWidget::StyleManager * sm = wm->getStyleManager();
	if(sm != 0){
		//upper half
		sm->applyStyles(m_pTabsGrid.first);

		//tabs
		for(TABS::iterator it = m_vTabs.begin(); it != m_vTabs.end(); ++it){
			sm->applyStyles(it->first);
		}

		//lower half
		if(m_pActiveTab != 0){
			sm->applyStyles(this->getByRowCol(0,0));
		}
	}

	this->resize(0, defaultToolbarHeight);
}

bool Toolbar::addTab(osgWidget::Widget * tab, bool enable, bool visible, bool active){
	return addTab(tab, m_vTabs.size(), enable, visible, active);
}

bool Toolbar::addTab(osgWidget::Widget * tab, int tabIndex, bool enable, bool visible, bool active){
	if(m_mRevTabs.find(tab) != m_mRevTabs.end()){
		return false;
	}

	if(tabIndex > m_vTabs.size()){
		tabIndex = m_vTabs.size();
	}else if(tabIndex < 0){
		tabIndex = 0;
	}

	unsigned int idx = m_vTabs.size() + 1;
	m_vTabs.resize(idx);
	m_pTabsGrid.first->setNumColumns(m_vTabs.size() + 1);

    ////set columns weights for resizing, only last one should change
    //for(int i = 0; i < m_vTabs.size(); i++){
    //    m_pTabsGrid.first->setColumnWeight(i, 0);
    //}

    //m_pTabsGrid.first->setColumnWeight(m_vTabs.size(), 1);

	//shift right elements if necessary
	--idx;
	while(idx > tabIndex){
		//tabs visual storage
		osg::ref_ptr<osgWidget::Widget> tmp = m_pTabsGrid.first->getByRowCol(0,idx-1);
		m_pTabsGrid.first->removeWidget(tmp);
		m_pTabsGrid.first->addWidget(tmp,0,idx);
		//tabs logical storage
		m_vTabs[idx] = m_vTabs[idx-1];
		m_mRevTabs[m_vTabs[idx-1].first] = idx;
		--idx;
	}

	TAB_DESC desc;

	if(visible == false){
		active = false;
	}

	desc.active = active;
	desc.enable = enable;
	desc.visible = visible;

	m_vTabs[tabIndex] = TAB(tab,desc);
	m_mRevTabs[tab] = tabIndex;

	SUBTOOLBAR st;
	st.first = new osgui::Grid(tab->getName() + "\\subtoolbarGrid",1,1);
	st.first->setStyle(m_sToolbarLowerHalfStyle);
	st.first->fillEmpty();
	st.first->resetFillables();

	st.second =  new osgui::Embedded<osgui::Grid>(tab->getName() + "\\subtoolbarGridEmbedded", st.first);
	st.second->setColor(0,0,0,0);
	st.second->setCanFill(true);
	m_mSubToolbars[tab] = st;

	//set PUSH event mask
	tab->addEventMask(osgWidget::EVENT_MOUSE_PUSH);
	//add PUSH event handler
	tab->addCallback(m_pTabPushEvent);
	setStyle(tab, m_sTabNormalStyle);

	if(isTabDisable(tab) == true){
		setStyle(tab, m_sTabDisableStyle);
	}

	if(visible == true){
		//add to proper cell in TAB grid
		//resize embedded window of TAB grid
		m_pTabsGrid.first->addWidget(tab,0,tabIndex);
	}else{
		//add empty widget with no fill with 0 width to proper cell
		osgWidget::Widget * nullWidth = dynamic_cast<osgWidget::Widget*>(m_pNullWidth->clone(osg::CopyOp::DEEP_COPY_ALL));
		m_pTabsGrid.first->addWidget(nullWidth,0,tabIndex);
	}

	m_pTabsGrid.first->fillEmpty();
	m_pTabsGrid.first->resetFillables();
	m_pTabsGrid.first->resize();
	m_pTabsGrid.second->setSize(m_pTabsGrid.first->getSize());

	if(visible == true && active == true){
		//set tab active
		activateTab(tabIndex);
	}

	this->resize();

	return true;
}

bool Toolbar::removeTab(osgWidget::Widget * tab){
	REV_TABS_MAP::iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return removeTab(it->second);
}

bool Toolbar::removeTab(int tabIndex){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	//shift left all elements if necessary
	osgWidget::Widget * toDelete = m_vTabs[tabIndex].first;
	if(m_vTabs.size() == 1){
		m_pTabsGrid.first->removeWidget(m_pTabsGrid.first->getByRowCol(0,0));
		m_pTabsGrid.first->setNumRows(1);
		TABS().swap(m_vTabs);

		if(m_pActiveTab == toDelete){
			resetActiveTab();
		}
	}else{
		int idx = tabIndex;
		++idx;
		while(idx < m_vTabs.size()){
			//tabs visual storage
			osg::ref_ptr<osgWidget::Widget> tmp = m_pTabsGrid.first->getByRowCol(0,idx);
			m_pTabsGrid.first->removeWidget(tmp);
			m_pTabsGrid.first->addWidget(tmp,0,idx-1);
			//tabs logical storage
			m_vTabs[idx-1] = m_vTabs[idx];
			m_mRevTabs[m_vTabs[idx].first] = idx - 1;
			++idx;
		}

		m_vTabs.resize(m_vTabs.size() - 1);
		m_pTabsGrid.first->setNumRows(m_vTabs.size() + 1);
		if(m_pActiveTab == toDelete){
			activateTab(0);
		}
	}

	m_pTabsGrid.first->fillEmpty();
	m_pTabsGrid.first->resetFillables();

	m_mRevTabs.erase(toDelete);
	m_mSubToolbars.erase(toDelete);

	return true;
}

bool Toolbar::showTab(osgWidget::Widget * tab){
	REV_TABS_MAP::iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return showTab(it->second);
}

bool Toolbar::showTab(int tabIndex){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	if(m_vTabs[tabIndex].second.visible == true){
		return true;
	}

	m_vTabs[tabIndex].second.visible = true;
	m_pTabsGrid.first->addWidget(m_vTabs[tabIndex].first,0,tabIndex);

	return true;
}

bool Toolbar::hideTab(osgWidget::Widget * tab){
	REV_TABS_MAP::iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return hideTab(it->second);
}

bool Toolbar::hideTab(int tabIndex){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	if(m_vTabs[tabIndex].second.visible == false){
		return true;
	}

	m_vTabs[tabIndex].second.visible = false;
	m_pTabsGrid.first->addWidget(dynamic_cast<osgWidget::Widget*>(m_pNullWidth->clone(osg::CopyOp::DEEP_COPY_ALL)),0,tabIndex);

	if(m_pActiveTab == m_vTabs[tabIndex].first){
		if(m_vTabs.size() > 1){
			activateTab(tabIndex == 0 ? m_vTabs[1].first : m_vTabs[0].first);
		}else{
			resetActiveTab();
		}
	}

	return true;
}

bool Toolbar::enableTab(osgWidget::Widget * tab, bool enable){
	REV_TABS_MAP::iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return enableTab(it->second, enable);
}

bool Toolbar::enableTab(int tabIndex, bool enable){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	if(m_vTabs[tabIndex].second.enable == enable){
		return true;
	}

	m_vTabs[tabIndex].second.enable = enable;

	if(m_vTabs[tabIndex].second.active == false){
		if(m_vTabs[tabIndex].second.enable == true){
			setStyle(m_vTabs[tabIndex].first, m_sTabNormalStyle);
		}else{
			setStyle(m_vTabs[tabIndex].first, m_sTabDisableStyle);
		}
	}

	return true;
}

bool Toolbar::activateTab(osgWidget::Widget * tab){
	REV_TABS_MAP::iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return activateTab(it->second);
}

bool Toolbar::activateTab(int tabIndex){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size() || m_vTabs[tabIndex].second.visible == false){
		return false;
	}

	if(m_pActiveTab == m_vTabs[tabIndex].first){
		return true;
	}

	if(m_pActiveTab != 0){
		m_vTabs[m_mRevTabs[m_pActiveTab]].second.active = false;

		//reset style
		if(m_vTabs[m_mRevTabs[m_pActiveTab]].second.enable == true){
			setStyle(m_pActiveTab, m_sTabNormalStyle);
		}else{
			setStyle(m_pActiveTab, m_sTabDisableStyle);
		}
	}

	m_pActiveTab = m_vTabs[tabIndex].first;
	m_vTabs[tabIndex].second.active = true;

	setStyle(m_pActiveTab, m_sTabActiveStyle);

	if(m_mSubToolbars[m_pActiveTab].second->getHeight() < m_pTabsGrid.second->getHeight()){
		m_mSubToolbars[m_pActiveTab].second->setHeight(m_pTabsGrid.second->getHeight());
	}

	this->addWidget(m_mSubToolbars[m_pActiveTab].second, 0,0);
	if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
		//getWindowManager()->getStyleManager()->applyStyles(m_mSubToolbars[m_pActiveTab].second);
		getWindowManager()->getStyleManager()->applyStyles(m_mSubToolbars[m_pActiveTab].first);
	}

	this->resize();
	return true;
}

const Toolbar::TABS & Toolbar::getTabs() const{
	return m_vTabs;
}

int Toolbar::getActiveTabIdx() const{
	return getTabIdx(m_pActiveTab);
}

osgWidget::Widget * Toolbar::getActiveTab() const{
	return m_pActiveTab;
}

osgWidget::Widget * Toolbar::getTabByIdx(int tabIndex) const{
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return 0;
	}

	return m_vTabs[tabIndex].first;
}

int Toolbar::getTabIdx(osgWidget::Widget * tab) const{
	REV_TABS_MAP::const_iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return -1;
	}

	return it->second;
}

unsigned int Toolbar::tabsCount() const{
	return m_vTabs.size();
}

bool Toolbar::isTabVisible(osgWidget::Widget * tab) const{
	return isTabVisible(getTabIdx(tab));
}

bool Toolbar::isTabVisible(int tabIndex) const{
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	return m_vTabs[tabIndex].second.visible;
}

bool Toolbar::isTabEnable(osgWidget::Widget * tab) const{
	return isTabEnable(getTabIdx(tab));
}

bool Toolbar::isTabEnable(int tabIndex) const{
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	return m_vTabs[tabIndex].second.enable;
}

bool Toolbar::isTabActive(osgWidget::Widget * tab) const{
	return tab == m_pActiveTab;
}

bool Toolbar::isTabActive(int tabIndex) const{
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	return m_vTabs[tabIndex].second.active;
}

bool Toolbar::addTabElement(osgWidget::Widget * tab, osgWidget::Widget * elem){
	REV_TABS_MAP::const_iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return addTabElement(it->second, elem);
}

bool Toolbar::addTabElement(int tabIndex, osgWidget::Widget * elem){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	return addTabElement(tabIndex, elem, m_vTabs[tabIndex].second.tabElements.size());
}

bool Toolbar::addTabElement(osgWidget::Widget * tab, osgWidget::Widget * elem, int elemIndex){
	REV_TABS_MAP::const_iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return addTabElement(it->second, elem, elemIndex);
}

bool Toolbar::addTabElement(int tabIndex, osgWidget::Widget * elem, int elemIndex){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	TAB & tabDesc = m_vTabs[tabIndex];

	if(std::find(tabDesc.second.tabElements.begin(), tabDesc.second.tabElements.end(), elem) != tabDesc.second.tabElements.end()){
		return false;
	}

	int idx = tabDesc.second.tabElements.size();
	tabDesc.second.tabElements.resize(idx + 1);

	osgui::Grid * grid = m_mSubToolbars[tabDesc.first].first;
	grid->setNumColumns(tabDesc.second.tabElements.size() + 1);

	if(elemIndex > idx){
		elemIndex = idx;
	}else if(elemIndex < 0){
		elemIndex = 0;
	}

	while(idx > elemIndex){
		osg::ref_ptr<osgWidget::Widget> tmp = grid->getByRowCol(0,idx-1);
		grid->removeWidget(tmp);
		grid->addWidget(tmp,0,idx);
		tabDesc.second.tabElements[idx] = tabDesc.second.tabElements[idx-1];
		--idx;
	}

	elem->setAlignHorizontal(osgWidget::Widget::HA_CENTER);
	elem->setAlignVertical(osgWidget::Widget::VA_CENTER);

	tabDesc.second.tabElements[elemIndex] = elem;
	grid->addWidget(elem,0,elemIndex);
	grid->fillEmpty();
	grid->resetFillables();
	grid->resize();
	m_mSubToolbars[tabDesc.first].second->setSize(grid->getSize());

	return true;
}

bool Toolbar::removeTabElement(osgWidget::Widget * tab, osgWidget::Widget * elem){
	REV_TABS_MAP::const_iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return removeTabElement(it->second, elem);
}

bool Toolbar::removeTabElement(osgWidget::Widget * tab, int elemIndex){
	REV_TABS_MAP::const_iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return false;
	}

	return removeTabElement(it->second, elemIndex);
}

bool Toolbar::removeTabElement(int tabIndex, osgWidget::Widget * elem){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return false;
	}

	return removeTabElement(tabIndex, elem);
}

bool Toolbar::removeTabElement(int tabIndex, int elemIndex){
	if(tabIndex < 0 || tabIndex >= m_vTabs.size() || elemIndex < 0){
		return false;
	}

	TAB & tabDesc = m_vTabs[tabIndex];
	unsigned int idx = tabDesc.second.tabElements.size();
	if(elemIndex >= idx){
		return false;
	}

	osgui::Grid * grid = m_mSubToolbars[m_vTabs[tabIndex].first].first;

	if(idx == 1){
		ELEMENTS().swap(tabDesc.second.tabElements);
		grid->removeWidget(grid->getByRowCol(0,0));
	}else{
		++idx;
		while(elemIndex < idx){
			osg::ref_ptr<osgWidget::Widget> tmp = grid->getByRowCol(0,elemIndex + 1);
			grid->removeWidget(tmp);
			grid->addWidget(tmp,0,elemIndex);
			tabDesc.second.tabElements[elemIndex] = tabDesc.second.tabElements[elemIndex+1];
			++elemIndex;
		}

		tabDesc.second.tabElements.resize(idx);
	}

	grid->setNumColumns(tabDesc.second.tabElements.size() + 1);
	grid->fillEmpty();
	grid->resetFillables();

	return true;
}

const Toolbar::ELEMENTS & Toolbar::getTabElements(int tabIndex) const{
	if(tabIndex < 0 || tabIndex >= m_vTabs.size()){
		return m_vEmptyElements;
	}

	return m_vTabs[tabIndex].second.tabElements;
}

const Toolbar::ELEMENTS & Toolbar::getTabElements(osgWidget::Widget * tab) const{
	REV_TABS_MAP::const_iterator it = m_mRevTabs.find(tab);
	if(it == m_mRevTabs.end()){
		return m_vEmptyElements;
	}

	return getTabElements(it->second);
}

void Toolbar::setTabsNormalStyleDefault(){
	setTabsNormalStyle(m_sTabNormalStyleDefault);
}

void Toolbar::setTabsNormalStyle(const std::string & tabsNormalStyle){
	if(m_sTabNormalStyle != tabsNormalStyle){
		m_sTabNormalStyle = tabsNormalStyle;
		setTabsStyle(m_sTabNormalStyle, &Toolbar::isTabNormal);
	}
}

const std::string & Toolbar::getTabsNormalStyle() const{
	return m_sTabNormalStyle;
}

void Toolbar::setTabsActiveStyleDefault(){
	setTabsActiveStyle(m_sTabActiveStyleDefault);
}

void Toolbar::setTabsActiveStyle(const std::string & tabsActiveStyle){
	if(m_sTabActiveStyle != tabsActiveStyle){
		m_sTabActiveStyle = tabsActiveStyle;
		setTabsStyle(m_sTabActiveStyle, &Toolbar::isTabActive);
	}
}

const std::string & Toolbar::getTabsActiveStyle() const{
	return m_sTabActiveStyle;
}

void Toolbar::setTabsDisableStyleDefault(){
	setTabsDisableStyle(m_sTabDisableStyleDefault);
}

void Toolbar::setTabsDisableStyle(const std::string & tabsDisableStyle){
	if(m_sTabDisableStyle != tabsDisableStyle){
		m_sTabDisableStyle = tabsDisableStyle;
		setTabsStyle(m_sTabDisableStyle, &Toolbar::isTabDisable);
	}
}

const std::string & Toolbar::getTabsDisableStyle() const{
	return m_sTabDisableStyle;
}

void Toolbar::setToolbarUpperHalfStyleDefault(){
	setToolbarUpperHalfStyle(m_sToolbarUpperHalfStyleDefault);
}

void Toolbar::setToolbarUpperHalfStyle(const std::string & toolbarUpperHalfStyle){
	if(m_sToolbarUpperHalfStyle != toolbarUpperHalfStyle){
		m_sToolbarUpperHalfStyle = toolbarUpperHalfStyle;
		m_pTabsGrid.first->setStyle(m_sToolbarUpperHalfStyle);
		//m_pTabsGrid.second->setStyle(m_sToolbarUpperHalfStyle);
		if(this->getWindowManager() != 0 && this->getWindowManager()->getStyleManager() != 0){
			this->getWindowManager()->getStyleManager()->applyStyles(m_pTabsGrid.first);
			//this->getWindowManager()->getStyleManager()->applyStyles(m_pTabsGrid.second);
		}
	}
}

const std::string & Toolbar::getToolbarUpperHalfStyle() const{
	return m_sToolbarUpperHalfStyle;
}

void Toolbar::setToolbarLowerHalfStyleDefault(){
	setToolbarLowerHalfStyle(m_sToolbarLowerHalfStyleDefault);
}

void Toolbar::setToolbarLowerHalfStyle(const std::string & toolbarLowerHalfStyle){
	if(m_sToolbarLowerHalfStyle != toolbarLowerHalfStyle){
		m_sToolbarLowerHalfStyle = toolbarLowerHalfStyle;
		for(SUBTOOLBARS::iterator it = m_mSubToolbars.begin(); it != m_mSubToolbars.end(); ++it){
			it->second.first->setStyle(m_sToolbarLowerHalfStyle);
			//it->second.second->setStyle(m_sToolbarLowerHalfStyle);
			if(this->getWindowManager() != 0 && this->getWindowManager()->getStyleManager() != 0){
				this->getWindowManager()->getStyleManager()->applyStyles(it->second.first);
				//this->getWindowManager()->getStyleManager()->applyStyles(it->second.second);
			}
		}

		setStyle(getByRowCol(0,0),toolbarLowerHalfStyle);
	}
}

const std::string & Toolbar::getToolbarLowerHalfStyle() const{
	return m_sToolbarLowerHalfStyle;
}

bool Toolbar::tabPush(osgWidget::Event& ev){

	activateTab(ev.getWidget());
	return false;
}

void Toolbar::resetActiveTab(){
	if(m_pActiveTab != 0){
		//reset tab state
		m_vTabs[m_mRevTabs[m_pActiveTab]].second.active = false;
		std::string newStyle = m_sTabNormalStyle;
		if(isTabDisable(m_pActiveTab) == true){
			newStyle = m_sTabDisableStyle;
		}

		setStyle(m_pActiveTab, newStyle);

		this->removeWidget(this->getByRowCol(0,0));
		this->fillEmpty();
		setStyle(this->getByRowCol(0,0), m_sToolbarLowerHalfStyle);
		this->resetFillables();

		m_pActiveTab = 0;
	}
}

void Toolbar::setStyle(osgWidget::Widget * w, const std::string & style){
	if(w != 0 && w->getStyle() != style){
		w->setStyle(style);
		if(this->getWindowManager() != 0 && this->getWindowManager()->getStyleManager() != 0){
			this->getWindowManager()->getStyleManager()->applyStyles(w);
		}
	}
}

void Toolbar::setTabsStyle(const std::string & style, cond condition){
	if(condition == 0){
		//all
		for(TABS::iterator it = m_vTabs.begin(); it != m_vTabs.end(); ++it){
			setStyle(it->first, style);
		}
	}else{
		//selected
		for(TABS::iterator it = m_vTabs.begin(); it != m_vTabs.end(); ++it){
			if((this->*condition)(it->first) == true){
				setStyle(it->first, style);
			}
		}
	}
}

bool Toolbar::isTabNormal(osgWidget::Widget * tab) const{
	const TAB_DESC & tabDesc = m_vTabs[m_mRevTabs.find(tab)->second].second;
	if(tabDesc.enable == true && tabDesc.active == false){
		return true;
	}

	return false;
}

bool Toolbar::isTabDisable(osgWidget::Widget * tab) const{
	const TAB_DESC & tabDesc = m_vTabs[m_mRevTabs.find(tab)->second].second;
	if(tabDesc.enable == false && tabDesc.active == false){
		return true;
	}

	return false;
}

}
