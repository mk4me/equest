/********************************************************************
	created:  2015/06/08	19:26:33
	filename: TreeNodeHelperVisitors.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__TREENODEHELPERVISITORS_H__
#define __HEADER_GUARD_TREECONTAINER__TREENODEHELPERVISITORS_H__

#include <treecontainerlib/Node.h>

namespace treeContainer
{
	//! Typ wyliczeniowy opisujący kierunek linearyzacji drzewa
	enum VisitOrder {
		Forward, //! Zgodnie z kolejnością
		Backward, //! W odwrotrnej kolejności
		Random		//! W losowej kolejności
	};

	//! Klasa wizytująca, zliczająca ilość węzłów drzewa
	class SizeVisitor
	{
	public:

		//! Konstruktor domyślny
		SizeVisitor() : treeSize_(0) {}
		//! Destruktor
		~SizeVisitor() {}

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr>
		//! \param node odwiedzany węzeł
		void operator()(NPtr node)
		{
			++treeSize_;
		}

		//! Resetuje licznik
		void reset()
		{
			treeSize_ = 0;
		}

		//! \return Ilość węzłów drzewa
		Node::SizeType treeSize() const
		{
			return treeSize_;
		}

	private:

		//! Ilość węzłów w drzewie
		Node::SizeType treeSize_;
	};

	//! Klasa wizytująca szukająca najgłebszego poziomu drzewa
	class FindMaxLevelVisitor
	{
	public:
		//! Konstruktor domyślny
		FindMaxLevelVisitor() : maxLevel_(0) {}
		//! Destruktor
		~FindMaxLevelVisitor() {}

		//! \tparam NPtr Typ wskaśnika węzła
		template<typename NPtr>
		//! \param node Odwiedzany węzeł
		//! \param level Aktualnie odwiedzany poziom (wzgledem węzła z którego startowaliśmy)
		void operator()(NPtr, const Node::SizeType level)
		{
			maxLevel_ = std::max(maxLevel_, level);
		}

		//! \return Najgłębszy poziom drzewa
		Node::SizeType maxLevel() const
		{
			return maxLevel_;
		}

		//! Zeruje najgłębszy poziom drzewa
		void reset()
		{
			maxLevel_ = 0;
		}

	private:
		//! Maksymalny poziom
		Node::SizeType maxLevel_;
	};

	//! \tparam NPtr Typ wskaźnika węzła
	template<typename NPtr, int VisitOrder>
	//! Klasa linearyzująca drzewo
	class LinearizeVisitor
	{
		static_assert(VisitOrder == Forward || VisitOrder == Backward, "Undefined order");

	public:

		//! Kolekcja węzłów
		using Nodes = Node::Nodes < NPtr > ;

	public:
		//! Konstruktor domyślny
		LinearizeVisitor() {}
		//! Destruktor
		~LinearizeVisitor() {}
		//! \param node odwiedzany węzeł
		void operator()(NPtr node)
		{
			linearizedTree_.push_back(node);
		}

		//! \return Zlinearyzowane drzewo
		const Nodes & linearizedNode() const
		{
			return linearizedTree_;
		}

		//! Czyści zlinearyzowaną reprezentacje drzewa
		void reset()
		{
			Nodes().swap(linearizedTree_);
		}

	private:

		//! Zlinearyzowane drzewo
		Nodes linearizedTree_;
	};

	//! \tparam NPtr Typ wskaźnika węzła
	template<typename NPtr>
	//! Klasa linearyzująca drzewo
	class LinearizeVisitor < NPtr, Backward >
	{
	public:
		//! Kolekcja węzłów
		using Nodes = Node::Nodes < NPtr > ;

	public:
		//! Konstruktor domyślny
		LinearizeVisitor() {}
		//! Destruktor
		~LinearizeVisitor() {}
		//! \param node odwiedzany węzeł
		void operator()(NPtr node)
		{
			linearizedTree_.push_front(node);
		}

		//! \return Zlinearyzowane drzewo
		const Nodes & linearizedNode() const
		{
			return linearizedTree_;
		}

		//! Czyści zlinearyzowaną reprezentacje drzewa
		void reset()
		{
			Nodes().swap(linearizedTree_);
		}

	private:

		//! Zlinearyzowane drzewo
		Nodes linearizedTree_;
	};

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa pozwalająca przechodzić wszystkie węzły drzewa
	class ConsumingVisitor
	{
	private:
		//! Odwiedzający
		Visitor & visitor;

	public:
		//! \param visitor Odwiedzający
		ConsumingVisitor(Visitor & visitor) : visitor(visitor) {}
		//! Destruktor
		~ConsumingVisitor() {}

		//! Operator "konsumuje" ewentualne nadmiarowe argumenty zostawiając tylko węzeł
		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param node odwiedzany węzeł
		auto operator() (NPtr node, ...) -> decltype(visitor(node))
		{
			return visitor(node);
		}
	};

	//! \tparam FilterType Typ filtra dla węzłów
	//! \tparam Visitor Typ odwiedzającego przefiltrowane węzły
	template<typename FilterType, typename Visitor>
	//! Klasa realizująca odwiedzanie przefiltrowanych węzłów
	class FilterVisitor
	{
	public:
		//! \param filter Filtr dla węzłów
		//! \param visitor Odwiedzający przefiltrowane węzły
		FilterVisitor(Visitor & visitor, FilterType filter = FilterType())
			: visitor(visitor), filter(filter){}
		//! Destruktor
		~FilterVisitor() {}

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr, typename... Args>
		//! \param node odwiedzany węzeł
		bool operator()(NPtr node, Args&&... args)
		{
			const auto ret = filter(node);
			if (ret == true){
				visitor(node, std::forward<Args>(args)...);
			}

			return ret;
		}

	private:
		//! Filtr
		FilterType filter;
		//! Odwiedzający
		Visitor & visitor;
	};

	//! Filtr na węzły nie-liście drzewa
	struct NonLeafFilter
	{
		//! \tparam NPtr Typ wskaźnika węzła		
		template <typename NPtr>
		//! \param node Odwiedzany węzeł
		//! \return Czy węzeł jest liściem
		inline bool operator()(NPtr node) const
		{
			return node->isLeaf() == false;
		}
	};

	//! Filtr na liście drzewa
	struct LeafFilter
	{
		//! \tparam NPtr Typ wskaźnika węzła		
		template <typename NPtr>
		//! \param node Odwiedzany węzeł
		//! \return Czy węzeł nie jest liściem
		inline bool operator()(NPtr node) const
		{
			return node->isLeaf() == true;
		}
	};

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa realizująca filtracje nie-liści w drzewie, pozwala przechodzić wszystkie pozostałe węzły
	using NonLeafVisitor = FilterVisitor < NonLeafFilter, Visitor > ;	

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa realizująca filtracje nie-liści w drzewie, pozwala przechodzić wszystkie pozostałe węzły
	using LeafVisitor = FilterVisitor < LeafFilter, Visitor > ;

	//! \tparam Visitor Typ odzwiedzającego
	template<typename Visitor>
	//! Klasa wizutująca drzewo i indeksująca węzły
	class IndexingVisitor
	{
	private:
		//! Aktualny index węzła
		Node::SizeType idx;
		//! Odwiedzający
		Visitor & visitor;

	public:
		//! \param visitor Odwiedzający drzewo
		IndexingVisitor(Visitor & visitor) : idx(0), visitor(visitor) {}
		//! Destruktor
		~IndexingVisitor() {}

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr>
		//! \param node odwiedzany węzeł		
		auto operator() (NPtr node) -> decltype(visitor(node, idx))
		{
			return visitor(node, idx++);
		}
	};

	//! \tparam NPtr Typ węzła
	template<typename NPtr>
	//! Klasa pozwalająca wydajnie pobierać węzeł o zadanym indeksie
	class IndexFilterVisitor
	{
	public:
		//! \param idx Indeks węzła który szukamy
		IndexFilterVisitor(const Node::SizeType idx) : idx(idx) {}
		//! Destruktor
		~IndexFilterVisitor() {}

		//! \param node odwiedzany węzeł		
		//! \param currentIDX Indeks aktualnego węzła
		bool operator()(NPtr node, const Node::SizeType currentIDX)
		{
			if (currentIDX == idx){
				node_ = node;
				return false;
			}

			return true;
		}

		//! \return Węzeł o zadanym indeksie
		NPtr node() const { return node_; }
		//! Metoda resetuje znaleziony węzeł
		void reset() { node_ = NPtr(); }

	private:
		//! Znaleziony węzeł
		NPtr node_;
		//! Indeks szukanego węzła
		const Node::SizeType idx;
	};

	//! Struktura opisuje minimalny i maksymalny stopień węzłów drzewa
	struct DegreeLimits
	{
		//! Domyślny konstruktor
		DegreeLimits() : min(std::numeric_limits<Node::SizeType>::max()),
			max(std::numeric_limits<Node::SizeType>::min()) {}

		//! \param other Kopiowany obiekt
		DegreeLimits(const DegreeLimits & other) : min(other.min), max(other.max) {}
		//! Destruktor
		~DegreeLimits() {}

		//! Minimalny stopień drzewa
		Node::SizeType min;
		//! Maksymalny stopień drzewa
		Node::SizeType max;
		//! \return Czy zaakres stopni jest poprawny
		inline bool valid() const { return min <= max; }
	};

	//! Klasa realizująca zbieranie informacji o max i min stopniu drzewa
	class DegreeLimitsVisitor
	{
	public:
		//! Konstruktor domyślny
		DegreeLimitsVisitor() {}
		//! Destruktor
		~DegreeLimitsVisitor() {}

		//! \tparam NPtr Typ wskaźnika węzła
		//! \tparam Args argumenty, np. dla przechodzenia wg poziomów
		template<typename NPtr, typename... Args>
		//! \param node odwiedzany węzeł
		void operator()(NPtr node)
		{
			const auto degree = node->degree();
			degreeLimits_.min = std::min(degreeLimits_.min, degree);
			degreeLimits_.max = std::max(degreeLimits_.max, degree);
		}

		//! Resetuje limity stopni drzewa
		void reset()
		{
			degreeLimits_ = DegreeLimits();
		}

		//! \return Limity stopni drzewa
		const DegreeLimits & degreeLimits() const { return degreeLimits_; }

	private:
		//! Limity stopni drzewa
		DegreeLimits degreeLimits_;
	};
}

#endif	// __HEADER_GUARD_TREECONTAINER__TREENODEHELPERVISITORS_H__
