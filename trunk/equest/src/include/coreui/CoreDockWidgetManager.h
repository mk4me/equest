/********************************************************************
	created:	2011/09/20
	created:	20:9:2011   8:37
	filename: 	CoreDockWidgetManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__DOCKWIDGETMANAGER_H__
#define HEADER_GUARD_CORE__DOCKWIDGETMANAGER_H__

#include <coreui/Export.h>
#include <boost/range.hpp>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>

class QTabWidget;
class QDockWidget;

namespace coreUI {

	class CoreDockWidgetSet;

//! Klasa zarządza calymi grupami dock widgetów, laczac je w sety
class COREUI_EXPORT CoreDockWidgetManager : public QWidget
{
    Q_OBJECT;
public:
    //! niemodyfikowalny zakres wszystkich podległych grup zakładek aplikacji
    typedef boost::iterator_range<std::list<CoreDockWidgetSet*>::iterator> range;
    //! zakres wszystkich podległych grup zakładek aplikacji
    typedef boost::iterator_range<std::list<CoreDockWidgetSet*>::const_iterator> const_range;

private:

	void removeTabsDistance();

public:
	//! typowy konstruktor Qt 
	//! \param parent 
	//! \param flags 
	explicit CoreDockWidgetManager(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~CoreDockWidgetManager();

	//! Dodaj grupę dock widgetów
	//! \param set dodawana grupa
	int addDockWidgetSet(CoreDockWidgetSet* set, const QString & label);
	//! Dodaj grupę dock widgetów
	//! \param set dodawana grupa
	int addDockWidgetSet(CoreDockWidgetSet* set, const QIcon & icon, const QString & label);

	//! Grupa dock widgetów stanie się aktywna (widoczna)
    
	//! Dodaje widget do grupy, która ma na niego miejsce, jeśli takiej nie ma to tworzona jest nowa
	//! \param widget dodawany dock widget
	CoreDockWidgetSet* autoAddDockWidget(QDockWidget* widget, const QString & label);
    //! Dodaje widget do grupy, która ma na niego miejsce, jeśli takiej nie ma to tworzona jest nowa
    //! \param widget dodawany dock widget
    CoreDockWidgetSet* autoAddDockWidget(QDockWidget* widget, const QIcon & icon, const QString & label);
    //! \return const range z wszystkimi grupami dock widgetów
    const_range getDockSet() const;
    //! \return range z wszystkimi grupami dock widgetów
    range getDockSet();
    //! Na podstawie dock widgeta pobierana jest grupa, do której należy
    //! \param widget badany dock widget
    //! \return odnaleziona grupa lub nullptr
    CoreDockWidgetSet* tryGetDockSet(QDockWidget* widget);


    int getMaxWidgetsInSetHint() const;
    void setMaxWidgetsInSetHint(int val);

	int count() const;
	int	currentIndex() const;
	CoreDockWidgetSet * currentSet() const;
	void clear();

	int indexOf(CoreDockWidgetSet* set) const;
	CoreDockWidgetSet* set(int index) const;

	QWidget * cornerWidget(Qt::Corner corner = Qt::TopRightCorner) const;
	bool documentMode() const;
	Qt::TextElideMode elideMode() const;
	QSize iconSize() const;
	int insertSet(int index, CoreDockWidgetSet* set, const QString & label);
	int insertSet(int index, CoreDockWidgetSet* set, const QIcon & icon, const QString & label);
	bool isMovable() const;
	bool isSetEnabled(int index) const;
	void removeSet(int index);
	void setCornerWidget(QWidget * widget, Qt::Corner corner = Qt::TopRightCorner);
	void setDocumentMode(bool set);
	void setElideMode(Qt::TextElideMode elideMode);
	void setIconSize(const QSize & size);
	void setMovable(bool movable);
	void setSetEnabled(int index, bool enable);
	void setSetIcon(int index, const QIcon & icon);
	void setTabPosition(QTabWidget::TabPosition tabPosition);
	void setTabShape(QTabWidget::TabShape s);
	void setSetText(int index, const QString & label);
	void setSetToolTip(int index, const QString & tip);
	void setSetWhatsThis(int index, const QString & text);
	void setSetsClosable(bool closeable);
	void setUsesScrollButtons(bool useButtons);
	QIcon setIcon(int index) const;
	QTabWidget::TabPosition tabPosition() const;
	QTabWidget::TabShape tabShape() const;
	QString setText(int index) const;
	QString setToolTip(int index) const;
	QString setWhatsThis(int index) const;
	bool setsClosable() const;
	bool usesScrollButtons() const;
	void setAutoCloseEmptySets(const bool val);
	const bool autoCloseEmptySets() const;

Q_SIGNALS:
    //! stan dock widgetów zmienił się
	void currentSetChanged(int index);
	void setCloseRequested(int index);

public Q_SLOTS:
	//! \param idx 
	void setCurrentSet(int idx);
	//! \param set
	void setCurrentSet(CoreDockWidgetSet * set);

private Q_SLOTS:

	void onTabWidgetChange(int idx);
	void onTabCloseRequest(int idx);
	void onWidgetSetChange(unsigned int number);

private:

	void innerAddDockWidgetSet( CoreDockWidgetSet* set,
		const int idx);

private:
	QTabWidget * tabWidget;
    //! lista wszystkich grup managera
	std::list<CoreDockWidgetSet*> dockList;
	//! lista prywatnych grup managera
	std::list<CoreDockWidgetSet*> privateDockList;
    //! maksymalna liczba widgetów w tworzonych setach
    int maxWidgetsInSetHint;
	//! Czy automatycznie zamykać puste grupy?
	bool autoCloseEmptySets_;
};

}

#endif