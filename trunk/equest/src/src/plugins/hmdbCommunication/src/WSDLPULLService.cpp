#include "CommunicationPCH.h"
#include "WSDLPULLService.h"
#include <wsdlparser/WsdlInvoker.h>

using namespace hmdbCommunication;

template<typename Invoker>
class WSDLPULLServiceImpl : public networkUtils::IWSDLService
{
public:
	WSDLPULLServiceImpl(CURL * curl) : invoker_(curl)
	{

	}

	virtual ~WSDLPULLServiceImpl()
	{

	}

	void setOperation(const std::string & operation)
	{
		invoker_->setOperation(operation);
	}

	void setValue(const std::string & name, const std::string & value)
	{
		invoker_->setValue(name, value);
	}

	void invoke(const bool process = false)
	{
		invoker_->invoke(process);
	}

	const std::string xmlResponse() const
	{
		return invoker_->xmlResponse();
	}

	const void * getValue(const std::string & name) const
	{
		return invoker_->getValue(name);
	}

private:
	//! Invoker Wsdlpull`a
	Invoker invoker_;
};
