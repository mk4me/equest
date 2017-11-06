/********************************************************************
	created:	2013/04/08
	created:	8:4:2013   20:25
	filename: 	IMdeTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__IMDETAB_H__
#define HEADER_GUARD_COREUI__IMDETAB_H__

#include <QtGui/QIcon>
#include <QtCore/QString>

#include <utils/SmartPtr.h>

class QWidget;

namespace coreUI {

//! interfejs dla zak³adki g³ównego okna MDE
class IMdeTab
{
public:
	virtual ~IMdeTab() {}

public:
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual QIcon getIcon() const = 0;
    virtual QString getLabel() const = 0;   
    virtual bool isActive() const = 0;   
    virtual void setActive(bool val) = 0; 
};
DEFINE_SMART_POINTERS(IMdeTab);

}

#endif
