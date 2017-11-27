#include "EquestExporterPCH.h"
#include "EquestExporterService.h"
#include "EquestExporterServiceWidget.h"
#include <utils/Filesystem.h>
#include <corelib/IServiceManager.h>
#include <corelib/PluginCommon.h>
#include <corelib/IDataManager.h>
#include <corelib/IDataManager.h>

using namespace equestExporter;

EquestExporterService::EquestExporterService()
: exportWidget(nullptr), memoryManager(nullptr)
{
	
}

EquestExporterService::~EquestExporterService()
{
}


QWidget* EquestExporterService::getWidget()
{
	return nullptr;
}

QWidgetList EquestExporterService::getPropertiesWidgets()
{	
	return QWidgetList();
}

const bool EquestExporterService::lateInit()
{	
	return true;
}

void EquestExporterService::finalize()
{
	
}

void EquestExporterService::update(double time)
{
	
}

void EquestExporterService::init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager,
	core::IDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager,
	core::IFileDataManager * fileDataManager, core::IDataHierarchyManager * hierarchyManager)
{
	this->memoryManager = memoryDataManager;
	exporterModel = utils::make_shared<ExporterModel>(fileDataManager, this->memoryManager);
}

QWidget* equestExporter::EquestExporterService::getExporterDialog()
{
	exportWidget = new EquestExporterServiceWidget(this);
	exportWidget->setExporter(exporterModel);
	QString exportDir = QString::fromStdString(plugin::getUserDataPath("Export").string());
	exportWidget->setExportDir(exportDir);
	return exportWidget;
}


