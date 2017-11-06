#include "CoreUiPCH.h"
#include <QtCore/QThread>
#include <coreui/CoreConsoleWidget.h>
#include "ui_CoreConsoleWidget.h"
#include <QtGui/QtEvents>
#include "coreui/finddialog.h"

using namespace coreUI;

CoreConsoleWidget::CoreConsoleWidget(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent, flags), ui(new Ui::CoreConsoleWidget)
{
	ui->setupUi(this);
    init();
}

CoreConsoleWidget::~CoreConsoleWidget()
{
	delete ui;
}

void CoreConsoleWidget::init()
{
    addAction(ui->actionToogle_word_wrap);
	addAction(ui->actionClear_console);
}

void CoreConsoleWidget::logEntry( const CoreConsoleWidgetEntry& entry )
{
    QTextCharFormat fmt;

    if ( entry.severity == loglib::ILog::LogSeverityInfo ) {
        fmt.setForeground( QColor(0, 0, 0, 255) );
    } else if ( entry.severity == loglib::ILog::LogSeverityWarning ) {
        fmt.setForeground( QColor(255, 130, 0, 255) );
    } else if ( entry.severity == loglib::ILog::LogSeverityError ) {
        fmt.setForeground( QColor(255, 0, 0, 255) );
    } else {
        fmt.setForeground( QColor(100, 150, 50, 255) );
    }

    ui->textEdit->setCurrentCharFormat(fmt);
    ui->textEdit->append( entry.message );
}

void CoreConsoleWidget::logEntry( CoreConsoleWidgetEntryPtr entry )
{
    logEntry(*entry);
}

void CoreConsoleWidget::flushQueue()
{
    QMutexLocker locker(&queueMutex);
    while ( !queuedEntries.empty() ) {
        logEntry( *queuedEntries.front() );
        queuedEntries.pop();
    }
}

void CoreConsoleWidget::queueEntry( CoreConsoleWidgetEntryPtr entry )
{
    QMutexLocker locker(&queueMutex);
    queuedEntries.push(entry);
}

void CoreConsoleWidget::logOrQueueEntry( CoreConsoleWidgetEntryPtr entry )
{
	QThread* widgetThread = this->thread();
	QThread* currentThred = QThread::currentThread();
    if ( currentThred == widgetThread ) {
        logEntry(*entry);
    } else {
        queueEntry(entry);
    }
}

void CoreConsoleWidget::setWordWrap( bool wrap )
{
    ui->textEdit->setWordWrapMode( wrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap );
}

void coreUI::CoreConsoleWidget::keyPressEvent(QKeyEvent *e)
{
	if (e->type() == QKeyEvent::KeyPress) {
		if (e->matches(QKeySequence::Find)) {
			FindDialog fd;
			fd.setTextEdit(ui->textEdit);
			fd.exec();
		}
	}
}
