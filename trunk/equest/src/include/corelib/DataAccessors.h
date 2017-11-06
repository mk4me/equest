/********************************************************************
    created:  2011/10/18
    created:  18:10:2011   11:43
    filename: DataAccessors.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___DATAACCESSORS_H__
#define HEADER_GUARD___DATAACCESSORS_H__

#include <vector>
#include <utils/Debug.h>
#include <corelib/IDataManagerReader.h>

namespace core {

    //! Przekierowanie z queryData dla poprawnych danych.
    template <class T, class Ptr>
    void queryDataIsConvertible__(IDataManagerReader::IOperations* manager, std::vector<Ptr> & target, bool exact, std::true_type)
    {
		const static utils::TypeInfo type = utils::TypeInfo(typeid(T));
        // pobieramy wrappery
        ConstVariantsList objects;
        manager->getObjects(objects, type, exact);

        for(auto it = objects.begin(); it != objects.end(); ++it){
            target.push_back((*it)->get(exact));
        }
    }

    //! Przekierowanie z queryData dla niepoprawnych danych.
    template <class T, class Ptr>
	void queryDataIsConvertible__(IDataManagerReader::IOperations*, std::vector<Ptr>&, bool, std::false_type)
    {
        // rev
        //static_assert( false, "Niewłaściwy typ elementu wektora lub niezdefiniowno wrap. Sprawdz CORE_DEFINE_WRAPPER dla poszukiwanego typu." );
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaźników na obiekty. Wskaźniki muszą być konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaźników dla wrappera.
    //! \param exact Czy mają być wyciągane obiekty konkretnie tego typu (z pominięciem polimorfizmu)?
    template <class SmartPtr>
    inline void queryDataPtr(IDataManagerReader::IOperations* manager, std::vector<SmartPtr>& target, bool exact = false)
    {
        static_assert(std::is_const<typename SmartPtr::element_type>::value, "Interfejs pobierania danych operuje na stalych obiektach.");
		static_assert(utils::ObjectWrapperTraits<typename std::remove_const<typename SmartPtr::element_type>::type>::isDefinitionVisible ||
			utils::ObjectWrapperTraits<typename SmartPtr::element_type>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
		queryDataIsConvertible__<typename SmartPtr::element_type, SmartPtr>(manager, target, exact, std::true_type());
    }

    //! \param manager Data manager.
    //! \param exact Czy mają być wyciągane obiekty konkretnie tego typu (z pominięciem polimorfizmu)?
    //! \return Wektor wskaźników na obiekty. Wskaźniki muszą być konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaźników dla wrappera.
    template <class SmartPtr>
    inline std::vector<SmartPtr> queryDataPtr(IDataManagerReader::IOperations* manager, bool exact = false, SmartPtr* /*dummy*/ = nullptr)
    {
        std::vector<SmartPtr> target;
        queryDataPtr<SmartPtr>(manager, target, exact);
        return target;
    }

    //! Zastosowanie idiomu "Return Type Resolver" do unikania podwójnego określania typu
    //! elementu kolekcji
    //! \see http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
    struct queryDataPtr_t
    {

    private:
        IDataManagerReader::IOperations* manager;
        bool exact;
        //! \param manager
        //! \param exact
    public:
        inline queryDataPtr_t(IDataManagerReader::IOperations* manager, bool exact = false) :
        manager(manager), exact(exact)
        {}

        //! \return Kolekcja wskaźników.
        template <class SmartPtr>
        inline operator std::vector<SmartPtr>() const
        {
            std::vector<SmartPtr> result;
            queryDataPtr<SmartPtr>(manager, result, exact);
            return result;
        }
    };

    //! Wersja funkcji queryData oparta o idiom "Return Type Resolver". Nie trzeba
    //! podawać jawnie typu elementu kolekcji jako parametru szablonu.
    inline queryDataPtr_t queryDataPtr(IDataManagerReader::IOperations* manager, bool exact = false)
    {
        return queryDataPtr_t(manager, exact);
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaźników na obiekty. Wskaźniki muszą być konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaźników dla wrappera.
    //! \param exact Czy mają być wyciągane obiekty konkretnie tego typu (z pominięciem polimorfizmu)?
    template <class T, class Ptr>
    inline void queryData(IDataManagerReader::IOperations* manager, std::vector<Ptr>& target, bool exact = false)
    {
		static_assert(utils::ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
		queryDataIsConvertible__<T, Ptr>(manager, target, exact, std::is_convertible<Ptr, typename utils::ObjectWrapperT<T>::ConstPtr>());
    }

    //! \param manager Data manager.
    //! \param target Wektor wskaźników na obiekty. Wskaźniki muszą być konwertowalne z tego
    //!     zdefiniowanego w zasadach wskaźników dla wrappera.
    //! \param exact Czy mają być wyciągane obiekty konkretnie tego typu (z pominięciem polimorfizmu)?
    template <class T>
	inline std::vector<typename utils::ObjectWrapperT<T>::ConstPtr> queryData(IDataManagerReader::IOperations* manager, bool exact = false, T* /*dummy*/ = nullptr)
    {
		static_assert(utils::ObjectWrapperTraits<T>::isDefinitionVisible, "Niewidoczna definicja wrappera.");
		std::vector<typename utils::ObjectWrapperT<T>::ConstPtr> target;
        queryData<T>(manager, target, exact);
        return target;
    }
}

#endif //HEADER_GUARD___DATAACCESSORS_H__
