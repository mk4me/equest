/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:53
    filename: IDataSourceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEMANAGER_H__
#define HEADER_GUARD___IDATASOURCEMANAGER_H__

#include <plugins/newVdf/IDataSource.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>
#include <boost/range/iterator_range.hpp>

namespace vdf {

    class IDataSourceManager : public utils::Observable<IDataSourceManager>
    {
    public:
        //! Lista źródeł danych.
        typedef std::vector<vdf::IDataSourcePtr> IDataSources;
        //! 
        typedef boost::iterator_range<IDataSources::const_iterator> IDataSourcesConstRange;
    public:
        virtual ~IDataSourceManager(void) {};

    public:
        virtual IDataSourcesConstRange enumPrototypes() const = 0;

    };
	DEFINE_SMART_POINTERS(IDataSourceManager);

    ////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  //  HEADER_GUARD___IDATASOURCEMANAGER_H__
