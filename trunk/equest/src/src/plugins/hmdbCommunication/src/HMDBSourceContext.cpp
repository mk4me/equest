#include "CommunicationPCH.h"
#include "HMDBSourceContext.h"
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <hmdbserviceslib/DateTimeUtils.h>
#include "HMDBSourceContextOperations.h"
#include <boost/lexical_cast.hpp>

using namespace hmdbCommunication;

HMDBSourceContext::HMDBSourceContext(IHMDBDataContextPtr dataContext,
	IHMDBLocalContextPtr localContext,
	IHMDBRemoteContextPtr remoteContext)
	: dataContext_(dataContext), localContext_(localContext), remoteContext_(remoteContext)
{

}

HMDBSourceContext::~HMDBSourceContext()
{
	
}

const IHMDBLocalContextPtr HMDBSourceContext::localContext()
{
	return localContext_;
}

const IHMDBLocalContextConstPtr HMDBSourceContext::localContext() const
{
	return localContext_;
}

const IHMDBRemoteContextPtr HMDBSourceContext::remoteContext()
{
	return remoteContext_;
}

const IHMDBRemoteContextConstPtr HMDBSourceContext::remoteContext() const
{
	return remoteContext_;
}

const IHMDBDataContextPtr HMDBSourceContext::dataContext()
{
	return dataContext_;
}

const IHMDBDataContextConstPtr HMDBSourceContext::dataContext() const
{
	return dataContext_;
}