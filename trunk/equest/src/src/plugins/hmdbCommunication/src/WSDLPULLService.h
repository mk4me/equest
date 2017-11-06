/********************************************************************
	created:  2014/05/27	17:51:37
	filename: WSDLPULLService.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__WSDLPULLSERVICE_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__WSDLPULLSERVICE_H__

#include <schemaparser/Schema.h>
#include <networkUtils/IWSDLService.h>

using namespace networkUtils;

namespace hmdbCommunication
{
	template<typename Invoker>
	class WSDLPULLServiceT : public IWSDLService
	{
	public:
		WSDLPULLServiceT(Invoker * invoker) : invoker_(invoker), exception_(false)
		{
			if (invoker == nullptr){
				throw WSDLServiceException("Uninitialized invoker", WSDLServiceException::Initialization);
			}
		}

		virtual ~WSDLPULLServiceT()
		{

		}

		void setOperation(const std::string & operation)
		{
			bool ok = true;
			try{
				ok = invoker_->setOperation(operation);
			}
			catch (std::exception & e){
				throw WSDLServiceException(e.what(), WSDLServiceException::Operation);
			}
			catch (...){
				throw WSDLServiceException("Unknown operation error", WSDLServiceException::Operation);
			}

			if (ok == false){
				throw WSDLServiceException("Could not find particular operation in service - probably wrong service address, operation name or server verification settings", WSDLServiceException::Operation);
			}

			operationName_ = operation;
		}

		void setValue(const std::string & name, const std::string & value)
		{
			try{
				invoker_->setValue(name, value);
			}
			catch (std::exception & e){
				throw WSDLServiceException(e.what(), WSDLServiceException::OperationValue);
			}
			catch (...){
				throw WSDLServiceException("Unknown operation value error", WSDLServiceException::OperationValue);
			}
		}

		void invoke(const bool process)
		{
			exception_ = false;
			try{
				invoker_->invoke(0, process);
			}
			catch (std::exception & e){
				exception_ = true;
				throw WSDLServiceException(e.what(), WSDLServiceException::Invoke);
			}
			catch (...){
				exception_ = true;
				throw WSDLServiceException("Unknown invoke error", WSDLServiceException::Invoke);
			}

			std::string::size_type s = std::string::npos;
			std::string::size_type e = std::string::npos;			

			//tutaj weryfikujê czy by³ jakis wyj¹tek - jeœli tak to go zg³oszê
			//pobieram pe³n¹ odpowiedŸ
			auto const & resp = invoker_->getXMLResponse();

			s = resp.find("<faultstring", 0);
			if (s != std::string::npos){
				exception_ = true;
				s = resp.find('>', s + 12);
				e = resp.find("</faultstring>", s);
				if (e == std::string::npos){
					e = resp.size();
				}

				if (resp.find("a:InvalidSecurityToken", 0) != std::string::npos){					
					throw WSDLServiceException(resp.substr(s, e - s).c_str(), WSDLServiceException::Security);
				}
				else{					
					throw WSDLServiceException(resp.substr(s, e - s).c_str(), WSDLServiceException::Response);
				}
			}
		}

		std::string xmlResponse() const
		{
			//musze przetworzyæ odpowiedŸ - chcê w niej mieæ tylko to co istotne bez zbêdnych nag³ówków SOAP i bezpieczeñstwa
			auto const & resp = invoker_->getXMLResponse();

			std::string::size_type s = 0;
			std::string::size_type e = resp.size();

			if (exception_ == false){

				s = resp.find("<" + operationName_ + "Result", 0);
				if (s != std::string::npos){
					e = resp.find("</" + operationName_ + "Result>", s);
					if (e == std::string::npos){
						e = resp.size();
					} else{
						e += 9 + operationName_.size();
					}
				}
			}

			return resp.substr(s, e - s);
		}

		const void * getValue(const std::string & name) const
		{
			Schema::Type type;
			return invoker_->getValue(name, type);
		}

	private:
		//! Invoker Wsdlpull`a
		utils::shared_ptr<Invoker> invoker_;
		//! Czy by³ wyj¹tek
		bool exception_;
		//! Nazwa operacji
		std::string operationName_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__WSDLPULLSERVICE_H__
