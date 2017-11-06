/********************************************************************
	created:	2015/05/11
	created:	14:21
	filename: 	ImageTableWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA__ImageTableWidget_H__
#define HEADER_GUARD_MEDUSA__ImageTableWidget_H__

#include <QtWidgets/QTableView>
#include <coreui/SimpleTab.h>
#include "plugins/dicom/DicomPerspective.h"

typedef QTableView ImageTableWidget;
class ImageTableModel;

class AnalisisWidget;
class ImageTableTab : public SimpleTab
{
	Q_OBJECT;
public:
	ImageTableTab(AnalisisWidget* aw, const QIcon& icon, const QString& label);
	virtual ~ImageTableTab() {}
public:
	virtual void setActive(bool val);
private Q_SLOTS:
	void onTableClicked(const QModelIndex&);
private:
	dicom::DicomPerspectiveConstPtr tryGetPerspective() const;
private:
	ImageTableWidget* tableWidget;
	ImageTableModel* tableModel;
	AnalisisWidget* analysisWidget;
};


#endif // HEADER_GUARD 
