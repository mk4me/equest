/********************************************************************
 created:  2015/05/06	15:04:24
 filename: Linearization.h
 author:	  Mateusz Janiak

 purpose:
 *********************************************************************/
#ifndef __HEADER_GUARD_TREECONTAINER__LINEARIZATION_H__
#define __HEADER_GUARD_TREECONTAINER__LINEARIZATION_H__

#include <vector>
#include <boost/bimap.hpp>
#include <treecontainerlib/Node.h>

namespace treeContainer {

struct Linearization
{
	//! Typ indeksu węzła
	using NodeIDX = Node::SizeType;

	template<typename T>
	//! Typ bimapy indeksów węzłów do zadanej wartości
	using Mapping = boost::bimap < NodeIDX, T >;

	template<typename T>
	//! Typ porzadkujacy po indeksach
	using Order = std::vector < T >;

	template<typename T>
	//! \param order Porządek
	//! \return Mapowanie wynikające z porządku
	static inline Mapping<T> convert(const Order<T> & order)
	{
		Mapping<T> ret;

		for (typename Order<T>::size_type i = 0; i < order.size(); ++i) {
			ret.insert(ret.end(), { i, order[i] });
		}
		return ret;
	}
};

}

#endif	// __HEADER_GUARD_TREECONTAINER__LINEARIZATION_H__
