/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   11:43
    filename: IServiceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISERVICEMANAGER_H__
#define HEADER_GUARD___ISERVICEMANAGER_H__

#include <corelib/IService.h>
#include <vector>
#include <utils/Debug.h>

namespace core {

    class IServiceManager
    {
    public:

		virtual ~IServiceManager() {}

        //! \return Liczba usług.
        virtual int getNumServices() const = 0;
        //! \param idx Indeks usługi.
        //! \return Usługa o zadanym indeksie.
        virtual plugin::IServicePtr getService(int idx) = 0;
        //! \param id ID usługi do wyszukania.
        //! \return Odnaleziona usługa bądź NULL.
        virtual plugin::IServicePtr getService(UniqueID id) = 0;
    };

    typedef utils::shared_ptr<IServiceManager> IServiceManagerPtr;
    typedef utils::shared_ptr<const IServiceManager> IServiceManagerConstPtr;
    typedef utils::weak_ptr<IServiceManager> IServiceManagerWeakPtr;
    typedef utils::weak_ptr<const IServiceManager> IServiceManagerWeakConstPtr;

    //! Metoda wyszukująca wszystkie usługi danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    utils::shared_ptr<T> queryService(IServiceManager* manager, T* dummy = nullptr)
    {
        std::vector<utils::shared_ptr<T>> result;
        queryServices(manager, result);
        if ( result.empty() ) {
            return utils::shared_ptr<T>();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple services found.");
            return result[0];
        }
    }

    //! Metoda wyszukująca wszystkie usługi danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    void queryServices(IServiceManager* manager, std::vector<utils::shared_ptr<T>>& target)
    {
        for ( int i = 0; i < manager->getNumServices(); ++i ) {
            plugin::IServicePtr service = manager->getService(i);
            utils::shared_ptr<T> casted = utils::dynamic_pointer_cast<T>(service);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif //HEADER_GUARD___ISERVICEMANAGER_H__
