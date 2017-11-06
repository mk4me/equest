#include "ModelPhysicalDimensionsWizardPage.h"
#include "ui_ModelPhysicalDimensionsWizardPage.h"

ModelPhysicalDimensionsWizardPage::ModelPhysicalDimensionsWizardPage(QWidget *parent)
	: QWizardPage(parent), ui(new Ui::ModelPhysicalDimensionsWizardPage)
{
	ui->setupUi(this);	
	for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
	{
		auto text = ui->tableWidget->item(i, 0)->data(Qt::DisplayRole).toString();		
		text.replace(',', '.');
		auto value = text.toDouble();		
		ui->tableWidget->item(i, 0)->setData(Qt::DisplayRole, value);
	}
}

ModelPhysicalDimensionsWizardPage::~ModelPhysicalDimensionsWizardPage()
{

}