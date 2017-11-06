/********************************************************************
	created:  2011/03/10
	created:  10:3:2011   17:15
	filename: CoreVisualizerWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
#define HEADER_GUARD_CORE__VISUALIZERWIDGET_H__

#include <coreui/Export.h>
#include <map>
#include <corelib/Visualizer.h>
#include <QtWidgets/QScrollArea>

class QAction;
class QLabel;
class QComboBox;
class QCheckBox;
class QMenu;

namespace coreUI {

	class CoreWidgetAction;
	class CoreAction;

//! Widget wizualizacyjny.
class COREUI_EXPORT CoreVisualizerWidget : public QScrollArea
{
    Q_OBJECT

private:
	//! Wybór aktywnej serii
	QComboBox * persistentActiveSerieSwitch;

	//! Wybór aktywnej serii
	std::list<QComboBox*> activeSerieSwitches;
	//! Akcja czyszcząca wszystkie dane aktualnego wizualizatora
	QAction * dataDeselectAll;
	//! Zbiór typów danych wspieranych przez wizualizator
	utils::TypeInfoSet supportedDataTypes;
    //! Bieżący wizualizator.
    core::VisualizerPtr visualizer_;
	//! Użyte indeksy dla danych bez opisu
	std::set<int> usedLocalNameIndexes;
	//! Mapa danych aktywnych - dane do serii wg typów
	std::map<utils::TypeInfo, std::map<core::VariantConstPtr, core::Visualizer::Serie*>> activeData;
	std::map<core::Visualizer::Serie*, int> serieLocalIdx;

public:
    //! Zerujący konstruktor.
    //! \param parent
    //! \param flags
    CoreVisualizerWidget(core::VisualizerPtr visualizer, QWidget* parent = nullptr);        

	CoreVisualizerWidget(const CoreVisualizerWidget & visualizer);  
            
    //! Zapewnia możliwość kasowanie widgeta wizualizatora przez jego implementację.
    ~CoreVisualizerWidget();

    //! \return Bieżący wizualizator.
    core::VisualizerPtr getVisualizer();

private:

	QWidget * createActiveSerieSwitch(QWidget * parent);

	QWidget * createSourceMenuWidget(QWidget * parent);

	void refreshActiveSerieSwitchSettings(QComboBox * activeSerieSwitch);

	void refreshActiveSerieSwitchContent(QComboBox * activeSerieSwitch);

	void refreshActiveSerieSwitch(QComboBox * activeSerieSwitch);

	void tryRefreshActiveSerieSwitchesSettings();

	void tryRefreshActiveSerieSwitchesContent();

	void tryRefreshActiveSerieSwitches();

	//! Generuje kolejny wolny lokalny identyfikator - id nie zmiania się przez cały czas dostępności danych
	//! \param startIdx Startowy identyfikator od którego zaczynamy szukać kolejnego wolnego - w szczególności będzie to on sam
	//! \return Kolejny wolny lokalny identyfikator dla danych bez nazwy
	const int nextValidLocalSerieIdx(int startIdx) const;
	//! \param data Dane dla których poszukujemy nazwy
	//! \param dataName [out] Nazwa danych
	//! \return Czy udało się pobrać nazwę dla danych
	static const bool getDataName(core::VariantConstPtr data, std::string & dataName);
	//! \param data Dane dla których poszukujemy źródła
	//! \param dataSource [out] Nazwa źródła
	//! \return Czy udało się pobrać źródło dla danych
	static const bool getDataSource(core::VariantConstPtr data, std::string & dataSource);
        
private slots:

	void activeSerieSwitchDestroyed(QObject * activeSerieSwitch);

	//! Usuwa wszystkie stworzone serie danych
    void removeAllSeries();  
	//! Wołane przez akcje w momencie użycia danych
	void addSerie();
	//! Wołane przez akcje w momencie usuwania danych
	void removeSerie();
    
	//! Wypełnia menu danych do wyboru
    void fillSourcesMenu();

	//! Czyści menu danych - potencjalnie zwalnia nieużywane dane
	void clearSourcesMenu();

    //! Zmiana aktywnej serii
    void serieSelected(int idx);

	//! Czy na bieżąco obserwować zmiany i aktualizować
	void onLiveObserveChange(bool observe);
};

}

#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
