/********************************************************************
	created:	2012/01/18
	created:	18:1:2012   11:35
	filename: 	HmmContexts.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_MDE_MDECONTEXTS_H__
#define HEADER_GUARD_MDE_MDECONTEXTS_H__

#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTreeWidget>
#include <boost/tuple/tuple.hpp>
#include <coreui/IAppUsageContext.h>
#include <coreui/CoreFlexiToolBar.h>

//! Kontekst bazowy dla kontekstów aplikacji medycznej
class MdeEmptyContext : public coreUI::IAppUsageContext
{
public:
	MdeEmptyContext() {}
	virtual ~MdeEmptyContext() {}

public:
	//! wywoływane, po aktywowaniu się kontekstu
	//! \param contextWidget wiget, który wywołał kontekst
	virtual void activateContext(QWidget * contextWidget) {}
	//! wywoływane, gdy kontekst przestaje być aktywny
	//! \param nextContextWidget 
	//! \param refresh 
	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh) {}
};

//! Placeholder dla kontekstu danych
class MdeDataContext : public MdeEmptyContext
{

};

//! placeholder dla kontekstu raportów
class MdeReportContext : public MdeEmptyContext
{

};



#endif
