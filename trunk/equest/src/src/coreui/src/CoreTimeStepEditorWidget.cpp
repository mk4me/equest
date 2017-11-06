#include "CoreUiPCH.h"
#include <coreui/CoreTimeStepEditorWidget.h>
#include "ui_CoreTimeStepEditorWidget.h"
#include <limits>
#include <QtCore/QTime>
#include <QtGui/QStandardItemModel>

//TODO
//use chrono or boost time to convert time values back and forth

using namespace coreUI;

struct PredefinedDescription
{
	quint64 step;
	QMap<QString, QString> descriptions;
};

Q_DECLARE_METATYPE(PredefinedDescription);

void fillPredefinedSteps(CoreTimeStepEditorWidget * stepEditor)
{
	stepEditor->registerPredefined(1000000LL / 50, "Video", "50 fps");
	stepEditor->registerPredefined(100000000LL / 2997, "Video", "29,97 fps");
	stepEditor->registerPredefined(1000000LL / 60, "Default", "60 fps");
	stepEditor->registerPredefined(1000000LL / 100, "IMU", "100 Hz");
}

CoreTimeStepEditorWidget::CoreTimeStepEditorWidget(quint64 value, QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::CoreTimeStepEditorWidget),
	step_(0), min_(0), max_(std::numeric_limits<quint64>::max())
{
	ui->setupUi(this);

	QStandardItemModel* model =	qobject_cast<QStandardItemModel*>(ui->predefinedComboBox->model());
	QModelIndex firstIndex = model->index(0, ui->predefinedComboBox->modelColumn(),
		ui->predefinedComboBox->rootModelIndex());
	QStandardItem* firstItem = model->itemFromIndex(firstIndex);
	firstItem->setSelectable(false);

	ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->predefinedTab), false);

	fillPredefinedSteps(this);

	setStep(value);
}

CoreTimeStepEditorWidget::CoreTimeStepEditorWidget(quint64 maxVal, quint64 minVal, quint64 value,
	QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), ui(new Ui::CoreTimeStepEditorWidget),
	step_(0), max_(std::numeric_limits<quint64>::max()), min_(0)
{
	ui->setupUi(this);

	setRange(maxVal, minVal);

	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->predefinedComboBox->model());
	QModelIndex firstIndex = model->index(0, ui->predefinedComboBox->modelColumn(),
		ui->predefinedComboBox->rootModelIndex());
	QStandardItem* firstItem = model->itemFromIndex(firstIndex);
	firstItem->setSelectable(false);

	ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->predefinedTab), false);

	fillPredefinedSteps(this);

	setStep(value);
}

CoreTimeStepEditorWidget::~CoreTimeStepEditorWidget()
{
	delete ui;
}

void CoreTimeStepEditorWidget::setRange(quint64 maxVal, quint64 minVal)
{
	if (maxVal >= minVal) {

		min_ = minVal;
		max_ = maxVal;

		if (step_ < min_) {
			setStep(min_);
		}
		else if (step_ > max_) {
			setStep(max_);
		}
	}
}

void CoreTimeStepEditorWidget::setMax(quint64 maxVal)
{
	if (maxVal != max_ && maxVal >= min_) {
		max_ = maxVal;
		if (step_ > max_) {
			setStep(max_);
		}
	}
}

void CoreTimeStepEditorWidget::setMin(quint64 minVal)
{
	if (minVal != min_ && minVal <= max_) {
		min_ = minVal;
		if (step_ < min_) {
			setStep(min_);
		}
	}
}

quint64 CoreTimeStepEditorWidget::step() const
{
	return step_;
}

void splitToTime(quint64 totalMicroseconds, quint64 & hours, unsigned char & minutes, unsigned char & seconds,
	unsigned short int & miliseconds, unsigned short int & microseconds)
{	
	hours = totalMicroseconds / (1000 * 1000 * 1LL * 60 * 60);
	totalMicroseconds -= hours * (1000 * 1000 * 1LL * 60 * 60);
	minutes = totalMicroseconds / (1000 * 1000 * 1 * 60);
	totalMicroseconds -= minutes * (1000 * 1000 * 1 * 60);
	seconds = totalMicroseconds / (1000 * 1000);
	totalMicroseconds -= seconds * (1000 * 1000);
	miliseconds = totalMicroseconds / 1000;
	microseconds = totalMicroseconds - miliseconds * 1000;
}

QString formatOutput(quint64 value, char type)
{
	QString ret;
	switch (type)
	{
		//time
	case 0:
	{
		quint64 hours = 0;
		unsigned char minutes = 0;
		unsigned char seconds = 0;
		unsigned short int miliseconds = 0;
		unsigned short int microseconds = 0;

		splitToTime(value, hours, minutes, seconds, miliseconds, microseconds);
		QString time(QTime(0, minutes, seconds, miliseconds).toString("hh:mm:ss.zzz"));		
		time.remove(0, time.indexOf(':', 0));
		ret = QString("%1%2").arg(hours).arg(time);
		if (microseconds > 0) {
			ret = QString("%1,%2").arg(ret).arg(microseconds, 4, 10, QChar('0'));
		}		
	}
	break;
	//micro
	case 1:
		ret = QString::number(value);
		break;
		//mili
	case 2:
		ret = QString::number(value / 1000.0);
		break;
		//seconds
	case 3:
		ret = QString::number(value / (1000.0 * 1000.0));
		break;
		//minutes
	case 4:
		ret = QString::number(value / (60.0 * 1000.0 * 1000.0));
		break;
		//hours
	case 5:
		ret = QString::number(value / (60.0 * 60.0 * 1000.0 * 1000.0));
		break;
	default:
		break;
	}

	return ret;
}

void CoreTimeStepEditorWidget::setStep(quint64 value)
{
	if (step_ != value && value >= min_ && value <= max_) {

		step_ = value;
		quint64 hours = 0;
		unsigned char minutes = 0;
		unsigned char seconds = 0;
		unsigned short int miliseconds = 0;
		unsigned short int microseconds = 0;

		splitToTime(step_, hours, minutes, seconds, miliseconds, microseconds);
		

		ui->microsecondsSpinBox->blockSignals(true);
		ui->milisecondsSpinBox->blockSignals(true);
		ui->secondsSpinBox->blockSignals(true);
		ui->minutesSpinBox->blockSignals(true);
		ui->hoursSpinBox->blockSignals(true);

		ui->microsecondsSpinBox->setValue(microseconds);
		ui->milisecondsSpinBox->setValue(miliseconds);
		ui->secondsSpinBox->setValue(seconds);
		ui->minutesSpinBox->setValue(minutes);
		ui->hoursSpinBox->setValue(hours);

		ui->microsecondsSpinBox->blockSignals(false);
		ui->milisecondsSpinBox->blockSignals(false);
		ui->secondsSpinBox->blockSignals(false);
		ui->minutesSpinBox->blockSignals(false);
		ui->hoursSpinBox->blockSignals(false);

		updateFormat();
	}
}

void CoreTimeStepEditorWidget::predefinedChanged(int idx)
{
	setStep(ui->predefinedComboBox->itemData(idx).value<PredefinedDescription>().step);
}

template<template<typename> class OP>
int findPredefinedIndex(quint64 step, QComboBox * list, OP<quint64> op = OP<quint64>())
{
	int ret = -1;

	for (int i = list->count() - 1; i > 0 ; --i) {
		auto sd = list->itemData(i).value<PredefinedDescription>();
		if (op(sd.step, step) == true) {
			ret = i;
			break;
		}
	}

	return ret;
}

void CoreTimeStepEditorWidget::registerPredefined(quint64 step, const QString & who, const QString & format)
{
	auto text = QString("[%1->%2]").arg(who).arg(format);

	auto pidx = ui->predefinedComboBox->count();

	ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->predefinedTab), true);
	if (ui->predefinedComboBox->count() == 1) {
		PredefinedDescription pd;
		pd.step = step;
		pd.descriptions.insert(who, format);
		ui->predefinedComboBox->addItem(text, QVariant::fromValue(pd));
	}
	else {

		int idx = ui->predefinedComboBox->count();
		bool match = false;

		if (ui->predefinedComboBox->itemData(idx - 1).value<PredefinedDescription>().step <= step) {

			idx = findPredefinedIndex<std::less_equal>(step, ui->predefinedComboBox);
			match = ui->predefinedComboBox->itemData(idx).value<PredefinedDescription>().step == step;
		}

		if (idx == ui->predefinedComboBox->count()) {
			PredefinedDescription pd;
			pd.step = step;
			pd.descriptions.insert(who, format);
			ui->predefinedComboBox->addItem(text, QVariant::fromValue(pd));
		}else if (match == true) {
			//todo
			//edit item
		}
		else {
			PredefinedDescription pd;
			pd.step = step;
			pd.descriptions.insert(who, format);
			ui->predefinedComboBox->insertItem(idx, text, QVariant::fromValue(pd));
			pidx = idx;
		}	
	}

	if (step == step_) {
		ui->predefinedComboBox->blockSignals(true);
		ui->predefinedComboBox->setCurrentIndex(pidx);
		ui->predefinedComboBox->blockSignals(false);
	}
}

void CoreTimeStepEditorWidget::formatChanged(int idx)
{
	ui->timeStepLineEdit->setText(formatOutput(step_, idx));
}

void CoreTimeStepEditorWidget::stepChanged()
{
	step_ = 0;
	step_ += ui->microsecondsSpinBox->value();
	step_ += ui->milisecondsSpinBox->value() * 1000LL;
	step_ += ui->secondsSpinBox->value() * 1000 * 1000LL;
	step_ += ui->minutesSpinBox->value() * 60 * 1000 * 1000LL;
	step_ += ui->hoursSpinBox->value() * 60 * 60 * 1000 * 1000LL;

	updateFormat();	
}

void CoreTimeStepEditorWidget::updateFormat()
{
	formatChanged(ui->formatComboBox->currentIndex());

	int idx = findPredefinedIndex<std::equal_to>(step_, ui->predefinedComboBox);
	if (idx == -1) {
		idx = 0;
	}

	ui->predefinedComboBox->blockSignals(true);
	ui->predefinedComboBox->setCurrentIndex(idx);
	ui->predefinedComboBox->blockSignals(false);

	emit stepChanged(step_);
}
