/********************************************************************
	created:  2015/07/22	13:41:20
	filename: TreeVisitPolicies.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__TREEVISITPOLICIES_H__
#define __HEADER_GUARD_TREECONTAINER__TREEVISITPOLICIES_H__

#include <treecontainerlib/NodeVisitPolicies.h>

namespace treeContainer
{
	namespace VisitPolicies
	{
		namespace Tree
		{
			namespace impl
			{
				//! Polityka przechodzenia drzewa schematem PreOrder
				template<typename NodeVisitPolicy>
				struct OrderT
				{
					using VisitPolicy = NodeVisitPolicy;

					//! \tparam TreeType Typ drzewa
					//! \tparam Visitor Typ odwiedzającego węzły
					template<typename TreeType, typename Visitor>
					//! \param tree Drzewo
					//! \param visitor Obiekt przeglądający węzły
					static void visit(TreeType & tree, Visitor & visitor)
					{
						VisitPolicy::visit(tree.root(), visitor);
					}

					//! \tparam TreeType Typ drzewa
					//! \tparam Visitor Typ odwiedzającego węzły
					template<typename TreeType, typename Visitor>
					//! \param tree Drzewo
					//! \param visitor Obiekt przeglądający węzły
					static void visit(const TreeType & tree, Visitor & visitor)
					{
						VisitPolicy::visit(tree.root(), visitor);
					}

					//! \tparam TreeType Typ drzewa
					//! \tparam CondVisitor Typ odwiedzającego węzły
					template<typename TreeType, typename CondVisitor>
					//! \param tree Drzewo
					//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
					//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
					static bool visitWhile(TreeType & tree, CondVisitor & condVisitor)
					{
						return VisitPolicy::visitWhile(tree.root(), condVisitor);
					}

					//! \tparam TreeType Typ drzewa
					//! \tparam CondVisitor Typ odwiedzającego węzły
					template<typename TreeType, typename CondVisitor>
					//! \param tree Drzewo
					//! \param condVisitor Obiekt przeglądający węzły i poziomy z warunkiem
					//! \return Czy nastąpiła przerwa przy przechodzeniu drzewa
					static bool visitWhile(const TreeType & tree, CondVisitor & condVisitor)
					{
						return VisitPolicy::visitWhile(tree.root(), condVisitor);
					}
				};
			}


			//! Polityka przechodzenia drzewa schematem PreOrder
			using PreOrder = impl::OrderT<VisitPolicies::Node::PreOrder>;

			//! Polityka przechodzenia drzewa schematem PostOrder
			using PostOrder = impl::OrderT<VisitPolicies::Node::PostOrder>;

			//! Polityka przechodzenia drzewa schematem LevelOrder
			using LevelOrder = impl::OrderT<VisitPolicies::Node::LevelOrder>;

			//! \tparam NodeVisitOrderT Polityka przechodzenia drzewa którą odwracamy
			template<typename NodeVisitOrderT>
			//! Polityka przechodzenia drzewa schematem odwrotnym do danego
			using ReverseOrder = impl::OrderT<VisitPolicies::Node::ReverseOrder<NodeVisitOrderT>>;
		}
	}
}

#endif	// __HEADER_GUARD_TREECONTAINER__TREEVISITPOLICIES_H__
