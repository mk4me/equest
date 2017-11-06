/********************************************************************
	created:	2013/06/20
	created:	20:6:2013   10:18
	filename: 	AnalysisTreeContext.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALYSISTREECONTEXT_H__
#define HEADER_GUARD_MDE_VIEW__ANALYSISTREECONTEXT_H__

#include <corelib/IHierarchyItem.h>
#include <coreui/CoreFlexiToolBarSection.h>
#include <coreui/HierarchyTreeModel.h>
#include <QtWidgets/QTabWidget>
#include "AnalisisModel.h"
#include <coreui/IAppUsageContext.h>

class AnalysisTreeContextMenu;

//! kontekst dla drzewa analiz
class AnalysisTreeContext : public QObject, public coreUI::IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget 
    //! \param hmm 
    AnalysisTreeContext(QTabWidget * flexiTabWidget, coreUI::HierarchyTreeModel* model, AnalysisTreeContextMenu* menuBuilder);
public:
    //! wywoływane, po aktywowaniu się kontekstu, do flexi bara trafią akcje związane z drzewem analiz
    //! \param contextWidget wiget, który wywołał kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywoływane, gdy kontekst przestaje być aktywny
    //! z flexibara znikają akcje związane z drzewem analiz
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja drzewa analiz
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie drzewa analiz
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

public Q_SLOTS:
    //! na podstawie aktualnego stanu drzewa odświeża flexiwidget
    void refresh();

private Q_SLOTS:
    //! zmienił się zaznaczony element drzewa analiz, należy zmienić flexi widget
    void onTreeItemActivated(const QModelIndex& idx);
        

private:
    //! odtworzenie flexi widgeta na podstawie wskazywanego elementu drzewa
    //! \param flexiSection modyfikowana sekcja
    //! \param helper aktualnie aktywny alement drzewa analiz
    void recreateFlexiSectionWidget(QWidget* flexiSection, core::IHierarchyItemConstPtr helper);

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja obsługiwanego flexi widgeta
    QWidget* flexiContent;

    QWidget * contextWidgetActions;

    coreUI::CoreFlexiToolBarSection * flexiSection;
    //! identyfikator grupy dla drzewa
    int groupID;

    coreUI::HierarchyTreeModel* treeModel;

    AnalysisTreeContextMenu* menuBuilder;
};
DEFINE_SMART_POINTERS(AnalysisTreeContext)
#endif
