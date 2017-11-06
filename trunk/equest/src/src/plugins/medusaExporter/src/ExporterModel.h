/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:04
	filename: 	ExporterModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__EXPORTERMODEL_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__EXPORTERMODEL_H__


#include <corelib/IDataManager.h>
#include <functional>
#include "IExporter.h"
#include "AnnotationData.h"
#include <plugins/dicom/IDicomInternalStruct.h>
#include <tuple>
#include "plugins/dicom/Annotations.h"

namespace medusaExporter {

	//! Serce exportera danych MEDUSA
	class ExporterModel
	{
	public:
		
		typedef std::function<void(double, const QString&)> CallbackFunction;
	public:
		ExporterModel(core::IFileDataManager* fileManager, core::IDataManager* dataManager);
		virtual ~ExporterModel() {}


	public:
		void addExporter(const IExporterConstPtr& exporter);
		void packImages(const QString& dirPath, const QString& outFile);
		void packImages(const QString& dirPath, const QString& outFile, CallbackFunction fun);
		void extractData(const QString& path, bool copyLocalAnnotations);
		void extractData(const QString& path, bool copyLocalAnnotations, CallbackFunction fun);
		void packMeta(const QString& dirPath, const QString& outFile);
		void packMeta(const QString& dirPath, const QString& outFile, CallbackFunction fun);
        void packBoth(const QString& dirPath, const QString& outFile);
        void packBoth(const QString& dirPath, const QString& outFile, CallbackFunction fun);
        void exportData(const QString& outDir, const QString& user, const QString& dirPath, const ExportConfig& config);
        void exportData(const QString& outDir, const QString& user, const QString& dirPath, const ExportConfig& config, CallbackFunction fun);
		QStringList getUsers(const QString& path, const ExporterModel::CallbackFunction& fun) const;
        void clearMedusaExportDir();
		void recreateAnnotationDrawings(const QString& outDir, const QString& userName, const AnnotationData& data, const RecreateAnnotationsConfig& config, CallbackFunction fun);

	private:
		void pack(const QString& dirPath, const QString& outFile, std::function<bool(const utils::Filesystem::Path&)> filter, CallbackFunction fun);
		std::map<std::string, bool> gatherPowerDopplers(const QString& dirPath, CallbackFunction fun);
		void gatherPowerDopplers(dicom::IDicomInternalStructConstPtr inter, std::map<std::string, bool>& dopplers);
		utils::Filesystem::PathsList gatherInternalSessionFiles(const QString &dirPath);
		bool possibleToAdd(dicom::annotations::annotationsIdx annotation, const RecreateAnnotationsConfig& config);
	private:
		std::vector<IExporterConstPtr> exporters;
		core::IDataManager* dataManager;
		core::IFileDataManager* fileManager;
    };
	DEFINE_SMART_POINTERS(ExporterModel);


    class CallbackCollector
    {
    public:
        typedef std::function<void(ExporterModel::CallbackFunction)> Operation;
        CallbackCollector(const ExporterModel::CallbackFunction& mainCallback);
        virtual ~CallbackCollector();

    public:
        void addOperation(const Operation& op, double weight, const QString& desc);
        void run();

    private:
        void innerCallback(double ratio, const QString& desc);
        double getWeightSum() const;

    private:
        ExporterModel::CallbackFunction mainCallback;
        std::vector<std::tuple<Operation, double, QString>> operations;
        int currentOperation;
    };
}

#endif
