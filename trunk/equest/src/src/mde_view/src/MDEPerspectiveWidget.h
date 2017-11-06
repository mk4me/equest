/********************************************************************
	created:  2014/09/19	12:30:44
	filename: MDEPerspectiveWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___MDEPERSPECTIVEWIDGET_H__
#define __HEADER_GUARD___MDEPERSPECTIVEWIDGET_H__

#include <QtWidgets/QWidget>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>

class DefaultPatientCardWidget;
class GeneralSourceViewWidget;

class MDEPerspectiveWidget : public QWidget
{
	Q_OBJECT;

public:

	MDEPerspectiveWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~MDEPerspectiveWidget();

	GeneralSourceViewWidget * generalDataView();

private Q_SLOTS:

	void onShallowCopyChange();
	void onItemChange(char currentItem, int currentID, char previousItem, int previousID);

private:
	hmdbCommunication::ShallowCopyConstPtr shallowCopy_;
	DefaultPatientCardWidget * patientCardWidget;
	GeneralSourceViewWidget * generalViewWidget;
};

#endif	// __HEADER_GUARD___MDEPERSPECTIVEWIDGET_H__