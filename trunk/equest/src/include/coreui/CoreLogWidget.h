/********************************************************************
    created:  2011/05/04
    created:  4:5:2011   8:42
    filename: CoreLogWidget.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_CORE__CORELOGWIDGET_H__
#define HEADER_GUARD_CORE__CORELOGWIDGET_H__

#include <coreui/Export.h>
#include <utils/SmartPtr.h>
#include <loglib/ILog.h>
#include <queue>
#include <QtWidgets/QWidget>
#include <QtCore/QDate>
#include <QtCore/QMutex>
#include <QtCore/QMetaType>


//class QKeyEvent;
namespace Ui {

	class CoreLogWidget;

}

namespace coreUI {

//! Wpis do konsoli.
struct COREUI_EXPORT CoreLogWidgetEntry {
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

typedef utils::shared_ptr<CoreLogWidgetEntry> CoreLogWidgetEntryPtr;

//Q_DECLARE_METATYPE(CoreConsoleWidgetEntryPtr);
Q_DECLARE_METATYPE(coreUI::CoreLogWidgetEntry);

class COREUI_EXPORT CoreLogWidget : public QWidget
{
    Q_OBJECT

public:
    CoreLogWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    virtual ~CoreLogWidget();

public:
    //! \param entry Komunikat do zakolejkowania bądź natychmiastowego zalogowania,
    //! w zależności od tego, czy mamy do czynienia z wątkiem właściwym konsoli.
    void logOrQueueEntry(CoreLogWidgetEntryPtr entry);
    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(CoreLogWidgetEntryPtr entry);
    //! \param entry Komunikat do zakolejkowania.
    void queueEntry(CoreLogWidgetEntryPtr entry);

public slots:

    //! \param entry Komunikat do natychmiastowgo zalogowania.
    void logEntry(const CoreLogWidgetEntry& entry);
    //! Opróżnia kolejkę komunikatów.
    void flushQueue();
    //! \param wrap Czy zawijać tekst?
    void setWordWrap(bool wrap);
	//! 
	void onFind();

private slots:

	void onTextContextMenu(const QPoint & position);

protected:

	//virtual void keyPressEvent(QKeyEvent *e);

private:

	void init();

private:
    //! Zakolejkowane wpisy.
    std::queue< CoreLogWidgetEntryPtr > queuedEntries;
    //! Muteks do kontroli kolejki wpisów.
    QMutex queueMutex;
	//! Ui
	Ui::CoreLogWidget * ui;
};

}

#endif  //  HEADER_GUARD_CORE__CORELOGWIDGET_H__

