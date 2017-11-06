/********************************************************************
	created:	2011/10/05
	created:	5:10:2011   10:22
	filename: 	FilterEntryWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__FILTERENTRYWIDGET_H__
#define HEADER_GUARD_HMM__FILTERENTRYWIDGET_H__

#include <functional>
#include <QtWidgets/QWidget>
#include <osg/Vec3>
#include <corelib/IFilterCommand.h>
#include <QtGui/QIcon>
//#include "FilterCommand.h"
////#include "ui_filterEntry.h"

namespace Ui {
    class FilterEntry;
}

//! widget z widokiem dla filtru
class FilterEntryWidget : public QWidget
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param parent obiekt rodzica
    //! \param hmm główne okno aplikacji, dostarcza drzewa
    //! \param bigLabelText tekst z dużą etykietą
    //! \param filterCommand akcja dokonująca właściwego filtrowania
    //! \param icon ikona reprezentująca filtr
    FilterEntryWidget(QWidget* parent, const QString& bigLabelText, core::IFilterCommandPtr filterCommand, const QIcon& icon = QIcon());
	//! destruktor
	virtual ~FilterEntryWidget();

public:
    //! \return akcja dokonująca właściwego filtrowania
    core::IFilterCommandPtr getFilterCommand() const { return filterCommand; }

    //! wyzwala akcję filtrującą i tworzy element drzewa
    //! \return gotowy element drzewa, z przefiltrowanymi danymi
    //QTreeWidgetItem* createTreeEntry(const core::ConstObjectsList& sessions);

    //! \return konfigurator filtru lub nullptr jeśli takiego nie dostarczono
    QWidget* getConfigurator() const { return filterCommand->getConfigurationWidget(); }
    //! \return nazwa filtru
    QString getName() const;
    //! \return czy filtr jest aktywny?
    bool isChecked() const;
    //! ustawia aktywność filtru
    //! \param val aktywuje / deaktuwuje filtr
    void setChecked(bool val);

Q_SIGNALS:
    //! sygnał informujący, że filtr został kliknięty
    //! \param filter kliknięty filtr
    void onFilterClicked(core::IFilterCommandPtr filter);

private Q_SLOTS:
    //! wywoływane po wciśnięciu przycisku filtra
    void onButton();

private:
    //! akcja dokonująca właściwego filtrowania
    core::IFilterCommandPtr filterCommand;
    //! obiekt z konfiguratorem dla filtra
    QWidget* configurator;
    Ui::FilterEntry* ui;
};


typedef utils::shared_ptr<FilterEntryWidget> FilterEntryWidgetPtr;
typedef utils::shared_ptr<const FilterEntryWidget> FilterEntryWidgetConstPtr;


#endif
