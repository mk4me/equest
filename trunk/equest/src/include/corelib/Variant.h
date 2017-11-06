/********************************************************************
	created:  2014/05/05	16:28:08
	filename: Variant.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_CORE__VARIANT_H__
#define __HEADER_GUARD_CORE__VARIANT_H__

#include <corelib/Export.h>
//#include <utils/SmartPtr.h>
//#include <utils/Utils.h>
#include <utils/pointer_traits.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <mutex>
#include <list>
#include <set>
#include <vector>

namespace core
{
	//! Forward declaration
	class Variant;

	//! Klasa realizujšca inicjalizację danych dla OW
	class IVariantInitializer
	{
	public:
		//! Destruktor wirtualny
		virtual ~IVariantInitializer() {}
		//! \param object Obiekt który inicjujemy
		virtual void initialize(Variant * object) = 0;
		//! \return Kopia inicjalizatora
		virtual IVariantInitializer * clone() const = 0;
		//! \param initializer Inicjalizator z ktorym się porównujemy
		//! \return Czy inicjalizatory dadzš takie same dane (częc wspólna)
		virtual bool isEqual(const IVariantInitializer & initializer) const { return false; }
	};

	typedef utils::shared_ptr<IVariantInitializer> VariantInitializerPtr;
	typedef utils::shared_ptr<const IVariantInitializer> VariantInitializerConstPtr;
	typedef utils::weak_ptr<IVariantInitializer> VariantInitializerWeakPtr;
	typedef utils::weak_ptr<const IVariantInitializer> VariantInitializerConstWeakPtr;

	DEFINE_SMART_POINTERS(Variant);
	typedef std::list<VariantPtr> VariantsList;
	typedef std::list<VariantWeakPtr> WeakVariantsList;
	typedef std::list<VariantConstPtr> ConstVariantsList;
	typedef std::list<VariantConstWeakPtr> ConstWeakVariantsList;
	typedef std::set<VariantPtr> VariantsSet;
	typedef std::set<VariantConstPtr> ConstVariantsSet;
	typedef std::set<VariantWeakPtr> WeakVariantsSet;
	typedef std::set<VariantConstWeakPtr> ConstWeakVariantsSet;
	typedef std::vector<VariantPtr> VariantsVector;
	typedef std::vector<VariantConstPtr> ConstVariantsVector;

	class CORELIB_EXPORT Variant
	{
	private:

		//! Typ przechowujšcy metadane OW
		typedef std::map<std::string, std::string> Metadata;
		//! Typ loklanej synchronizacji
		typedef std::lock_guard<std::recursive_mutex> ScopedLock;

	public:

		//! Proxy dostarczajšce danych i synchronizujšce je
		struct CORELIB_EXPORT data_t
		{
		private:
			//! Variant dla którego będziemy wycišgać dane
			const Variant* variant;

		public:
			//! \param variant Variant z którego wycišgamy dane
			data_t(const Variant* variant);
			//! Destruktor niepolimorficzny!!
			~data_t();
			//! Operator wycišgajšcy dane w formie stałej
			utils::ObjectWrapperConstPtr operator->() const;
		};

		friend struct data_t;

		//! Typ klonowania
		typedef utils::ObjectWrapper::CloneOp CloneOp;

	private:
		//! Inicjalizator danych
		VariantInitializerPtr initializer_;
		//! Czy obiekt był już inicjalizowany
		mutable bool initialized_;
		//! Metadane w formie klucz -> wartoć tekstowo
		utils::shared_ptr<Metadata> metadata_;
		//! Faktyczne dane
		utils::ObjectWrapperPtr wrapper_;
		//! Obiekt synchronizujšcy
		mutable std::recursive_mutex sync_;
		//! Czy jestemy w trakcie inicjalizacji
		mutable bool initializing_;

	private:

		//! \param ow Kopiowany obiekt
		//! \param co Typ klonowania
		Variant(const Variant & ow, const CloneOp co);

		//! \param wrapper Placeholder dla danych
		Variant(const utils::ObjectWrapperPtr & wrapper);

	public:

		//! \param wrapper Wrapowane dane
		//! \return Pełny wrapper z obsługa metadanych i leniwš inicjalizacjš
		static VariantPtr create(const utils::ObjectWrapperPtr & wrapper);

		//! \tparam T Typ dla jakiego tworzymy wrapper
		//! \param dummy Nieuzywany
		//! \return Pełny wrapper z obsługa metadanych i leniwš inicjalizacjš
		template<typename T>
		static VariantPtr create(const T * /*dummy*/ = nullptr)
		{
			return VariantPtr(new Variant(utils::ObjectWrapper::create<T>()));
		}

		//! \tparam T Typ obiektu dla ktorego chcemy utworzych OW
		//! \tparam dummy Weryfikacja warunków metody
		template < typename T, ENABLE_IF(utils::ObjectWrapperTraits<T>::isDefinitionVisible)>
			//! \param value Wartość z jaką chcemy utowrzyć OW
			//! \return Wrapper obiektu.
			static inline VariantPtr wrap(const typename utils::ObjectWrapperTraits<T>::Ptr value)
		{			
			return VariantPtr(new Variant(utils::ObjectWrapper::wrap<T>(value)));
		}

		//! \tparam T Typ obiektu dla ktorego chcemy utworzych OW
		//! \tparam dummy Weryfikacja warunków metody
		template < typename T, ENABLE_IF(utils::is_general_pointer<T>::value)>
			//! \param value Wartość z jaką chcemy utowrzyć OW
			//! \return Wrapper obiektu.
			static inline VariantPtr wrap(const T & value)
		{
			return wrap<typename utils::pointed_type<T>::type>(value);
		}

		//! \tparam Ptr
		//! \param object
		template <typename Ptr>
		void set(const Ptr& object)
		{
			ScopedLock lock(sync_);
			if (!wrapper_->trySet(object)) {
				throw std::bad_cast();
			}

			tryResetInitializer();
		}

		//! \param wrapper Obiekt z danymi
		void set(const utils::ObjectWrapperPtr & wrapper);

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłšcznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <typename Ptr>
		bool tryGet(Ptr& object, bool exact = false)
		{
			innerInitialize();
			return wrapper_->tryGet(object, exact);
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
		//! \param object Rezultat.
		//! \param exact Czy ma być tylko i wyłšcznie ten typ czy też może być rzutowanie w dół?
		//! \return Sukces/porażka.
		template <typename Ptr>
		bool tryGet(Ptr& object, bool exact = false) const
		{
			innerInitialize();
			return utils::ObjectWrapperConstPtr(wrapper_)->tryGet(object, exact);
		}

		//! \param exact
		//! \return
		utils::ObjectWrapper::get_t get(const bool exact = false);
		//! \param exact
		//! \return
		const utils::ObjectWrapper::get_t get(const bool exact = false) const;
		//! \return
		void * getRawPtr();
		//! \return
		const void * getRawPtr() const;

		//! Zeruje dane
		void resetData();

		//! Destruktor niepolimorficzny!!
		~Variant();

		//! \return Pusty obiekt dla danych mojego typu
		VariantPtr create() const;
		//! \param key Klucz metadanych który sprawdzamy czy istnieje
		//! \return Czy dany klucz istnieje
		bool existMetadata(const std::string & key) const;
		//! \param key Klucz metadanych który sprawdzamy czy istnieje
		//! \param val [out] Wartoć dla zadanego klucza, w przypadku braku klucza zmienna nie zostanie nadpisana
		//! \return Czy udalo sie pobrac wartosc klucza
		bool getMetadata(const std::string & key, std::string & val) const;
		//! \param key Klucz metadanych
		//! \param val Wartoć dla zadanego klucza
		void setMetadata(const std::string & key, const std::string & val);
		//! \param key Klucz ktorego wpis usuwamy
		void removeMetadata(const std::string & key);
		//! \param srcWrapper Zrodlowy OW z ktorego kopiujemy wpisy
		void copyMetadata(const Variant & srcWrapper);
		//! Metoda usuwa wszystkie metadane
		void clearMetadata();

		//! \param initializer Obiekt leniwie inicjujšcy wartoć OW
		void setInitializer(const VariantInitializerPtr & initializer);
		//! \return Obiekt leniwie inicjujšcy wartoć OW
		VariantInitializerPtr initializer();
		//! \return Obiekt leniwie inicjujšcy wartoć OW
		VariantInitializerConstPtr initializer() const;
		//! \return Czy obiekt był inicjalizowany
		bool initialized() const;
		//! Metoda próbuje inicjalizować dane
		void tryInitialize();
		//! Metoda ponownie inicjuje dane
		void forceInitialize();
		//! \return Dostęp do danych do odczytu
		const data_t data() const;
		//! \return Dostęp do danych do odczytu po inicjalizacji
		const data_t initializedData() const;
		//! \param co Typ klonowania
		//! \return Kopia naszego obiektu
		VariantPtr clone(const CloneOp co = utils::ObjectWrapper::DeepClone) const;
		//! \param dest [out] Obiekt docelowy
		//! \param co Typ klonowania
		void clone(Variant & dest, const CloneOp co = utils::ObjectWrapper::DeepClone) const;
		//! Zamienia zawartosc OW jesli to możliwe
		//! \param ow Obiekt z którym zamienimy przechowywane wartoci
		void swap(Variant & ow);
		//! \param obj Obiekt z którym się porównujemy
		//! \return Czy obiekty sš takie same - trzymajš te same dane
		bool isEqual(const Variant & obj) const;
		//! \param srcWrapper Zrodlowy OW z ktorego kopiujemy dane
		void copyData(const Variant & srcWrapper);

	private:
		//! Próbuje resetować inicjalizator kiedy ustawia się bezporednio dane
		void tryResetInitializer();
		//! Metoda próbuje inicjowac obiekt
		void innerInitialize() const;
		//! Prywatny operator kopiowania - nie można kopiować sync_
		Variant & operator=(const Variant &);
	};
}

////////////////////////////////////////////////////////////////////////////////
// Operatory porównania dla OW
////////////////////////////////////////////////////////////////////////////////

//! Operator poównania realizowany w oparciu o metodę isEqual
bool operator==(const core::Variant & a, const core::Variant & b);

//! Operator różnoci realizowany w oparciu o operator porównania
bool operator!=(const core::Variant & a, const core::Variant & b);

#endif	// __HEADER_GUARD_CORE__VARIANT_H__
