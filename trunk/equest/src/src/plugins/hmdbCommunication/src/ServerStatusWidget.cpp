#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/ServerStatusWidget.h>
#include "ui_ServerStatusWidget.h"

ServerStatusWidget::ServerStatusWidget(const QPixmap & online,
	const QPixmap & offline, const QPixmap & unknown,
	const QPixmap & partialyOnline, QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::ServerStatusWidget), online(online),
	offline(offline), unknown(unknown), partialyOnline(partialyOnline)
{
	ui->setupUi(this);
	setServerStatus(UnknownStatus);
}

ServerStatusWidget::~ServerStatusWidget()
{

}

void ServerStatusWidget::setServerStatus(char status)
{
	switch (status)
	{
	case OnlineStatus:
		ui->statusPlaceholderLabel->setText(tr("On line"));
		ui->iconPlaceholderLabel->setPixmap(online);
		break;

	case OfflineStatus:
		ui->statusPlaceholderLabel->setText(tr("Off line"));
		ui->iconPlaceholderLabel->setPixmap(offline);
		break;

	case MixedStatus:
		ui->statusPlaceholderLabel->setText(tr("Partially on line"));
		ui->iconPlaceholderLabel->setPixmap(partialyOnline);
		break;

	default:
		ui->statusPlaceholderLabel->setText(tr("Unknown"));
		ui->iconPlaceholderLabel->setPixmap(unknown);
		break;
	}
}