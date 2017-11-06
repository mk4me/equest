/********************************************************************
	created:	2013/04/15
	created:	15:4:2013   9:06
	filename: 	ReportsTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__REPORTSTAB_H__
#define HEADER_GUARD_COREUI__REPORTSTAB_H__

#include "AbstractTab.h"
#include <coreui/CoreFlexiToolBar.h>
#include <boost/tuple/tuple.hpp>
#include <QtWidgets/QTabWidget>
#include <coreui/Export.h>

namespace coreUI {
//! kontekst dla zak³adki z raportami
class COREUI_EXPORT ReportsTabContext : public QObject, public coreUI::IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm g³ówne okno aplikacji
    ReportsTabContext(QTabWidget * flexiTabWidget);

public:
    //! wywo³ywane, po aktywowaniu siê kontekstu, do flexi bara trafi¹ akcje zwi¹zane z oknem raportów
    //! \param contextWidget wiget, który wywo³a³ kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywo³ywane, gdy kontekst przestaje byæ aktywny
    //! z flexibara znikaj¹ akcje zwi¹zane z oknem raportów
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja okna raportów
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie okna raportów
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:
    //! tworzy zbiorczy widget, który jest kontenerem dla dwóch wierszy widgetów potomnych
    //! \return krotka z utworzonym widgetem oraz layoutami dla wiersza górnego i dolnego
    boost::tuple<QWidget*, QLayout*, QLayout*> createTwoLineWidget();
    //! automatycznie rozmieszcza widgety w dwóch wierszach
    //! \param editList lista z akcjami i obiektami
    //! \param lowerLayout dolny wiersz
    //! \param upperLayout górny wiersz
    //! \param actionsOnTop czy akcje powinny byæ w górnym wierszu, czy bez znaczenia
    void placeObjects( const QList<QObject*> &editList, QLayout* lowerLayout, QLayout* upperLayout, bool actionsOnTop = false );

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja z akcjami zwi¹zanymi z edycja dokumentu
    QWidget* editSection;
    //! sekcja z akcjami zwi¹zanymi z uk³adem tekstu dokumentu
    QWidget* textSection;
    //! sekcja z akcjami zwi¹zanymi z zapisem, odczytem dokumentu
    QWidget* fileSection;

    coreUI::CoreFlexiToolBar * widget;
    //! identyfikator grupy dla miniatur raportów
    int groupID;
};
typedef utils::shared_ptr<ReportsTabContext> ReportsTabContextPtr;
typedef utils::shared_ptr<const ReportsTabContext> ReportsTabContextConstPtr;


class COREUI_EXPORT ReportsTab : public AbstractTab
{
    Q_OBJECT
public:
    ReportsTab(const QIcon& icon, const QString& label);
	virtual ~ReportsTab() {}

public:
    virtual QWidget* getMainWidget();
    virtual void registerConxtext( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget * flexiTabWidget ) ;

public Q_SLOTS:
    void enable();

private:
    QWidget* widget;
    ReportsTabContextPtr context;
};
DEFINE_SMART_POINTERS(ReportsTab);
}

#endif
