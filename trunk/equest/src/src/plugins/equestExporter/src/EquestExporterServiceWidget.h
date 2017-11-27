/********************************************************************
	created:	2014/04/29
	created:	29:4:2014   11:16
	filename: 	EquestExporterServiceWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST_EXPORTER__EQUESTEXPORTERSERVICEWIDGET_H__
#define HEADER_GUARD_EQUEST_EXPORTER__EQUESTEXPORTERSERVICEWIDGET_H__


#include <QtWidgets/QWidget>
#include <corelib/IFileDataManager.h>
#include "ExporterModel.h"
#include <QtCore/QThread>
#include <QtWidgets/QAbstractButton>

class QPushButton;
class QLayout;

namespace core
{
    class IDataManager;
}

namespace Ui {
    class EquestExporter;
}

namespace equestExporter {

    class EquestExporterService;



    class ExporterThread : public QThread
    {
        Q_OBJECT;
    public:
        ExporterThread() : 
			cc(std::bind(&ExporterThread::callback, this, std::placeholders::_1, std::placeholders::_2)) {}
        void run() {
            cc.run();
        }

        void addOperation(const CallbackCollector::Operation& op, double weight, const QString& desc)
        {
            cc.addOperation(op, weight, desc);
        }
    Q_SIGNALS:
        void progressChanged(double ratio, const QString& description);

    private:
        void callback(double ratio, const QString& description)
        {
            emit progressChanged(ratio, description);
        }

    private:
        CallbackCollector cc;
    };


    class WidgetDisabler
    {
    public:
        WidgetDisabler() {}
        WidgetDisabler(QWidget* w) { addWidget(w); }
        WidgetDisabler(QWidget* w1, QWidget* w2) { addWidget(w1); addWidget(w2); }
        virtual ~WidgetDisabler()
        {
            for (auto it = widgets.begin(); it != widgets.end(); ++it) {
                (*it)->setEnabled(true);
            }

            for (auto it = checkedButtons.begin(); it != checkedButtons.end(); ++it) {
                (*it)->setChecked(false);
            }
        }

        void addWidget(QWidget* w) {
            w->setEnabled(false);
            widgets.push_back(w);
        }

        void checkButton(QAbstractButton* button) {
            button->setChecked(true);
            checkedButtons.push_back(button);
        }

    private:
        QWidgetList widgets;
        QList<QAbstractButton*> checkedButtons;
    };
    DEFINE_SMART_POINTERS(WidgetDisabler);

	class EquestExporterServiceWidget : public QWidget
    {
    private:
        Q_OBJECT;
    public:
		EquestExporterServiceWidget(EquestExporterService* service);

	public:
		void setExporter(ExporterModelPtr exporterModel);
        void setExportDir(const QString& dir);
        QString getExportDir() const;

	private Q_SLOTS:
		void onExport();
		void onExtract();
		void onExtractDirDialog();
		void onExportFrom();
        void onPackChecks();
        void onClearEquestExport();
        void onOpenEquestExport();
	    void callback(double ratio, const QString& description);
        void afterExtract();
        void afterExport();
        void afterClear();
		void curveDescChanged(const QString &);

	private:
        void setExportFrom(const QString& dir, const QStringList& users);
		bool isKeyPoints();


    private:
		EquestExporterService* equestService;
		Ui::EquestExporter* ui;
		ExporterModelPtr exporterModel;
		ExporterModel::CallbackFunction callbackFunction;
        mutable QStringList tempUsers;
        mutable QString innerExportDirPath;
        WidgetDisablerPtr disabler;
    };
}

#endif
