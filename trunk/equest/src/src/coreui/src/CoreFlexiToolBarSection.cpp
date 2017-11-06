#include "CoreUiPCH.h"
#include <coreui/CoreFlexiToolBarSection.h>
#include "ui_CoreFlexiToolBarSection.h"

using namespace coreUI;

CoreFlexiToolBarSection::CoreFlexiToolBarSection(const QString & sectionName, const QPixmap & icon,
	QAction * configAction, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f),
	ui(new Ui::CoreFlexiToolBarSection), innerWidget_(nullptr)
{
	ui->setupUi(this);
	setName(sectionName);
	setIcon(icon);
	setConfigurationAction(configAction);
}

CoreFlexiToolBarSection::~CoreFlexiToolBarSection()
{
	delete ui;
}

void CoreFlexiToolBarSection::setName(const QString & sectionName)
{
	ui->sectionNameLabel->setText(sectionName);
}

const QString CoreFlexiToolBarSection::name() const
{
	return ui->sectionNameLabel->text();
}

void CoreFlexiToolBarSection::setConfigurationAction(QAction * configAction)
{
	ui->configurationButton->setDefaultAction(configAction);
	if(configAction == nullptr){
		ui->configurationButton->setVisible(false);
	}else{
		ui->configurationButton->setVisible(true);
	}
}

QAction * CoreFlexiToolBarSection::configurationAction() const
{
	return ui->configurationButton->defaultAction();
}

void CoreFlexiToolBarSection::setIcon(const QPixmap & icon)
{
	ui->iconPlaceholder->setPixmap(icon);
	if(icon.isNull() == true){
		ui->iconPlaceholder->setVisible(false);
	}else{
		ui->iconPlaceholder->setVisible(true);
	}
}

const QPixmap CoreFlexiToolBarSection::icon() const
{
	return *(ui->iconPlaceholder->pixmap());
}

void CoreFlexiToolBarSection::setInnerWidget(QWidget * widget)
{
	if(innerWidget_ != nullptr){
		ui->innerWidgetPlaceholder->layout()->removeWidget(innerWidget_);
	}

	ui->innerWidgetPlaceholder->layout()->addWidget(widget);
	innerWidget_ = widget;
}

QWidget * CoreFlexiToolBarSection::innerWidget() const
{
	return innerWidget_;
}