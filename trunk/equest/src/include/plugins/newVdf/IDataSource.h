/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:21
    filename: IDataSource.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCE_H__
#define HEADER_GUARD___IDATASOURCE_H__

#include <plugins/newVdf/IOutputDescription.h>
#include <plugins/newVdf/IWorkflowItemBase.h>

//! Forward declaration
class QIcon;

namespace vdf {
//! Interfejs do implemntacji źródeł danych
class IDataSource : public IOutputDescription, public IWorkflowItemBase

{
};
DEFINE_SMART_POINTERS(IDataSource);
}

#endif  //  HEADER_GUARD___IDATASOURCE_H__
