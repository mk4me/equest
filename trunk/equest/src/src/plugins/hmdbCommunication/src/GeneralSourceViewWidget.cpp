#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/GeneralSourceViewWidget.h>
#include "ui_GeneralSourceViewWidget.h"
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include <plugins/hmdbCommunication/OperationProgressWidget.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>
#include <plugins/hmdbCommunication/DataViewConfigurationWidget.h>
#include <QtWidgets/QVBoxLayout>

GeneralSourceViewWidget::GeneralSourceViewWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
	QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::GeneralSourceViewWidget), dataView_(new DataViewWidget(shallowCopyContext)),
	dataViewConfiguration_(new DataViewConfigurationWidget),
	operationProgress(new OperationProgressWidget)
{
	ui->setupUi(this);
	operationProgress->setVisible(false);
	QVBoxLayout * l = new QVBoxLayout;
	l->addWidget(dataViewConfiguration_);
	l->addWidget(dataView_);
	l->addWidget(operationProgress);
	setLayout(l);

	connect(dataView_, SIGNAL(operationAboutToStart()), this, SLOT(onOperationStarts()));
	connect(dataView_, SIGNAL(operationFinished()), this, SLOT(onOperationEnds()));
	connect(dataViewConfiguration_, SIGNAL(perspectiveChanged()), this, SLOT(onPerspectiveChanged()));
	connect(dataViewConfiguration_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
	connect(dataViewConfiguration_, SIGNAL(filterChanged()), this, SLOT(onFilterChanged()));
}

GeneralSourceViewWidget::~GeneralSourceViewWidget()
{

}

DataViewConfigurationWidget * GeneralSourceViewWidget::dataViewConfiguration()
{
	return dataViewConfiguration_;
}

DataViewWidget * GeneralSourceViewWidget::dataView()
{
	return dataView_;
}

void GeneralSourceViewWidget::onOperationStarts()
{	
	operationProgress->setOperation(dataView_->operation());
	operationProgress->setVisible(true);
}

void GeneralSourceViewWidget::onOperationEnds()
{
	operationProgress->setVisible(false);
	operationProgress->setOperation(nullptr);
}

void GeneralSourceViewWidget::onPerspectiveChanged()
{
	dataView_->setPerspective(dataViewConfiguration_->currentPerspective());
}

void GeneralSourceViewWidget::onContentChanged()
{
	dataView_->setContent(dataViewConfiguration_->currentContent());
}

void GeneralSourceViewWidget::onFilterChanged()
{
	dataView_->setFilter(dataViewConfiguration_->currentFilter());
}