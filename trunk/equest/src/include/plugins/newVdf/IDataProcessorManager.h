/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   9:02
    filename: IDataProcessorManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATAPROCESSORMANAGER_H__
#define HEADER_GUARD___IDATAPROCESSORMANAGER_H__

#include <plugins/newVdf/IDataProcessor.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>
#include <boost/range/iterator_range.hpp>

namespace vdf{

    class IDataProcessorManager :  public utils::Observable<IDataProcessorManager>
    {
    public:
        //! Lista elementów przetwarzających.
        typedef std::vector<vdf::IDataProcessorPtr> IDataProcessors;
        //! 
        typedef boost::iterator_range<IDataProcessors::const_iterator> IDataProcessorsConstRange;
    public:
        virtual ~IDataProcessorManager(void) {};
        virtual IDataProcessorsConstRange enumPrototypes() const = 0;

    };
	DEFINE_SMART_POINTERS(IDataProcessorManager);

////////////////////////////////////////////////////////////////////////////////
} // namespace vdf
////////////////////////////////////////////////////////////////////////////////

#endif //HEADER_GUARD___ISERVICEMANAGER_H__
