/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   16:51
    filename: CoreAction.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COREACTION_H__
#define HEADER_GUARD___COREACTION_H__

#include <coreui/Export.h>
#include <QtWidgets/QAction>
#include <coreui/CoreTitleBar.h>
#include <coreui/ICoreActionSection.h>

namespace coreUI {

class COREUI_EXPORT CoreAction : public QAction, public CoreTitleBar::ICoreTitleBarAction, public ICoreActionSection
{
	Q_OBJECT
public:
	CoreAction(const QString & sectionName, const QString &text, QObject* parent, CoreTitleBar::SideType side = CoreTitleBar::Left);
	CoreAction(const QString & sectionName, const QIcon &icon, const QString &text, QObject* parent, CoreTitleBar::SideType side = CoreTitleBar::Left);
	virtual ~CoreAction();

	virtual CoreTitleBar::SideType side() const;
	virtual const QString section() const;

private:
	CoreTitleBar::SideType side_;
	QString sectionName_;
};

}

#endif	//	HEADER_GUARD___COREACTION_H__
