/********************************************************************
	created:  2016/07/28
	filename: TimelineControlActionsWidget.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD___TIMELINECONTROLACTIONSWIDGET_H__
#define __HEADER_GUARD___TIMELINECONTROLACTIONSWIDGET_H__

#include <QtWidgets/QWidget>
#include <timelinelib/Controller.h>

namespace Ui
{
	class TimelineControlActionsWidget;
}

class QAction;

class TimelineControlActionsWidget : public QWidget
{
	Q_OBJECT

public:

	TimelineControlActionsWidget(const timeline::ControllerPtr & controller, quint64 step, QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~TimelineControlActionsWidget();

	QAction * playPauseAction() const;
	QAction * stopAction() const;
	QAction * toBeginAction() const;
	QAction * toEndAction() const;
	QAction * stepForwardAction() const;
	QAction * stepBackwardAction() const;
	QAction * loopAction() const;
	QAction * forwardBackwardSwitchAction() const;	

	void setController(const timeline::ControllerPtr & controller);

	const timeline::ControllerPtr & controller() const;

	quint64 step() const;

public slots:

	void setStep(quint64 step);

	void updateStatus(double normalizedTime);

private slots:
	
	void toEnd();
	void toBegin();
	void stop();
	void stepForward();
	void stepBackward();
	void play(bool p);
	void setLoop(bool loop);
	void setForward(bool forward);	
	void setBackward(bool backward);

private:

	timeline::ControllerPtr controller_;
	quint64 step_;
	double stepS_;
	Ui::TimelineControlActionsWidget * ui;
};

#endif  // __HEADER_GUARD___TIMELINECONTROLACTIONSWIDGET_H__