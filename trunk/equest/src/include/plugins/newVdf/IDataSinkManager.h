/********************************************************************
	created:	2013/01/07
	created:	7:1:2013   20:51
	filename: 	IDataSinkManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD___IDATASINKMANAGER_H__
#define HEADER_GUARD___IDATASINKMANAGER_H__

#include <plugins/newVdf/IDataSink.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>
#include <boost/range/iterator_range.hpp>

namespace vdf{

    class IDataSinkManager : public utils::Observable<IDataSinkManager>
    {
    public:
        //! Lista źródeł danych.
        typedef std::vector<vdf::IDataSinkPtr> IDataSinks;
        //! 
        typedef boost::iterator_range<IDataSinks::const_iterator> DataSinksConstRange;

    public:
        virtual ~IDataSinkManager(void) {};
        virtual DataSinksConstRange enumPrototypes() const = 0;

    };

    DEFINE_SMART_POINTERS(IDataSinkManager);

    ////////////////////////////////////////////////////////////////////////////////
} // namespace vdf
////////////////////////////////////////////////////////////////////////////////


#endif  
