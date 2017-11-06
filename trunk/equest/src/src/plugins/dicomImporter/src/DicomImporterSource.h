/********************************************************************
	created:	2013/11/18
	created:	18:11:2013   12:10
	filename: 	DicomImporterSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTERSOURCE_H__
#define HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTERSOURCE_H__

#include <corelib/ISource.h>
#include <utils/Filesystem.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyHelper.h>

class DcmDirectoryRecord;

namespace dicomImporter {


//class LayerHelper : public core::HierarchyHelper
//{
//public:
//    LayerHelper(const core::ObjectWrapperConstPtr& imgWrapper);
//    virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series );
//    virtual core::VisualizerPtr createVisualizer( core::IVisualizerManager* manager );
//    virtual std::vector<core::TypeInfo> getTypeInfos() const;
//
//protected:
//    LayerHelper() {}
//
//private:
//    utils::ObjectWrapperConstPtr imageWrapper;
//
//};
//DEFINE_SMART_POINTERS(LayerHelper);

class DicomImporterSource : public plugin::ISource
{
    UNIQUE_ID("{25C00108-F75C-491B-812B-9B3C8386723E}")
    CLASS_DESCRIPTION("Dicom Source", "Dicom Source")
public:
    DicomImporterSource();
	virtual ~DicomImporterSource() {}


public:
	virtual void init(core::IDataManager * memoryDM,
					  core::IStreamDataManager * streamDM,
					  core::IFileDataManager * fileDM,
					  core::IDataHierarchyManager * hierarchyDM);

    virtual bool lateInit();

    virtual void finalize();

    virtual void update( double deltaTime );

    virtual QWidget* getWidget();

    virtual QWidget* getControlWidget();

    virtual QWidget* getSettingsWidget();

    virtual void getOfferedTypes( utils::TypeInfoList & offeredTypes ) const;
    //void addFile( const utils::Filesystem::Path& path );

    //void loadDirFile(const utils::Filesystem::Path& dirPath );
    void import( const utils::Filesystem::Path& from, const utils::Filesystem::Path& to );
    void openInternalDataMainFile( utils::Filesystem::Path path );
//
//private:
//    void handlePatientRecord( DcmDirectoryRecord * patientRecord, core::IHierarchyItemPtr root, std::string basePath );
//    void handleStudyRecord( DcmDirectoryRecord * studyRecord, core::IHierarchyItemPtr root, std::string basePath );
//    void handleSeriesRecord( DcmDirectoryRecord * seriesRecord, core::IHierarchyItemPtr root, std::string basePath );
//    void handleFileRecord( DcmDirectoryRecord * fileRecord, core::IHierarchyItemPtr root, std::string basePath, std::vector<LayerHelperPtr>& helpers );
//    template <class Helper>
//    core::IHierarchyItemPtr transactionPart( const utils::Filesystem::Path &path );
private:
    core::IDataManager* memoryDM;
    core::IFileDataManager* fileDM;
    core::IStreamDataManager* streamDM;
};
DEFINE_SMART_POINTERS(DicomImporterSource);

}
#endif
