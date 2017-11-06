/********************************************************************
	created:  2015/05/05	23:35:18
	filename: Tree.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__TREE_H__
#define __HEADER_GUARD_TREECONTAINER__TREE_H__

#include <treecontainerlib/NodeVisitPolicies.h>

namespace treeContainer
{
	struct Tree
	{
		//! Typ opisujacy ilość, rozmiar
		using SizeType = Node::SizeType;

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Czy drzewo jest puste
		static inline bool empty(const TreeType & tree)
		{
			return tree.root() == nullptr;
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Ilość węzłów drzewa
		static inline SizeType size(const TreeType & tree)
		{
			if (empty(tree) == true){
				return 0;
			}

			return tree.root()->size();
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Limity stopni węzłów
		static inline DegreeLimits degreeLimits(const TreeType & tree)
		{
			DegreeLimitsVisitor v;
			
			VisitPolicies::Node::PreOrder::visit(tree, v);

			return v.degreeLimits();
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Wysokość drzewa
		static inline SizeType height(const TreeType & tree)
		{
			if (empty(tree) == true){
				return -1;
			}

			return tree.root()->height();
		}

		//! \tparam TreeType Typ drzewa
		template<typename TreeType>
		//! \param tree Drzewo
		//! \return Poziom drzewa
		static inline SizeType level(const TreeType & tree)
		{
			return height(tree) + 1;
		}

		//! \tparam TreeTypeA Typ drzewa A
		//! \tparam TreeTypeB Typ drzewa B
		template<typename TreeTypeA, typename TreeTypeB>
		//! \param treeA Drzewo A
		//! \param treeB Drzewo B
		//! \return Czy struktury drzew są identyczne
		static inline bool compareStructure(const TreeTypeA & treeA, const TreeTypeB & treeB)
		{
			if ((void*)&treeA == (void*)&treeB){
				return true;
			}

			if (treeA.root() == treeB.root()){
				return true;
			}

			if (empty(treeA) != empty(treeB)){
				return false;
			}			

			return Node::compareStructure(treeA.root(), treeB.root());
		}

		//! \tparam TreeType Typ drzewa
		//! \tparam Comp Typ obiektu porównującego dane drzewa
		template<typename TreeType, typename Comp>
		//! \param treeA Drzewo A
		//! \param treeB Drzewo B
		//! \return Czy struktury i wartości drzew są identyczne
		static inline bool compare(const TreeType & treeA, const TreeType & treeB, Comp comp = Comp())
		{
			if (&treeA == &treeB){
				return true;
			}

			if (treeA.root() == treeB.root()){
				return true;
			}

			if (empty(treeA) != empty(treeB)){
				return false;
			}

			return Node::compare(treeA.root(), treeB.root(), comp);
		}
	};
}

#endif	// __HEADER_GUARD_TREECONTAINER__TREE_H__
