#ifndef HEADER_GUARD_CORE_IDATAMANAGERREADER_H__
#define HEADER_GUARD_CORE_IDATAMANAGERREADER_H__

#include <list>
#include <corelib/BaseDataTypes.h>
#include <corelib/Variant.h>
#include <corelib/VariantsCollection.h>
////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
	
	//! Podstawowe operacje związane z danymi - pobieranie danych, informacje o hierarchi typów danych, informacje o wspieranych typach danych
    class IDataManagerReader
    {
	public:

		class IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}
			//! \param objects [out] Wszystkie obiekty DM
			virtual void getObjects(ConstVariantsList & objects) const = 0;
			//! \param objects [out] Obiekty pasujące do zapytania
			//! \param type Typ obniektów o które pytamy
			//! \param exact Czy typ musi być zgodny czy moga to być tez typy pochodne
			//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
			virtual void getObjects(ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const = 0;
			//! \param objects [out] Obiekty zarządzane przez DM
			//! \param initialzie Czy automatycznie zainicjalizować wszystkie obiekty? UWAGA - te, których się nie udało zainicjalizować nie zostaną zwrócone - DM usunie je jako popsute dane
			virtual void getObjects(VariantsCollection& objects) const = 0;
			//! \param object Obiekt który sprawdzamy pod kątem zarządzania przez DM
			//! \return Prawda jesli dany OW jest zarządzany przez DM
			virtual const bool isManaged(const VariantConstPtr & object) const = 0;
			//! \param type Czy DM aktualnie zarządza jakimkolwiek obiektem danego typu
			//! \param exact Czy typ musi się zgadzać czy może być pochodny
			//! \return Prawda jesli obiekt danego typu jest zarządzany przez DM
			virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const = 0;
		};

		//! Typ zmian danych w managerze
		enum ModificationType {
			ADD_OBJECT,		//! Dodajemy obiekt do DM
			REMOVE_OBJECT,	//! Usuwamy obiekt z DM
			UPDATE_OBJECT	//! Aktualizujemy obiekt w DM
		};

		//! Obiekt opisujący zmianę w DM
		struct Change
		{
			VariantConstPtr previousValue;	//! Kopia OW przed zmianą, nie jest zarejestrowana w DM
			VariantConstPtr currentValue;	//! Aktualny OW zarejestrowany w DM
			utils::TypeInfo type;			//! Typ OW
			ModificationType modyfication;	//! Typ zmiany na OW
		};

		//! Agregat zmian w DM
		typedef std::list<Change> ChangeList;
		//! Typ obserwatora zmian danych
		typedef IChangesObserver<ChangeList> IObserver;
		//! Wskaźnik na obiek obserwujący zmiany
		typedef utils::shared_ptr<IObserver> ObserverPtr;
		//! Wskaźnik transakcji czytania danych
		typedef utils::shared_ptr<IOperations> TransactionPtr;

    public:
		//! Destruktor wirtualny
		virtual ~IDataManagerReader() {}

		//! Dodaje obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void addObserver(const ObserverPtr & objectWatcher) = 0;
		//! Usuwa obserwatora DM
		//! \param objectWatcher Obiekt obserwujący DM
		virtual void removeObserver(const ObserverPtr & objectWatcher) = 0;
		//! \return Transakcja odczytu danych
		virtual TransactionPtr transaction() const = 0;
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif // HEADER_GUARD_CORE_IDATAMANAGERREADER_H__
