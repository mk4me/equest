#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/OperationProgressWidget.h>
#include "ui_OperationProgressWidget.h"

OperationProgressWidget::OperationProgressWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::OperationProgressWidget), operation(nullptr)
{
	ui->setupUi(this);
	connect(&timer, SIGNAL(timeout()), this, SLOT(refreshOperationStatus()));
	timer.setInterval(1000 / 25);
}

OperationProgressWidget::~OperationProgressWidget()
{

}

void OperationProgressWidget::resetStatus()
{
	ui->operationPlaceholder->setText(QString());
	ui->progressBar->setValue(0.0);
}

void OperationProgressWidget::setOperation(DataViewWidget::IOperation * op)
{
	operation = op;
	if (op != nullptr){
		refreshOperationStatus();
		timer.start();
		ui->cancelPushButton->setEnabled(true);
	}
	else{
		cursorChanger.reset();
		ui->cancelPushButton->setEnabled(false);
		timer.stop();
	}
}

void OperationProgressWidget::refreshOperationStatus()
{
	ui->operationPlaceholder->setText(operation->name());
	ui->progressBar->setValue(operation->normalizedProgress() * 100.0);
}

void OperationProgressWidget::onCancel()
{
	cursorChanger.reset(new coreUI::CoreCursorChanger);
	ui->cancelPushButton->setEnabled(false);
	operation->abort();
}