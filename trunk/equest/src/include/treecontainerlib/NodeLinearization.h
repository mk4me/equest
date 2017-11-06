/********************************************************************
 created:  2015/05/06	15:04:24
 filename: NodeLinearization.h
 author:	  Mateusz Janiak

 purpose:
 *********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__NODELINEARIZATION_H__
#define __HEADER_GUARD_TREECONTAINER__NODELINEARIZATION_H__

#include <algorithm>
#include <treecontainerlib/Linearization.h>
#include <treecontainerlib/HelperVisitors.h>

namespace treeContainer {

template<typename NodeVisitPolicy, int VisitOrder = Forward>
struct NodeLinearization
{
	using VisitPolicy = NodeVisitPolicy;

	//! \tparam NPtr Typ wskaźnika węzła
	//! \tparam Visitor Typ odwiedzającego węzły
	template<typename NPtr>
	//! \param node Węzeł od którego zaczynamy linearyzację struktury
	//! \param lp Polityka linearyzowania drzewa
	//! \return Ścieżka zlinearyzowanego drzewa
	static inline Node::Nodes<NPtr> linearize(const NPtr & node)
	{
		Node::Nodes<NPtr> ret;

		if(node != nullptr){

			LinearizeVisitor<NPtr, VisitOrder> tlv;
			VisitPolicy::visit(node, tlv);
			ret = tlv.linearizedNode();
		}

		return ret;
	}
};

template<typename NodeVisitPolicy>
struct NodeLinearization<NodeVisitPolicy, Random>
{
	using VisitPolicy = NodeVisitPolicy;

	//! \tparam NPtr Typ wskaźnika węzła
	//! \tparam Visitor Typ odwiedzającego węzły
	template<typename NPtr>
	//! \param node Węzeł od którego zaczynamy linearyzację struktury
	//! \param lp Polityka linearyzowania drzewa
	//! \return Ścieżka zlinearyzowanego drzewa
	static inline Node::Nodes<NPtr> linearize(const NPtr & node)
	{
		auto ln = NodeLinearization<VisitPolicy>::linearize(node);
		std::random_shuffle(ln.begin(), ln.end());
		return ln;
	}
};

template<typename NodeVisitOrder>
struct LinearizedNode
{
	using VisitOrder = NodeVisitOrder;

	template<typename NPtr>
	static inline NPtr node(const NPtr & n, const Node::SizeType idx)
	{
		NPtr ret;

		if (n != nullptr){
			if(idx == 0){
				ret = n;
			}else{
				IndexFilterVisitor<NPtr> idxVisitor(idx);
				IndexingVisitor<IndexFilterVisitor<NPtr>> iV(idxVisitor);
				ConsumingVisitor<IndexingVisitor<IndexFilterVisitor<NPtr>>> cV(iV);
				if (VisitOrder::visitWhile(n, cV) == true){
					ret = idxVisitor.node();
				}
			}
		}

		return ret;
	}

	template<typename NPtr>
	static inline auto value(const NPtr & n, const Node::SizeType idx) -> decltype(n->value())
	{
		auto nn = node(n, idx);

		if (nn == nullptr){
			throw std::runtime_error("Node index out of range");		
		}

		return nn->value();
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename NPtr, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły
	static inline void visit(const NPtr & node, Visitor && visitor)
	{
		ConsumingVisitor<Visitor> cv(visitor);
		VisitOrder::visit(node, cv);
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename NPtr, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły
	static inline void globalIndexedVisit(const NPtr & node, Visitor && visitor)
	{
		visit(node, IndexingVisitor<Visitor>(visitor));
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename NPtr, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static inline void visitNonLeaf(const NPtr & node, Visitor && visitor)
	{
		NonLeafVisitor<Visitor> nlv(visitor);
		visit(node, nlv);
	}

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename NPtr, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static inline void localIndexedVisitNonLeaf(const NPtr & node,
			Visitor && visitor)
	{
		IndexingVisitor<Visitor> iv(visitor);
		visit(node,	NonLeafVisitor<IndexingVisitor<Visitor>>(iv));
	}	

	//! \tparam NPtr Typ wskaźnika węzła drzewa z którego zaczynamy
	//! \tparam Visitor Typ odwiedzającego
	template<typename NPtr, typename Visitor>
	//! \param node Węzeł od którego zaczynamy odwiedzanie struktury
	//! \param Visitor Obiekt odwiedzający węzły nie liście
	static inline void globalIndexedVisitNonLeaf(const NPtr & node,
			Visitor && visitor)
	{
		NonLeafVisitor<Visitor> nlv(visitor);
		visit(node,	IndexingVisitor<NonLeafVisitor<Visitor>>(nlv));
	}

	//! \tparam TreeType Typ drzewa
	template<typename NPtr>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline Linearization::Order<NPtr>
		createCompleteOrder(const NPtr & node) {
		LinearizeVisitor<NPtr, Forward> lv;

		visit(node, lv);

		return Linearization::Order<NPtr>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename NPtr, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteOrderKey(const NPtr & node, KeyExtractor && keyExtractor) -> Linearization::Order<decltype(keyExtractor(node->value()))>
	{
		Linearization::Order<decltype(std::declval<KeyExtractor>()(std::declval<NPtr>()->value()))> ret;

		auto vis = [&ret, &keyExtractor](NPtr node)
				{
					ret.push_back(keyExtractor(node->value()));
				};
		visit(node, vis);

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename NPtr>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline Linearization::Mapping<NPtr>
		createCompleteMapping(const NPtr & node)
	{
		Linearization::Mapping<NPtr> ret;

		globalIndexedVisit(node,
				[&ret](NPtr node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename NPtr, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
	//! \return Mapowanie jointów szkieletu
	static inline auto createCompleteMappingKey(const NPtr & node, KeyExtractor && keyExtractor) -> Linearization::Mapping<decltype(keyExtractor(node->value()))>
	{
		Linearization::Mapping<decltype(std::declval<KeyExtractor>()(std::declval<NPtr>()->value()))> ret;

		globalIndexedVisit(node,
				[&ret, &keyExtractor](NPtr node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, keyExtractor(node->value())});
				});

		return ret;
	}


	//! \tparam TreeType Typ drzewa
	template<typename NPtr>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline Linearization::Order<NPtr> createNonLeafOrder(const NPtr & node)
	{
		LinearizeVisitor<NPtr, Forward> lv;

		visitNonLeaf(node, lv);

		return Linearization::Order<NPtr>(lv.linearizedNode().begin(),
				lv.linearizedNode().end());
	}

	//! \tparam TreeType Typ drzewa
	template<typename NPtr, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafOrderKey(const NPtr & node, KeyExtractor && keyExtractor) -> Linearization::Order<decltype(keyExtractor(node->value()))>
	{
		Linearization::Order<decltype(std::declval<KeyExtractor>()(std::declval<NPtr>()->value()))> ret;

		auto vis = [&ret, &keyExtractor](NPtr node)
				{
					ret.push_back(keyExtractor(node->value()));
				};
		visitNonLeaf(node, vis);

		return ret;
	}


	//! \tparam TreeType Typ drzewa
	template<typename NPtr>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline Linearization::Order<NPtr> createNonLeafMapping(const NPtr & node)
	{
		Linearization::Mapping<NPtr> ret;

		globalIndexedVisitNonLeaf(node,
				[&ret](NPtr node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, node});
				});

		return ret;
	}

	//! \tparam TreeType Typ drzewa
	template<typename NPtr, typename KeyExtractor>
	//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (maj�cych dzieci)
	//! \return Mapowanie aktywnych jointów szkieletu
	static inline auto createNonLeafMappingKey(const NPtr & node, KeyExtractor && keyExtractor) -> Linearization::Mapping<decltype(keyExtractor(node->value()))>
	{
		Linearization::Mapping<decltype(std::declval<KeyExtractor>()(std::declval<NPtr>()->value()))> ret;

		globalIndexedVisitNonLeaf(node,
				[&ret, &keyExtractor](NPtr node, const Linearization::NodeIDX idx)
				{
					ret.left.insert(ret.left.end(), {idx, keyExtractor(node->value())});
				});

		return ret;
	}
};
}

#endif	// __HEADER_GUARD_TREECONTAINER__NODELINEARIZATION_H__
