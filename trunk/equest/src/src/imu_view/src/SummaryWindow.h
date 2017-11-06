/********************************************************************
	created:	2012/05/04
	created:	4:5:2012   13:43
	filename: 	SummaryWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__SUMMARYWINDOW_H__
#define HEADER_GUARD_HMM__SUMMARYWINDOW_H__

#include <plugins/subject/ISubject.h>
#include <corelib/HierarchyHelper.h>
#include <QtWidgets/QTreeWidgetItem>
#include "AnalisisModel.h"

namespace coreUI {
	class CoreVisualizerWidget;
}

//! okno służy do wyświetlenia podsumowania i dodatkowych informacji dla wizualizatora lub elementu drzewa danych
class SummaryWindow
{
public:
    //! konstruktor
    //! \param hmm dostarcza drzewa analiz, przechowuje okienko z podsumowaniami
    SummaryWindow(QWidget* placeholder): placeholder(placeholder), tree(nullptr) {}
	//! destruktor
	virtual ~SummaryWindow() {}

public:
    //! kolekcja z obiektami pomocniczymi dla elementów drzewa analiz
    typedef std::list<core::IHierarchyItemConstWeakPtr> HelpersCollection;

public:
    //! inicjalizuje widget z podsumowaniami
    void initialize();
    //! wyświetla konkretne informacje w okienku podsumowań
    //! \param helpers kolekcja z obiektami pomocniczymi dla elementów drzewa analiz, dostarczają one informacje do wyświetlenia
    void display(const HelpersCollection& helpers);
    void display(const core::IHierarchyItemConstWeakPtr& helper);
    void clear();

private:
    //! dodaje element z informacjami
    //! \param text tekst do wyświetlenia
    //! \param root korzeń, do którego zostanie dodany element
    void addItem( QString text, QTreeWidgetItem* root );

private:
    //! drzewo, które będzie wyświetlane w oknie z podsumowaniami
    QTreeWidget* tree;
    //! tutaj trafi okno z podsumowaniami
    QWidget* placeholder;
};
typedef utils::shared_ptr<SummaryWindow> SummaryWindowPtr;
typedef utils::shared_ptr<const SummaryWindow> SummaryWindowConstPtr;

//! Kontroler dla okienka z podsumowaniem
class SummaryWindowController : public QObject
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param sw obiekt z oknem podsumowań
    //! \param hmm główne okno aplikacji
    SummaryWindowController(SummaryWindowPtr sw, AnalisisModelPtr analysisModel);

public Q_SLOTS:
    //! pobiera dane z elementu i przesyła je do okna podsumowań
    //! \param item elmenet drzewa analiz
    //! \param column zaznaczona kolumna, ignorowane
    //void onTreeItemSelected(QTreeWidgetItem* item, int column);
    //! pobiera dane na podstawie elementów zawartych w wizualizatorze i przesyła je do okna podsumowań
    //! \param visualizatorWidget wizualizator, który uzyskał focus
    void onVisualizer(coreUI::CoreVisualizerWidget* visualizatorWidget);

private:
    //! obiekt z oknem podsumowań
    SummaryWindowPtr summary;
    //! model zakładki analiz
    AnalisisModelPtr analysisModel;
};



#endif
