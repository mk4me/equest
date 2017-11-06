#ifndef HEADER_GUARD__DFMODEL_H__
#define HEADER_GUARD__DFMODEL_H__

#include <dfmlib/DFPin.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/Model.h>
#include <dfmlib/DFInterface.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
	////////////////////////////////////////////////////////////////////////////////

	//! Klasa modelu obsługującego przepływ danych. Model też jest częścią tego przepływu - zbiera informację o przetworzeniu danych przez węzły liście.
	//! Na bazie tej informacji kontroluje moment wprowadzenia nowych danych do modelu (wyzwolenie źródeł).
	class DFModel : public Model
	{
	public:

		//! Typ przechowujący zestaw źródeł na potrzeby wstrzykiwania kolejnej porcji danych do data flow
		typedef std::set<DFSNPtr> SourceNodes;

		typedef std::set<DFNPtr> Nodes;

		typedef std::set<DFNPtr> Nodes;

		private:

			class ModelRunner
			{
			public:
				ModelRunner(DFModel * model);
				~ModelRunner();

				void run();
				void finishProcessing();

			private:
				DFModel * model;
				bool finish;
			};

			friend class ModelRunner;

	public:

		//! Domyśłny konstruktor
		DFModel(void);

		//! Wirtualny destruktor
		virtual ~DFModel(void);

		//---------------------- Model interface ----------------------------

		//! \param node Węzeł którego kompatybilność z modelem sprawdzamy
		//! \return Czy węzeł jest wspierany przez model
		virtual bool isNodeSupported(const NPtr & node) const;

		//---------------------- DFModel -------------------------------------

		//! \return Zwraca zbiór węzłów źródłowych
		const SourceNodes & getSourceNodes() const;

		//! Wyzwala źródła, rozpoczyna przepływ informacji w modelu
		void run();

		//! \return Czy model jest uruchomiony (może być spauzpowany)
		bool isRunning() const;

		//! \return Czy wszystkie dane zostały przetworzone
		bool isFinished() const;

		//! Wstrzymuje wyzpolenie źródeł po zakończeniu aktualnie trwającego cyklu danych
		void pause();

		//! \return Czy model jest wstrzymany (rownoznatrzne że isRunning daje true jeśli isPaused również true)
		bool isPaused() const;

		//! Zatrzymuje model, resetuje wszystkie węzły i piny do ich stanu pocvzątkowego
		void stop();

		void reset();

	protected:

		//---------------- Interfejs Model --------------------

		//! \return true jeśli można dokonać zmiany modelu, inaczej false lub wyjątek
		virtual bool isModelChangeAllowed() const;

		//! \param src Pin źródłowy (wyjściowy)
		//! \param src Pin docelowy (wejściowy)
		//! \return Czy mozna połączyć piny
		virtual bool additionalConnectRules(const CPinPtr & src, const CPinPtr & dest) const;

		//! \return true jeśli model jest poprawny
		virtual bool additionalModelValidation() const;

		//! \param node Węzeł do dodania
		virtual void afterNodeAdd(const NPtr & node);

		//! \param node Węzeł do usunięcia
		virtual void beforeNodeRemove(const NPtr & node);

		//! \param node Węzeł który stał się właśnie liściem
		virtual void afterLeafAdd(const NPtr & node);

		//! \param node Węzeł który przestaje być liściem
		virtual void beforeLeafRemove(const NPtr & node);

		//! Resetuje stany wezłów
		void resetNodeStates();

		//! Resetuje stany pinów
		void resetPinStates();

		//! \param pin Pin którego stan resetujemy
		void resetPinState(const PinPtr & pin);

		//! Powiadamia źródła o konieczności zasilenia model w nowe dane
		void notifySources();

		//! \return Prawda jeśli źródła mają więcej danych do zasilenia modelu
		bool sourcesHaveMoreData() const;

	private:

		//! Informuje że liść przetworzył dane
		void leafHasProcessedData();

	private:

		//! Zbiór węzłów źródłowych
		SourceNodes sourceNodes;

		//! Czy model jest uruchomiony
		bool running;

		//! Czy wszystkie dane przetworzone
		bool finished;

		//! Czy model jest spauzowany
		bool paused;

		bool stopPending;

		bool pausePending;

		//! Licznik ilości węzłów liści które przetworzyły dane
		unsigned int finishedLeafes;

		//! mutex do kontroli wątku notyfikującego źródła
		std::mutex pauseMutex;

		//! mutex do synchronizacji aktualizacji ilości liści które przetworzyły dane
		std::mutex leavesMutex;

		//! mutex do synchronizacji operacji uruchamiania/zatrzymywania/pauzowania dataflow
		mutable std::recursive_mutex runningMutex;

		//! wątek obsługujący wyzwalanie źródeł
		utils::shared_ptr<ModelRunner> modelRunner;
	};
}

#endif
