#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/DataViewConfigurationWidget.h>
#include "ui_DataViewConfigurationWidget.h"

DataViewConfigurationWidget::DataViewConfigurationWidget(QWidget * parent,
	Qt::WindowFlags f) : QWidget(parent, f), ui(new Ui::DataViewConfigurationWidget)
{
	ui->setupUi(this);

	filters.reset(new ItemHelper<hmdbCommunication::ShallowCopyFilter>(ui->filterLabel, ui->filterComboBox));
	perspectives.reset(new ItemHelper<hmdbCommunication::IDataSourcePerspective>(ui->perspectiveLabel, ui->perspectiveComboBox));
	contents.reset(new ItemHelper<hmdbCommunication::IDataSourceContent>(ui->contentLabel, ui->contentComboBox));

	filters->add(QObject::tr("No filter"));
}

DataViewConfigurationWidget::~DataViewConfigurationWidget()
{

}


hmdbCommunication::ShallowCopyFilter * DataViewConfigurationWidget::currentFilter()
{
	return filters->currentItem();
}

hmdbCommunication::IDataSourcePerspective * DataViewConfigurationWidget::currentPerspective()
{
	return perspectives->currentItem();
}

hmdbCommunication::IDataSourceContent * DataViewConfigurationWidget::currentContent()
{
	return contents->currentItem();
}

void DataViewConfigurationWidget::registerFilter(hmdbCommunication::ShallowCopyFilter * filter)
{
	filters->add(filter);
}

void DataViewConfigurationWidget::addFilterSeparator()
{
	filters->addSeparator();
}

void DataViewConfigurationWidget::registerPerspective(hmdbCommunication::IDataSourcePerspective * perspective)
{
	perspectives->add(perspective);
}

void DataViewConfigurationWidget::addPerspectiveSeparator()
{
	perspectives->addSeparator();
}

void DataViewConfigurationWidget::registerContent(hmdbCommunication::IDataSourceContent * content)
{
	contents->add(content);
}

void DataViewConfigurationWidget::addContentSeparator()
{
	contents->addSeparator();
}

void DataViewConfigurationWidget::clearFilters()
{
	filters->clear();
}

void DataViewConfigurationWidget::clearPerspectives()
{
	perspectives->clear();
}

void DataViewConfigurationWidget::clearContents()
{
	contents->clear();
}

const unsigned int DataViewConfigurationWidget::filtersCount() const
{
	return filters->count();
}

const unsigned int DataViewConfigurationWidget::perspectivesCount() const
{
	return perspectives->count();
}

const unsigned int DataViewConfigurationWidget::contentsCount() const
{
	return contents->count();
}

void DataViewConfigurationWidget::setFilterVisible(const bool visible)
{
	filters->setVisible(visible);
}

void DataViewConfigurationWidget::setFilterEnabled(const bool enable)
{	
	filters->setEnable(enable);
}

void DataViewConfigurationWidget::setCurrentFilter(const int idx)
{
	if (filters->setCurrentItem(idx, true) == true){
		emit filterChanged();
	}
}

void DataViewConfigurationWidget::setPerspectiveVisible(const bool visible)
{
	perspectives->setVisible(visible);
}

void DataViewConfigurationWidget::setPerspectiveEnabled(const bool enable)
{
	perspectives->setEnable(enable);
}

void DataViewConfigurationWidget::setCurrentPerspective(const int idx)
{
	if (perspectives->setCurrentItem(idx, true) == true){
		emit perspectiveChanged();
	}
}

void DataViewConfigurationWidget::setContentVisible(const bool visible)
{
	contents->setVisible(visible);
}

void DataViewConfigurationWidget::setContentEnabled(const bool enable)
{
	contents->setEnable(enable);
}

void DataViewConfigurationWidget::setCurrentContent(const int idx)
{
	if (contents->setCurrentItem(idx, true) == true){
		emit contentChanged();
	}
}

void DataViewConfigurationWidget::onFilterChange(int idx)
{
	filters->setCurrentItem(idx);
	emit filterChanged();
}

void DataViewConfigurationWidget::onPerspectiveChange(int idx)
{
	perspectives->setCurrentItem(idx);
	emit perspectiveChanged();
}

void DataViewConfigurationWidget::onContentChange(int idx)
{
	contents->setCurrentItem(idx);
	emit contentChanged();
}