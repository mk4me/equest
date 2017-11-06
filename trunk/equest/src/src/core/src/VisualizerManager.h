/********************************************************************
	created:  2011/03/11
	created:  11:3:2011   14:29
	filename: VisualizerManager.h
	author:	  Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERMANAGER_H__
#define HEADER_GUARD_CORE__VISUALIZERMANAGER_H__

#include <vector>
#include <list>
#include <mutex>

#include <boost/range.hpp>
#include <boost/iterator.hpp>
#include <QtGui/QIcon>
#include <corelib/IVisualizerManager.h>
#include <corelib/IVisualizer.h>

namespace core {
	class VisualizerManager : public IVisualizerManager {
	private:
		//! Struktura opisująca prototyp wizualizatora
		struct VisualizerPrototypeData {
			//! Prototyp wizualizatora
			VisualizerPtr visualizerPrototype;
			//! Zbiór wspieranych przez niego typów bezpośrednio
			utils::TypeInfoSet basicSupportedTypes;
			//! Zbiór wspieranych przez niego typów
			utils::TypeInfoSet derrivedSupportedTypes;
		};
		//! Typ agregujący aktualnie żywe instancje wizualizatorów
		typedef std::list<Visualizer*> InnerVisualizerInstances;
		//! Lista prototypów vizualizatorów
		typedef std::map<UniqueID, VisualizerPrototypeData> VisualizerPrototypes;
		//! Typ listy obserwatorów
		typedef std::set<IVisualizerManager::IObserver*> Observers;
		//! Obsługa synchronizacji
		typedef std::recursive_mutex SyncPolicy;
		typedef std::lock_guard<SyncPolicy> ScopedLock;

	private:
		//! Prototypy wizualizatorów
		VisualizerPrototypes visualizerPrototypes_;
		//! Obserwatorzy
		Observers observers_;
		//! Instancje wizualizatorów
		InnerVisualizerInstances visualizerInstances_;

		//! Obiekt na potrzeby synchronizacji update
		mutable SyncPolicy updateSync;
		//! Obiekt na potrzeby synchronizacji obserwatorów
		mutable SyncPolicy observerSync;
		//! Flaga pozwalająca szybko pominąć notyfikację MemoryManager, którą sami spowodowalismy
		bool skipNotify;

	public:
		//! Domyslny konstruktor
		VisualizerManager();
		//! Destruktor wirtualny
		virtual ~VisualizerManager();

	public:

		//! API IVisualizerManager

		//! \param prototypes [out] Wszystkie prototypy wizualizatorów
		virtual void visualizerPrototypes(IVisualizerManager::VisualizerPrototypes & prototypes);
		//! \param id ID źródła do wyszukania.
		//! \return Odnalezione źródło bądź NULL.
		virtual VisualizerConstPtr getVisualizerPrototype(UniqueID id);
		//! \param type Typ dla którego poszukujeemy wizualizatorów
		//! \param prototypes [out] Prototypy wizualizatorów potrafiących obsłużyć zadany typ
		virtual void getVisualizerPrototypes(const utils::TypeInfo & type, IVisualizerManager::VisualizerPrototypes & prototypes, bool exact);
		//! \param visInstances [out] Aktualne instancje wizualizatorów
		virtual void getVisualizersInstances(VisualizerInstances & visInstances) const;
		//! \param observer Obserwator tworzonych wizualziatorów włączony do obserwacji
		virtual void registerObserver(IVisualizerManager::IObserver * observer);
		//! \param observer Obserwator tworzonych wizualziatorów wyłanczany z obserwacji
		virtual void unregisterObserver(IVisualizerManager::IObserver * observer);

	private:
		//! \param visualizer Wizualizator który się tworzy i jest rejestrowany w managerze
		virtual void registerVisualizer(Visualizer* visualizer);
		//! \param visualizer Wizualizator który zostaje zwolniony i wyrejesetrowany
		virtual void unregisterVisualizer(Visualizer* visualizer);

	public:
		//! \param visualizerPrototype Rejestrowany prototyp wizualizatora
		void registerVisualizerPrototype(plugin::IVisualizerPtr visualizerPrototype);
		//! \param deltaTime Zmiana czasu w wizualizatorach
		void update(double deltaTime);

	private:
		//! \param visualizer Wizualizator który notyfikuje
		//! \param modyfication Typ notyfikowanej zmiany
		void notify(Visualizer * visualizer, VisuzalizerOperation modyfication);
	};
}

#endif  // HEADER_GUARD_CORE__VISUALIZERMANAGER_H__
