#ifndef HEADER_GUARD__TOOLTIP_H__
#define HEADER_GUARD__TOOLTIP_H__

#include <osgWidget/Box>
#include <osgWidget/Label>
#include <osgui/Borderized.h>
#include <set>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class osgWidget::Label;

class Tooltip :
	public osgWidget::Box
{
public:
	Tooltip(void);
	~Tooltip(void);

	virtual void managed(osgWidget::WindowManager * wm);
    virtual void update();

	void setText(const std::string & text);
	std::string getText() const;

	bool isEmpty() const;

	void clear();

    void show();
	void show(const osgWidget::XYCoord & pos);
	void show(osgWidget::point_type x, osgWidget::point_type y);
    void hide();

	void registerItem(osgWidget::Widget * widget);
	void registerItem(osgWidget::Window * window);

	void unregisterItem(osgWidget::Widget * widget);
	void unregisterItem(osgWidget::Window * window);

private:
	bool onEnterWidget(osgWidget::Event& ev);
	bool onEnterWindow(osgWidget::Event& ev);
	bool onLeaveWidget(osgWidget::Event& ev);
	bool onLeaveWindow(osgWidget::Event& ev);

	bool onDragWidget(osgWidget::Event& ev);
	bool onDragWindow(osgWidget::Event& ev);

protected:

//typedef osgui::Borderized<osgWidget::Label, osg::PrimitiveSet::LINE_STRIP> BLABEL;
	typedef osgui::Borderized<osgWidget::Label> BLABEL;

protected:

	std::set<osg::observer_ptr<osgWidget::Window> > registeredWindows;
	std::set<osg::observer_ptr<osgWidget::Widget> > registeredWidgets;

private:
	osg::ref_ptr<osgWidget::Callback> onEnterWindowCallback;
	osg::ref_ptr<osgWidget::Callback> onEnterWidgetCallback;
	osg::ref_ptr<osgWidget::Callback> onLeaveWidgetCallback;
	osg::ref_ptr<osgWidget::Callback> onLeaveWindowCallback;

	osg::ref_ptr<osgWidget::Callback> onDragWidgetCallback;
	osg::ref_ptr<osgWidget::Callback> onDragWindowCallback;

	osgWidget::Widget * currentWidget;
	osgWidget::Window * currentWindow;

	osg::ref_ptr<BLABEL> tooltipLabel;
    std::string labelText;
};

}

#endif
