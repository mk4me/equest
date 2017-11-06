/********************************************************************
	created:  2015/07/22	13:07:42
	filename: NodeT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__NODET_H__
#define __HEADER_GUARD_TREECONTAINER__NODET_H__

#include <treecontainerlib/HelperVisitors.h>
#include <treecontainerlib/NodeVisitPolicies.h>

namespace treeContainer
{
	//! \tparam ValueType Typ rpzechowywanej wartości
	template<typename ValueType>
	class NodeT
	{
	public:
		//! Typ przechowywanych w węzłach wartości
		using value_type = ValueType;
		//! Typ naszego węzła
		using node_type = NodeT<value_type>;
		//! Wskazniki do naszego wezla
		DEFINE_SMART_POINTERS_EXT(node_type, Node);
		//! Dzieci wezla
		using Children = Node::Nodes<NodePtr>;
		//! Typ ilo�ci
		using size_type = Node::SizeType;

	private:

		//! \param other Węzeł przenoszony wraz z wartością
		NodeT(NodeT && other)
			: parent_(std::move(other.parent_)),
			children_(std::move(other.children_)),
			value_(std::move(other.value_))
		{}

		template<typename TT = ValueType>
		//! \param value Wartość przechowywana w węźle (kopiowanie)
		NodeT(TT && value) : value_(std::forward<TT>(value)) {}

		template<typename TT = ValueType>
		//! \param value Wartość przechowywana w węźle (kopiowanie)
		NodeT(NodePtr parent, TT && value) : parent_(parent), value_(std::forward<TT>(value)) {}

	public:

		//! \param node Węzeł od którego zaczynamy kopiowanie - automatycznie staje się rootem!!
		//! \return Klon danego węzła będący rootem nowego drzewa
		static NodePtr clone(NodeConstPtr node)
		{
			NodePtr ret(create(node->value_));

			for (const auto & cn : node->children_)
			{
				auto c(clone(cn));
				c->parent_ = ret;
				ret->children_.push_back(c);
			}

			return ret;
		}

		template<typename TT = ValueType>
		//! \param value Wartość z jaką tworzymy Węzeł
		//! \return Nowy Węzeł z zadaną wartością
		static NodePtr create(TT && value = TT())
		{
			return NodePtr(new node_type(std::forward<TT>(value)));
		}

		//! \param parentNode Węzeł do którego dodajemy wartość dziecka
		//! \param node Węzeł
		//! \return Czy udało się dodać węzeł
		static void add(NodePtr parentNode, NodePtr node)
		{
			if (node == parentNode){
				throw std::runtime_error("Recursive addition");
			}

			if (node->isRoot() == false) {
				throw std::runtime_error("Addition of non-root node");
			}

			parentNode->children_.push_back(node);
			node->parent_ = parentNode;
		}

		template<typename TT = ValueType>
		//! \param parentNode Węzeł do którego dodajemy wartość dziecka
		//! \param value Wartość dodawanego węzła
		//! \return Węzeł dziecko z zadaną wartością
		static NodePtr add(NodePtr parentNode, TT && value)
		{
			NodePtr ret(new NodeT(parentNode, std::forward<TT>(value)));
			parentNode->children_.push_back(ret);
			return ret;
		}

		//! \param parentNode Węzeł do którego dodajemy wartość dziecka
		//! \param value Wartość dodawanego węzła
		//! \return Węzeł dziecko z zadaną wartością
		static void swap(NodePtr nodeToReplace, NodePtr node)
		{
			if ((nodeToReplace == node) ||
				((nodeToReplace->parent() == nullptr) && (node->parent() == nullptr)))
			{
				return;
			}

			//wspólny rodzic? To znaczy że zamieniamy ich tylko miejscami
			if (nodeToReplace->parent() == node->parent())
			{
				auto parentNode = nodeToReplace->parent();
				auto itA = std::find(parentNode->children_.begin(), parentNode->children_.end(), nodeToReplace);
				auto itB = std::find(parentNode->children_.begin(), parentNode->children_.end(), node);

				std::swap(*itA, *itB);
			}
			else{
				//przypadek ogólny - zmiana struktury drzew
				auto parentNode = nodeToReplace->parent();
				if (parentNode != nullptr){
					auto it = std::find(parentNode->children_.begin(), parentNode->children_.end(), nodeToReplace);
					*it = node;
				}

				nodeToReplace->parent_ = node->parent_;
				node->parent_ = parentNode;

				parentNode = nodeToReplace->parent();
				if (parentNode != nullptr){
					auto it = std::find(parentNode->children_.begin(), parentNode->children_.end(), node);
					*it = nodeToReplace;
				}
			}
		}

		//! \param node usuwany węzeł z hierarchi, jego dzieci przechodza wyżej na koniec listy dzieci (nie może być to root)
		static void remove(NodePtr node)
		{
			auto parent = node->parent();
			if (parent != nullptr){
				parent->children_.erase(std::remove(parent->children_.begin(), parent->children_.end(), node), parent->children_.end());
				node->parent_ = NodePtr();
			}
		}

		//! Destruktor
		~NodeT() {}

		//! \return Rodzenstwo na tym samym poziomie bez mojego wezla
		Children siblings() const
		{
			auto ret = allSiblings();

			auto it = std::find_if(ret.begin(), ret.end(), [this](NodePtr node) { return node.get() == this; });

			if (it != ret.end()){
				ret.erase(it);
			}

			return ret;
		}

		//! \return Całe rodzeństwo - razem z moim węzłem
		Children allSiblings() const
		{
			auto p = parent();
			if (p != nullptr){
				return p->children_;
			}

			return Children();
		}

		//! \param node Węzeł który sprawdzamy czy jest naszym przodkiem
		//! \return Prawda jeśli węzeł jest naszym przodkiem (znajdziemy go id�c w g�re po parentach)
		bool isAncestor(NodeConstPtr node) const
		{
			return Node::isAncestor(this, node);
		}

		//! \param node Węzeł który sprawdzamy czy jest naszym potomkiem
		//! \return Prawda jeśli węzeł jest naszym potomkiem (nasz Węzeł znajdziemy go id�c w g�re po parentach tego węzła)
		bool isDescendant(NodeConstPtr node) const
		{
			return Node::isDescendant(this, node);
		}

		//! \return Czy węzeł jest liściem - nie ma dzieci
		bool isLeaf() const
		{
			return children_.empty();
		}

		//! \return Czy węzeł jest rootem - nie ma parenta
		bool isRoot() const
		{
			return parent() == nullptr;
		}

		//! \return Stopień węzła - Ilość połączeń jakie tworzy
		size_type degree() const
		{
			return children_.size();
		}

		//! \return Wysokość węzła - odległość do roota
		size_type depth() const
		{
			size_type l = 0;
			auto p = this;

			while (p->isRoot() == false) { p = p->parent_.lock().get(); ++l; }

			return l;
		}

		//! \return Głębokość węzła - poziom najdalej oddalonego dziecka
		size_type height() const
		{
			size_type l = 0;

			if (isLeaf() == false){
				for (const auto & child : children_)
				{
					FindMaxLevelVisitor mlVisitor;
					visitLevelOrder(child, mlVisitor);
					l = std::max(l, mlVisitor.maxLevel());
				}

				++l;
			}

			return l;
		}

		//! To samo co depth
		size_type level() const
		{
			return depth() + 1;
		}

		//! \return Ilość węzłów w tym drzewie
		size_type size() const
		{
			return 1 + Node::childrenCount(this);
		}

		//! Rodzic
		NodePtr parent() { return parent_.lock(); };
		//! Rodzic
		NodeConstPtr parent() const { return parent_.lock(); };
		//! Wartość
		value_type & value() { return value_; };
		//! Wartość
		const value_type & value() const { return value_; };
		//TODO - udostępnianie dzieci
		//! Dzieci
		const Children & children() const { return children_; }
		//TODO - udostępnianie dzieci
		//! Dzieci
		//Children children() { return children_; }

	private:

		//! Rodzic
		NodeWeakPtr parent_;
		//! Wartość
		value_type value_;
		//! Dzieci
		Children children_;
	};
}

#endif	// __HEADER_GUARD_TREECONTAINER__NODET_H__
