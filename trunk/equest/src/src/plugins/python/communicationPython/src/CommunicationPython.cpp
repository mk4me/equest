#include "CommunicationPython.h"
#include <boost/python.hpp>
#include "corelib/ISourceManager.h"
#include "corelib/PluginCommon.h"
#include "corelib/IServiceManager.h"
#include "plugins/python/python/PythonPluginUtils.h"
#include "corelib/IDataManager.h"
#include <QtWidgets/QWidget>
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include "plugins/hmdbCommunication/SourceOptionsWidget.h"
#include "plugins/hmdbCommunication/ContextConfigurationSettingsFile.h"

using namespace boost::python;



utils::shared_ptr<hmdbCommunication::IHMDBSource> python::CommunicationPython::getCommunicationSource()
{
	auto icomm = core::querySource<hmdbCommunication::IHMDBSource>(getSourceManager());
	if (icomm) {
		return icomm;
	}

	throw std::runtime_error("Unable to find communication source");
}


void python::CommunicationPython::login(const hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration& conf)
{
	auto sourceOptionsWidget = getSourceOptionsWidget();
	if (sourceOptionsWidget) {
		sourceOptionsWidget->setConnectionProfile(conf);
		sourceOptionsWidget->onLogin();
		QStringList errors;
		sourceOptionsWidget->verify(errors);
		if (!errors.isEmpty()) {
			throw std::runtime_error("There was problem during logging");
		}
	} else {
		throw std::runtime_error("Unable to find source");
	}
}

python::CommunicationPython::ContextConf python::CommunicationPython::loadConf(const QString& path)
{
	return hmdbCommunication::ContextConfigurationSettingsFile::read(path);
}

int python::CommunicationPython::shallowContextsCount()
{
	auto icomm = getCommunicationSource(); 
	return icomm->shallowContextsCount();
}

python::CommunicationPython::ContextConf python::CommunicationPython::getCurrentConf()
{
	auto sourceOptionsWidget = getSourceOptionsWidget();
	return sourceOptionsWidget->getConnectionProfile();
}

SourceOptionsWidget* python::CommunicationPython::getSourceOptionsWidget()
{
	auto icomm = getCommunicationSource();
	plugin::ISourcePtr commSource = utils::dynamic_pointer_cast<plugin::ISource>(icomm);

	if (commSource) {
		auto commWidget = commSource->getWidget();
		auto list = commWidget->findChildren<SourceOptionsWidget*>();
		if (!list.isEmpty()) {
			return *list.begin();
		}
	}

	throw std::runtime_error("Unable to find Source Wigdet");
}

python::ShallowContext python::CommunicationPython::getShallowContext(int number)
{
	auto icomm = getCommunicationSource();
	auto ctx = icomm->shallowContext(number);
	if (ctx) {
		return ShallowContext(ctx);
	} else {
		throw std::runtime_error("Wrong context number");
	}
}

std::vector<std::string> python::ShallowContext::listTrialsToLoad()
{
	std::vector<std::string> res;
	auto allShallow = context->shallowCopyDataContext()->shallowCopy();
	if (allShallow) {
		auto& shallowCopy = allShallow->motionShallowCopy;
		auto& trials = shallowCopy.trials;
		auto storage = context->shallowCopyLocalContext()->localContext()->dataContext()->storage()->transaction();

		for (auto& trial : trials) {
			auto& name = trial.second->trialName;
			for (auto& f : trial.second->files) {
				if (storage->exists(f.second->fileName)) {
					res.push_back(name);
					break;
				}
			}
		}
	}
	return res;
}

void python::ShallowContext::loadTrial(const std::string& trialName)
{
	auto allShallow = context->shallowCopyDataContext()->shallowCopy();
	if (allShallow) {
		auto& shallowCopy = allShallow->motionShallowCopy;
		auto& trials = shallowCopy.trials;
		auto storage = context->shallowCopyLocalContext()->localContext()->dataContext()->storage()->transaction();

		for (auto& trial : trials) {
			if (trialName == trial.second->trialName) {
				context->shallowCopyLocalContext()->load(hmdbCommunication::MotionType, trial.first);
				/*for (auto& f : trial.second->files) {
					context->shallowCopyLocalContext()->load(hmdbCommunication::MotionType, f);
				}*/
			}
		}
	}
}
