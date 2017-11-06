/********************************************************************
	created:  2011/03/14
	created:  14:3:2011   15:08
	filename: Visualizer.h
	author:	  Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZER_H__
#define HEADER_GUARD_CORE__VISUALIZER_H__

#include <corelib/Export.h>
#include <utils/SmartPtr.h>
#include <corelib/VariantsCollection.h>
#include <corelib/IVisualizer.h>
#include <QtCore/QObject>
#include <QtGui/QIcon>

namespace core {
	class IDataManagerReader;
	class IVisualizerManager;
	class VisualizerManager;

	//! Wizualizator. Jego zadaniem jest stworzyć widget (gdzie on będzie osadzony - nie jego sprawa),
	//! zarządzać seriami danych wizualizatora, informować o dodaniu/usunięciu serii danych, automatycznie
	//! aktualizować serie danych w momencie edycji związanych z nimi danych w DM
	class CORELIB_EXPORT Visualizer : public QObject
	{
		Q_OBJECT;
		//! Zaprzyjaźneinie z managerem wizualizatorów
		friend class VisualizerManager;

	private:
		//! Forward declaration implementacji wizualizatora
		class VisualizerImpl;

	public:

		//! Seria vizualizatora
		class CORELIB_EXPORT Serie
		{
			//! Zaprzyjaźnienie z wizualizatorem
			friend class Visualizer;
		private:
			//! Prywatny konstruktor - tylko wizualizator może tworzyć takie obiekty
			//! \param visualizer Wizualizator który stworzył ta serię
			//! \param serieBase Faktyczna seria zaimplementowana przez opakowany wizualizator
			//! \param timeSerieFeatures Możliwości serii pod kątem zarządzania czasem - nie muszą być implementowane -> wtedy nullptr
			Serie(Visualizer * visualizer, plugin::IVisualizer::ISerie * serieBase);
			//! Prywatny destruktor - tylko wizualizator może niszczyć serie danych
			~Serie();

		public:
			//! \return Wizualizator który stworzył ta serie danych
			Visualizer * visualizer();
			//! \return Wizualizator który stworzył ta serie danych
			const Visualizer * visualizer() const;
			//! \return Podstawowy interfejs serii danych
			const plugin::IVisualizer::ISerie * innerSerie() const;
			//! \return Podstawowy interfejs serii danych
			plugin::IVisualizer::ISerie * innerSerie();
			//! \tparam Typ który powinna wspierać seria danych
			template<typename T>
			//! \return Wskaźnik do danego typu jeśli go wspiera, nullptr jesli nie ma serii
			//! lub nie wspiera typu
			T * serieFeatures(T * dummy = nullptr)
			{
				return dynamic_cast<T*>(innerSerie());
			}
			//! \tparam Typ który powinna wspierać seria danych
			template<typename T>
			//! \return Wskaźnik do danego typu jeśli go wspiera, nullptr jesli nie ma serii
			//! lub nie wspiera typu
			const T * serieFeatures(T * dummy = nullptr) const
			{
				return dynamic_cast<const T*>(innerSerie());
			}

			//! \tparam Typ który powinna wspierać seria danych
			template<typename T>
			//! \return Wskaźnik do danego typu jeśli go wspiera, nullptr jesli nie ma serii
			//! lub nie wspiera typu
			bool serieHasFeatures(T * dummy = nullptr) const
			{
				return serieFeatures<T>() != nullptr;
			}

		private:
			//! Seria danych
			plugin::IVisualizer::ISerie * innerSerie_;
			//! Wizualizator
			Visualizer * visualizer_;
		};

		friend class Serie;

		//! Interfejs źródła danych wizualizatora
		class IDataSource
		{
		public:
			//! \param type Typdanych jaki nas interesuje
			//! \param objects [out] Lista obiektów zadanego typu
			//! \param exact Czy typ ma się zgadzać czy może być też typ pochodny
			virtual void getData(const utils::TypeInfo & type, ConstVariantsList & objects, bool exact = false) const = 0;
		};

		//! Smart pointer do źródła
		typedef utils::shared_ptr<IDataSource> DataSourcePtr;
		//! Agregat źródeł danych
		typedef std::list<DataSourcePtr> DataSources;

		//! Agregat aktualnie istniejących serii danych
		typedef std::list<Serie*> DataSeries;

		//! Opis zmian w seriach danych
		enum SerieModyfication {
			ADD_SERIE,				//! Dodanie serii danych, notyfikacja wywoływana po dodaniu serii, przed return
			REMOVE_SERIE,			//! Usunięcie serii danych, notyfikacja wywoływana przed usunięciem serii
			ACTIVE_SERIE_CHANGED	//! Zmiana aktywnej serii danych
		};

		//! Interfejs obiektu obserwującego zmiany w seriach danych wizualizatora
		class IObserver
		{
		public:
			//! \param serie Seria która ulega modyfikacji
			//! \param modyfication Typ modyfikacji na serii danych
			virtual void update(Serie * serie, SerieModyfication modyfication) = 0;
		};

	private:
		//! Implementacja
		utils::shared_ptr<VisualizerImpl> visualizerImpl;

	Q_SIGNALS:
		//! \param screenshot Screen z wizualizatora
		void screenshotTaken(const QPixmap & screenshot);

		public Q_SLOTS:
		//! Trigger na screenshot
		void onScreenshotTrigger();

	public:

		//! \param impl Implementacja wizualizatora. Obiekt przejmowany na własność.
		Visualizer(plugin::IVisualizerPtr vis, IDataManagerReader * dmr, IVisualizerManager * visManager);
		//! Konstuktor kopiujący - głęboka kopia. Nie kopiuje widgeta, źródeł danych, obserwatorów
		Visualizer(const Visualizer& visualizer);

		//! Destruktor
		~Visualizer();

		//! \return Unikalny identyfikator obiektu
		core::UniqueID getID() const;
		//! \return nazwa wizualizatora
		const std::string getName() const;
		//! \return Opis wizualizatora
		const std::string  getDescription() const;
		//! \return Klon wizualizatora poza konfiguracją sesji
		Visualizer * create() const;

		//! \param Czy vizualizator powinien sam automatycznie obserwować zmiany po stronie dataManagera dla danych związanych z seriami danych
		void setLiveObserveActive(bool active);
		//! \return Czy vizualizator powinien sam automatycznie obserwować zmiany po stronie dataManagera dla danych związanych z seriami danych
		const bool isLiveObserveActive() const;

		//! \return Widget wizualizatora.
		QWidget* getOrCreateWidget();
		//! \return Widget wizualizatora. Może być nullptr, jeżeli jeszcze nie jest stworzony.
		QWidget* getWidget();
		//! \return Widget wizualizatora. Może być nullptr, jeżeli jeszcze nie jest stworzony.
		const QWidget* getWidget() const;
		//! \return Ikona wizualizatora
		const QIcon getIcon() const;
		//! \return Maksymalna ilość serii jaką wizualizator może obsłuzyć
		const int getMaxSeries() const;
		//! \param data Dane na bazie których ma powstać seria, muszą być wspierane przez wizualizator,
		//! w przeciwnym wypadku seria nie zostanie stworozna i dostaniemy nullptr, podobna reakcja będzie miała miejsce
		//! gdy osiągneliśmy już maksymalną ilość seri jaką wspiera wizualizator i chcemy utworzyć nową
		Serie * createSerie(const utils::TypeInfo & requestedType, const VariantConstPtr & data);
		//! \param data Dane na bazie których ma powstać seria, muszą być wspierane przez wizualizator,
		//! w przeciwnym wypadku seria nie zostanie stworozna i dostaniemy nullptr, podobna reakcja będzie miała miejsce
		//! gdy osiągneliśmy już maksymalną ilość seri jaką wspiera wizualizator i chcemy utworzyć nową
		Serie * createSerie(const VariantConstPtr & data);
		//! \param serie Seria na bazie której ma powstać nowa seria, musi pochodzić od tego wizualizatora,
		//! w przeciwnym wypadku seria nie zostanie stworzozna i dostaniemy nullptr, podobna reakcja będzie miała miejsce
		//! gdy osiągneliśmy już maksymalną ilość seri jaką wspiera wizualizator i chcemy utworzyć nową
		Serie * createSerie(Serie * serie);
		//! \param serie Seria do zniszczenia, musi pochodzić z tego wizualizatora
		void destroySerie(Serie * serie);
		//! \return Ilośc serii w wizualizatorze
		const int getNumSeries() const;
		//! \param idx Indeks serii
		//! \return Seria dla zadanego indeksu
		Serie * getSerie(int idx);
		//! \param serie Seria danych wizualizatora
		//! \return Aktualny indeks serii danych
		const int serieIdx(Serie * serie) const;

		//! Ustawia daną serię aktywną
		void setActiveSerie(Serie * serie);
		//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
		const Serie * getActiveSerie() const;
		//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
		Serie * getActiveSerie();

		//! \return Zbiór wspieranych typów przez wizualizator (od nich można też brać pochodne, któe je wspierają -> iść w górę ścieżki typów)
		void getSupportedTypes(utils::TypeInfoSet & supportedTypes) const;
		//! Usuwa wszystkie serie
		void destroyAllSeries();

		//! \param observer Obserwator do dodania
		void addObserver(IObserver * observer);
		//! \param observer Obserwator do usunięcia
		void removeObserver(IObserver * observer);

		//! \param datSource Źródło danych wizualizatora do dodania
		void addDataSource(DataSourcePtr dataSource);
		//! \param datSource Źródło danych wizualizatora do usunięcia
		void removeDataSource(DataSourcePtr dataSource);
		//! \return ilość źródeł
		const int getNumDataSources() const;
		//! \param idx Indeks źródła które chcemy pobrać
		DataSourcePtr getDataSource(int idx);

		//! \param type Typ danych jaki chcemy pobrać
		//! \param objects [out] Lista obiektów z danymi zadanego typu
		//! \param exact Czy interesują nas tylko typy zgodne czy mogą być też pochodne
		void getData(const utils::TypeInfo & type, ConstVariantsList & objects, bool exact);

		//! \return Wizualizator opakowany
		plugin::IVisualizer * visualizer();
		//! \return Wizualizator opakowany
		const plugin::IVisualizer * visualizer() const;		

	private:
		//! Metoda aktualizująca wizualizator
		//! \param deltaTime Różnica czasu względem poprzedniego odświeżenia
		void update(double deltaTime);
	};

	DEFINE_SMART_POINTERS(Visualizer);

	//! Klasa obsługująca jako źródło danych manager danych
	class CORELIB_EXPORT VisualizerMemoryDataSource : public Visualizer::IDataSource
	{
	public:
		//! \param dmr DataManagerReader jako źródło danych
		VisualizerMemoryDataSource(core::IDataManagerReader * dmr);

		virtual void getData(const utils::TypeInfo & type, ConstVariantsList & objects, bool exact = false) const;

	private:
		//! DataManagerReader
		core::IDataManagerReader * dmr;
	};
}

#endif  // HEADER_GUARD_CORE__VISUALIZER_H__
