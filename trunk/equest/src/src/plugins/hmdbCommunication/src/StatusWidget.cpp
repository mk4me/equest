#include "CommunicationPCH.h"
#include "StatusWidget.h"
#include <QtCore/QTimer>
#include <QtWidgets/QToolTip>
#include <coreui/CorePopup.h>
#include <plugins/hmdbCommunication/IHMDBService.h>

hmdbCommunication::StatusWidget::StatusWidget(IHMDBService * hmdbService, const std::string& pingUrl, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/) :
    QWidget(parent, f),
    pingUrl(pingUrl),
	hmdbService(hmdbService)
{

    connectedPixmap = QPixmap(":/hmdbCommunication/icons/dostepny.png").scaledToHeight(32, Qt::SmoothTransformation);
    disconnectedPixmap = QPixmap(":/hmdbCommunication/icons/niedostepny.png").scaledToHeight(32, Qt::SmoothTransformation);
    setupUi(this);   
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer.start(1000);

    setConnected(false);
}

void hmdbCommunication::StatusWidget::stopUpdating()
{
	timer.stop();
}

hmdbCommunication::StatusWidget::~StatusWidget()
{
	
}

void hmdbCommunication::StatusWidget::setConnected( bool connected )
{
    iconLabel->setPixmap(connected ? connectedPixmap : disconnectedPixmap);
}

void hmdbCommunication::StatusWidget::setUserName( const QString& val )
{
    userLabel->setText(tr("User: ") + val);
}

void hmdbCommunication::StatusWidget::updateTimer()
{
    // TODO : moze bedzie lepiej odpytywac o to w osobnym watku (serverStatus jest chroniony mutexem, moze to blokowac aplikacje)        
	iconLabel->setPixmap(hmdbService->serverOnline(pingUrl) ? connectedPixmap : disconnectedPixmap);
    iconLabel->update();    
}

QPushButton* hmdbCommunication::StatusWidget::getLogoutButton()
{
    return this->logoutButton;
}

