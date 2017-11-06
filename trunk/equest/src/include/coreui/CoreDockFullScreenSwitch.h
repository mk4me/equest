/********************************************************************
	created:  2015/09/28	15:31:28
	filename: CoreDockFullScreenSwitch.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__COREDOCKFULLSCREENSWITCH_H__
#define __HEADER_GUARD_COREUI__COREDOCKFULLSCREENSWITCH_H__

#include <QtCore/QObject>

class QDockWidget;

namespace coreUI
{
	class CoreDockFullScreenSwitch : public QObject
	{
		Q_OBJECT

	public:

		CoreDockFullScreenSwitch(QDockWidget * dock, QObject *parent = 0);
		virtual ~CoreDockFullScreenSwitch();

	public slots:

		void toggleFullScreen();

	private slots:

		void onTopLevelChanged(bool floating);

	private:

		virtual bool eventFilter(QObject * watched, QEvent * event) override;

	private:

		QDockWidget * dock;
		bool wasFloating;
		bool update;
	};
}

#endif	// __HEADER_GUARD_COREUI__COREDOCKFULLSCREENSWITCH_H__