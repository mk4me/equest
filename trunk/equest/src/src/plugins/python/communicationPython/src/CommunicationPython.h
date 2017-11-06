/********************************************************************
	created:	2015/10/20
	created:	12:49
	filename: 	CommunicationPython.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CommunicationPython__CommunicationPython_H__
#define HEADER_GUARD_CommunicationPython__CommunicationPython_H__

#include <utils/SmartPtr.h>
#include <plugins/python/python/PythonPluginUtils.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include "plugins/hmdbCommunication/IHMDBShallowCopyContext.h"
#include "plugins/hmdbCommunication/SourceOptionsWidget.h"

namespace python {


	class ShallowContext : private python::PythonPluginUtils
	{
	public:
		ShallowContext() {}
		ShallowContext(hmdbCommunication::IHMDBShallowCopyContextPtr ctx) : context(ctx) {}
		std::vector<std::string> listTrialsToLoad();
		void loadTrial(const std::string& trialName);
	private:
		hmdbCommunication::IHMDBShallowCopyContextPtr context;
	};

	class CommunicationPython : private python::PythonPluginUtils
	{
	public:
		typedef hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration ContextConf;
		void login(const ContextConf& conf);

		int shallowContextsCount();
		ShallowContext getShallowContext(int number);
		

		ContextConf loadConf(const QString& path);
		ContextConf getCurrentConf();

	private:
		SourceOptionsWidget* getSourceOptionsWidget();
		utils::shared_ptr<hmdbCommunication::IHMDBSource> getCommunicationSource();
	};

}

#endif
