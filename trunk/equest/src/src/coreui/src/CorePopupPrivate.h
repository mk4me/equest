/********************************************************************
	created:	2014/01/17
	created:	17:1:2014   0:41
	filename: 	CorePopupPrivate.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__COREPOPUPPRIVATE_H__
#define HEADER_GUARD_COREUI__COREPOPUPPRIVATE_H__

#include <QtCore/QTimer>
#include <QtWidgets/QLabel>

#include "ui_CorePopup.h"

namespace coreUI {

    class CorePopupPrivate : public QWidget, private Ui::CorePopup
    {
        Q_OBJECT
    public:
        CorePopupPrivate(const QString& title, const QString& message, int ms, const QRect& rect, QWidget* parent = 0, Qt::WindowFlags f = 0);
        virtual ~CorePopupPrivate() {}

        void start();
            
    Q_SIGNALS:
        void done(CorePopupPrivate*);

    private Q_SLOTS:
        void closePopup();

    private:
        QTimer timer;
        QRect rect;
    };

    class CorePopupDeleter : public QObject
    {
        Q_OBJECT;
    public Q_SLOTS:
        void deletePopup(CorePopupPrivate*);
    };

    class CorePopupStarter : public QObject
    {
        Q_OBJECT;
    public Q_SLOTS:
        void start(const QString& title, const QString& message, int ms, const QRect& rect);
        void start(const QString& title, const QString& message, int ms = 3000);
    };
}
#endif
