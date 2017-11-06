#include "MedusaExporterPCH.h"
#include "MedusaExporterService.h"
#include "MedusaExporterServiceWidget.h"
#include <utils/Filesystem.h>
#include <corelib/IServiceManager.h>
#include <corelib/PluginCommon.h>
#include <corelib/IDataManager.h>
#include <corelib/IDataManager.h>

using namespace medusaExporter;

MedusaExporterService::MedusaExporterService()
: exportWidget(nullptr), memoryManager(nullptr)
{
	
}

MedusaExporterService::~MedusaExporterService()
{
}


QWidget* MedusaExporterService::getWidget()
{
	return nullptr;
}

QWidgetList MedusaExporterService::getPropertiesWidgets()
{	
	return QWidgetList();
}

const bool MedusaExporterService::lateInit()
{	
	return true;
}

void MedusaExporterService::finalize()
{
	
}

void MedusaExporterService::update(double time)
{
	
}

void MedusaExporterService::init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager,
	core::IDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager,
	core::IFileDataManager * fileDataManager, core::IDataHierarchyManager * hierarchyManager)
{
	this->memoryManager = memoryDataManager;
	exporterModel = utils::make_shared<ExporterModel>(fileDataManager, this->memoryManager);
}

QWidget* medusaExporter::MedusaExporterService::getExporterDialog()
{
	exportWidget = new MedusaExporterServiceWidget(this);
	exportWidget->setExporter(exporterModel);
	QString exportDir = QString::fromStdString(plugin::getUserDataPath("Export").string());
	exportWidget->setExportDir(exportDir);
	return exportWidget;
}


