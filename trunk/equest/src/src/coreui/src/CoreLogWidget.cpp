#include "CoreUiPCH.h"
#include <QtCore/QThread>
#include <QtWidgets/QMenu>
#include <coreui/CoreLogWidget.h>
#include "ui_CoreLogWidget.h"
#include "coreui/finddialog.h"

using namespace coreUI;

CoreLogWidget::CoreLogWidget(QWidget *parent, Qt::WindowFlags flags)
	: QWidget(parent, flags), ui(new Ui::CoreLogWidget)
{
	ui->setupUi(this);
    init();
}

CoreLogWidget::~CoreLogWidget()
{
	delete ui;
}

void CoreLogWidget::init()
{	
	addAction(ui->actionFind);
    addAction(ui->actionToogle_word_wrap);
	addAction(ui->actionClear_log);
}

void CoreLogWidget::logEntry( const CoreLogWidgetEntry& entry )
{
    QTextCharFormat fmt;

    if ( entry.severity == loglib::ILog::Info ) {
        fmt.setForeground( QColor(0, 0, 0, 255) );
	} else if (entry.severity == loglib::ILog::Warning) {
        fmt.setForeground( QColor(255, 130, 0, 255) );
	} else if (entry.severity == loglib::ILog::Error) {
        fmt.setForeground( QColor(255, 0, 0, 255) );
    } else {
        fmt.setForeground( QColor(100, 150, 50, 255) );
    }

    ui->textBrowser->setCurrentCharFormat(fmt);
    ui->textBrowser->append( entry.message );
}

void CoreLogWidget::logEntry( CoreLogWidgetEntryPtr entry )
{
    logEntry(*entry);
}

void CoreLogWidget::flushQueue()
{
    QMutexLocker locker(&queueMutex);
    while ( !queuedEntries.empty() ) {
        logEntry( *queuedEntries.front() );
        queuedEntries.pop();
    }
}

void CoreLogWidget::queueEntry( CoreLogWidgetEntryPtr entry )
{
    QMutexLocker locker(&queueMutex);
    queuedEntries.push(entry);
}

void CoreLogWidget::logOrQueueEntry( CoreLogWidgetEntryPtr entry )
{
	QThread* widgetThread = this->thread();
	QThread* currentThred = QThread::currentThread();
    if ( currentThred == widgetThread ) {
        logEntry(*entry);
    } else {
        queueEntry(entry);
    }
}

void CoreLogWidget::setWordWrap( bool wrap )
{
	ui->textBrowser->setWordWrapMode(wrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
}

void coreUI::CoreLogWidget::onFind()
{
	FindDialog fd;
	fd.setTextEdit(ui->textBrowser);
	fd.exec();
}

void coreUI::CoreLogWidget::onTextContextMenu(const QPoint & position)
{
	auto menu = ui->textBrowser->createStandardContextMenu();

	auto actions = menu->actions();

	QAction * beforeAction = nullptr;

	if (actions.empty() == false){
		beforeAction = actions.first();		
	}

	menu->insertAction(beforeAction, ui->actionFind);

	if (beforeAction != nullptr){
		menu->insertSeparator(beforeAction);
	}

	menu->exec(ui->textBrowser->mapToGlobal(position));
	delete menu;
}
