#include "CoreLibPCH.h"
#include <corelib/Variant.h>
#include <corelib/Visualizer.h>
#include <corelib/IVisualizerManager.h>
#include <corelib/IDataManagerReader.h>
//#include <corelib/PluginCommon.h>

using namespace core;
using namespace plugin;

//! Klasa implementująca funkcjonalność wizualizatora w zakresie jego logiki
class Visualizer::VisualizerImpl
{
private:
	//! Klasa pomocnicza implementacji wizualizatora przy obserwowaniu zmian danych dla serii
	class VisualizerHelper : public IDataManagerReader::IObserver
	{
	public:
		//! \param visualizer Wizualizaotor który wspieramy
		VisualizerHelper(VisualizerImpl * visualizer) : visualizer_(visualizer), liveObserve(false) {}

		//! \param changes Zmiany jakie zaszły w managerze danych
		virtual void observe(const IDataManagerReader::ChangeList & changes)
		{
			std::lock_guard<std::mutex> lock(sync);

			if (liveObserve == false){
				return;
			}

			for (auto it = changes.begin(); it != changes.end(); ++it){
				if ((*it).modyfication == IDataManagerReader::UPDATE_OBJECT){
					auto dataIT = dataSeriesToObserve.find((*it).currentValue);
					if (dataIT != dataSeriesToObserve.end()){
						dataToUpdate.insert(*dataIT);
					}
				}
			}

			tryUpdate();
		}

		//! \param active Czy auto odświeżanie i obserwacja danych w menagerze danych są aktywne
		void setLiveObserveActive(bool active)
		{
			std::lock_guard<std::mutex> lock(sync);
			liveObserve = active;
			tryUpdate();
		}

		//! \return Czy auto odświeżanie i obserwacja danych w menagerze danych są aktywne
		const bool isLiveObserveActive() const
		{
			std::lock_guard<std::mutex> lock(sync);
			return liveObserve;
		}

		//! \param serie Seria której dane będą obserwowane
		void addSerieToObserve(Visualizer::Serie* serie)
		{
			std::lock_guard<std::mutex> lock(sync);
			dataSeriesToObserve[serie->innerSerie()->getData()].push_back(serie);
		}

		//! \param serie Seria której dane nie będą obserwowane
		void removeSerieToObserve(Visualizer::Serie* serie)
		{
			std::lock_guard<std::mutex> lock(sync);
			dataSeriesToObserve[serie->innerSerie()->getData()].remove(serie);
			dataToUpdate[serie->innerSerie()->getData()].remove(serie);
		}

		//! \param dataSource Nowe źródło danych na potrzeby pobierania danych dostępnych dla wizualizatorów
		void addDataSource(DataSourcePtr dataSource)
		{
			std::lock_guard<std::mutex> lock(sourcesSync);
			sources_.push_back(dataSource);
		}

		//! \param dataSource Usuwane źródło danych na potrzeby pobierania danych dostępnych dla wizualizatorów
		void removeDataSource(DataSourcePtr dataSource)
		{
			std::lock_guard<std::mutex> lock(sourcesSync);
			sources_.remove(dataSource);
		}

		//! \return Ilość źródeł danych
		const int getNumDataSources() const
		{
			std::lock_guard<std::mutex> lock(sourcesSync);
			return sources_.size();
		}

		//! \param idx Indeks źródła danych
		//! \return Źródło danych o zadanym indeksie
		DataSourcePtr getDataSource(int idx)
		{
			std::lock_guard<std::mutex> lock(sourcesSync);
			auto it = sources_.begin();
			std::advance(it, idx);
			return *it;
		}

		//! \param type Typ danych o które pytamy
		//! \param objects [out] Lista obiektów zadanego typu z wszystkich źródeł danych
		//! \param exact Czy dane muszą być dokłądnie zadanego typu czy mogą go wspierać
		void getData(const utils::TypeInfo & type, ConstVariantsList & objects, bool exact)
		{
			std::lock_guard<std::mutex> lock(sourcesSync);
			ConstVariantsList locObjects;
			for (auto it = sources_.begin(); it != sources_.end(); ++it){
				try{
					(*it)->getData(type, locObjects, exact);
				}
				catch (...){
				}
			}

			ConstVariantsSet uniqueObjects(locObjects.begin(), locObjects.end());
			objects.insert(objects.end(), uniqueObjects.begin(), uniqueObjects.end());
		}

	private:

		//! Próbujemy odświeżać dane z serii wizualizatora jeśli nastąpiła w nich jakaś zmiana
		void tryUpdate()
		{
			for (auto dataIT = dataToUpdate.begin(); dataIT != dataToUpdate.end(); ++dataIT){
				for (auto serieIT = dataIT->second.begin(); serieIT != dataIT->second.end(); ++serieIT){
					(*serieIT)->innerSerie()->requestUpdate();
				}
			}

			std::map<VariantConstPtr, std::list<Visualizer::Serie*>>().swap(dataToUpdate);
		}

	private:
		//! Obiekt synchornizujący stan
		mutable std::mutex sync;
		//! Obiekt synchronizujący operacje na źródłach
		mutable std::mutex sourcesSync;
		//! Lista źródeł danych wizualizatora
		DataSources sources_;
		//! Prywatna implementacja wizualizatora
		VisualizerImpl * visualizer_;
		//! Czy live observe jest aktywne
		bool liveObserve;
		//! Mapa seri wizualizatorów obserwujących dane
		std::map<VariantConstPtr, std::list<Visualizer::Serie*>> dataSeriesToObserve;
		//! Mapa danych do aktualizacji (odświeżenia) w seriach
		std::map<VariantConstPtr, std::list<Visualizer::Serie*>> dataToUpdate;
	};

	//! Zaprzyjaźniona klasa pomocnicza wizualizatora
	friend class VisualizerHelper;

private:

	//! Inicjalizacja wizualizatora na bazie dostarczonej implementacji
	void init()
	{
		{
			utils::unique_ptr<QIcon> i(innerVisualizer_->createIcon());
			icon = *i;
		}

		utils::TypeInfoList types;
		innerVisualizer_->getSupportedTypes(types);
		supportedTypes.insert(types.begin(), types.end());

		visualizerHelper_.reset(new VisualizerHelper(this));
		dmr->addObserver(visualizerHelper_);
	}

public:

	//! \param vis Wizualizator dostarczony przez klienta, opakowany przez nasz obiekt
	//! \param dmr Manager pamięci pozwalający czytać załadowane dane
	//! \param visManager Manager wizualizatorów - informujemy o usunięci/zamknięciu naszego wizualizatora
	VisualizerImpl(plugin::IVisualizerPtr vis, IDataManagerReader * dmr, IVisualizerManager * visManager) :
		visualizer_(nullptr),
		innerVisualizer_(vis),
		widget(nullptr),
		dmr(dmr), visManager(visManager),
		activeSerie(nullptr),
		currentDeltaTime(0.0)
	{
		init();
	}

	//! Konstruktor kopiujący
	//! \param vis Wizualizator do skopiowania
	//TODO dodać kopiowanie serii danych!!
	VisualizerImpl(const VisualizerImpl& vis) :
		visualizer_(nullptr),
		innerVisualizer_(vis.visualizer()->create()),
		widget(nullptr),
		dmr(vis.dmr), visManager(vis.visManager),
		activeSerie(nullptr),
		currentDeltaTime(0.0)
	{
		init();

		setLiveObserveActive(vis.isLiveObserveActive());
	}

	//! \param visualizer Obsługiwany wizualizator
	void setVisualizer(Visualizer * visualizer)
	{
		visualizer_ = visualizer;
		visManager->registerVisualizer(visualizer_);
	}

	//! \return Obsługiwany wizualizator
	plugin::IVisualizer * visualizer()
	{
		return innerVisualizer_.get();
	}

	const plugin::IVisualizer * visualizer() const
	{
		return innerVisualizer_.get();
	}

	//! Destruktor
	~VisualizerImpl()
	{
		dmr->removeObserver(visualizerHelper_);
		visManager->unregisterVisualizer(visualizer_);
		destroyAllSeries();
	}

	//! \return Pobiera lub tworzy widget wizualizatora do pokazania w aplikacji
	QWidget* getOrCreateWidget()
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		if (!widget) {
			//PLUGIN_LOG_DEBUG("Creating Visualizer " << innerVisualizer_->getName() <<  " widget");
			widget = innerVisualizer_->createWidget();
			UTILS_ASSERT(widget, "Nie udało się stworzyć widgeta.");
		}
		return widget;
	}

	//! \return Ikona wizualizatora
	const QIcon getIcon() const
	{
		return icon;
	}

	//! \return Widget wizualizatora do pokazania w aplikacji
	QWidget* getWidget()
	{
		return widget;
	}

	//! \return Widget wizualizatora do pokazania w aplikacji
	const QWidget* getWidget() const
	{
		return widget;
	}

	//! \return Nazwa wizualizatora
	const std::string getName() const
	{
		return innerVisualizer_->name();
	}

	//! \return Unikalny ID wizualizatora
	core::UniqueID getID() const
	{
		return innerVisualizer_->ID();
	}

	//! \return Opis wizualizatora
	const std::string  getDescription() const
	{
		return innerVisualizer_->description();
	}

	//! \return Nowa instancja wizualizatora
	Visualizer * create() const
	{
		return new Visualizer(plugin::IVisualizerPtr(innerVisualizer_->create()), dmr, visManager);
	}

	//! \return Maksumalna ilość serii danych wizualizatora
	const int getMaxSeries() const
	{
		return innerVisualizer_->getMaxDataSeries();
	}

	//! \param supportedTypes Typy wspierane przez wizualizator
	void getSupportedTypes(utils::TypeInfoSet & supportedTypes) const
	{
		utils::TypeInfoList list;
		innerVisualizer_->getSupportedTypes(list);
		supportedTypes.insert(list.begin(), list.end());
	}

	//! \param requestedType Typ danych oczekiwanych do wyświetlenia
	//! \param data Dane
	//! \return Seria danych wizualizatora
	Visualizer::Serie * createSerie(const utils::TypeInfo & requestedType, const VariantConstPtr & data)
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		Visualizer::Serie * serie = nullptr;
		auto s = innerVisualizer_->createSerie(requestedType, data);
		if (s != nullptr){
			serie = new Visualizer::Serie(visualizer_, s);
			dataSeries.push_back(serie);
			visualizerHelper_->addSerieToObserve(serie);
			notifyChange(serie, ADD_SERIE);
			if (dataSeries.size() == 1){
				setActiveSerie(serie);
			}
		}

		return serie;
	}

	//! \param serie Seria danych do sklonowania
	//! \return sklonowana seria danych
	Visualizer::Serie * createSerie(Visualizer::Serie * serie)
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		Visualizer::Serie * retserie = nullptr;
		if (serie->visualizer_ == visualizer_){
			auto s = innerVisualizer_->createSerie(serie->innerSerie());
			if (s != nullptr){
				retserie = new Visualizer::Serie(visualizer_, s);
				dataSeries.push_back(retserie);
				visualizerHelper_->addSerieToObserve(serie);
				notifyChange(serie, ADD_SERIE);
			}
		}

		return retserie;
	}

	//! \param serie Seria danych do usunięcia/zniszczenia
	void destroySerie(Visualizer::Serie * serie)
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		if (serie->visualizer_ == visualizer_){
			if (activeSerie == serie){
				if (dataSeries.size() - 1 > 0){
					auto idx = serieIdx(serie);
					if (idx > 0){
						setActiveSerie(getSerie(idx - 1));
					}
					else{
						setActiveSerie(getSerie(idx + 1));
					}
				}
				else{
					setActiveSerie(nullptr);
				}
			}

			notifyChange(serie, REMOVE_SERIE);
			dataSeries.remove(serie);
			visualizerHelper_->removeSerieToObserve(serie);
			innerVisualizer_->removeSerie(serie->innerSerie());
			delete serie;
		}
	}

	//! \param serie Seria danych do odświeżenia
	//! \param modyfication typ modyfikacji serii
	void notifyChange(Visualizer::Serie * serie, SerieModyfication modyfication)
	{
		for (auto it = observers_.begin(); it != observers_.end(); ++it){
			(*it)->update(serie, modyfication);
		}
	}

	//! Metoda niszczy wszystkie serie wizualizatora
	void destroyAllSeries()
	{
		std::lock_guard<std::recursive_mutex> lock(sync);

		setActiveSerie(nullptr);

		while (dataSeries.empty() == false){
			destroySerie(*(dataSeries.begin()));
		}

		DataSeries().swap(dataSeries);
	}

	//! \param observer Obiekt obserwujący wizualizator
	void addObserver(Visualizer::IObserver * observer)
	{
		observers_.push_back(observer);
	}
	//! \param observer Usuwany obiekt obserwujący wizualizator
	void removeObserver(Visualizer::IObserver * observer)
	{
		observers_.remove(observer);
	}
	//! \param dataSource Nowe źródło danych wizualizatora
	void addDataSource(DataSourcePtr dataSource)
	{
		visualizerHelper_->addDataSource(dataSource);
	}
	//! \param dataSource Usuwane źródło dancyh wizualizatora
	void removeDataSource(DataSourcePtr dataSource)
	{
		visualizerHelper_->removeDataSource(dataSource);
	}
	//! \return Ilość źródeł danych wizualizatora
	const int getNumDataSources() const
	{
		return visualizerHelper_->getNumDataSources();
	}
	//! \param idx Indeks źródła danych wizualizatora
	//! \return Źródło danych
	Visualizer::DataSourcePtr getDataSource(int idx)
	{
		return visualizerHelper_->getDataSource(idx);
	}

	//! \param type Typ danych
	//! \param objects [out] Dane pobrane ze źróeł danych wizualizatora dla danego typu
	//! \param exact Czy dane musza być dokłądnie  tego samego typu o który pytano
	void getData(const utils::TypeInfo & type, ConstVariantsList & objects, bool exact)
	{
		visualizerHelper_->getData(type, objects, exact);
	}

	void setLiveObserveActive(bool active)
	{
		visualizerHelper_->setLiveObserveActive(active);
	}

	const bool isLiveObserveActive() const
	{
		return visualizerHelper_->isLiveObserveActive();
	}

	const int getNumSeries() const
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		return dataSeries.size();
	}

	Visualizer::Serie * getSerie(int idx)
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		auto it = dataSeries.begin();
		std::advance(it, idx);
		return *it;
	}

	const int serieIdx(Visualizer::Serie * serie) const
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		auto it = std::find(dataSeries.begin(), dataSeries.end(), serie);
		if (it == dataSeries.end()){
			return -1;
		}

		return std::distance(dataSeries.begin(), it);
	}

	void setActiveSerie(Visualizer::Serie * serie)
	{
		std::lock_guard<std::recursive_mutex> lock(sync);

		if (serie == nullptr){
			innerVisualizer_->setActiveSerie(nullptr);
		}
		else{
			innerVisualizer_->setActiveSerie(serie->innerSerie());
		}

		activeSerie = serie;

		notifyChange(activeSerie, Visualizer::ACTIVE_SERIE_CHANGED);
	}

	const Visualizer::Serie * getActiveSerie() const
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		return activeSerie;
	}

	Visualizer::Serie * getActiveSerie()
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		return activeSerie;
	}

	void update(double deltaTime)
	{
		std::lock_guard<std::recursive_mutex> lock(sync);
		
		bool needUpdate = false;

		for (auto & serie : dataSeries)
		{
			try{
				needUpdate |= serie->innerSerie()->tryUpdate();
			}
			catch (std::exception & e){
				/*
				PLUGIN_LOG_ERROR("Failed to update visualizer: "
					<< innerVisualizer_->name() << "(" << innerVisualizer_->ID()
					<< ") serie " << serie->serie()->getName() << " with requested data type "
					<< serie->serie()->getRequestedDataType().name());
					*/
			}
		}

		if (needUpdate == true){
			innerVisualizer_->update(currentDeltaTime + deltaTime);
			currentDeltaTime = 0.0;
		}
		else if (innerVisualizer_->innerUpdateRequired() == true){		
			innerVisualizer_->update(-1);
			currentDeltaTime += deltaTime;
		}
	}

private:
	//! Obiekt obserwujący dane w MemoryDM - tylko update nas interesuje aby odświeżać serię
	//! Reszta będzie robiona w widgetach na odpowiednie akcje (czy dane są jeszcze w DM)
	utils::shared_ptr<VisualizerHelper> visualizerHelper_;
	//! Faktyczny wizualizator dla danych
	utils::shared_ptr<plugin::IVisualizer> innerVisualizer_;

	//! Faktyczny widget.
	QWidget* widget;
	//! Ikona widgeta
	QIcon icon;
	//! Wspierane typy danych przez wizualizator
	utils::TypeInfoSet supportedTypes;
	//! Serie danych utrworzone przez użytkownika
	DataSeries dataSeries;
	//! Lista obserwujących
	std::list<Visualizer::IObserver*> observers_;

	//! DataManagerReader
	IDataManagerReader * dmr;
	//! VisualizerManager
	IVisualizerManager * visManager;
	//! Aktywna seria
	Visualizer::Serie * activeSerie;
	//! Obiekt synchronizujący stan
	mutable std::recursive_mutex sync;
	//! Obsługiwany wizualizator
	Visualizer * visualizer_;
	//! Faktyczna delta
	double currentDeltaTime;
};

Visualizer::Serie::Serie(Visualizer * visualizer, plugin::IVisualizer::ISerie * serieBase)
: visualizer_(visualizer), innerSerie_(serieBase)
{
}

Visualizer::Serie::~Serie()
{
	delete innerSerie_;
}

const Visualizer * Visualizer::Serie::visualizer() const
{
	return visualizer_;
}

Visualizer * Visualizer::Serie::visualizer()
{
	return visualizer_;
}

const plugin::IVisualizer::ISerie * Visualizer::Serie::innerSerie() const
{
	return innerSerie_;
}

plugin::IVisualizer::ISerie * Visualizer::Serie::innerSerie()
{
	return innerSerie_;
}

Visualizer::Visualizer(plugin::IVisualizerPtr vis, IDataManagerReader * dmr, IVisualizerManager * visManager) :
visualizerImpl(new VisualizerImpl(vis, dmr, visManager))
{
	visualizerImpl->setVisualizer(this);
}

Visualizer::Visualizer(const Visualizer& visualizer) :
visualizerImpl(new VisualizerImpl(*(visualizer.visualizerImpl)))
{
	visualizerImpl->setVisualizer(this);
}

plugin::IVisualizer * Visualizer::visualizer()
{
	return visualizerImpl->visualizer();
}

const plugin::IVisualizer * Visualizer::visualizer() const
{
	return visualizerImpl->visualizer();
}

Visualizer::~Visualizer()
{
}

QWidget* Visualizer::getOrCreateWidget()
{
	return visualizerImpl->getOrCreateWidget();
}

const QIcon Visualizer::getIcon() const
{
	return visualizerImpl->getIcon();
}

QWidget* Visualizer::getWidget()
{
	return visualizerImpl->getWidget();
}

const QWidget* Visualizer::getWidget() const
{
	return visualizerImpl->getWidget();
}

const std::string Visualizer::getName() const
{
	return visualizerImpl->getName();
}

core::UniqueID Visualizer::getID() const
{
	return visualizerImpl->getID();
}

const std::string  Visualizer::getDescription() const
{
	return visualizerImpl->getDescription();
}

Visualizer * Visualizer::create() const
{
	return visualizerImpl->create();
}

const int Visualizer::getMaxSeries() const
{
	return visualizerImpl->getMaxSeries();
}

void Visualizer::getSupportedTypes(utils::TypeInfoSet & supportedTypes) const
{
	visualizerImpl->getSupportedTypes(supportedTypes);
}

Visualizer::Serie * Visualizer::createSerie(const utils::TypeInfo & requestedType, const VariantConstPtr & data)
{
	return visualizerImpl->createSerie(requestedType, data);
}

Visualizer::Serie * Visualizer::createSerie(const VariantConstPtr & data)
{
	return visualizerImpl->createSerie(data->data()->getTypeInfo(), data);
}

Visualizer::Serie * Visualizer::createSerie(Serie * serie)
{
	return visualizerImpl->createSerie(serie);
}

void Visualizer::destroySerie(Serie * serie)
{
	visualizerImpl->destroySerie(serie);
}

void Visualizer::destroyAllSeries()
{
	visualizerImpl->destroyAllSeries();
}

void Visualizer::addObserver(IObserver * observer)
{
	visualizerImpl->addObserver(observer);
}

void Visualizer::removeObserver(IObserver * observer)
{
	visualizerImpl->removeObserver(observer);
}

void Visualizer::onScreenshotTrigger()
{
	if (visualizerImpl->getWidget() != nullptr){
		emit screenshotTaken(visualizerImpl->visualizer()->takeScreenshot());
	}
}

void Visualizer::addDataSource(DataSourcePtr dataSource)
{
	visualizerImpl->addDataSource(dataSource);
}

void Visualizer::removeDataSource(DataSourcePtr dataSource)
{
	visualizerImpl->removeDataSource(dataSource);
}

const int Visualizer::getNumDataSources() const
{
	return visualizerImpl->getNumDataSources();
}

Visualizer::DataSourcePtr Visualizer::getDataSource(int idx)
{
	return visualizerImpl->getDataSource(idx);
}

void Visualizer::getData(const utils::TypeInfo & type, ConstVariantsList & objects, bool exact)
{
	visualizerImpl->getData(type, objects, exact);
}

void Visualizer::setLiveObserveActive(bool active)
{
	visualizerImpl->setLiveObserveActive(active);
}

const bool Visualizer::isLiveObserveActive() const
{
	return visualizerImpl->isLiveObserveActive();
}

const int Visualizer::getNumSeries() const
{
	return visualizerImpl->getNumSeries();
}

Visualizer::Serie * Visualizer::getSerie(int idx)
{
	return visualizerImpl->getSerie(idx);
}

const int Visualizer::serieIdx(Serie * serie) const
{
	return visualizerImpl->serieIdx(serie);
}

void Visualizer::setActiveSerie(Serie * serie)
{
	visualizerImpl->setActiveSerie(serie);
}

const Visualizer::Serie * Visualizer::getActiveSerie() const
{
	return visualizerImpl->getActiveSerie();
}

Visualizer::Serie * Visualizer::getActiveSerie()
{
	return visualizerImpl->getActiveSerie();
}

void Visualizer::update(double deltaTime)
{
	visualizerImpl->update(deltaTime);
}

VisualizerMemoryDataSource::VisualizerMemoryDataSource(core::IDataManagerReader * dmr) :dmr(dmr)
{
}

void VisualizerMemoryDataSource::getData(const utils::TypeInfo & type, ConstVariantsList & objects, bool exact) const
{
	dmr->transaction()->getObjects(objects, type, exact);
}
