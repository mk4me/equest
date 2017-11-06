/********************************************************************
	created:  2013/01/14
	created:  14:1:2013   16:55
	filename: Application.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___APPLICATION_H__
#define HEADER_GUARD___APPLICATION_H__

#include <utils/SmartPtr.h>
#include <corelib/Version.h>
#include <corelib/JobManagerTypes.h>
#include <corelib/BaseDataTypes.h>
#include <loglib/ILog.h>
#include <corelib/IVisualizer.h>
#include <corelib/ISource.h>
#include <corelib/IService.h>
#include <corelib/IParser.h>
#include "JobManager.h"
#include "ThreadPool.h"
#include "Plugin.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>

class QSplashScreen;
class QTranslator;

namespace coreUI {
	class UIApplication;
	class CoreMainWindow;
}

namespace core {
	class DataManager;
	class StreamDataManager;
	class FileDataManager;
	class Log;
	class Path;
	class DataHierarchyManager;
	class RegisteredDataTypesManager;
	class ParserManager;
	class LogInitializer;
	class PluginLoader;
	class VisualizerManager;
	class SourceManager;
	class ServiceManager;	
	class LanguagesManager;
	class ApplicationDescription;

	class Application : private QObject
	{
		Q_OBJECT;

		friend class UIApplication;

	private:
		std::thread::id mainThreadID_;
		plugin::VendorDescription vendor_;
		ExtendedVersion version_;
		utils::shared_ptr<Path> paths_;
		utils::shared_ptr<LogInitializer> logInitializer_;
		loglib::ILogPtr loggerPrototype_;
		loglib::ILogPtr logger_;
		utils::shared_ptr<ApplicationDescription> applicationDescription_;
		utils::shared_ptr<LanguagesManager> languagesManager_;
		utils::shared_ptr<PluginLoader> pluginLoader_;
		utils::shared_ptr<DataHierarchyManager> dataHierarchyManager_;
		utils::shared_ptr<RegisteredDataTypesManager> registeredDataTypesManager_;
		utils::shared_ptr<DataManager> dataManager_;
		utils::shared_ptr<ParserManager> parserManager_;
		utils::shared_ptr<StreamDataManager> streamDataManager_;
		utils::shared_ptr<FileDataManager> fileDataManager_;
		utils::shared_ptr<coreUI::UIApplication> uiApplication_;
		std::vector<utils::shared_ptr<QTranslator>> translators_;
		utils::shared_ptr<ServiceManager> serviceManager_;
		utils::shared_ptr<SourceManager> sourceManager_;
		utils::shared_ptr<VisualizerManager> visualizerManager_;
		utils::shared_ptr<InnerThreadPool> innerThreadPool;
		utils::shared_ptr<ThreadPool> threadPool_;
		utils::shared_ptr<InnerWorkManager> innerWorkManager_;
		utils::shared_ptr<InnerJobManager> innerJobManager_;
		utils::shared_ptr<JobManager> jobManager_;

		utils::Filesystem::Path additionalPluginsPath;

		QTimer visualizerUpdateTimer;
		QTimer servicesUpdateTimer;

		double visualizerTimeDelta;
		double servicesTimeDelta;

		coreUI::CoreMainWindow * mainWindow;

		bool uiInit;

	private:

		static bool trySetPathsFromRegistry(utils::shared_ptr<Path> & path, const std::string & appName);
		static void setDefaultPaths(utils::shared_ptr<Path> & path, const std::string & appName);
		void showSplashScreenMessage(const QString & message);

		void safeRegisterService(const plugin::IServicePtr & service);
		void safeRegisterSource(const plugin::ISourcePtr & source);
		void safeRegisterParser(const plugin::IParserPtr & parser);
		void safeRegisterObjectWrapperPrototype(const utils::ObjectWrapperPtr & prototype);
		void safeRegisterVisualizer(const plugin::IVisualizerPtr & visualizer);
		//void registerCoreDomainTypes();
		void unpackPlugin(coreUI::CoreMainWindow * splashScreen, const PluginPtr & plugin);

		private slots:

		void updateServices();

		void updateVisualizers();

	public:
		Application();
		~Application();

		int initUIContext(int & argc, char *argv[], const std::string & appName,
						  utils::Filesystem::PathsList & coreTranslations);

		void initWithUI(coreUI::CoreMainWindow * mainWindow,
						utils::Filesystem::PathsList & translations);

		int run();
		std::thread::id mainThreadID() const;
		ApplicationDescription * description();
		LanguagesManager * languageManager();
		DataManager* dataManager();
		StreamDataManager* streamDataManager();
		FileDataManager* fileDataManager();
		loglib::ILog* logger();
		loglib::ILog* loggerPrototype();
		Path* paths();
		DataHierarchyManager* dataHierarchyManager();
		RegisteredDataTypesManager* registeredDataTypesManager();
		ParserManager* parserManager();
		VisualizerManager* visualizerManager();
		ServiceManager* serviceManager();
		SourceManager* sourceManager();
		ThreadPool* threadPool();
		JobManager* jobManager();

		void finalizeUI();
	};
}

#endif	//	HEADER_GUARD___APPLICATION_H__
