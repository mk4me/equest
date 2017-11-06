/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   23:11
    filename: IStreamManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTREAMMANAGERREADER_H__
#define HEADER_GUARD___ISTREAMMANAGERREADER_H__

#include <iostream>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>

namespace core {

	//! Interfejs zapewniaj¹cy dostêp do danych strumieniowych zarzazanych przez aplikacjê
	class IStreamManagerReader
	{
	public:

		//! Lista danych strumieni
		typedef std::list<std::string> StreamsList;

		class IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			//! \param files Zbiór plików ktrymi aktualnie zarz¹dza ten DataManager
			virtual void getStreams(StreamsList & streams) const = 0;

			//! \param stream Nazwa strumienia
			//! \return Prawda jeœli strumieñ jest zarz¹dzany przez ten DM
			virtual const bool isManaged(const std::string & stream) const = 0;

			//! \param stream Strumieñ który weryfikujemy czy jest w pe³ni za³adowany
			//! \return Prawda jeœli strumieñ jest w pe³ni za³adowany
			virtual const bool isLoadedCompleately(const std::string & stream) const = 0;

			//! \param stream Nazwa strumienia
			//! \return Mapa obiektów wzglêdem plików z których pochodza
			virtual void getObjects(const std::string & stream, ConstVariantsList & objects) const = 0;
		};

		//! Typ zmian danych w managerze
		enum ModificationType {
			ADD_STREAM,				//! Dodajemy strumieñ do DM
			REMOVE_STREAM,			//! Usuwamy strumieñ z DM
			REMOVE_UNUSED_STREAM	//! Usuwamy strumieñ dla którego nie ma ju¿ danych w Memory DM
		};

		//! Obiekt opisuj¹cy zmianê w DM
		struct Change
		{
			std::string stream;				//! Strumieñ
			ModificationType modyfication;	//! Typ zmiany na strumieniu
		};

		//! Agregat zmian w DM
		typedef std::list<Change> ChangeList;

		//! Interfejs obserwatora zmian
		typedef IChangesObserver<ChangeList> IObserver;

		//! WskaŸnik na obiek obserwuj¹cy zmiany
		typedef utils::shared_ptr<IObserver> ObserverPtr;
		//! WskaŸnik transakcji
		typedef utils::shared_ptr<IOperations> TransactionPtr;

	public:

		//! \param protocol Nazwa protoko³y na którym dostêpny jest strumieñ
		//! \param source Nazwa strumienia z pominiêciem protoko³u
		//! \return Pe³na nazwa strumienia
		inline static const std::string streamName(const std::string & protocol,
			const std::string & source)
		{
			return protocol + "://" + source;
		}

		//! \param streamWatcher Rejestrowany obiekt obserwuj¹cy zmiany managera strumieni
		virtual void addObserver(const ObserverPtr & streamWatcher) = 0;
		//! \param streamWatcher Wyrejestrowywany obiekt obserwuj¹cy zmiany managera strumieni
		virtual void removeObserver(const ObserverPtr & streamWatcher) = 0;
		//! \return Nowa transakcja
		virtual const TransactionPtr transaction() const = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMMANAGERREADER_H__
