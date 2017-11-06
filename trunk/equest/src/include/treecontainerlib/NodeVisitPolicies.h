/********************************************************************
	created:  2015/07/22	13:15:05
	filename: NodeVisitPolicies.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__NODEVISITPOLICIES_H__
#define __HEADER_GUARD_TREECONTAINER__NODEVISITPOLICIES_H__

#include <treecontainerlib/Node.h>
#include <treecontainerlib/NodeLinearization.h>

namespace treeContainer
{
	namespace VisitPolicies
	{
		namespace Node
		{
			//! Polityka odwiedzania węzłów
			struct PreOrder
			{
				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename NPtr, typename Visitor>
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param visitor Obiekt przeglądający węzły
				static void visit(NPtr root, Visitor && visitor)
				{
					if(root == nullptr){
						return;
					}

					visitor(root);
					for (const auto & child : root->children())
					{
						visit(NPtr(child), visitor);
					}
				}

				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename NPtr, typename CondVisitor>
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
				static bool visitWhile(NPtr root, CondVisitor && condVisitor)
				{
					if(root == nullptr){
						return false;
					}

					if (condVisitor(root) == true){
						for (const auto & child : root->children())
						{
							if (visitWhile(NPtr(child), condVisitor) == false){
								return false;
							}
						}

						return true;
					}
					else {
						return false;
					}
				}
			};

			//! Polityka odwiedzania węzłów
			struct PostOrder
			{
				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename NPtr, typename Visitor>
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param visitor Obiekt przeglądający węzły
				static void visit(NPtr root, Visitor && visitor)
				{
					if(root == nullptr){
						return;
					}

					for (const auto & child : root->children())
					{
						visit(NPtr(child), visitor);
					}
					visitor(root);
				}

				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename NPtr, typename CondVisitor>
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
				static bool visitWhile(NPtr root, CondVisitor && condVisitor)
				{
					if(root == nullptr){
						return false;
					}
					for (const auto & child : root->children())
					{
						if (visitWhile(NPtr(child), condVisitor) == false){
							return false;
						}
					}
					return condVisitor(root);
				}
			};

			//! Polityka odwiedzania węzłów
			struct LevelOrder
			{
				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename NPtr, typename Visitor>
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param visitor Obiekt przeglądający węzły i poziomy
				static void visit(NPtr root, Visitor && visitor)
				{
					if(root == nullptr){
						return;
					}

					treeContainer::Node::Nodes<NPtr> nodes;
					nodes.push_back(root);
					treeContainer::Node::SizeType level = 0;
					while (nodes.empty() == false)
					{
						//pobieramy dla nast�pnego poziomu dzieci
						treeContainer::Node::Nodes<NPtr> nextLevelNodes;
						for (const auto & node : nodes)
						{
							visitor(node, level);

							for (const auto & child : node->children()){
								nextLevelNodes.push_back(NPtr(child));
							}
						}

						nodes.swap(nextLevelNodes);
						++level;
					}
				}

				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename NPtr, typename CondVisitor>
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				static bool visitWhile(NPtr root, CondVisitor && condVisitor)
				{
					if(root == nullptr){
						return false;
					}

					treeContainer::Node::Nodes<NPtr> nodes;
					nodes.push_back(root);
					treeContainer::Node::SizeType level = 0;
					while (nodes.empty() == false)
					{
						//pobieramy dla nast�pnego poziomu dzieci
						treeContainer::Node::Nodes<NPtr> nextLevelNodes;
						for (const auto & node : nodes)
						{
							if (condVisitor(node, level) == false){
								return false;
							}

							for (const auto & child : node->children()) {
								nextLevelNodes.push_back(NPtr(child));
							}
						}

						nodes.swap(nextLevelNodes);
						++level;
					}

					return true;
				}
			};

			//! \tparam VisitOrder Sposób oryginalnego odwiedzania, które chcemy odwrócić
			template<typename VisitOrder>
			//! Polityka odwiedzania węzłów
			struct ReverseOrder
			{
				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam Visitor Typ odwiedzającego węzły
				template<typename NPtr, typename Visitor>
				//! \param visitOrder Sposób przegl�dania w kierunku który będziemy odwracać
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param visitor Obiekt przeglądający węzły i poziomy
				static void visit(NPtr root, Visitor && visitor)
				{
					auto lt = NodeLinearization<VisitOrder, Backward>::linearize(root);
					for (const auto & node : lt)
					{
						visitor(node);
					}
				}

				//! \tparam NPtr Typ wskaźnika węzła
				//! \tparam CondVisitor Typ odwiedzającego węzły
				template<typename NPtr, typename CondVisitor>
				//! \param visitOrder Sposób przegl�dania w kierunku który będziemy odwracać
				//! \param node Węzeł w którym zaczynamy przeglądanie drzewa
				//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
				static bool visitWhile(NPtr root, CondVisitor && condVisitor)
				{
					auto lt = NodeLinearization<VisitOrder, Backward>::linearize(root);
					for (const auto & node : lt)
					{
						if (condVisitor(node) == false){
							return false;
						}
					}

					return !lt.empty();
				}
			};
		}
	}
}

#endif	// __HEADER_GUARD_TREECONTAINER__NODEVISITPOLICIES_H__
