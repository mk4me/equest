#include "CommunicationPCH.h"
#include "HMDBSource.h"
#include "HMDBSourceWidget.h"
#include "HMDBSourceContext.h"
#include "HMDBDataContext.h"
#include "HMDBLocalContext.h"
#include "HMDBRemoteContext.h"
#include "HMDBSession.h"
#include <corelib/IServiceManager.h>
#include <corelib/PluginCommon.h>
#include <plugins/hmdbCommunication/IHMDBService.h>
#include "HMDBService.h"
#include <hmdbserviceslib/AccountFactoryWS.h>
#include "ShallowCopyUtils.h"
#include "HMDBShallowCopyContext.h"
#include "HMDBSourceViewManager.h"
//#include "TreeBuilder.h"
#include <loglib/Exceptions.h>

using namespace hmdbCommunication;

HMDBSource::HMDBSource()
	: memoryDM(nullptr), streamDM(nullptr), fileDM(nullptr),
	hierarchyDM(nullptr),
	mainWidget(nullptr), viewManager_(new HMDBSourceViewManager)
{

}

HMDBSource::~HMDBSource()
{

}

void HMDBSource::init(core::IDataManager * memoryDM,
	core::IStreamDataManager * streamDM,
	core::IFileDataManager * fileDM,
	core::IDataHierarchyManager * hierarchyDM)
{
	this->memoryDM = memoryDM;
	this->streamDM = streamDM;
	this->fileDM = fileDM;
	this->hierarchyDM = hierarchyDM;
}

bool HMDBSource::lateInit()
{
	return true;
}

void HMDBSource::finalize()
{
	//TODO
	//zamknąć wszystkie widgety z kontekstami,
	//posprzątać po nich - usuwamy
}

void HMDBSource::update(double deltaTime)
{

}

QWidget * HMDBSource::getWidget()
{
	if (mainWidget == nullptr){
		mainWidget = new HMDBSourceWidget(this);
	}

	return mainWidget;
}

QWidget * HMDBSource::getControlWidget()
{
	return nullptr;
}

QWidget * HMDBSource::getSettingsWidget()
{
	return nullptr;
}

void HMDBSource::getOfferedTypes(utils::TypeInfoList & offeredTypes) const
{

}

const IHMDBSourceContextPtr HMDBSource::createSourceContext(IHMDBStoragePtr storage,
	const std::string & user, const std::string & password,
	IHMDBSessionPtr session)
{
	IHMDBSourceContextPtr ret;

	if (storage != nullptr && user.empty() == false && password.empty() == false){
		IHMDBDataContextPtr dc(new HMDBDataContext(storage, user, ShallowCopyUtils::userHash(user, password)));
		IHMDBRemoteContextPtr rc;
		if (session != nullptr){
			rc.reset(new HMDBRemoteContext(session, ShallowCopyUtils::userHash(user, password)));
		}

		IHMDBLocalContextPtr lc(new HMDBLocalContext(dc, memoryDM, streamDM));

		ret.reset(new HMDBSourceContext(dc, lc, rc));
	}

	return ret;
}

const IHMDBShallowCopyContextPtr HMDBSource::createShallowCopyContext(IHMDBSourceContextPtr sourceContext)
{
	IHMDBShallowCopyContextPtr ret;

	if (sourceContext != nullptr && sourceContext->dataContext() != nullptr
		&& sourceContext->localContext() != nullptr){
		IHMDBShallowCopyDataContextPtr scdc(new HMDBShallowCopyDataContext());
		IHMDBShallowCopyLocalContextPtr sclc(new HMDBShallowCopyLocalContext(scdc, sourceContext->localContext(), hierarchyDM));
		IHMDBShallowCopyRemoteContextPtr scrc;

		if (sourceContext->remoteContext() != nullptr){
			scrc.reset(new HMDBShallowCopyRemoteContext(scdc, sourceContext->remoteContext()));
		}

		ret.reset(new HMDBShallowCopyContext(scdc, sclc, scrc));
	}

	return ret;
}

const networkUtils::IWSDLServicePtr createSystemService(const std::string & url,
	const std::string & caPath)
{
	networkUtils::IWSDLServicePtr ret;

	auto sm = plugin::getServiceManager();
	if (sm != nullptr){
		auto hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);
		if (hmdbService != nullptr){

			ret = hmdbService->createHMDBSystemService(url, caPath, networkUtils::HVMatch);

			if (ret == nullptr){
				throw loglib::runtime_error("Could not create system service");
			}
		}
		else{
			throw loglib::runtime_error("Could not find HMDBService");
		}
	}
	else{
		throw loglib::runtime_error("Uninitialized service manager");
	}

	return ret;
}

void HMDBSource::registerAccount(const std::string & url,
	const std::string & login, const std::string & password,
	const std::string & name, const std::string & surname,
	const std::string & email, const bool multi,
	const std::string & caPath)
{

	auto s = createSystemService(url, caPath);

	if (multi == true){

		hmdbServices::MultiAccountFactoryWS(s).createUserAccount(login, email, password, name, surname, true);

	}
	else{

		hmdbServices::SingleAccountFactoryWS(s).createUserAccount(login, email, password, name, surname);

	}
}

void HMDBSource::activateAccount(const std::string & url,
	const std::string & login,
	const std::string & activationCode, const bool multi,
	const std::string & caPath)
{
	auto s = createSystemService(url, caPath);

	if (multi == true){

		hmdbServices::MultiAccountFactoryWS(s).activateUserAccount(login, activationCode, true);

	}
	else{

		hmdbServices::SingleAccountFactoryWS(s).activateUserAccount(login, activationCode);

	}
}

void HMDBSource::resetPassword(const std::string & url,
	const std::string & email, const bool multi,
	const std::string & caPath)
{
	auto s = createSystemService(url, caPath);

	if (multi == true){

		hmdbServices::MultiAccountFactoryWS(s).resetPassword(email, true);

	}
	else{

		hmdbServices::SingleAccountFactoryWS(s).resetPassword(email);

	}
}

IHMDBSourceViewManager * HMDBSource::viewManager()
{
	return viewManager_.get();
}

const IHMDBSourceViewManager * HMDBSource::viewManager() const
{
	return viewManager_.get();
}

const IHMDBSourceContextPtr HMDBSource::sourceContextForData(const core::VariantConstPtr data) const
{
	auto it = std::find_if(sourceContexts_.begin(), sourceContexts_.end(), [=](const IHMDBSourceContextConstWeakPtr wsc)
	{
		auto sc = wsc.lock();
		if (sc != nullptr){
			if (sc->localContext()->transaction()->isMyData(data) == true){
				return true;
			}
		}

		return false;
	});

	if (it != sourceContexts_.end()){
		return (*it).lock();
	}
	else{
		auto sc = shallowContextForData(data);
		if (sc != nullptr){
			return IHMDBSourceContextPtr(new HMDBSourceContext(sc->shallowCopyLocalContext()->localContext()->dataContext(), sc->shallowCopyLocalContext()->localContext(), sc->shallowCopyRemoteContext()->remoteContext()));
		}
	}

	return IHMDBSourceContextPtr();
}

const IHMDBShallowCopyContextPtr HMDBSource::shallowContextForData(const core::VariantConstPtr data) const
{
	auto it = std::find_if(shallowContexts_.begin(), shallowContexts_.end(), [=](const IHMDBShallowCopyContextConstWeakPtr wsc)
	{
		auto sc = wsc.lock();
		if (sc != nullptr){
			if (sc->shallowCopyLocalContext()->localContext()->transaction()->isMyData(data) == true){
				return true;
			}
		}

		return false;
	});

	if (it != shallowContexts_.end()){
		return (*it).lock();
	}

	return IHMDBShallowCopyContextPtr();
}

const IHMDBSourceContextPtr HMDBSource::sourceContextForData(const void * data) const
{
	auto it = std::find_if(sourceContexts_.begin(), sourceContexts_.end(), [=](const IHMDBSourceContextConstWeakPtr wsc)
	{
		auto sc = wsc.lock();
		if (sc != nullptr){
			if (sc->localContext()->transaction()->isMyData(data) == true){
				return true;
			}
		}

		return false;
	});

	if (it != sourceContexts_.end()){
		return (*it).lock();
	}
	else{
		auto sc = shallowContextForData(data);
		if (sc != nullptr){
			return IHMDBSourceContextPtr(new HMDBSourceContext(sc->shallowCopyLocalContext()->localContext()->dataContext(), sc->shallowCopyLocalContext()->localContext(), sc->shallowCopyRemoteContext()->remoteContext()));
		}
	}

	return IHMDBSourceContextPtr();
}

const IHMDBShallowCopyContextPtr HMDBSource::shallowContextForData(const void * data) const
{
	auto it = std::find_if(shallowContexts_.begin(), shallowContexts_.end(), [=](const IHMDBShallowCopyContextConstWeakPtr wsc)
	{
		auto sc = wsc.lock();
		if (sc != nullptr){
			if (sc->shallowCopyLocalContext()->localContext()->transaction()->isMyData(data) == true){
				return true;
			}
		}

		return false;
	});

	if (it != shallowContexts_.end()){
		return (*it).lock();
	}

	return IHMDBShallowCopyContextPtr();
}

void HMDBSource::registerSourceContext(const IHMDBSourceContextPtr sourceContext)
{
	sourceContexts_.push_back(sourceContext);
}

void HMDBSource::registerShallowContext(const IHMDBShallowCopyContextPtr shallowContext)
{
	shallowContexts_.push_back(shallowContext);
}

unsigned int HMDBSource::shallowContextsCount() const
{
	return shallowContexts_.size();
}

const IHMDBShallowCopyContextPtr HMDBSource::shallowContext(const unsigned int idx)
{
	if (idx >= shallowContexts_.size()){
		return IHMDBShallowCopyContextPtr();
	}

	auto it = shallowContexts_.begin();
	std::advance(it, idx);
	return (*it).lock();
}
