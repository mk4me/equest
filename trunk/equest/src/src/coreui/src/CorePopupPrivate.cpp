#include "CoreUiPCH.h"
#include <coreui/CorePopup.h>
#include "CorePopupPrivate.h"
#include <coreui/CoreCursorChanger.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>
#include "corelib/PluginCommon.h"

using namespace coreUI;

coreUI::CorePopupDeleter deleter;
coreUI::CorePopupStarter starter;

void coreUI::CorePopup::showMessage(const QString& title, const QString& message, int ms, const QRect& rect )
{
    const bool isGui = QThread::currentThread() == QCoreApplication::instance()->thread();
    auto connectionType = isGui ? Qt::DirectConnection : Qt::QueuedConnection;
    QMetaObject::invokeMethod(&starter, "start", connectionType, 
        Q_ARG(const QString&, title),
        Q_ARG(const QString&, message),
        Q_ARG(int, ms),
        Q_ARG(const QRect&, rect));
}

void coreUI::CorePopup::showMessage(const QString& title, const QString& message, int ms ) 
{
    const bool isGui = QThread::currentThread() == QCoreApplication::instance()->thread();
    auto connectionType = isGui ? Qt::DirectConnection : Qt::QueuedConnection;
    QMetaObject::invokeMethod(&starter, "start", connectionType, 
        Q_ARG(const QString&, title),
        Q_ARG(const QString&, message),
        Q_ARG(int, ms));
}

coreUI::CorePopupPrivate::CorePopupPrivate(const QString& title, const QString& message, int ms, const QRect& rect, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
    QWidget(parent, f),
    rect(rect)
{
    setupUi(this);
    this->text->setText(message);
    this->header->setText(title);
    timer.setInterval(ms);
    connect(&timer, SIGNAL(timeout()), this, SLOT(closePopup()));
}

void coreUI::CorePopupPrivate::closePopup()
{
    hide();
    Q_EMIT done(this);
}

void coreUI::CorePopupPrivate::start()
{
    timer.start();
    setGeometry(rect);
    show();
}

void coreUI::CorePopupDeleter::deletePopup( CorePopupPrivate* cp)
{
    delete cp;
}

void coreUI::CorePopupStarter::start( const QString& title, const QString& message, int ms /*= 3000*/ )
{
    const int width = 300;
    const int height = 100;
    QDesktopWidget widget;
    QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());
    QRect popupRect(mainScreenSize.right() - width, mainScreenSize.bottom() - height, width, height);
    start(title, message, ms, popupRect);
}

void coreUI::CorePopupStarter::start(const QString& title, const QString& message, int ms, const QRect& rect) 
{
	PLUGIN_LOG_INFO(title.toStdString() << ": " << message.toStdString());
    CorePopupPrivate* popup = new CorePopupPrivate(title, message, ms, rect, nullptr, Qt::Dialog | Qt::FramelessWindowHint);
    QObject::connect(popup, SIGNAL(done(CorePopupPrivate*)), &deleter, SLOT(deletePopup(CorePopupPrivate*)));
    popup->start();
}
