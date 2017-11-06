/********************************************************************
 created:  2015/05/06	15:04:24
 filename: TreeLinearization.h
 author:	  Mateusz Janiak

 purpose:
 *********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__TREELINEARIZATION_H__
#define __HEADER_GUARD_TREECONTAINER__TREELINEARIZATION_H__

#include <treecontainerlib/NodeLinearization.h>

namespace treeContainer {

template<typename TreeVisitPolicy, int VisitOrder = Forward>
struct TreeLinearization
{

	using VisitPolicy = TreeVisitPolicy;
	using NodeVisitPolicy = typename TreeVisitPolicy::VisitPolicy;

	//! \tparam NPtr Typ wskaźnika węzła
	//! \tparam Visitor Typ odwiedzającego węzły
	template<typename TreeType>
	//! \param node Węzeł od którego zaczynamy linearyzację struktury
	//! \param lp Polityka linearyzowania drzewa
	//! \return Ścieżka zlinearyzowanego drzewa
	static inline auto linearize(TreeType & tree) -> decltype(NodeLinearization<NodeVisitPolicy, VisitOrder>::linearize(tree.root()))
	{
		return NodeLinearization<NodeVisitPolicy, VisitOrder>::linearize(tree.root());
	}
};

template<typename TreeVisitOrder>
struct LinearizedTree
{
	using VisitOrder = TreeVisitOrder;

	using LN = LinearizedNode<typename TreeVisitOrder::VisitPolicy>;

	template<typename TreeType>
	static inline auto node(const TreeType & tree, const Node::SizeType idx) -> decltype(tree.root())
	{
		return LN::node(tree.root(), idx);
	}

	template<typename TreeType>
	static inline auto node(TreeType & tree, const Node::SizeType idx) -> decltype(tree.root())
	{
		return LN::node(tree.root(), idx);
	}

	template<typename TreeType>
	static inline auto value(const TreeType & tree, const Node::SizeType idx) -> decltype(tree.root()->value())
	{
		return LN::value(tree.root(), idx);
	}

	template<typename TreeType>
	static inline auto value(TreeType & tree, const Node::SizeType idx) -> decltype(tree.root()->value())
	{
		return LN::value(tree.root(), idx);
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły
	static void visit(const TreeType & tree, Visitor && visitor)
	{
		LN::visit(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły
	static inline void visit(TreeType & tree, Visitor && visitor)
	{
		LN::visit(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły
	static inline void globalIndexedVisit(const TreeType & tree, Visitor && visitor)
	{
		LN::globalIndexedVisit(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły
	static inline void globalIndexedVisit(TreeType & tree, Visitor && visitor)
	{
		LN::globalIndexedVisit(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static inline void visitNonLeaf(const TreeType & tree, Visitor && visitor)
	{
		LN::visitNonLeaf(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static void visitNonLeaf(TreeType & tree, Visitor && visitor)
	{
		LN::visitNonLeaf(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static void localIndexedVisitNonLeaf(const TreeType & tree,
			Visitor && visitor)
	{
		LN::localIndexedVisitNonLeaf(tree.root(), std::forward<Visitor>(visitor));
	}	

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static inline void localIndexedVisitNonLeaf(TreeType & tree, Visitor && visitor)
	{
		LN::localIndexedVisitNonLeaf(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static void globalIndexedVisitNonLeaf(const TreeType & tree,
			Visitor && visitor)
	{
		LN::globalIndexedVisitNonLeaf(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename TreeType, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static void globalIndexedVisitNonLeaf(TreeType & tree, Visitor && visitor)
	{
		LN::globalIndexedVisitNonLeaf(tree.root(), std::forward<Visitor>(visitor));
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteOrder(const TreeType & tree) ->  Linearization::Order<decltype(tree.root())>
	{
		return LN::createCompleteOrder(tree.root());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteOrderKey(const TreeType & tree, KeyExtractor && keyExtractor) -> Linearization::Order<decltype(keyExtractor(tree.root()->value()))>
	{
		return LN::createCompleteOrderKey(tree.root(), std::forward<KeyExtractor>(keyExtractor));
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteOrder(TreeType & tree) -> Linearization::Order<decltype(tree.root())>
	{
		return LN::createCompleteOrder(tree.root());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteMapping(const TreeType & tree) -> Linearization::Mapping<decltype(tree.root())>
	{
		return LN::createCompleteMapping(tree.root());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteMappingKey(const TreeType & tree, KeyExtractor && keyExtractor) ->  Linearization::Mapping<decltype(keyExtractor(tree.root()->value()))>
	{
		return LN::createCompleteMappingKey(tree.root(), std::forward<KeyExtractor>(keyExtractor));
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteMapping(TreeType & tree) -> Linearization::Mapping<decltype(tree.root())>
	{
		return LN::createCompleteMapping(tree.root());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafOrder(const TreeType & tree) -> Linearization::Order<decltype(tree.root())>
	{
		return LN::createNonLeafOrder(tree.root());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafOrderKey(const TreeType & tree, KeyExtractor && keyExtractor) -> Linearization::Order<decltype(keyExtractor(tree.root()->value()))>
	{
		return LN::createNonLeafOrderKey(tree.root(), std::forward<KeyExtractor>(keyExtractor));
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafOrder(TreeType & tree) -> Linearization::Order<decltype(tree.root())>
	{
		return LN::createNonLeafOrder(tree.root());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafMapping(const TreeType & tree) ->  Linearization::Order<decltype(tree.root())>
	{
		return LN::createNonLeafMapping(tree.root());
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafMappingKey(const TreeType & tree, KeyExtractor && keyExtractor) -> Linearization::Mapping<decltype(keyExtractor(tree.root()->value()))>
	{
		return LN::createNonLeafMappingKey(tree.root(), std::forward<KeyExtractor>(keyExtractor));
	}

	//! \tparam TreeType Typ drzewa
	template<typename TreeType>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafMapping(TreeType & tree) -> Linearization::Mapping<decltype(tree.root())>
	{
		return LN::createNonLeafMapping(tree.root());
	}
};
}

#endif	// __HEADER_GUARD_TREECONTAINER__TREELINEARIZATION_H__
