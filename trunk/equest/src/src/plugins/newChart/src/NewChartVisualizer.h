/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   15:12
	filename: 	ChartVisualizer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__CHARTVISUALIZER_H__
#define HEADER_GUARD_NEW_CHART__CHARTVISUALIZER_H__

#include <QtCore/QObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_item.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <corelib/IVisualizer.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include "INewChartSeriePrivate.h"
#include <vector>
#include <functional>

#include "NewChartSerie.h"
#include "NewChartState.h"
#include "NewChartPicker.h"
#include "NewChartEvents.h"
#include "corelib/IHierarchyProvider.h"

class StatsTable;
class NewChartLegend;
class PercentScaleDraw;
class NewChartLegendItem;
class NewChartVisualizer;
class GeneratorHelper : public QObject
{
	Q_OBJECT
public:
	GeneratorHelper(NewChartVisualizer* v);
	QList<QAction*> getActions();
private:
	std::map<INewChartCurveGeneratorConstPtr, std::vector<std::function<INewChartCurveGenerator::Param (void)>>> generatorParameterCreator;
	std::map<INewChartCurveGeneratorConstPtr, std::set<INewChartSeriePrivate*>> generator2Generated;
	NewChartVisualizer* visualizer;

private:
	INewChartCurveGenerator::Params getCurrentParams(INewChartCurveGeneratorConstPtr g);
};

//! Wizualizator wykresów, oparty o QWT
//! \version 0.9.1
class NewChartVisualizer : public QObject, public INewChartVisualizer, public core::IHierarchyProvider
{
	friend class GeneratorHelper;
    friend class NewChartSerie;
    friend class NewChartStreamSerie;
    Q_OBJECT;
    UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}");
	CLASS_DESCRIPTION("New Chart Visualizer", "New Chart Visualizer");

public:
    NewChartVisualizer();
	virtual ~NewChartVisualizer();

public:
      //! \return wykres QWT, który jest sercem wizualizatora
      QwtPlot* getPlot();
      //! \return aktualnie aktywna seria lub nullptr, jeśli takiej nie ma
	  const INewChartSeriePrivate* tryGetCurrentSerie() const;
      //! \return aktualnie aktywna seria lub nullptr, jeśli takiej nie ma
      INewChartSeriePrivate* tryGetCurrentSerie();
      //! \return pusty obiekt wizualizatora
      virtual IVisualizer* create() const;

      //! Tworzy obiekt typu NewChartSerie
      //! \param data ObjectWrapper z interfejsem do odczytu DataChannela
      //! \param name nazwa tworzonej serii danych
      //! \return utworzona seria
      plugin::IVisualizer::ISerie *createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr& data);
      // \return metoda nie jest obsługiwana, nullptr
      plugin::IVisualizer::ISerie *createSerie(const plugin::IVisualizer::ISerie * serie);

	  plugin::IVisualizer::ISerie *createSerie(const plugin::IVisualizer::ISerie * serie,
		  const utils::TypeInfo & requestedType, const core::VariantConstPtr& data);
      //! Usuwa serie z wizualizatora
      //! \param serie seria do usunięcia, musi należeć do wizualizatora i musi być przez niego stworzona
      virtual void removeSerie(plugin::IVisualizer::ISerie *serie);

	  virtual void setActiveSerie(plugin::IVisualizer::ISerie * serie);
	  virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;
	  virtual plugin::IVisualizer::ISerie * getActiveSerie();
      //! Tworzy główny widget wizualizatora
      //! \param manager Manager z akcjami do flexi bara
      //! \return utworzony widget
      virtual QWidget* createWidget();
      //! \return ikona wizualizatora
      virtual QIcon* createIcon();
      //! \return makymalna liczba serii, -1 bo nie ma limitu
      virtual int getMaxDataSeries() const;
      //! Zwraca info o wspieranych typach (tylko ScalarChannelReaderInterface)
      //! \param info zwracana struktura z opisami
      virtual void getSupportedTypes(utils::TypeInfoList & supportedTypes) const;
      //! Metoda wywoływana cyklicznie pozwala odświeżyć stan wizualizatora
      //! \param deltaTime czas od ostatniego wywołania metody
      virtual void update(double deltaTime);
      //! metoda, na podstawie stanu wizualizatora, ustawia widoczne przedziały wykresu
      void setScale();
      //! ustawia sformatowwany tytuł wizualizatora
      virtual void setTitle( const QString& title ) { qwtPlot->setTitle(title); }
      //! \return sformatowany tytuł wizualizatora
      virtual QString getTitle() const { return qwtPlot->title().text(); }
      //! Umożliwia manipulowanie wykresem, przesuwanie , zbliżanie
      //! \param val włącza / wyłącza możliwość manipulacji
      void setManipulation(bool val);
      //! \return czy pokazywać legendę
      bool isShowLegend() const { return showLegend; }
      //! \param val włącza/wyłącza rysowanie legendy
      void setShowLegend(bool val);
      //! Filtrowanie eventów Qt, przekazywane do obiektu obsługującego stan wizualizatora
      //! \param object
      //! \param event
      bool eventFilter( QObject *object, QEvent *event );
      //! \return wszystkie serie aktualnie obsługiwane przez wizualizator
      boost::iterator_range<std::vector<INewChartSeriePrivate*>::const_iterator> getSeries() const;
      //! \return czy wizualizator jest w trybie wizualizacji eventów
	  bool isEventMode() const { return context != c3dlib::C3DEventsCollection::IEvent::General; }
      //! Zrzut wizualizatora do pixmapy
      virtual QPixmap takeScreenshot() const;
	  virtual void setAxisScale(const Axis axis, const double min, const double max, unsigned int steps = 10);

private:
      //! Dodanie krzywej do wykresu, aktualizacja podziałek
      //! \param curve dodawana krzywa
      //! \param scales ekstrema dodawanego wykresu
      void addPlotCurve(QwtPlotCurve* curve, const Scales& scales);
      //! Odtwarza podziałki na podstawie widocznych krzywych
      void recreateScales();
      //! Ustawia zdarzenia C3D związane z próba pomiarowa
      //! \param serie seria, której dotycza eventy
      //! \param val kolekcja ze zdarzeniami
	  void setEvents(NewChartSerie* serie, c3dlib::EventsCollectionConstPtr val);
      //! Tworzy drzewo ze statystykami
      //! \param stats statystyki, dla których utworzone zostanie drzewo
	  void recreateStats();
      //! odświeża widzialne serie, przypisanie współrzędnej Z (przykrywanie)
      void refreshSerieLayers();
      //! odświeża spinboxy zawierające przesunięcie i skale aktywnej serii
      void refreshSpinBoxes();
      //! Ustawia podziałki wykresu
      //! \param scaleToActive czy skalowanie ma być względem aktywnej serii czy całości
      //! \param eventMode czy jest aktywny eventMode
      void setScale(bool scaleToActive, bool eventMode);
      //! Ustawia podziałki wykresu, czesciowo na podstawie stanu wizualizatora
      //! \param scaleToActive czy skalowanie ma być względem aktywnej serii czy całości
      void setGlobalScales(bool scaleToActive);
      //! Dostosowywuje krok, z którym można przesuwać wykres (adaptacyjne spinboxy)
      //! \param spinBox modyfikowany spinbox
      //! \param axis os, na podstawie której ma być obliczony nowy krok
      void adjustOffsetStep(QDoubleSpinBox* spinBox, QwtPlot::Axis axis);
      //! Czy krzywa należy do jakiejs serii czy może jest krzywa dodatkowa
      //! \param curve badana krzywa
      bool isCurveFromSerie(const QwtPlotCurve* curve) const;
      //! chowa lub pokazuje wszystkie etykiety
      void setLabelsVisible(bool);
	 
private slots:
      //! Ustawia aktywną serię danych
      //! \param idx indeks w comboboxie
      void setActiveSerie(int idx);
      //! Wywołane przez picker, ustawia aktywną serię
      //! \param item krzywa zwiazana z seria
      void onSerieSelected(QwtPlotItem* item);
      //! Zaznacza aktywną seria, metoda wywoływana przez legendę
      //! \param dataSerie krzywa zwiazana z seria
      //! \param on stan przycisku legendy (on = uczyń aktywną)
      //! \param idx niewykorzystywane
      void onSerieSelected(const QVariant& dataSerie, bool on, int idx);

      NewChartLegendItem* getLegendLabel(QwtPlotCurve* curve);

      //! Zmienia widoczność aktywnej serii, metoda wywoływana przez legendę
      //! \param dataSerie krzywa zwiazana z seria
      //! \param visible czy seria ma być widoczna czy ukryta
      void onSerieVisible(const QVariant& info, bool visible);
      //! wybrano zmianę stanu wizualizatora
      void onStateAction();
      //! zmiana kontekstu eventów, czyli sposobu rysowania wykresu
      //! \param  okresla czy wykres ma obsługiwac zdarzenia, a jeśli tak to czy lewe, czy prawe
      void onEventContext(int);
      //! zarządza widocznością statystyk
      //! \param visible pokaż / ukryj
      void showStatistics(bool visible);
      //! \return czy czas aktywnej serii jest wewnątrz jakiegoś eventu związanego z próbą pomiarową
      bool timeInsideEvent();
      //! zmiana podziałki
      //! \param czy skalowanie powinno być do aktywnej serii czy do całego wykresu
      void scaleToActiveSerie(bool);
      //! Zmieniono przesunięcie aktywnej serii
      //! \param d przesunięcie względem osi X
      void onShiftX(double d);
      //! Zmieniono przesunięcie aktywnej serii
      //! \param d przesunięcie względem osi Y
      void onShiftY(double d);
      //! Zmieniono skale aktywnej serii
      //! \param d skala względem osi X
      void onScaleX(double d);
      //! Zmieniono skala aktywnej serii
      //! \param d skale względem osi Y
      void onScaleY(double d);
	  INewChartSeriePrivate* generateCurve(const INewChartCurveGenerator* generator, const INewChartCurveGenerator::Params& params);

	  INewChartSeriePrivate* tryGetCurrentOriginCurve();

	  //! Usuwa wybraną serię danych
	  void removeCurrentSerie();
	  void addToHierarchy();

public:
	  virtual void initHierarchyProvider(core::IHierarchyProviderProxyPtr proxy);
	  virtual void disconnectedFromHierarchy();

private:
	//! Zawiera mapowanie utworzonych krzywych do ich oryginałów
	std::map<INewChartSeriePrivate*, INewChartSeriePrivate*> additionalCurve2Origin;
    //! Obiekt wykresu z Qwt, serce wizualizatora
    QwtPlot* qwtPlot;
    //! Legenda, która została wzbogacona w customowe widgety
    NewChartLegend* legend;
    //! Marker obrazujący aktualna wartość na wykresie
    QwtPlotMarker* qwtMarker;
    //! "kratka" na wykresie
    utils::shared_ptr<QwtPlotGrid> grid;
    //! obiekt przechwujący ekstrema krzywych
    Scales plotScales;
    //! kolekcja z wszystkimi seriami danych
    std::vector<INewChartSeriePrivate*> series;
    //! mapa przypisująca akcje do stanu wykresu
    std::map<QAction*, NewChartStatePtr> statesMap;
    //! flaga mowiaca o tym, czy legenda jest widoczna
    bool showLegend;
    //! aktualny stan wykresu, nie może być nullem
    NewChartStatePtr currentState;
    //! indeks aktualnej serii
    int currentSerie;

    // TODO porzadek z manipulatorami
    //! Manipulator całego wykresu
    utils::shared_ptr<QwtPlotZoomer> zoomer;
    //! Manipulator całego wykresu
    QwtPlotPanner* plotPanner;
    //! Manipulator całego wykresu
    QwtPlotMagnifier* plotMagnifier;

    //! Tabela statystyk
    StatsTable* statsTable;
    //! czy wykres powinno się skalować do aktywnego
    bool scaleToActive;
    //! obiekt umożliwiający rysowanie podziałki z procentami
    PercentScaleDraw* percentDraw;
    //! menu eventów (general, left, right)
    QComboBox * eventsMenu;
    //! akcja wywołująca stan 'picker'
    QAction* pickerAction;
    //! akcja wywołująca stan zaznaczania wartości na wykresie
    QAction* valueMarkerAction;
    //! akcja wywołująca stan zaznaczania roznicy wartości na wykresie
    QAction* vMarkerAction;
    //! akcja wywołująca stan zaznaczania roznicy argumentów na wykresie
    QAction* hMarkerAction;
    //! akcja wywołująca skalowanie do aktywnej serii
    QAction* scaleAction;
    //! spinbox z aktualnym przesunięciem w X aktywnej serii
    QDoubleSpinBox* shiftSpinX;
    //! spinbox z aktualnym przesunięciem w Y aktywnej serii
    QDoubleSpinBox* shiftSpinY;
    //! spinbox z aktualna skala w X aktywnej serii
    QDoubleSpinBox* scaleSpinX;
    //! spinbox z aktualna skala w T aktywnej serii
    QDoubleSpinBox* scaleSpinY;
    //! aktualny kontekst eventów (general, left, right)
	c3dlib::C3DEventsCollection::Context context;
    //! pomocnicze, do ustalenia czy zmienił się aktualnie obrazowany event
    EventsHelper::SegmentConstPtr oldSegment;
    //! stan wizualizatora, picker - umożliwia wybieranie aktywnej serii klikając na nia
    NewChartPickerPtr picker;
    //! czas aktywnej serii
    float currentSerieTime;
    //! aktualna wartość dla serii
    float currentSerieValue;
    // TODO to jest hack dla skali z zewnątrz, powinno stworzyć się obiekt, dla każdego przypadku rysowania skali,
    // np. skala globalna automatyczna, manualna, do aktywnej, procentowa i podmieniac je tak jak we wzorcu strategia
    bool customScale;

	core::IHierarchyProviderProxyPtr proxy;
	GeneratorHelper generatorHelper;
};
typedef utils::shared_ptr<NewChartVisualizer> NewChartVisualizerPtr;
typedef utils::shared_ptr<const NewChartVisualizer> NewChartVisualizerConstPtr;



#endif
