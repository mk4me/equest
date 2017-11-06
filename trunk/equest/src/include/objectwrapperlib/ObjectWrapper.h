/********************************************************************
created:  2014/04/29	14:32:28
filename: ObjectWrapper.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__
#define __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__

#include <set>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <utils/SmartPtr.h>
#include <utils/pointer_traits.h>
#include <objectwrapperlib/ObjectWrapperTraits.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
	////////////////////////////////////////////////////////////////////////////////
	class ObjectWrapper;

	DEFINE_SMART_POINTERS(ObjectWrapper);

	using Objects = std::set<ObjectWrapperPtr>;
	using ConstObjects = std::set<ObjectWrapperConstPtr>;

	using ObjectsList = std::list<ObjectWrapperPtr>;
	using ConstObjectsList = std::list<ObjectWrapperConstPtr>;

	using ObjectsVector = std::vector<ObjectWrapperPtr>;
	using ConstObjectsVector = std::vector<ObjectWrapperConstPtr>;

	//! Forward declaration
	template<typename T>
	class ObjectWrapperT;

	//! Baza dla typu wrapującego jakiś obiekt. Poza trzymaniem metadanych klasy pochodne
	//! trzymają referencje do obiektów.
	class ObjectWrapper
	{
	public:

		//! Para opisująca typ
		using TypeInfoPair = std::pair<TypeInfo, TypeInfo>;

		//! Typ klonowania
		enum CloneOp {
			DeepClone,		//! Pełne, głębokie
			ShallowClone	//! Płytkie
		};

		//! Lista typów.
		using Types = TypeInfoList;

		//! Return type resolver
		struct get_t
		{
			ObjectWrapper* wrapper;
			const ObjectWrapper* constWrapper;
			bool exact;

			template <class Ptr>
			inline operator Ptr()
			{
				Ptr result;
				if (wrapper->tryGet(result, exact)) {
					return result;
				}
				else {
					//throw std::bad_cast("Invalid cast");
					throw std::bad_cast();
				}
			}

			template <class Ptr>
			inline operator Ptr() const
			{
				Ptr result;
				if (constWrapper->tryGet(result, exact)) {
					return result;
				}
				else {
					//throw std::bad_cast("Invalid cast");
					throw std::bad_cast();
				}
			}
		};

	public:

		//! \tparam Ptr Typ wskaznika o który pytamy
		template <class Ptr>
		inline operator Ptr()
		{
			Ptr result;
			tryGet(result, false);
			return result;
		}

		//! \tparam Ptr Typ wskaznika o który pytamy
		template <class Ptr>
		inline operator Ptr() const
		{
			Ptr result;
			tryGet(result, false);
			return result;
		}

		//! \tparam T Typ obiektu dla ktorego chcemy utworzych OW
		//! \param dummy Parametr nie powinien być nigdy uzywany
		//! \return Wrapper obiektu.
		template <class T>
		static inline ObjectWrapperPtr create(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc uzywany");
			return ObjectWrapperT<T>::create();
		}

		//! \tparam T Typ obiektu dla ktorego chcemy utworzych OW
		//! \param value Wartość z jaką chcemy utowrzyć OW
		//! \return Wrapper obiektu.
		template <class T, ENABLE_IF(utils::ObjectWrapperTraits<T>::isDefinitionVisible)>
		static inline ObjectWrapperPtr wrap(typename ObjectWrapperTraits<T>::Ptr value)
		{
			return ObjectWrapperT<T>::wrap(value);
		}

		//! \tparam T Typ obiektu dla ktorego chcemy utworzych OW
		//! \tparam dummy Weryfikacja warunków metody
		template < typename T, ENABLE_IF(utils::is_general_pointer<T>::value)>
		//! \param value Wartość z jaką chcemy utowrzyć OW
		//! \return Wrapper obiektu.
		static inline ObjectWrapperPtr wrap(const T & value)
		{
			return wrap<typename utils::pointed_type<T>::type>(value);
		}

		//! \tparam T Typ obiektu ktorego nazwe OW chcemy pobrac
		//! \return Nazwa dla zadanego typu.
		template <class T>
		static std::string getClassName(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			return ObjectWrapperT<T>::className();
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		bool tryGet(Ptr& object, bool exact = false)
		{
			TypeInfo ptrInfo(typeid(Ptr));
			auto p = getPtrTypeInfo();
			if (ptrInfo == p.first || ptrInfo == p.second) {
				return __tryUnpackData(&object, ptrInfo);
			}
			else if (exact == false && isPtrSupported(ptrInfo) == true) {
				return __tryUnpackBaseData(&object, ptrInfo);
			}
			else {
				return false;
			}
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <class Ptr>
		bool tryGet(Ptr& object, bool exact = false) const
		{
			static_assert((std::is_const<typename utils::pointed_type<Ptr>::type>::value), "Ta metoda mozna pobierac tylko obiekty typu const");

			TypeInfo ptrInfo(typeid(Ptr));
			if (ptrInfo == getPtrTypeInfo().second) {
				return __tryUnpackData(&object, ptrInfo);
			}
			else if (exact == false && isPtrSupported(ptrInfo) == true) {
				return __tryUnpackBaseData(&object, ptrInfo);
			}
			else {
				return false;
			}
		}

		//! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Return Type Resolver
		get_t get(bool exact = false);
		//! \tparam T Typ obiektu ktory chemy pobrac z OW
		//! \param dummy Pozostawić pusty.
		//! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
		template <typename T>
		typename ObjectWrapperT<T>::Ptr get(T* dummy = nullptr)
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			return static_cast<typename ObjectWrapperT<T>::Ptr>(get());
		}

		//! \tparam T Typ obiektu ktory chemy pobrac z OW
		//! \param dummy Pozostawić pusty.
		//! \return Wrappowany obiekt. Gdy wrapper jest innego typu niż parametr szablonu rzucany jest wyjątek.
		template <typename T>
		typename ObjectWrapperT<T>::ConstPtr get(T* dummy = nullptr) const
		{
			UTILS_ASSERT((dummy == nullptr), "Parametr nie powinien byc używany");
			typename ObjectWrapperT<T>::ConstPtr ret = get();
			return ret;
		}

		//! Pobiera obiekt z wrappera. W razie błędu rzuca bad_castem.
		//! \param exact Czy ma być tylko i wyłącznie ten typ czy też może być rzutowanie w dół?
		//! \return Return Type Resolver
		const get_t get(bool exact = false) const;
		//! Ustawia obiekt wrappera. W razie błędu rzuca bad_castem.
		//! \tparam Ptr Typ wskaznika ktory chcemy przypisac do OW
		//! \param object Obiekt.
		template <class Ptr>
		void set(const Ptr& object)
		{
			if (!trySet(object)) {
				throw std::bad_cast();
			}
		}

		//! Ustawia obiekt wrappera.
		//! \tparam Ptr Typ wskaznika ktory chcemy przypisac do OW
		//! \param object Obiekt.
		//! \return Sukces/porażka.
		template <class Ptr>
		bool trySet(const Ptr& object)
		{
			static_assert((!std::is_const<typename utils::pointed_type<Ptr>::type>::value), "Nalezy zapisywac dane bez modyfikatora const");
			if (getPtrTypeInfo().first == typeid(Ptr)) {
				__setData(&object);
				return true;
			}
			return false;
		}

		//! Pusty polimorficzny destruktor.
		virtual ~ObjectWrapper();
		//! \return Ilość referencji do naszych danych
		virtual int getReferenceCount() const = 0;
		//! \return Nazwa typu.
		virtual std::string getClassName() const = 0;
		//! \return Informacje o typie.
		virtual TypeInfo getTypeInfo() const = 0;
		//! \param type
		//! \return Czy obiekt wspiera określony typ?
		virtual bool isSupported(const TypeInfo& type) const;
		//! \param supported Lista wspieranych rozszerzeń.
		virtual void getSupportedTypes(Types& supported) const = 0;
		//! \param co Typ klonowania
		//! \return Klon bieżącego obiektu. Wewnętrzny wskaźnik również jest kopiowany.
		ObjectWrapperPtr clone(const CloneOp co = DeepClone) const;
		//! \param dest [out] Obiekt docelowy
		//! \param co Typ klonowania
		void clone(ObjectWrapper & dest, const CloneOp co = DeepClone) const;
		//! \param dest [out] Obiekt docelowy
		//! \param co Typ klonowania
		//! \return Czy udało się sklonować obiekt
		bool tryClone(ObjectWrapper & dest, const CloneOp co = DeepClone) const;
		//! \return Pusty OW dla danego typu
		virtual ObjectWrapperPtr create() const = 0;
		//! \return Surowa wartość wskaźnika danych przechowywanych w OW
		const void* getRawPtr() const;
		//! \return Surowa wartość wskaźnika danych przechowywanych w OW
		void* getRawPtr();
		//! \return Informacje o typie odpowiednio normalnego i stałego wskaźnika.
		virtual TypeInfoPair getPtrTypeInfo() const = 0;
		//! \param ptrInfo Informacja o wskaźniku
		//! \return Prawda jeśli wskaźnik można pobrać z danego OW
		virtual bool isPtrSupported(const TypeInfo & ptrInfo) const = 0;
		//! Zeruje obiekt
		void reset();
		//! Zamienia zawartosc OW jesli to możliwe
		//! \param ow Obiekt z którym zamienimy przechowywane wartości
		void swap(ObjectWrapper & ow);
		//! \param obj Obiekt z którym się porównujemy
		//! \return Czy obiekty są takie same - trzymają te same dane
		bool isEqual(const ObjectWrapper & obj) const;

	protected:

		//! \param className
		//! \param classId
		ObjectWrapper();
		//! Konstruktor kopiujący
		//! \param wrapper Obiekt ktory kopiujemy
		//ObjectWrapper(const ObjectWrapper & wrapper);

		ObjectWrapper(ObjectWrapper &&);

	private:

		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual bool __tryUnpackData(void * object, const TypeInfo & ptrType);
		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual bool __tryUnpackData(void * object, const TypeInfo & ptrType) const;
		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual bool __tryUnpackBaseData(void * object, const TypeInfo & ptrType);
		//! \param object Wskaźnik z którego rozpakowujemy dane
		//! \param ptrType Typ wskaźnika do któego chcemy przypisać dane
		virtual bool __tryUnpackBaseData(void * object, const TypeInfo & ptrType) const;
		//! \return Surowy wskaźnik do przechowywanego obiektu
		virtual const void* __getRawPtr() const = 0;
		//! \return Surowy wskaźnik do przechowywanego obiektu
		virtual void* __getRawPtr() = 0;
		//! \param dest Obiekt docelowy do którego kopiujemy dane
		virtual void __clone(ObjectWrapper & dest, const CloneOp co) const = 0;
		//! Ważne!! Swap może rzucać wyjątkiem!!
		//! \param ow Object wrapper z ktorym zamieniamy zawartość
		virtual void __swap(ObjectWrapper & ow) = 0;
		//! Zeruje dane OW
		virtual void __reset() = 0;
		//! \return Czy udało się ustawić mądry wskaźnik?
		virtual void __setData(const void * object) = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual bool __tryGetData(void * object, const TypeInfo & ptrType) = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual bool __tryGetData(void * object, const TypeInfo & ptrType) const = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual bool __tryGetBaseData(void * object, const TypeInfo & ptrType) = 0;
		//! \return Czy udało się pobrać mądry wskaźnik?
		virtual bool __tryGetBaseData(void * object, const TypeInfo & ptrType) const = 0;
		//! \param obj Obiekt z którym się porównujemy
		//! \return Czy obiekty są takie same - trzymają te same dane
		virtual bool __isEqual(const ObjectWrapper & obj) const = 0;
	};
	////////////////////////////////////////////////////////////////////////////////
} // namespace utils
  ////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////
  // Operatory porównania dla OW
  ////////////////////////////////////////////////////////////////////////////////

  //! Operator poównania realizowany w oparciu o metodę isEqual
bool operator==(const utils::ObjectWrapper & a, const utils::ObjectWrapper & b);

//! Operator różności realizowany w oparciu o operator porównania
bool operator!=(const utils::ObjectWrapper & a, const utils::ObjectWrapper & b);

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Makra dla definiowaia wrapperów
////////////////////////////////////////////////////////////////////////////////

// wciągamy implementację
#include <objectwrapperlib/ObjectWrapperT.h>

// Makro tworzące specjalizację ObjectWrapperT. Musi występować w globalnym
// namespace. Drugim parametrem może być dowolny typ mający cechy ptrPolicy,
// wbudowane: PtrPolicyRaw, PtrPolicyStd, PtrPolicyBoost, PtrPolicyOSG
#define DEFINE_WRAPPER(typeT, ptrPolicyT, clonePolicyT)\
namespace utils {\
	__DEFINE_WRAPPER_META(typeT, ptrPolicyT, clonePolicyT)\
	__DEFINE_WRAPPER_CLASS(typeT)\
}

// Tutaj PtrPolicy i ClonePolicy są dziedziczone po typie bazowym,
// dla któego wcześniej musiał być zdefiniowany wrapper
#define DEFINE_WRAPPER_INHERITANCE(typeT, baseTypeT)\
namespace utils {\
	__DEFINE_WRAPPER_DERIVED_META(typeT, baseTypeT)\
	__DEFINE_WRAPPER_CLASS(typeT)\
}

#endif  // __HEADER_GUARD_UTILS__OBJECTWRAPPER_H__
