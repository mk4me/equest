/********************************************************************
	created:	2012/09/17
	created:	17:9:2012   12:47
	filename: 	MisSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MISCOMMON__MISSOURCEWIDGET_H__
#define HEADER_GUARD_MISCOMMON__MISSOURCEWIDGET_H__

#include <QtWidgets/QWidget>
#include <corelib/IFileDataManager.h>

class QPushButton;
class QLayout;
class FileSource;

namespace core
{
    class IDataManager;
}

class FileSourceWidget : public QWidget
{
private:
    Q_OBJECT;
public:
    FileSourceWidget(FileSource *source);

private slots:
    void onLoadFiles();
	void onLoadAsfAmc();
	void onAddChartStream();
    void onTimer();
private:
    FileSource *fileSource;
};


#endif
