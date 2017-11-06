/********************************************************************
    created:  2013/01/09
    created:  9:1:2013   8:04
    filename: IStreamDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTREAMDATAMANAGER_H__
#define HEADER_GUARD___ISTREAMDATAMANAGER_H__

#include <utils/SmartPtr.h>
#include <corelib/ITransaction.h>
#include <corelib/IStreamManagerReader.h>
#include <corelib/IDataManager.h>
#include <corelib/IParser.h>

namespace core {

	//! Manager udostêpniaj¹cy operacje na strumieniach zarz¹dzanych przez DM w oparciu o zarejestrowane parsery i rozszerzenia
	class IStreamDataManager : public IStreamManagerReader
	{
	public:

		//! Typ strumienia dla grabbera
		typedef plugin::IStreamParser::IStreamPtr IStreamPtr;

		//! Interfejs obiektu dostarczaj¹cego strumieñ
		class IStreamGrabber
		{
		public:
			//! Destruktor wirtualny
			virtual ~IStreamGrabber() {}
			//! \return Strumieñ
			virtual const IStreamPtr stream() const = 0;
			//! \return Nazwa, identyfikator strumienia
			virtual const std::string name() const = 0;
		};

		//! WskaŸnik obiektu dostarczaj¹cego struemienia
		typedef utils::shared_ptr<IStreamGrabber> StreamGrabberPtr;

		//! Zbiór operacji na strumieniach obs³ugiwanych przez aplikacjê
		class IOperations
		{
		public:
			//! Destrutkor wirtualny
			virtual ~IOperations() {}

			//! \param stream Strumieñ dodawany do managera
			//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
			virtual void addStream(const StreamGrabberPtr streamGrabber) = 0;
			//! \param stream Strumieñ usuwany z DM
			//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
			virtual void removeStream(const std::string & stream) = 0;
			//! \param stream Strumieñ dodawany do managera
			//! \return Prawda jeœli pomyœlnie dodano strumieñ
			virtual const bool tryAddStream(const StreamGrabberPtr streamGrabber) = 0;
			//! \param stream Strumieñ usuwany z managera
			//! \return Prawda jeœli pomyœlnie usuniêto strumieñ
			virtual const bool tryRemoveStream(const std::string & stream) = 0;
		};

		//! Interfejs transakcji managera strumieni
		class ITransaction : public core::ITransaction, public IOperations, public IStreamManagerReader::IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITransaction() {}
		};

		//! Typ transkacji na strumieniach - dzia³a w oparciu o RAII -> próbuje "commitowaæ" zmiany przy niszczeniu obiektu transakcji
		typedef utils::shared_ptr<ITransaction> TransactionPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IStreamDataManager() {}

		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMDATAMANAGER_H__
