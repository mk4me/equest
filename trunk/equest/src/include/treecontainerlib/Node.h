/********************************************************************
	created:  2015/01/15	13:30:08
	filename: Node.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__NODE_H__
#define __HEADER_GUARD_TREECONTAINER__NODE_H__

#include <utils/SmartPtr.h>
#include <algorithm>
#include <list>
#include <set>

namespace treeContainer
{
	template<typename T>
	class NodeT;

	//! Klasa z podstawowymi typami i generycznymi algorytmami dla drzewa
	struct Node
	{
		//! Wskaźniki
		template<typename T>
		using Ptr = utils::shared_ptr < NodeT<T> >;
		template<typename T>
		using ConstPtr = utils::shared_ptr < const NodeT<T> >;
		template<typename T>
		using WeakPtr = utils::weak_ptr < NodeT<T> >;
		template<typename T>
		using WeakConstPtr = utils::weak_ptr < NodeT<T> >;

		//! \tparam Typ dziecka
		template<typename T>
		//Typ kolekcji węzłów
		using Nodes = std::list < T > ;		

		//! Typ rozmiaru
		using SizeType = Nodes<Node>::size_type;

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy przodka
		//! \param ancestor Weryfikowany węzeł przodek
		static inline bool isAncestor(NPtrA me, NPtrB * ancestor)
		{
			if (me == nullptr || me.get() == ancestor){
				return false;
			}

			auto p = me->parent();

			while (p.get() != ancestor) { p = p->parent(); }

			return p.get() == ancestor;
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy przodka
		//! \param ancestor Weryfikowany węzeł przodek
		static inline bool isAncestor(NPtrA me, NPtrB ancestor)
		{
			if (me == nullptr || me == ancestor){
				return false;
			}

			auto p = me->parent();

			while (p != ancestor) { p = p->parent(); }

			return p == ancestor;
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtrA, typename NPtrB>
		//! \param me Węzeł dla którego sprawdzamy potomka
		//! \param ancestor Weryfikowany węzeł potomek
		static inline bool isDescendant(NPtrA me, NPtrB descendant)
		{
			return isAncestor(descendant, me);
		}

		//! \tparam NPtrA Typ wskaźnika węzła startowego pierwszego drzewa
		//! \tparam NPtrB Typ wskaźnika węzła startowego drugiego drzewa
		//! \tparam Comp Typ obiektu używanego do poównania elementów drzew
		template<typename NPtrA, typename NPtrB,
			typename Comp = std::equal_to<>>
		//! \param nodeA Węzeł startowy pierwszego drzewa
		//! \param nodeB Węzeł startowy drugiego drzewa
		//! \param comp Obiekt porównyjący wartości węzłów
		//! \return Czy oba drzewa są równe - struktura i dane węzłów
		static inline bool compare(NPtrA nodeA, NPtrB nodeB, Comp comp = Comp())
		{
			//zakładamy że struktury takie same
			bool ret = true;
			//czy już początek nam się rózni?
			if (nodeA == nodeB){

			} else {
				//no to porównujemy
				//szybko po rozmiarze
				ret = (nodeA->children_.size() == nodeB->children_.size());

				if (ret == true){

					ret == comp(nodeA->value, nodeB->value);

					if (ret == true){
						//idziemy głębiej
						auto itA = nodeA->children_.begin();
						auto itB = nodeB->children_.begin();
						for (;
							(itA != nodeA->children_.end()) && (itB != nodeB->children_.end()) && ((ret = compare(*itA, *itB, comp)) == true);
							++itA, ++itB) {
						}

						if (ret == true && (itA != nodeA->children_.end() || itB != nodeB->children_.end())){
							ret = false;
						}
					}
				}
			}

			return ret;
		}

		//! \tparam NPtrA Typ wskaźnika węzła startowego pierwszego drzewa
		//! \tparam NPtrB Typ wskaźnika węzła startowego drugiego drzewa
		template<typename NPtrA, typename NPtrB>
		//! \param nodeA Węzeł startowy pierwszego drzewa
		//! \param nodeB Węzeł startowy drugiego drzewa
		//! \return Czy struktury drzew są takie same
		static inline bool compareStructure(NPtrA nodeA, NPtrB nodeB)
		{	
			//zakładamy że struktury takie same
			bool ret = true;
			if (nodeA == nodeB){

			} else{
				//no to porównujemy
				//szybko po rozmiarze
				ret = (nodeA->children_.size() == nodeB->children_.size());

				if (ret == true){
					//idziemy głębiej
					auto itA = nodeA->children_.begin();
					auto itB = nodeB->children_.begin();
					for (;
						(itA != nodeA->children_.end()) && (itB != nodeB->children_.end()) && ((ret = compareStructure(*itA, *itB)) == true);
						++itA, ++itB) {}

					if (ret == true && (itA != nodeA->children_.end() || itB != nodeB->children_.end())){
						ret = false;
					}
				}
			}

			return ret;
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param node Węzeł dla którego liczymy dzieci
		static inline SizeType childrenCount(NPtr node)
		{
			const auto children = node->children();
			SizeType ret = children.size();

			for (const auto & child : children)
			{
				ret += childrenCount(child);
			}

			return ret;
		}
	};
}

#endif	// __HEADER_GUARD_TREECONTAINER__NODE_H__
