/********************************************************************
	created:	2013/04/08
	created:	8:4:2013   19:46
	filename: 	IContextRoot.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__ICONTEXTROOT_H__
#define HEADER_GUARD_COREUI__ICONTEXTROOT_H__

#include <QtWidgets/QTabWidget>
#include <coreui/IAppUsageContext.h>

namespace coreUI {

//! Kontekst bazowy dla kontekstów aplikacji medycznej
class MdeEmptyContext : public coreUI::IAppUsageContext
{
public:
    MdeEmptyContext() {}
    virtual ~MdeEmptyContext() {}

public:
    //! wywo³ywane, po aktywowaniu siê kontekstu
    //! \param contextWidget wiget, który wywo³a³ kontekst
    virtual void activateContext(QWidget * contextWidget) {}
    //! wywo³ywane, gdy kontekst przestaje byæ aktywny
    //! \param nextContextWidget 
    //! \param refresh 
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh) {}
};
DEFINE_SMART_POINTERS(MdeEmptyContext);


class IContextRoot
{
public:
	virtual ~IContextRoot() {}
    virtual void registerConxtext(coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget* flexiWidget ) = 0;
};
DEFINE_SMART_POINTERS(IContextRoot);

}
#endif
