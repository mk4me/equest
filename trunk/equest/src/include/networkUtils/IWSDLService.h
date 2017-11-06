/********************************************************************
created:  2014/05/14	20:06:38
filename: IWSDLService.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__
#define __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__

#include <string>
#include <utils/SmartPtr.h>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace networkUtils
{
	//! Wyjątek obsługi serwisu wsdl
	class WSDLServiceException : public std::runtime_error
	{
	public:
		//! Typ wyjątku
		enum ExceptionType
		{
			Unknown,		//! Nieznany typ
			Security,		//! Wyjątek bezpieczeństwa
			Operation,		//! Wyjątek operacji
			OperationValue,	//! Wyjątek parametru operacji
			Initialization,	//! Wyjątek inicjalizacji usługi
			Response,		//! Wyjątek odpowiedzi
			Invoke			//! Wyjątek wykonania usługi
		};

	public:
		//! \param type Typ wyjatku usługi
		WSDLServiceException(const ExceptionType type = Unknown) throw() : std::runtime_error(std::string()), type_(type) {}
		//! \param exception Wujatek który kopiujemy
		WSDLServiceException(const WSDLServiceException & exception) throw() : std::runtime_error(exception), type_(exception.type_) {}
		//! \param exception Opis wyjatku
		//! \param type Typ wyjatku usługi
		WSDLServiceException(const char * exception, const ExceptionType type = Unknown) throw() : std::runtime_error(exception), type_(type) {}

		//! \return Typ wyjątku
		inline ExceptionType type() const { return type_; }

	private:
		//! Typ wyjątku
		const ExceptionType type_;
	};

	class IWSDLService
	{
	public:
		//! Desturktor wirtualny
		virtual ~IWSDLService() {}
		//! \param operation Metoda serwisu do wywołania
		virtual void setOperation(const std::string & operation) = 0;
		//! \param name Nazwa wartości do ustawienia
		//! \param value Wartość zmiennej
		virtual void setValue(const std::string & name, const std::string & value) = 0;
		//! Wykonuje operację na serwisie webowym
		//! \param process Czy odpowiedź ma być przetworzona i można o rezultaty pytać metodami getValue
		//! W przeciwnym wypadku należy samemu parsować odpowiedź
		virtual void invoke(const bool process = false) = 0;
		//! \return Pełna odpowiedź serwisu webowego w formacie html/xml
		virtual std::string xmlResponse() const = 0;
		//! \param name Nazwa wartości którą chcemy pobrać
		//! \return Wskaźnik do wartości, nullptr jeśli nie ma takiej wartości, wskaxnik pozostaje pod kontrolą implementacji IWSDLService
		virtual const void * getValue(const std::string & name) const = 0;
		//! \tparam Typ jaki chemy ustawić jako parametr
		//! \param param Nazwa parametru dla którego ustawiamy wartość wywołania
		//! \param value Wartośc dla zadanego parametru
		template<class T>
		void setValue(const std::string& param, const T& value)
		{
			try{
				setValue(param, boost::lexical_cast<std::string>(value));
			}
			catch (std::exception & e){
				throw WSDLServiceException(e.what(), WSDLServiceException::OperationValue);
			}
			catch (...){
				throw WSDLServiceException("Unknown set operation value error", WSDLServiceException::OperationValue);
			}
		}
		//! \tparam Typ jaki chemy pobrać
		//! \param param Nazwa parametru dla którego pobieramy wartość jeśli wykonywaliśmy zapytanie z parsowanie
		//! \param value [out] Wartośc dla zadanego parametru
		template<class T>
		bool getValue(const std::string & name, T & value) const
		{
			const void * ret = getValue(name);
			if (ret != nullptr){
				value = *((T*)ret);
				return true;
			}

			return false;
		}
	};

	DEFINE_SMART_POINTERS(IWSDLService);
}

#endif	// __HEADER_GUARD_NETWORKUTILS__IWSDLSERVICE_H__
