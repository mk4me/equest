#include "CoreUiPCH.h"
#include <coreui/CoreLabeledWidget.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

using namespace coreUI;

CoreLabeledWidget::CoreLabeledWidget(QWidget* parent, Qt::WindowFlags f)
	: QWidget(parent, f), layout_(new QHBoxLayout), label_(new QLabel),
	labeledWidget_(nullptr)
{
	layout_->addWidget(label_);
	setLayout(layout_);
}

CoreLabeledWidget::~CoreLabeledWidget()
{

}

QLabel * CoreLabeledWidget::label()
{
	return label_;
}

const QLabel * CoreLabeledWidget::label() const
{
	return label_;
}

void CoreLabeledWidget::setLabeledWidget(QWidget * widget, const bool buddy)
{
	if(labeledWidget_ != nullptr){
		label_->setBuddy(nullptr);
		layout_->removeWidget(labeledWidget_);
	}

	labeledWidget_ = widget;
	
	if(labeledWidget_ != nullptr){
		layout_->addWidget(labeledWidget_);
		if(buddy == true){
			label_->setBuddy(labeledWidget_);
		}
	}
}

QWidget * CoreLabeledWidget::labeledWidget()
{
	return labeledWidget_;
}

const QWidget * CoreLabeledWidget::labeledWidget() const
{
	return labeledWidget_;
}

CoreLabeledWidget * CoreLabeledWidget::create(const QString & text, QWidget * widget,
	const bool buddy)
{
	CoreLabeledWidget * ret = new CoreLabeledWidget;
	ret->label()->setText(text);
	ret->setLabeledWidget(widget, buddy);

	return ret;
}