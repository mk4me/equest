#include "CommunicationPCH.h"
#include "HMDBSourceWidget.h"
#include "HMDBSource.h"

#include <QtWidgets/QSplitter>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

#include <plugins/hmdbCommunication/SourceOptionsWidget.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

HMDBSourceWidget::HMDBSourceWidget(hmdbCommunication::HMDBSource * source,
	QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), source(source),
	mainWidget(new QSplitter), viewsWidget(new QMainWindow), sourceOptionsWidget(new SourceOptionsWidget)
{
	sourceOptionsWidget->setMaximumWidth(std::max(600, QApplication::desktop()->screenGeometry().width() / 4));
	sourceOptionsWidget->setSourcesPlaceholder(viewsWidget);

	//Splitter
	auto l = new QVBoxLayout;
	l->addWidget(mainWidget);
	setLayout(l);
	mainWidget->addWidget(sourceOptionsWidget);
	mainWidget->addWidget(viewsWidget);

	mainWidget->setCollapsible(mainWidget->indexOf(sourceOptionsWidget), true);
	mainWidget->setCollapsible(mainWidget->indexOf(viewsWidget), false);
}

HMDBSourceWidget::~HMDBSourceWidget()
{

}