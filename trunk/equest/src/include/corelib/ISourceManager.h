/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   9:19
    filename: ISourceManager.h
    author:   Mateusz Janiak
    
    purpose:  Mechanizm źródeł oparty na zasadach serwisów - pluginy dostarczają już instancji źródeł które należy obsłużyć.
              Nie są one prototypami ani klonowalne. Więcej w IService i IServiceManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCEMANAGER_H__
#define HEADER_GUARD_CORE__ISOURCEMANAGER_H__

#include <corelib/ISource.h>
#include <vector>
#include <utils/Debug.h>

namespace core {

    //! Interfejs zarządzający/agregujący źródła danych aplikacji
    class ISourceManager
    {
    public:

		virtual ~ISourceManager() {}

        //! \return Liczba źródeł.
        virtual int getNumSources() const = 0;
        //! \param idx Indeks źródła.
        //! \return Źródło o zadanym indeksie.
        virtual plugin::ISourcePtr getSource(int idx) = 0;
        //! \param id ID źródła do wyszukania.
        //! \return Odnalezione źródło bądź NULL.
        virtual plugin::ISourcePtr getSource(UniqueID id) = 0;
    };

    typedef utils::shared_ptr<ISourceManager> ISourceManagerPtr;
    typedef utils::shared_ptr<const ISourceManager> ISourceManagerConstPtr;
    typedef utils::weak_ptr<ISourceManager> ISourceManagerWeakPtr;
    typedef utils::weak_ptr<const ISourceManager> ISourceManagerWeakConstPtr;

    //! Metoda wyszukująca wszystkie źródła danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    utils::shared_ptr<T> querySource(ISourceManager* manager, T* dummy = nullptr)
    {
        std::vector<utils::shared_ptr<T>> result;
        querySources(manager, result);
        if ( result.empty() ) {
            return utils::shared_ptr<T>();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple sources found.");
            return result[0];
        }
    }

    //! Metoda wyszukująca wszystkie źródła danego typu (np. implementujące
    //! dany interfejs).
    template <class T>
    void querySources(ISourceManager* manager, std::vector<utils::shared_ptr<T>>& target)
    {
        for ( int i = 0; i < manager->getNumSources(); ++i ) {
            plugin::ISourcePtr service = manager->getSource(i);
            utils::shared_ptr<T> casted = utils::dynamic_pointer_cast<T>(service);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
} // namespace core

#endif // HEADER_GUARD_CORE__ISOURCEMANAGER_H__
