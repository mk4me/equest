/********************************************************************
    created:  2011/05/04
    created:  4:5:2011   8:42
    filename: CoreConsoleWidget.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__CORECONSOLEWIDGET_H__
#define HEADER_GUARD_CORE__CORECONSOLEWIDGET_H__

#include <coreui/Export.h>
#include <utils/SmartPtr.h>
#include <loglib/ILog.h>
#include <queue>
#include <QtWidgets/QWidget>
#include <QtCore/QDate>
#include <QtCore/QMutex>
#include <QtCore/QMetaType>


class QKeyEvent;
namespace Ui {

	class CoreConsoleWidget;

}

namespace coreUI {

//! Wpis do konsoli.
struct COREUI_EXPORT CoreConsoleWidgetEntry {
    //! Poziom ważności komuniaktu.
    loglib::ILog::LogSeverity severity;
    //! Wiadomość.
    QString message;
    //! Plik źródłowy.
    QString file;
    //! Linia w pliku.
    unsigned line;
    //! Moment zalogowania.
    QDate timestamp;
    //! Id wątku logującego.
    Qt::HANDLE theadId;
};

typedef utils::shared_ptr<CoreConsoleWidgetEntry> CoreConsoleWidgetEntryPtr;

//Q_DECLARE_METATYPE(CoreConsoleWidgetEntryPtr);
Q_DECLARE_METATYPE(coreUI::CoreConsoleWidgetEntry);

class COREUI_EXPORT CoreConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    CoreConsoleWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    ~CoreConsoleWidget();

public:
    //! \param entry Komunikat do zakolejkowania bądź natychmiastowego zalogowania,
    //! w zależności od tego, czy mamy do czynienia z wątkiem właściwym konsoli.
    void logOrQueueEntry(CoreConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(CoreConsoleWidgetEntryPtr entry);
    //! \param entry Komunikat do zakolejkowania.
    void queueEntry(CoreConsoleWidgetEntryPtr entry);


public slots:

    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(const CoreConsoleWidgetEntry& entry);
    //! Opróżnia kolejkę komunikatów.
    void flushQueue();
    //! \param wrap Czy zawijać tekst?
    void setWordWrap(bool wrap);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

private:
	void init();
private:
    //! Zakolejkowane wpisy.
    std::queue< CoreConsoleWidgetEntryPtr > queuedEntries;
    //! Muteks do kontroli kolejki wpisów.
    QMutex queueMutex;
	//! Ui
	Ui::CoreConsoleWidget * ui;
};

}

#endif  //  HEADER_GUARD_CORE__CORECONSOLEWIDGET_H__

