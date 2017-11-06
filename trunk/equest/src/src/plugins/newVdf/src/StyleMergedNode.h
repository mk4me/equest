/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:32
	filename: 	StyleMergedNode.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLEMERGEDNODE_H__
#define HEADER_GUARD_NEWVDF__STYLEMERGEDNODE_H__

#include "StyleInputPinNode.h"

namespace vdf {
	class MergedInputPin : public StyleInputPinNode
	{
	public:
		MergedInputPin(IVisualInputPinPtr inputPin);

	public:
		virtual df::IInputPin* getModelPin() const;

	private:
		IVisualInputPinPtr input;

	};


	
}


#endif
