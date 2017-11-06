#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <map>
#include <QtWidgets/QToolButton>
#include <coreui/IMdeTab.h>
#include <corelib/PluginCommon.h>
#include <coreui/CoreMainWindow.h>
#include <coreui/CoreDockWidget.h>
#include <coreui/IAppUsageContextManager.h>
#include "ContextEventFilter.h"
#include "AnalisisModel.h"
#include <coreui/SingleInstanceWindow.h>

class QWidget;
class MdeServiceWindow;
class MdeMainWindow;
namespace Ui { class HMMMain; }

namespace coreUI { class ScopedKiosk; }

class MdeMainWindowController : public QObject
{
    Q_OBJECT;
public:
    MdeMainWindowController(MdeMainWindow* mw);

public Q_SLOTS:
    void addTab(coreUI::IMdeTabPtr tab);
    void activateTab(coreUI::IMdeTabPtr tab);
    void update();

private Q_SLOTS:
    void buttonClicked();
    void activateTab();

private:
    void handleContext( coreUI::IMdeTabPtr tab, QToolButton* button );

private:
    MdeMainWindow* window;
    //! mapa [przycisk -> zakładka]
    std::map<QToolButton*, coreUI::IMdeTabPtr> button2TabWindow;
    //! mapa [zakładka -> jej kontekst]
    std::map<coreUI::IMdeTabPtr, coreUI::IAppUsageContextPtr> tab2Contex;
};




//! Klasa realizuje widok aplikacji dla medyków
//! Z czasem klasa zaczela się rozrastac, wymaga glebszej refaktoryzacji
class MdeMainWindow : public coreUI::CoreMainWindow, public coreUI::IAppUsageContextManager, coreUI::SingleInstanceWindow
{
    Q_OBJECT;
    friend class MdeMainWindowController;

public:
    MdeMainWindow(const CloseUpOperations & closeUpOperations, const std::string & appName);
    virtual ~MdeMainWindow();

public:
    virtual void showSplashScreenMessage(const QString & message);
    AnalisisModelConstPtr getAnalysisModel() const { return analysisModel; }
    ContextEventFilterPtr getContextEventFilter() const { return contextEventFilter; }

Q_SIGNALS:
    void tabAdded(coreUI::IMdeTabPtr tab);
    void activateTab(coreUI::IMdeTabPtr tab);
    void modelChanged();

private Q_SLOTS:
	void onAbout();

private:	
	virtual void initializeSplashScreen(QSplashScreen * splashScreen);
	virtual bool customViewInit(QWidget * log);

    void addPropertiesToServiceWindow( plugin::IServicePtr service, MdeServiceWindow * w );
    MdeServiceWindow* createServiceWidget( plugin::IServicePtr service );
    
    void addTab(coreUI::IMdeTabPtr tab);
	
private:
    std::list<coreUI::IMdeTabPtr> tabs;
    MdeMainWindowController controller;

    // TODO : utworzyc klase widoku i przeniesc to tam
    QTabWidget * contextPlaceholder;
    AnalisisModelPtr analysisModel;
    ContextEventFilterPtr contextEventFilter;
    Ui::HMMMain* ui;
	utils::unique_ptr<coreUI::ScopedKiosk> scopedKiosk;
};


#endif // TOOLBOXMAIN_H
