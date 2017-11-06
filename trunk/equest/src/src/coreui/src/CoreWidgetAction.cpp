#include "CoreUiPCH.h"
#include <coreui/CoreWidgetAction.h>

using namespace coreUI;

CoreWidgetAction::CoreWidgetAction(QObject *parent, const QString & sectionName, CoreTitleBar::SideType side)
	: QWidgetAction(parent), sectionName_(sectionName), side_(side)
{

}

CoreWidgetAction::~CoreWidgetAction()
{

}

CoreTitleBar::SideType CoreWidgetAction::side() const
{
	return side_;
}

const QString CoreWidgetAction::section() const
{
	return sectionName_;
}

CoreCustomWidgetAction::CoreCustomWidgetAction(QObject *parent, WidgetCreator widgetCreator, const QString & sectionName, CoreTitleBar::SideType side)
	: QWidgetAction(parent), sectionName_(sectionName), side_(side), widgetCreator_(widgetCreator)
{
	if(widgetCreator == false){
		throw loglib::runtime_error("Empty widget creator");
	}
}

CoreCustomWidgetAction::~CoreCustomWidgetAction()
{

}

CoreTitleBar::SideType CoreCustomWidgetAction::side() const
{
	return side_;
}

const QString CoreCustomWidgetAction::section() const
{
	return sectionName_;
}

QWidget * CoreCustomWidgetAction::createWidget(QWidget * parent)
{
	return widgetCreator_(parent);
}

void CoreCustomWidgetAction::deleteWidget(QWidget * widget)
{
	delete widget;
}