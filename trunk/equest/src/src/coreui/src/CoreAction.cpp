#include "CoreUiPCH.h"
#include <coreui/CoreAction.h>

namespace coreUI
{

CoreAction::CoreAction(const QString & sectionName, const QString &text, QObject* parent, CoreTitleBar::SideType side)
	: QAction(text, parent), side_(side), sectionName_(sectionName)
{

}

CoreAction::CoreAction(const QString & sectionName, const QIcon &icon, const QString &text, QObject* parent, CoreTitleBar::SideType side)
	: QAction(icon, text, parent), side_(side), sectionName_(sectionName)
{

}

CoreAction::~CoreAction()
{

}

CoreTitleBar::SideType CoreAction::side() const
{
	return side_;
}

const QString CoreAction::section() const
{
	return sectionName_;
}

}