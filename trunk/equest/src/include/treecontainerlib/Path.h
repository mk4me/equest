/********************************************************************
	created:  2015/07/22	13:58:40
	filename: Path.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__PATH_H__
#define __HEADER_GUARD_TREECONTAINER__PATH_H__

#include <treecontainerlib/Node.h>

namespace treeContainer
{
	struct Path
	{
		//Typ ściezki
		//! \tparam T Element ścieżki
		template<typename T>
		using Type = Node::Nodes < T > ;

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param startNode Węzeł z którego ma wystartować ścieżka
		//! \param endNode Węzeł do którego dojdę ścieżką
		//! \return Długość ścieżki pomiędzy węzłami
		static inline Node::SizeType distance(NPtr startNode, NPtr endNode)
		{
			return findPath(startNode, endNode).size() - 1;
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param path [out] ścieżka którą odwracamy
		static inline void reversePath(Type<NPtr> & path)
		{
			std::reverse(path.begin(), path.end());
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param node Węzeł z którego chcemy pobrać ścieżkę do roota
		static inline Type<NPtr> upPath(NPtr startNode)
		{
			Type<NPtr> ret;

			ret.push_back(startNode);
			if (startNode->isRoot() == false){
				startNode = startNode->parent();
				while (startNode->isRoot() == false) { ret.push_back(startNode); startNode = startNode->parent(); }
				ret.push_back(startNode);
			}

			return ret;
		}

		//! \tparam NPtr Typ wskaźnika węzła
		template<typename NPtr>
		//! \param startNode Węzeł z którego ma się rozpocząć ścieżka
		//! \param endNode Węzeł w którym ma się kończyć ścieżka
		//! \return ścieżka pomiędzy węzłami - kolejne węzły
		static inline Type<NPtr> findPath(NPtr startNode, NPtr endNode)
		{
			Type<NPtr> ret;

			if (startNode != endNode){
				auto p1 = upPath(startNode);

				//wariant end node jest przodkiem startNode
				{
					auto it = std::find(p1.begin(), p1.end(), endNode);

					if (it != p1.end()){
						ret.insert(ret.end(), p1.begin(), it + 1);
						return ret;
					}
				}

				auto p2 = upPath(endNode);

				//wariant end node jest potomkiem startNode
				{
					auto it = std::find(p2.begin(), p2.end(), startNode);

					if (it != p2.end()){
						ret.insert(ret.end(), p2.begin(), it + 1);
						reversePath(ret);
						return ret;
					}
				}

				if (p1.empty() == false && p2.empty() == false){

					std::set<NPtr> s1(p1.begin(), p1.end());
					std::set<NPtr> s2(p2.begin(), p2.end());

					std::vector<NPtr> intersectResult(std::min(p1.size(), p2.size()));
					auto retIT = std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), intersectResult.begin());

					if (retIT != intersectResult.end()){
						//mamy punkt przecięcia - drogi się schodzą
						//szukam najbliższego punktu przecięcia
						std::set<NPtr> intersectionSet(intersectResult.begin(), retIT);
						NPtr intersectionPoint;

						if (p1.size() < p2.size()){
							for (const auto & n : p1)
							{
								if (intersectionSet.find(n) != intersectionSet.end()){
									intersectionPoint = n;
									break;
								}
							}
						}
						else{
							for (const auto & n : p2)
							{
								if (intersectionSet.find(n) != intersectionSet.end()){
									intersectionPoint = n;
									break;
								}
							}
						}

						ret.insert(ret.end(), p1.begin(), std::find(p1.begin(), p1.end(), intersectionPoint));
						ret.insert(ret.end(), std::find(p2.begin(), p2.end(), intersectionPoint), p2.end());
					}
				}
			}
			else{
				ret.push_back(startNode);
			}

			return ret;
		}
	};
}

#endif	// __HEADER_GUARD_TREECONTAINER__PATH_H__
