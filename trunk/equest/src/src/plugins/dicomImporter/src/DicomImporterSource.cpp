//#include "DicomImporterPCH.h"
#include "DicomImporterSource.h"
#include <utils/Filesystem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "DicomImporterSourceWidget.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include <corelib/HierarchyHelper.h>
//#include <plugins/dicom/ILayeredImage.h>
#include "corelib/IVisualizerManager.h"
//#include "LayeredImageVisualizer.h"
#include "dicomlib/DicomImporter.h"
#include <boost/archive/xml_iarchive.hpp>
#include "DicomVisualizer.h"
#include <dicomlib/Dicom.h>
//#include "BackgroundLayer.h"

using namespace dicomImporter;



//DEFINE_SMART_POINTERS(LayerHelper);

//class MultiLayerHelper : public core::HierarchyHelper
//{
//public:
//    MultiLayerHelper(const std::vector<LayerHelperConstPtr>& helpers ) :
//      helpers(helpers)
//      {
//
//      }
//
//      virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
//      {
//          for (auto it = helpers.begin(); it != helpers.end(); ++it) {
//              (*it)->createSeries(visualizer, path, series);
//          }
//      }
//
//      virtual core::VisualizerPtr createVisualizer( core::IVisualizerManager* manager )
//      {
//          //auto vis = manager->getVisualizerPrototype(core::UID::GenerateUniqueID("{4CBA33A4-F0EA-4607-9CB1-C1816697D1A1}"));
//          auto vis = manager->getVisualizerPrototype(LayeredImageVisualizer::getClassID());
//          UTILS_ASSERT(vis);
//          return core::VisualizerPtr(vis->create());
//      }
//
//      virtual std::vector<core::TypeInfo> getTypeInfos() const
//      {
//          std::vector<core::TypeInfo> ret;
//          ret.push_back(typeid(ILayeredImage));
//          return ret;
//      }
//
//private:
//    const std::vector<LayerHelperConstPtr>& helpers;
//
//};
//
//template <class HelperPtr>
//class MultiLayerHelper : public HelperPtr::element_type
//{
//public:
//    MultiLayerHelper(const std::vector<HelperPtr>& helpers ) :
//      helpers(helpers)
//      {
//
//      }
//
//      virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
//      {
//          for (auto it = helpers.begin(); it != helpers.end(); ++it) {
//              (*it)->createSeries(visualizer, path, series);
//          }
//      }
//
//private:
//    const std::vector<HelperPtr> helpers;
//
//};
//typedef MultiLayerHelper<LayerHelperPtr> MultiHelper;
//DEFINE_SMART_POINTERS(MultiHelper);

DicomImporterSource::DicomImporterSource() :
    fileDM(nullptr),
    memoryDM(nullptr),
	streamDM(nullptr)
{

}


void DicomImporterSource::init(core::IDataManager * memoryDM,
							   core::IStreamDataManager * streamDM,
							   core::IFileDataManager * fileDM,
							   core::IDataHierarchyManager * hierarchyDM)
{
    this->fileDM = fileDM;
    this->memoryDM = memoryDM;
    this->streamDM = streamDM;
}

bool DicomImporterSource::lateInit()
{
    return true;
}

void DicomImporterSource::finalize()
{
    
}

void DicomImporterSource::update( double deltaTime )
{
    
}

QWidget* DicomImporterSource::getWidget()
{
    return new DicomImporterSourceWidget(this);
}

QWidget* DicomImporterSource::getControlWidget()
{
    return nullptr;
}

QWidget* DicomImporterSource::getSettingsWidget()
{
    return nullptr;
}

void DicomImporterSource::getOfferedTypes( utils::TypeInfoList & offeredTypes ) const
{
    
}

//void DicomImporterSource::addFile( const utils::Filesystem::Path& path )
//{
//    auto root = transactionPart<core::WrappedItemHelper>(path);
//    auto hierarchyTransaction = memoryDM->hierarchyTransaction();
//    hierarchyTransaction->addRoot(root);
//}

//void createHierarchy(core::IHierarchyItemPtr root, DcmDirectoryRecord* record)
//{
//    auto count = record->cardSub();
//    auto elemCount = record->card();
//    for (auto i = 0; i < elemCount; ++i) {
//        DcmElement* element = record->getElement(i);
//        
//        if (element) {
//            auto tag = element->getTag();
//            QString desc(tag.getTagName());
//            core::IHierarchyItemPtr itm = utils::make_shared<core::HierarchyItem>(QString("Element %1").arg(i + 1), desc, QIcon());
//            root->appendChild(itm);
//        }
//    }
//
//    for (auto i = 0; i < count; ++i) {
//        DcmDirectoryRecord* child = record->getSub(i);
//        core::IHierarchyItemPtr itm = utils::make_shared<core::HierarchyItem>(QString("childDir"), QString("desc"), QIcon());
//        root->appendChild(itm);
//        createHierarchy(itm, child);
//    }
//}
//
//void DicomImporterSource::loadDirFile( const utils::Filesystem::Path& dirPath )
//{
//    utils::Filesystem::Path dirfile = dirPath / "DICOMDIR";
//    if (utils::Filesystem::pathExists(dirfile)) {
//        /*DcmDicomDir* dir = new DcmDicomDir(dirfile.string().c_str());
//        DcmDirectoryRecord *rec = &(dir->getRootRecord());
//
//        core::IHierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(QString("DICOMDIR"), QString("main dicom directory"), QIcon());
//        createHierarchy(root, rec);
//
//        auto hierarchyTransaction = memoryDM->hierarchyTransaction();
//        hierarchyTransaction->addRoot(root);
//        int test = 0;*/
//
//        DcmDicomDir dicomdir(dirfile.string().c_str());
//        DcmDirectoryRecord* root = &(dicomdir.getRootRecord());
//                
//        std::string basePath = dirPath.string();
//        
//        if (root) {
//            core::IHierarchyItemPtr rootItm = utils::make_shared<core::HierarchyItem>(QString("DICOMDIR"), QString("main dicom directory"), QIcon());
//            DcmDirectoryRecord *patientRecord = nullptr;
//            while ((patientRecord = root->nextSub(patientRecord)) != nullptr) {
//                handlePatientRecord(patientRecord, rootItm, basePath);
//            }
//
//            auto hierarchyTransaction = memoryDM->hierarchyTransaction();
//            hierarchyTransaction->addRoot(rootItm);
//        } 
//
//    } else {
//        throw loglib::runtime_error("file not found");
//    }
//}
//
//void DicomImporterSource::handleFileRecord( DcmDirectoryRecord * fileRecord, core::IHierarchyItemPtr root, std::string basePath, std::vector<LayerHelperPtr>& helpers )
//{
//    OFString tmpString;
//    if(fileRecord->findAndGetOFStringArray(DCM_DirectoryRecordType,tmpString).good() && (tmpString == "IMAGE" || tmpString == "ENCAP DOC"))
//    {
//        if(fileRecord->findAndGetOFStringArray(DCM_ReferencedFileID,tmpString).good())
//        {
//            OFString uidImagen,instanceNumber;
//            fileRecord->findAndGetOFString(DCM_ReferencedSOPInstanceUIDInFile, uidImagen);
//            fileRecord->findAndGetOFString(DCM_InstanceNumber, instanceNumber);
//
//            utils::Filesystem::Path currentPath(basePath);
//            utils::Filesystem::Path suffix(tmpString.c_str());
//            currentPath /= suffix;
//            if (utils::Filesystem::pathExists(currentPath)) {
//                //utils::ObjectWrapperPtr wrapper;
//                ////HierarchyDataItemPtr dataItem = utils::make_shared<HierarchyDataItem>(wrapper, QString());                
//                //core::HierarchyItemPtr dataItem = utils::make_shared<core::HierarchyItem>(QString("image"), QString());                
//                auto dataItem = transactionPart<core::WrappedItemHelper>(currentPath);
//                core::HierarchyDataItemPtr di = utils::dynamic_pointer_cast<core::HierarchyDataItem>(dataItem); 
//                if (di) {
//                    LayerHelperPtr helper = utils::make_shared<LayerHelper>(di->getData());
//                    helpers.push_back(helper);
//                    di->addHelper(helper);
//                }
//                root->appendChild(dataItem);
//            } 
//        }
//    }
//}
//
//void DicomImporterSource::handleSeriesRecord( DcmDirectoryRecord * seriesRecord, core::IHierarchyItemPtr root, std::string basePath )
//{
//    OFString serieId, modality, serieDate, serieTime, serieDesc, serieNumber, physicianName;
//    seriesRecord->findAndGetOFString(DCM_SeriesInstanceUID, serieId);
//    seriesRecord->findAndGetOFString(DCM_Modality, modality);
//    seriesRecord->findAndGetOFString(DCM_SeriesDate, serieDate);
//    seriesRecord->findAndGetOFString(DCM_SeriesTime, serieTime);
//    seriesRecord->findAndGetOFString(DCM_SeriesDescription, serieDesc);
//    seriesRecord->findAndGetOFString(DCM_SeriesNumber, serieNumber);
//    seriesRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
//      
//
//    QString name = QString("Serie: %1").arg(serieNumber.c_str());
//
//    core::HierarchyDataItemPtr serieItem = utils::make_shared<core::HierarchyDataItem>(QIcon(), name, QString());
//    DcmDirectoryRecord * fileRecord = nullptr;
//    std::vector<LayerHelperPtr> helpers;
//
//    while((fileRecord = seriesRecord->nextSub(fileRecord)) != nullptr) {
//        handleFileRecord(fileRecord, serieItem, basePath, helpers);
//    }	
//
//    int childrenCount = serieItem->getNumChildren();
//    if (childrenCount > 0) {  
//        MultiHelperPtr multi = utils::make_shared<MultiHelper>(helpers);
//        serieItem->addHelper(multi);
//
//        root->appendChild(serieItem);
//    }
//}
//
//void DicomImporterSource::handleStudyRecord( DcmDirectoryRecord * studyRecord, core::IHierarchyItemPtr root, std::string basePath )
//{
//    OFString studyId, studyDesc, studyDate, studyTime, physicianName, accesionNumber;
//    studyRecord->findAndGetOFString(DCM_StudyInstanceUID, studyId);
//    studyRecord->findAndGetOFString(DCM_StudyDescription, studyDesc);
//    studyRecord->findAndGetOFString(DCM_StudyDate, studyDate);
//    studyRecord->findAndGetOFString(DCM_StudyTime, studyTime);
//    studyRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
//    studyRecord->findAndGetOFString(DCM_AccessionNumber, accesionNumber);
//
//    QString name = QString("Study");
//    if (studyDate.length() == 8) {
//        name += QString(" %1.%2.%3")
//            .arg(studyDate.substr(0, 4).c_str())
//            .arg(studyDate.substr(4, 2).c_str())
//            .arg(studyDate.substr(6, 2).c_str());
//    }
//    core::IHierarchyItemPtr studyItem = utils::make_shared<core::HierarchyItem>(name, QString());
//
//    DcmDirectoryRecord* seriesRecord = nullptr;    
//    while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != nullptr)) {
//        handleSeriesRecord(seriesRecord, studyItem, basePath);
//    }
//
//    if (studyItem->getNumChildren() > 0) {
//        root->appendChild(studyItem);
//    }
//}
//
//void DicomImporterSource::handlePatientRecord( DcmDirectoryRecord * patientRecord, core::IHierarchyItemPtr root, std::string basePath )
//{
//    OFString patientID, patientName, patientBirthdate, patientSex;
//    patientRecord->findAndGetOFString(DCM_PatientID,patientID);
//    patientRecord->findAndGetOFString(DCM_PatientName,patientName);
//    patientRecord->findAndGetOFString(DCM_PatientBirthDate, patientBirthdate);
//    patientRecord->findAndGetOFString(DCM_PatientSex, patientSex);
//    
//    QString name = QString("Patient: %1 %2 %3 %4").arg(patientID.c_str()).arg(patientName.c_str()).arg(patientBirthdate.c_str()).arg(patientSex.c_str());
//    core::IHierarchyItemPtr patientItem = utils::make_shared<core::HierarchyItem>(name, QString());
//
//    DcmDirectoryRecord* studyRecord = nullptr;
//    while ((studyRecord = patientRecord->nextSub(studyRecord)) != nullptr)  {
//        handleStudyRecord(studyRecord, patientItem, basePath);
//    }
//
//    if (patientItem->getNumChildren() > 0) {
//        root->appendChild(patientItem);
//    }
//}
//
//template <class Helper>
//core::IHierarchyItemPtr DicomImporterSource::transactionPart( const utils::Filesystem::Path &path )
//{
//    auto transaction = fileDM->transaction();
//    transaction->addFile(path);
//    core::ConstObjectsList oList;
//    transaction->getObjects(path, oList);
//
// /*   core::ObjectsList oList;
//    std::transform(cList.begin(), cList.end(), std::back_inserter(oList), 
//        [&](utils::ObjectWrapperConstPtr w) 
//        { 
//            return utils::const_pointer_cast<utils::ObjectWrapper>(w); 
//        });
// */   QString filename = QString::fromStdString(path.stem().string());
//    QString desc = QString::fromStdString(path.string());
//
//    /*if (oList.size() == 1) {
//        utils::ObjectWrapperPtr wrapper = *oList.begin();
//        ILayeredImagePtr img = wrapper->get();
//        for (auto it = s.begin(); it != s.end(); ++it) {
//            img->addLayer(*it);
//        }
//        return utils::make_shared<core::HierarchyDataItem>(*oList.begin(), QIcon(), filename, desc, utils::make_shared<Helper>(*oList.begin()));
//    }*/
//
//    //UTILS_ASSERT(false, "Obsolete code");
//    core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(filename, desc, QIcon());
//    for (auto it = oList.begin(); it != oList.end(); ++it) {
//        core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(*it, QIcon(), QString((*it)->getClassName().c_str()), desc,  utils::make_shared<Helper>(*it));
//        root->appendChild(item);
//    }
//
//    return root;
//}

void dicomImporter::DicomImporterSource::import( const utils::Filesystem::Path& from, const utils::Filesystem::Path& to )
{
    DicomImporter importer;
    auto inter = importer.import(from);
    std::vector<DicomInternalStructPtr> splits = importer.split(inter);

    DicomSaver saver;
    for (auto it = splits.begin(); it != splits.end(); ++it) {
        importer.convertImages(*it, from, to);
        saver.save(to, *it);
    }
}

//void dicomImporter::DicomImporterSource::openInternalDataMainFile( utils::Filesystem::Path path )
//{
//    DicomLoader loader;
//    auto inter = loader.load(path);
//    int count = inter->getNumPatients();
//    for (int i = 0; i < count; ++i) {
//        auto patient = inter->getPatient(i);
//        for (auto itSession = patient->sessions.begin(); itSession != patient->sessions.end(); ++itSession) {
//            QString name((*itSession)->getOutputDirectory().c_str());
//
//            core::HierarchyDataItemPtr sessionItem = utils::make_shared<core::HierarchyDataItem>(QIcon(), name, QString());
//            std::vector<LayerHelperPtr> helpers;
//
//            for (auto itSerie = (*itSession)->series.begin(); itSerie !=  (*itSession)->series.end(); ++itSerie) {
//                for (auto itImage = (*itSerie)->images.begin(); itImage != (*itSerie)->images.end(); ++itImage) {
//                    auto dataItem = transactionPart<LayerHelper>(path.parent_path() / (*itImage)->imageFile);
//                    core::HierarchyDataItemPtr di = utils::dynamic_pointer_cast<core::HierarchyDataItem>(dataItem); 
//                    if (di) {
//                        auto itemHelpers = di->getHelpers();
//                        for (auto itHelper = itemHelpers.begin(); itHelper != itemHelpers.end(); ++itHelper) {
//                            LayerHelperPtr helper = utils::dynamic_pointer_cast<LayerHelper>(*itHelper);
//                            if (helper) {
//                                helpers.push_back(helper);
//                            }
//                        }
//                        //helpers.insert(helpers.end(), di->getHelpers().begin(), di->getHelpers().end());
//                        //LayerHelperPtr helper = utils::make_shared<LayerHelper>(di->getData());
//                        //helpers.push_back(helper);
//                        //di->addHelper(helper);
//                    }
//                    sessionItem->appendChild(dataItem);
//
//                }
//            }
//            
//            int childrenCount = sessionItem->getNumChildren();
//            if (childrenCount > 0) {  
//                MultiHelperPtr multi = utils::make_shared<MultiHelper>(helpers);
//                sessionItem->addHelper(multi);
//            }
//
//            if (sessionItem->getNumChildren()) {
//                auto hierarchyTransaction = memoryDM->hierarchyTransaction();
//                hierarchyTransaction->addRoot(sessionItem);
//            }
//        }
//    }
//}

//std::vector<core::TypeInfo> dicomImporter::LayerHelper::getTypeInfos() const
//{
//    std::vector<core::TypeInfo> ret;
//    UTILS_ASSERT(false);
//    // DOKONCZYC
//    ///ret.push_back(typeid(ILayeredImage));
//    return ret;
//}

//core::VisualizerPtr dicomImporter::LayerHelper::createVisualizer( core::IVisualizerManager* manager )
//{
//    //auto vis = manager->getVisualizerPrototype(core::UID::GenerateUniqueID("{4CBA33A4-F0EA-4607-9CB1-C1816697D1A1}"));
//    auto vis = manager->getVisualizerPrototype(DicomVisualizer::getClassID());
//    UTILS_ASSERT(vis);
//    return core::VisualizerPtr(vis->create());
//}
//
//void dicomImporter::LayerHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
//{
//    utils::ObjectWrapperConstPtr layered;
//    if (imageWrapper->isSupported(typeid(DicomImage))) {
//        //DicomImageConstPtr dcm = imageWrapper->get();
//        //utils::ObjectWrapperPtr lay = utils::ObjectWrapper::create<ILayeredImage>();
//        UTILS_ASSERT(false);
//        // DOKONCZYC
//        /*ILayeredImagePtr img = utils::make_shared<LayeredImage>(convertToPixmap(utils::const_pointer_cast<DicomImage>(dcm)));
//        lay->set(img);
//        layered = lay;*/
//    } else {
//        layered = imageWrapper;
//    }
//    auto serie = visualizer->createSerie(layered->getTypeInfo(), layered);
//    serie->serie()->setName(path.toStdString());
//    series.push_back(serie);
//}
//
//dicomImporter::LayerHelper::LayerHelper( const core::ObjectWrapperConstPtr& imgWrapper ) :
//imageWrapper(imgWrapper)
//{
//
//}
