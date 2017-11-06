/********************************************************************
	created:	2011/07/29
	created:	29:7:2011   10:41
	filename: 	AppInitializer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__APPINITIALIZER_H__
#define HEADER_GUARD_CORE__APPINITIALIZER_H__

#include <utils/SmartPtr.h>
#include <type_traits>
#include <coreui/CoreMainWindow.h>

namespace core {

class AppInitializer;

typedef utils::shared_ptr<AppInitializer> AppInitializerPtr;

class AppInitializer
{
private:

	class AppInitializerImpl;

private:

	utils::shared_ptr<AppInitializerImpl> impl;
	

public:

	template<class FrontPageWidget>
	static int runApp(int & argc, char *argv[], const std::string & appName)
	{
		try{
			static_assert((std::is_base_of<coreUI::CoreMainWindow, FrontPageWidget>::value), "FrontPageWidget class should inherit from coreUI::CoreMainWindow");
			//we create application UI context and initialize everything
			AppInitializer application(argc, argv, appName);
			//now it is safe to create view widget
			FrontPageWidget widget(application.cleanUp(), appName);
			widget.setAttribute(Qt::WA_DeleteOnClose, false);
			//we run application
			return application.start(&widget);
		}catch(...){
			return -100;
		}
	}

private:

	AppInitializer(int & argc, char *argv[], const std::string & appName);
	~AppInitializer();

	//! Metoda uruchamiająca aplikacje, pobiera jako parametr wzorca widok który będzie uruchomiony, widok powinien dziedziczyć po MainWindow
	int start(coreUI::CoreMainWindow * mainWindow);

	coreUI::CoreMainWindow::CloseUpOperations cleanUp() const;
};

}
#endif
