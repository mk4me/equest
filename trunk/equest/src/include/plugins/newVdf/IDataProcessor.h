/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   17:29
    filename: IDataProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IDATAPROCESSOR_H__
#define HEADER_GUARD_CORE__IDATAPROCESSOR_H__

#include <utils/SmartPtr.h>
#include <plugins/newVdf/IInputDescription.h>
#include <plugins/newVdf/IOutputDescription.h>
#include <plugins/newVdf/IWorkflowItemBase.h>
//! Forward declaration
class QIcon;

namespace vdf 
{
	//! Interfejs do implemntacji ujść danych
	class IDataProcessor : public IOutputDescription, public IInputDescription, public IWorkflowItemBase
	{
	};
	
	DEFINE_SMART_POINTERS(IDataProcessor);
} 

#endif
