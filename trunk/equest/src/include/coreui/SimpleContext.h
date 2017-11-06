/********************************************************************
	created:	2014/02/25
	created:	25:2:2014   11:09
	filename: 	SimpleContext.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__SIMPLECONTEXT_H__
#define HEADER_GUARD_COREUI__SIMPLECONTEXT_H__

#include <coreui/IAppUsageContext.h>
#include <coreui/CoreFlexiToolBar.h>

class QTabWidget;

namespace coreUI {

    //! uniwersalny kontekst, sam rozmieszcza akcje
    class COREUI_EXPORT SimpleContext : public IAppUsageContext
    {
    public:
        //! konstruktor
        //! \param flexiTabWidget flexi widget kontekstu
        SimpleContext(QTabWidget * flexiTabWidget, const QString& name, const QStringList& sectionsToSkip = QStringList() );

    public:
        //! wywo³ywane, po aktywowaniu siê kontekstu, do flexi bara trafi¹ akcje 
        //! \param contextWidget wiget, który wywo³a³ kontekst
        virtual void activateContext(QWidget * contextWidget);
        //! wywo³ywane, gdy kontekst przestaje byæ aktywny
        //! z flexibara znikaj¹ akcje 
        virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
        //! rejestracja kontekstu
        virtual void onRegisterContextWidget(QWidget * contextWidget);
        //! wyrejstrowanie kontekstu
        virtual void onUnregisterContextWidget(QWidget * contextWidget);


    private:
        //! flexi widget
        QTabWidget * flexiTabWidget;

        coreUI::CoreFlexiToolBar * widget;
        //! identyfikator grupy dla miniatur raportów
        int groupID;
        //! nazwa dla glownej zak³adki flexi bara
        QString name;
        QList<QString> sectionsToSkip;
    };
    DEFINE_SMART_POINTERS(SimpleContext);

}
#endif
