#ifndef HEADER_GUARD__MODEL_H__
#define HEADER_GUARD__MODEL_H__

#include <utils/ObserverPattern.h>
#include <dfmlib/DFLMTypes.h>
#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>
#include <utils/SmartPtr.h>
#include <set>
#include <vector>
#include <map>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
	////////////////////////////////////////////////////////////////////////////////

	//! Klasa opisująca model logiczny data flow. Pozwala na jego edycję - dodawanie, usuwanie węzłów oraz łączenie/rozłanczanie pinów wg ściśle określonych zasad.
	class Model : public utils::enable_shared_from_this<Model>, public utils::Observable<Model>
	{
	public:
		//! Typ opisujący pętle/cykle w połaczeniach
		typedef std::vector<ConnPtr> CyclePath;

		//! Typreprezentujący zbiór węzłów
		typedef std::set<NPtr> Nodes;

		//! Typ opisujący zbiór połączeń
		typedef std::set<ConnPtr> Connections;

		//! Typ opisujący zbiór pinów
		typedef std::set<PinPtr> PinsSet;

		//! Typ przechowujący lokalne niezgodności modelu logicznego z wytyczonymi zasadami jego tworzenia
		typedef std::map<NPtr, PinsSet > RequiringConnection;

	protected:

		typedef std::lock_guard<std::recursive_mutex> ScopedLock;

	private:

		//! Struktura pomocnicza przy analizie pętli (cyklów) w modelu
		typedef struct PathEntry{
			//! Aktualny węzeł
			NPtr node;
			//! Aktualny pin
			//Node::Pins::const_iterator pinIT;
			int pinIndex;

			//! Aktualne połączenie
			//Connections::const_iterator connIT;
			int connectionIndex;

			//! Operator porównania
			bool operator==(const PathEntry & pe) const;
		}PathEntry;

	public:

		//! Konstruktor zerujący
		Model(void);

		//! Wirtualny destruktor
		virtual ~Model(void);

		//! \param node Węzeł którego kompatybilność z modelem sprawdzamy
		//! \return Czy węzeł jest wspierany przez model
		virtual bool isNodeSupported(const NPtr & node) const;

		//! \return Czy model poprawnie skonfigurowany - pod kątem połączeń i węzłów
		bool isModelValid() const;

		//! \param node Węzeł do dodania
		void addNode(const NPtr & node);

		//! \param node Węzeł do usunięcia
		void removeNode(const NPtr & node);

		//! Usuwa wszystkie węzły i połączenia z modelu
		void clearNodes();

		//! \param src Pin źródłowy (wyjściowy)
		//! \param src Pin docelowy (wejściowy)
		//! \return Czy można połaczyć piny ze względu na obowiązujące reguły
		bool canConnect(const CPinPtr & src, const CPinPtr & dest) const;

		//! \param src Pin źródłowy (wyjściowy)
		//! \param src Pin docelowy (wejściowy)
		//! \return Czy połączenie tych pinów wprowadzi cykl (pętle) w modelu
		bool createCycle(const CPinPtr & src, const CPinPtr & dest) const;

		//! \param src Pin źródłowy (wyjściowy)
		//! \param src Pin docelowy (wejściowy)
		//! \return Opis cyklu (ściezki) jaki powstanie przez połączenie tych dwóch pinów
		CyclePath getCycle(const CPinPtr & src, const CPinPtr & dest) const;

		//! \param src Pin źródłowy (wyjściowy)
		//! \param src Pin docelowy (wejściowy)
		//! \return Połączenie utworzone pomiędzy zadanymi pinami
		ConnPtr connect(const PinPtr & src, const PinPtr & dest);

		//! \param connection Połączenie do usunięcia
		void removeConnection(const ConnPtr & connection);

		//! \param Wezeł do rozłączenia - wszystke jego połaczenia zostają usuniete
		void disconnectNode(const NPtr & node);

		//! Usuwa wszystkie połączenia w modelu
		void clearConnections();

		//! \return Wszystkie węzły modelu
		const Nodes & getNodes() const;

		//! \return Wszystkie połączenia w modelu
		const Connections & getConnections() const;

		//! \return Zwraca węzły wymagające interwencji by model był poprawny
		const RequiringConnection & getRequiringConnections() const;

		//! \return Zwraca kolekcję liści - elementów które posiadają wejścia ale nie posiadają wyjść lub mają wyjścia niepodłączone
		const Nodes & getLeafNodes() const;

		//! \param node Węzeł któremu nazwę zmieniamy
		//! \param name Nowa nazwa węzła
		void setNodeName(const NPtr & node, const std::string & name);

		//! \param pin Pin któremu nazwę zmieniamy
		//! \param name Nowa nazwa pinu
		void setPinName(const PinPtr & pin, const std::string & name);

	protected:

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

		//! \param connection Połaczenie do usuniecia z modelu
		virtual void beforeRemoveConnection(const ConnPtr & connection);

		//! \param src Pin źródłowy (wyjściowy)
		//! \param src Pin docelowy (wejściowy)
		virtual void afterConnect(const ConnPtr & connection);

		//! \param nodes Lista węzłów uznana za sprawdzone - tutaj mogą się podpinać klasy pochodne z szerszą wiedzą na temat węzłów
		//! W ten sposób mogą przyspieszyć wykrywanie cykli
		virtual void initCycleCheckedNodes(Nodes & nodes) const;

		//! \param node Węzeł który stał się właśnie liściem
		virtual void afterLeafAdd(const NPtr & node);

		//! \param node Węzeł który przestaje być liściem
		virtual void beforeLeafRemove(const NPtr & node);

	private:

		bool isLockedNonBlocking() const;

		//! \param node Węzeł który rozłanczamy
		void quickDisconnectNode(const NPtr & node);

		//! \param connection Połączenie które usuwamy
		void quickRemoveConnection(const Connections::iterator & connection);

		//! \param nodeIt Iterator węzła który usuwamy
		void quickRemoveNode(const Nodes::iterator & nodeIt);

		//! \param node Wezeł dla którego tworzymy strukture opisująca kolejność analizowania połączen dla sprawdzania cykli
		//! \return Aktualny stan analizowanych połączen
		static PathEntry getFirstNodeOutputConnection(const NPtr & node);

		//! \param pathEntry Element opisujący kolejność przegladania połączeń w węźle - jej aktualny stan
		//! \return Kolejny stan analizowanych połączen
		static PathEntry getNextNodeOutputConnection(const PathEntry & pathEntry);

		//! \param src Pin źródłowy (wyjściowy)
		//! \param src Pin docelowy (wejściowy)
		//! \return Połączenie pomiędzy danymi pinami lub nullptr w przypadku braku połączenia
		ConnPtr findConnection(CPinPtr src, CPinPtr dest) const;

		//! \param node Węzeł który staje się liściem
		void addLeaf(const NPtr & node);

		//! \param node Węzeł który przestaje być liściem
		void removeLeaf(const NPtr & node);

	protected:
		//! mutex dla zmiany stanu modelu
		mutable std::recursive_mutex editMutex;

		const Nodes & innerGetNodes() const { return nodes; }
		const Nodes & innerGetLeafNodes() const { return leafNodes; }

		const Connections & innerGetConnections() const { return connections; }

		const RequiringConnection & innerGetRequiringConnections() const { return pinsRequiringConnections; }

	private:

		//! Zbiór wszystkich węzłów modelu
		Nodes nodes;

		//! Zbiór wszystkich połączeń modelu
		Connections connections;

		//! Mapa węzłów wymagających interwencji aby model był poprawnie zbudowany
		RequiringConnection pinsRequiringConnections;

		//! Zbiór węzłów liści
		Nodes leafNodes;

		//! mutex dla zmiany stanu modelu
		//mutable OpenThreads::Mutex lockMutex;
	};
}

#endif
