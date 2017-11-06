/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   16:59
    filename: CoreWidgetAction.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COREWIDGETACTION_H__
#define HEADER_GUARD___COREWIDGETACTION_H__

#include <coreui/Export.h>
#include <QtWidgets/QWidgetAction>
#include <coreui/CoreTitleBar.h>
#include <coreui/ICoreActionSection.h>
#include <functional>

namespace coreUI {

class COREUI_EXPORT CoreWidgetAction : public QWidgetAction, public CoreTitleBar::ICoreTitleBarAction, public ICoreActionSection
{

	Q_OBJECT

public:
	explicit CoreWidgetAction(QObject *parent, const QString & sectionName, CoreTitleBar::SideType side = CoreTitleBar::Left);
	virtual ~CoreWidgetAction();

	virtual CoreTitleBar::SideType side() const;
	virtual const QString section() const;

private:
	CoreTitleBar::SideType side_;
	QString sectionName_;
};

class COREUI_EXPORT CoreCustomWidgetAction : public QWidgetAction, public CoreTitleBar::ICoreTitleBarAction, public ICoreActionSection
{
	Q_OBJECT

public:

	typedef std::function<QWidget*(QWidget*)> WidgetCreator;

public:
	explicit CoreCustomWidgetAction(QObject *parent, WidgetCreator widgetCreator, const QString & sectionName, CoreTitleBar::SideType side = CoreTitleBar::Left);
	virtual ~CoreCustomWidgetAction();

	virtual CoreTitleBar::SideType side() const;
	virtual const QString section() const;

protected:

	virtual QWidget * createWidget(QWidget * parent);		
	virtual void deleteWidget (QWidget * widget);

private:
	CoreTitleBar::SideType side_;
	QString sectionName_;
	WidgetCreator widgetCreator_;
};

}

#endif	//	HEADER_GUARD___COREWIDGETACTION_H__
