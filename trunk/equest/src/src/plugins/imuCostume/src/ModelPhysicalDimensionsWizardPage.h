/********************************************************************
	created:  2015/09/17	16:51:41
	filename: ModelPhysicalDimensionsWizardPage.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__MODELPHYSICALDIMENSIONSWIZARDPAGE_H__
#define __HEADER_GUARD_IMU__MODELPHYSICALDIMENSIONSWIZARDPAGE_H__

#include <utils/SmartPtr.h>
#include <QtWidgets/QWizardPage>

namespace Ui
{
	class ModelPhysicalDimensionsWizardPage;
}

class ModelPhysicalDimensionsWizardPage : public QWizardPage
{
	Q_OBJECT

public:
	ModelPhysicalDimensionsWizardPage(QWidget *parent = 0);

	virtual ~ModelPhysicalDimensionsWizardPage();	

private:

	utils::unique_ptr<Ui::ModelPhysicalDimensionsWizardPage> ui;
};

#endif	// __HEADER_GUARD_IMU__MODELPHYSICALDIMENSIONSWIZARDPAGE_H__