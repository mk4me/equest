/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   12:38
    filename: TypeInfo.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__TYPEINFO_H__
#define HEADER_GUARD_UTILS__TYPEINFO_H__

#include <typeinfo>
#include <list>
#include <set>
#include <vector>
#include <type_traits>

namespace utils {
	
    //! Struktura reprezentująca typ obiektu.
    class TypeInfo
    {
    private:
        //! Wewnętrzna informacja o typie.
         const std::type_info* typeinfo;

    public:
        //! Konstruktor
		//! \param ti Informacja o typie
		TypeInfo(const std::type_info & ti = typeid(void));
        //! \param type Kopiowany obiekt
        TypeInfo(const TypeInfo& type);

		//! Destruktor virtualny
		virtual ~TypeInfo();

		//! Konstruktor
		//! \tparam T Typ obiektu
		template<typename T>
		//! \return Informacje o obiekcie
		static inline TypeInfo info(const T * dummy = nullptr)
		{
			return TypeInfo(typeid(T));
		}

        //! \param other Przypisywana informacja o typie
        TypeInfo& operator =(const TypeInfo & other);		

        //! Jawny operator rzutowania na type_info.
        operator const std::type_info&() const;

		//! \param obj Obiekt porównywany
		//! \return Czy nasz typ jest przed obj
        bool operator<(const TypeInfo& obj) const;

		//! \param obj Obiekt porównywany
		//! \return Czy nasz typ jest przed obj
		bool operator>(const TypeInfo& obj) const;

		//! \param obj Obiekt porównywany
		//! \return Czy nasz typ równa się obj
        bool operator==(const TypeInfo& obj) const;

        //! \param obj Obiekt porównywany
		//! \return Czy nasz typ różni się od obj
        bool operator!=(const TypeInfo& obj) const;

        //! \param obj Obiekt porównywany
		//! \return Czy nasz obiekt jest przed obj
        bool before(const TypeInfo& obj) const;

		//! \param obj Obiekt porównywany
		//! \return Czy typy są identyczne
		bool compare(const TypeInfo& obj) const;

        //! \return Nazwa typu
        const char* name() const;

        //! \return Skrót informacji o typie
        std::size_t hash_code() const;		
    };

	//! Struktura reprezentująca typ obiektu z informacją o jego rozmiarze
	class ExtendedTypeInfo : public TypeInfo
	{
	private:
		//! Rozmiar typu [bytes]
		std::size_t size_;

	private:

		//! \tpara T Typ dla którego chcemy pobrać rozmiar
		//! Wersja kiedy T = void
		template<typename T>
		static inline std::size_t typeSize(std::true_type)
		{
			return 0;
		}

		//! \tpara T Typ dla którego chcemy pobrać rozmiar
		//! Wersja kiedy T != void
		template<typename T>
		static inline std::size_t typeSize(std::false_type)
		{
			return sizeof(T);
		}

		//! Konstruktor prywatny
		//! \param ti Informacje o typie
		//! \param size Rozmiar typu
		ExtendedTypeInfo(const std::type_info & ti, const std::size_t size);

	public:

		//! Domyślny konstruktor
		ExtendedTypeInfo();

		//! Konstruktor
		//! \param eit Rozszerzona informacja o typie
		ExtendedTypeInfo(const ExtendedTypeInfo & eti);

		//! Konstruktor
		//! \tparam T Typ obiektu
		template<typename T>
		static inline ExtendedTypeInfo info(const T * dummy = nullptr)
		{
			return ExtendedTypeInfo(typeid(T), typeSize<T>(std::is_void<T>()));
		}

		//! Destruktor wirtualny
		virtual ~ExtendedTypeInfo();

		//! \param other Kopiowana infroamcja o typie
		ExtendedTypeInfo& operator =(const ExtendedTypeInfo & other);

		//! \param obj Obiekt porównywany
		//! \return Czy typy są identyczne
		bool compare(const ExtendedTypeInfo& obj) const;

		//! \return Rozmiar zajmowany przez ten typ
		std::size_t size() const;
	};

    using TypeInfoList = std::list<TypeInfo>;
    using TypeInfoSet = std::set<TypeInfo>;
	using TypeInfoVector = std::vector<TypeInfo>;
}

#endif  // HEADER_GUARD_UTILS__TYPEINFO_H__
