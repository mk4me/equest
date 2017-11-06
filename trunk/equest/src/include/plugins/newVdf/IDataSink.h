/********************************************************************
	created:	2013/01/07
	created:	7:1:2013   20:50
	filename: 	IDataSink.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD___IDATASINK_H__
#define HEADER_GUARD___IDATASINK_H__

#include <plugins/newVdf/IInputDescription.h>
#include <plugins/newVdf/IWorkflowItemBase.h>
//! Forward declaration
class QIcon;

namespace vdf {
    
//! Interfejs do implemntacji ujść danych
class IDataSink : public IInputDescription, public IWorkflowItemBase
{
};
DEFINE_SMART_POINTERS(IDataSink);
}

#endif 
