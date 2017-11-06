#include "CorePCH.h"
#include <core/AppInitializer.h>
#include "ApplicationCommon.h"
#include <corelib/PluginCommon.h>
#include <QtCore/QCoreApplication>
#include "PluginApplication.h"
#include <coreui/SingleInstanceWindow.h>
#include <QtWidgets/QMessageBox>

DEFINE_CORE_APPLICATION_ACCESSOR;
PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR;

using namespace core;
using namespace coreUI;

class AppInitializer::AppInitializerImpl
{
public:

	AppInitializerImpl(int & argc, char *argv[], const std::string & appName) : coreApplication(new Application())
	{
		cleanUp_ = std::bind(&Application::finalizeUI, coreApplication.get());
		__application = coreApplication.get();
		plugin::__coreApplication = &mainViewApplication;
		initUIContextRes = coreApplication->initUIContext(argc, argv, appName, translations);
	}

	~AppInitializerImpl()
	{
	}

	//! Metoda uruchamiająca aplikacje, pobiera jako parametr wzorca widok który będzie uruchomiony, widok powinien dziedziczyć po CoreMainWindow
	int start(CoreMainWindow * mainWindow)
	{
		int result = 0;
		if (initUIContextRes == 1){
			result = 1;
		}
		else{
			SingleInstanceWindow * siw = dynamic_cast<SingleInstanceWindow*>(mainWindow);
			bool allowed = true;
			if (siw != nullptr){
				if (siw->isSingle() == false){
					mainWindow->setCloseConfirmationRequired(false);
					mainWindow->setAttribute(Qt::WA_QuitOnClose);
					mainWindow->close();

					QMessageBox::warning(mainWindow, mainWindow->windowTitle(), QObject::tr("Detected other instance of application already running. Only one instance of this application is allowed to be run. This application will be closed now."));
					allowed = false;
				}
			}

			if (allowed == true){
				try{
					//inicjalizujemy widok
					coreApplication->initWithUI(mainWindow, translations);

					if (translations.empty() == false){
						auto it = translations.begin();
						std::string files((*it).string());
						++it;

						for (; it != translations.end(); ++it){
							files += "\n" + (*it).string();
						}

						CORE_LOG_INFO("Unused translation files:\n" << files);

						utils::Filesystem::PathsList().swap(translations);
					}

					if (!mainWindow->init(plugin::__coreApplication)) {
						return 1;
					}

					//ustawiamy tutaj żeby nadpisać ewentualne zmiany z widoków
					qApp->setOrganizationName("PJATK");

					try{
						//faktycznie blokowane wywołanie
						//uruchamia kontekst Qt
						result = coreApplication->run();
					}
					catch (std::exception & e){
						result = -2;
						CORE_LOG_ERROR("Error while UI run: " << e.what());
					}
					catch (...){
						result = -2;
						CORE_LOG_ERROR("Unknown error while UI run");
					}
				}
				catch (std::exception & e){
					result = -1;
					CORE_LOG_ERROR("Error while UI initialization: " << e.what());
				}
				catch (...){
					result = -1;
					CORE_LOG_ERROR("Unknown error while UI initialization");
				}
			}
		}

		return result;
	}

	CoreMainWindow::CloseUpOperations cleanUp() const
	{
		return cleanUp_;
	}

private:
	utils::Filesystem::PathsList translations;
	utils::shared_ptr<Application> coreApplication;
	MainViewApplication mainViewApplication;
	int initUIContextRes;
	CoreMainWindow::CloseUpOperations cleanUp_;
};

AppInitializer::AppInitializer(int & argc, char *argv[], const std::string & appName) : impl(new AppInitializerImpl(argc, argv, appName))
{
}

AppInitializer::~AppInitializer()
{
}

int AppInitializer::start(CoreMainWindow * mainWindow)
{
	return impl->start(mainWindow);
}

CoreMainWindow::CloseUpOperations AppInitializer::cleanUp() const
{
	return impl->cleanUp();
}
