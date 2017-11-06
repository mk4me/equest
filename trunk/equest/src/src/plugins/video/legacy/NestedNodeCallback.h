/********************************************************************
	created:  2010/06/11
	created:  11:6:2010   11:09
	filename: NestedNodeCallback.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__NESTEDNODECALLBACK_H__
#define __HEADER_GUARD__NESTEDNODECALLBACK_H__

#include <osg/NodeCallback>

template <class Node>
struct NestedCallback : public osg::NodeCallback
{
  //! Węzeł właściwy.
  Node * node;
  //! \param node Węzeł do którego się podpiąć.
  NestedCallback(Node * node) : node(node) {}
  //! Operator przekazywania callbacka do węzła.
  //! \param node
  //! \param nv
  void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    (*this->node)(node, nv);
  }
};

#endif  // __HEADER_GUARD__NESTEDNODECALLBACK_H__
