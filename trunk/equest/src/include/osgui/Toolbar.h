#ifndef HEADER_GUARD__TOOLBAR_H__
#define HEADER_GUARD__TOOLBAR_H__

#include <osgui/Grid.h>
#include <osgui/AspectRatioKeeper.h>
#include <osgui/EmbeddedWindow.h>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class Toolbar :
	public osgui::Grid
{
public:

	typedef std::vector<osgWidget::Widget *> ELEMENTS;

	typedef struct{
		bool active;
		bool enable;
		bool visible;
		ELEMENTS tabElements;
	}TAB_DESC;

	typedef std::pair<osgWidget::Widget*, TAB_DESC> TAB;
	typedef std::vector<TAB> TABS;

protected:
	typedef std::map<osgWidget::Widget*, int> REV_TABS_MAP;

private:

	typedef std::pair<osg::ref_ptr<osgui::Grid>, osg::ref_ptr<osgui::Embedded<osgui::Grid> > > SUBTOOLBAR;
	typedef std::map<osgWidget::Widget*, SUBTOOLBAR> SUBTOOLBARS;
	typedef bool (Toolbar::*cond)(osgWidget::Widget * w) const;

public:
	Toolbar(const std::string & name = "");
	~Toolbar(void);

	virtual void managed(osgWidget::WindowManager* wm);

	bool addTab(osgWidget::Widget * tab, bool enable = true, bool visible = true, bool active = false);
	bool addTab(osgWidget::Widget * tab, int tabIndex, bool enable = true, bool visible = true, bool active = false);

	bool removeTab(osgWidget::Widget * tab);
	bool removeTab(int tabIndex);

	bool showTab(osgWidget::Widget * tab);
	bool showTab(int tabIndex);

	bool hideTab(osgWidget::Widget * tab);
	bool hideTab(int tabIndex);

	bool enableTab(osgWidget::Widget * tab, bool enable);
	bool enableTab(int tabIndex, bool enable);

	bool activateTab(osgWidget::Widget * tab);
	bool activateTab(int tabIndex);

	const TABS & getTabs() const;

	int getActiveTabIdx() const;
	osgWidget::Widget * getActiveTab() const;

	osgWidget::Widget * getTabByIdx(int tabIndex) const;
	int getTabIdx(osgWidget::Widget * tab) const;

	unsigned int tabsCount() const;

	bool isTabVisible(osgWidget::Widget * tab) const;
	bool isTabVisible(int tabIndex) const;

	bool isTabEnable(osgWidget::Widget * tab) const;
	bool isTabEnable(int tabIndex) const;

	bool isTabActive(osgWidget::Widget * tab) const;
	bool isTabActive(int tabIndex) const;

	bool addTabElement(osgWidget::Widget * tab, osgWidget::Widget * elem);
	bool addTabElement(int tabIndex, osgWidget::Widget * elem);
	bool addTabElement(osgWidget::Widget * tab, osgWidget::Widget * elem, int elemIndex);
	bool addTabElement(int tabIndex, osgWidget::Widget * elem, int elemIndex);

	bool removeTabElement(osgWidget::Widget * tab, osgWidget::Widget * elem);
	bool removeTabElement(osgWidget::Widget * tab, int elemIndex);
	bool removeTabElement(int tabIndex, osgWidget::Widget * elem);
	bool removeTabElement(int tabIndex, int elemIndex);

	const ELEMENTS & getTabElements(int tabIndex) const;
	const ELEMENTS & getTabElements(osgWidget::Widget * tab) const;

	void setTabsNormalStyleDefault();
	void setTabsNormalStyle(const std::string & tabsNormalStyle);
	const std::string & getTabsNormalStyle() const;

	void setTabsActiveStyleDefault();
	void setTabsActiveStyle(const std::string & tabsActiveStyle);
	const std::string & getTabsActiveStyle() const;

	void setTabsDisableStyleDefault();
	void setTabsDisableStyle(const std::string & tabsDisableStyle);
	const std::string & getTabsDisableStyle() const;

	void setToolbarUpperHalfStyleDefault();
	void setToolbarUpperHalfStyle(const std::string & toolbarUpperHalfStyle);
	const std::string & getToolbarUpperHalfStyle() const;

	void setToolbarLowerHalfStyleDefault();
	void setToolbarLowerHalfStyle(const std::string & toolbarLowerHalfStyle);
	const std::string & getToolbarLowerHalfStyle() const;

protected:

	bool tabPush(osgWidget::Event& ev);
	void resetActiveTab();
	void setStyle(osgWidget::Widget * w, const std::string & style);

private:
	void setTabsStyle(const std::string & style, cond condition);
	bool isTabNormal(osgWidget::Widget * tab) const;
	bool isTabDisable(osgWidget::Widget * tab) const;

protected:

	TABS m_vTabs;
	REV_TABS_MAP m_mRevTabs;
	osgWidget::Widget * m_pActiveTab;

	osg::ref_ptr<osgWidget::Widget> m_pNullWidth;

	const ELEMENTS m_vEmptyElements;



private:

	SUBTOOLBAR m_pTabsGrid;
	SUBTOOLBARS m_mSubToolbars;
	osg::ref_ptr<osgWidget::Callback> m_pTabPushEvent;

	std::string m_sTabNormalStyleDefault;
	std::string m_sTabActiveStyleDefault;
	std::string m_sTabDisableStyleDefault;

	std::string m_sToolbarUpperHalfStyleDefault;
	std::string m_sToolbarLowerHalfStyleDefault;

	std::string m_sTabNormalStyle;
	std::string m_sTabActiveStyle;
	std::string m_sTabDisableStyle;

	std::string m_sToolbarUpperHalfStyle;
	std::string m_sToolbarLowerHalfStyle;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui 
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TOOLBAR_H__
