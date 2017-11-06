/********************************************************************
	created:	2013/04/08
	created:	8:4:2013   20:20
	filename: 	AbstractTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__ABSTRACTTAB_H__
#define HEADER_GUARD_COREUI__ABSTRACTTAB_H__

#include <coreui/IContextRoot.h>
#include <coreui/IMdeTab.h>
#include <coreui/Export.h>

namespace coreUI {
//! Zak³adka g³ównego okna MDE, zapewnia podstawow¹ funkcjonalnoœæ.
//! Wymaga podania widgeta dla zak³adki oraz zarejstrowania kontekstu we flexi tabie
class COREUI_EXPORT AbstractTab : public QObject, public IContextRoot, public IMdeTab
{
    Q_OBJECT
public:
    AbstractTab(const QIcon& icon, const QString& label);
	virtual ~AbstractTab() {}

public:
    virtual void attachChildContext( coreUI::IAppUsageContextPtr child );
    virtual void setEnabled( bool enabled );
    virtual bool isEnabled() const;
    virtual QIcon getIcon() const;
    virtual QString getLabel() const;
    virtual bool isActive() const;
    virtual void setActive( bool val );

Q_SIGNALS:
    void tabEnabled(bool enabled);
    void tabActivated(bool activated);

private:
    QString label;
    QIcon icon;
    bool active;
    bool enabled;
};
DEFINE_SMART_POINTERS(AbstractTab);
}

#endif
