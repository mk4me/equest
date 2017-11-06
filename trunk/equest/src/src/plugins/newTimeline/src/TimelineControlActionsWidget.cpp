#include "TimelinePCH.h"
#include "TimelineControlActionsWidget.h"
#include "ui_TimelineControlActionsWidget.h"
#include <corelib/IServiceManager.h>
#include <corelib/PluginCommon.h>
#include <plugins/newTimeline/ITimelineService.h>

TimelineControlActionsWidget::TimelineControlActionsWidget(const timeline::ControllerPtr & controller,
	quint64 step, QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f),
	ui(new Ui::TimelineControlActionsWidget),
	step_(step), stepS_(step / 1000000.0), controller_(controller)
{
	if (step_ == 0) {
		throw std::invalid_argument("Step equal to 0");
	}

	if (controller_ == nullptr) {
		throw std::invalid_argument("Uninitialized controller");
	}

	ui->setupUi(this);
}

TimelineControlActionsWidget::~TimelineControlActionsWidget()
{
	delete ui;
}

void TimelineControlActionsWidget::setController(const timeline::ControllerPtr & controller)
{
	if (controller == nullptr) {
		throw std::invalid_argument("Uninitialized controller");
	}

	controller_ = controller;
}

const timeline::ControllerPtr & TimelineControlActionsWidget::controller() const
{
	return controller_;
}

void TimelineControlActionsWidget::setStep(quint64 step)
{
	if (step_ == 0) {
		throw std::invalid_argument("Step equal to 0");
	}

	step_ = step;
	stepS_ = step_ / 1000000.0;
}

quint64 TimelineControlActionsWidget::step() const
{
	return step_;
}

QAction * TimelineControlActionsWidget::playPauseAction() const
{
	return ui->actionPlayPause;
}

QAction * TimelineControlActionsWidget::stopAction() const
{
	return ui->actionStop;
}

QAction * TimelineControlActionsWidget::toBeginAction() const
{
	return ui->actionToBegining;
}

QAction * TimelineControlActionsWidget::toEndAction() const
{
	return ui->actionToEnd;
}

QAction * TimelineControlActionsWidget::stepForwardAction() const
{
	return ui->actionStepForward;
}

QAction * TimelineControlActionsWidget::stepBackwardAction() const
{
	return ui->actionStepBackward;
}

QAction * TimelineControlActionsWidget::loopAction() const
{
	return ui->actionLoop;
}

QAction * TimelineControlActionsWidget::forwardBackwardSwitchAction() const
{
	return ui->actionForwardBackward;
}

void TimelineControlActionsWidget::toEnd()
{
	controller_->setNormalizedTime(1.0);
	updateStatus(1.0);
}

void TimelineControlActionsWidget::toBegin()
{
	controller_->setNormalizedTime(0.0);
	updateStatus(0.0);
}

void TimelineControlActionsWidget::stop()
{
	controller_->pause();
	ui->actionPlayPause->blockSignals(true);
	ui->actionPlayPause->setChecked(false);
	ui->actionPlayPause->blockSignals(false);
	(controller_->getPlaybackDirection() == timeline::IController::PlayForward) ? toBegin() : toEnd();
	ui->actionStop->setEnabled(false);
}

void TimelineControlActionsWidget::stepForward()
{
	controller_->setTime(controller_->getTime() + stepS_);	
}

void TimelineControlActionsWidget::stepBackward()
{
	controller_->setTime(controller_->getTime() - stepS_);
}

void TimelineControlActionsWidget::play(bool p)
{
	auto timeline = core::queryService<ITimelineService>(plugin::getServiceManager());
	if (timeline != nullptr) {
		timeline->setPlaying(p);
	}	
}

void TimelineControlActionsWidget::setLoop(bool loop)
{
	controller_->setLoopbackMode(loop == true ? timeline::IController::LoopbackOn : timeline::IController::LoopbackOff);
}

void TimelineControlActionsWidget::setForward(bool forward)
{
	controller_->setPlaybackDirection(forward == true ? timeline::IController::PlayForward : timeline::IController::PlayBackward);
}

void TimelineControlActionsWidget::setBackward(bool backward)
{
	setForward(!backward);
}

void TimelineControlActionsWidget::updateStatus(double normalizedTime)
{
	if (normalizedTime == 0.0) {		
		ui->actionToBegining->setEnabled(false);
		ui->actionToEnd->setEnabled(true);
		ui->actionStepForward->setEnabled(true);
		ui->actionStepBackward->setEnabled(false);
	} else if (normalizedTime == 1.0) {
		ui->actionToBegining->setEnabled(true);
		ui->actionToEnd->setEnabled(false);
		ui->actionStepForward->setEnabled(false);
		ui->actionStepBackward->setEnabled(true);
	}
	else {
		ui->actionToBegining->setEnabled(true);
		ui->actionToEnd->setEnabled(true);
		ui->actionStepForward->setEnabled(true);
		ui->actionStepBackward->setEnabled(true);
	}
}