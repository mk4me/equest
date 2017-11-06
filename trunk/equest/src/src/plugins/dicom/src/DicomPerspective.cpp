#include "DicomPCH.h"
#include "plugins/dicom/DicomPerspective.h"
#include <corelib/HierarchyItem.h>

#include <corelib/IDataManagerReader.h>
#include <corelib/DataAccessors.h>
#include <plugins/subject/IMotion.h>
#include <corelib/HierarchyDataItem.h>
#include <corelib/HierarchyHelper.h>
#include <plugins/subject/ISubject.h>
#include <plugins/hmdbCommunication/IPatient.h>
#include <corelib/PluginCommon.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include "plugins/dicom/ILayeredImage.h"
#include "DicomInternalStruct.h"
#include "LayeredImage.h"
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <corelib/ISourceManager.h>
#include <corelib/PluginCommon.h>
#include "PointsLayer.h"
#include <QtWidgets/QApplication>
#include <corelib/ISourceManager.h>
#include <boost/format.hpp>
#include "LayeredSerie.h"
#include "LayeredImageVisualizerView.h"
#include "plugins/hmdbCommunication/IHMDBShallowCopyContext.h"
#include "DicomLoader.h"
#include <QtWidgets/QProgressDialog>
#include "DescriptionMaker.h"

typedef utils::Filesystem fs;

dicom::LayersVectorConstPtr resolveLocalXml( const fs::Path& xmlPath, dicom::LayersVectorConstPtr layersVector = dicom::LayersVectorConstPtr() )
{
    if (fs::pathExists(xmlPath)) {
        dicom::LayersVectorPtr layers = dicom::DicomLoader::loadLayers(xmlPath);
        if (layers->size()) {
            return layers;
        }
    }
    return layersVector;
}

std::string getUserName(const void * data)
{
    auto comm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (comm != nullptr){
		auto shallowCtx = comm->shallowContext(0);
		if (shallowCtx != nullptr) {
			return shallowCtx->shallowCopyLocalContext()->localContext()->dataContext()->userName();
		}
		/*auto shallowCtx = comm->shallowContextForData(data);
		if (shallowCtx != nullptr){
		return shallowCtx->shallowCopyLocalContext()->localContext()->dataContext()->userName();
		}*/
	}

	return std::string();
}

std::string getUserName(const core::VariantConstPtr data)
{
	auto comm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (comm != nullptr){
		auto shallowCtx = comm->shallowContextForData(data);
		if (shallowCtx != nullptr){
			return shallowCtx->shallowCopyLocalContext()->localContext()->dataContext()->userName();
		}
	}

	return std::string();
}

core::IHierarchyItemPtr dicom::DicomPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
    std::string name = getUserName(subject.get());

	auto comm = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
    
    fs::Path tmpDir = plugin::getUserDataPath() / "MEDUSA" / "tmp";
    if (!fs::pathExists(tmpDir)) {
        fs::createDirectory(tmpDir);
    }


    bool hasData = false;
    auto rootItem = utils::make_shared<core::HierarchyItem>(QString(), QString(), QIcon());
    core::ConstVariantsList sessions;
    subject->getSessions(sessions);
    for (auto it = sessions.crbegin(); it != sessions.crend(); ++it) {
        core::ConstVariantsList motions;
        PluginSubject::SessionConstPtr s = (*it)->get();
        QString label(QString::fromUtf8(s->getLocalName().c_str()));
        QString desc("");
        core::VariantConstPtr sessionWrp;
        if (s->hasObject(typeid(dicom::DicomInternalStruct),false)) {
            core::ConstVariantsList inter;
			s->getObjects(inter, typeid(dicom::DicomInternalStruct), false);
            sessionWrp = *inter.begin();
            std::string sessionName;
            if (sessionWrp->getMetadata("core/source", sessionName)) {
                fs::Path sp(sessionName);
                label = QString(sp.stem().string().c_str());
            }
            dicom::DicomInternalStructConstPtr internalStruct = sessionWrp->get();
            auto session = internalStruct->tryGetSession(label.toStdString());
            if (session) {
                desc = dicom::DescriptionMaker::createDesc(*session);
            }
            //DicomInternalStructConstPtr test = (*inter.begin())->get();
		} else {
			PLUGIN_LOG_ERROR("No session file found!");
			// nie znaleziono pliku sesji, takie dane nie moga miec informacji o power dopplerze
			return core::IHierarchyItemPtr(); // < ------------	return !
		}
        core::HierarchyDataItemPtr sessionItem(new core::HierarchyDataItem(QIcon(), label, desc));
	    
        rootItem->appendChild(sessionItem);
		std::vector<DicomHelperPtr> helpers;
        s->getMotions(motions);

        for(auto motionOW : motions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
            std::string trialName;
            motionOW->getMetadata("core/source", trialName);
            
			if (motion->hasObject(typeid(dicom::ILayeredImage), false)) {
                core::ConstVariantsList images;
                core::ConstVariantsList layers;
                motion->getObjects(images, typeid(dicom::ILayeredImage), false);	
                motion->getObjects(layers, typeid(dicom::LayersVector), false);
                
                // w bazie jest po jednym pliku na motion
                if (images.size() == 1) {
                    core::VariantConstPtr wrapper = (*images.begin());
                    std::string sourceFile;
                    if (wrapper->getMetadata("core/source", sourceFile)) {
                        fs::Path stem = fs::Path(sourceFile).stem();
						std::string sstem = stem.string();
						std::string xmlFilenamePattern = (tmpDir / sstem).string() + ".%1%.xml";
                        fs::Path xmlFilename(boost::str(boost::format(xmlFilenamePattern) % name));
                        
                        LayeredImageConstPtr img = wrapper->get();
						LayeredImagePtr ncimg = boost::const_pointer_cast<LayeredImage>(img);
                        
                        std::string imageFilename = stem.string();
                        if (sessionWrp) {
                            dicom::DicomInternalStructConstPtr internalStruct = sessionWrp->get();
                            auto internalImage = internalStruct->tryGetImage(imageFilename + ".png");
                            if (internalImage) {
								desc = dicom::DescriptionMaker::createDesc(*internalImage);
								ncimg->setIsPowerDoppler(internalImage->isPowerDoppler);
                            }
                        }
						int trialID = -1;
						{
							auto ss = stem.string();
							const hmdbCommunication::IHMDBShallowCopyContextPtr shallow = comm->shallowContextForData(wrapper);
							auto & shallowCopy = shallow->shallowCopyDataContext()->shallowCopy()->motionShallowCopy;
							auto& trials = shallowCopy.trials;
							for (auto it = trials.begin(); it != trials.end(); ++it) {
								auto* trial = it->second;
								if (ss.find(trial->trialName) != std::string::npos) {
									trialID = trial->trialID;
									break;
								}
							}
						}
						ncimg->setTrialID(trialID);
                        
                        QIcon icon;
                        auto hasAnnotation = [&](const std::string& filename, const core::ConstVariantsList& layers) -> bool {
                            for (auto it = layers.begin(); it != layers.end(); ++it) {
                                std::string source;
                                (*it)->getMetadata("core/source", source);
                                if (source.find(filename) != std::string::npos) {
                                    return true;
                                }
                            }
                            return false;
                        };
                        if (fs::pathExists(xmlFilename) || hasAnnotation(name, layers)) {
                            icon = QIcon(":/dicom/file_done.png");
                        } else if (!layers.empty()) {
                            icon = QIcon(":/dicom/file_blue.png");
                        } else {
                            icon = QIcon(":/dicom/file.png");
                        }
                        
                        DicomHelperPtr helper = DicomHelperPtr(new DicomHelper(wrapper, layers, xmlFilenamePattern, trialName));
						helpers.push_back(helper);
                        core::HierarchyDataItemPtr imgItem(new core::HierarchyDataItem(icon, QString::fromStdString(imageFilename), desc, helper));
                        std::string mapName = imageFilename + ".png";

                        // TODO : to rozwiazanie moze byc problematyczne w przyszlosci, bo w calosci zalezy od implementacji update'a drzewa danych
                        auto itName = name2hierarchy.find(mapName);
                        if (itName == name2hierarchy.end()) {
                            name2hierarchy[imageFilename + ".png"] = imgItem;
                        }

                        sessionItem->appendChild(imgItem);
                        hasData = true;
                    } else {
                        UTILS_ASSERT(false);
                    }
                } else {
                    // powinien byc zawsze jeden obraz na motion
                    UTILS_ASSERT(false);
                }
            }
        }

		DicomMultiHelperPtr helper = utils::make_shared<DicomMultiHelper>(sessionItem);
		sessionItem->addHelper(helper);
    }

    if (hasData) {
        return rootItem;
    }

    return core::IHierarchyItemPtr();
}

core::HierarchyDataItemPtr dicom::DicomPerspective::tryGetHierarchyItem( const std::string& filename ) const
{
    auto it = name2hierarchy.find(filename);
    if (it != name2hierarchy.end()) {
        return (it->second).lock();
    }

    return core::HierarchyDataItemPtr();
}


void dicom::DicomHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series )
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    core::VariantPtr wrp = utils::const_pointer_cast<core::Variant>(wrapper);
    std::string name = getUserName(wrapper);
	fs::Path realXmlFilename(boost::str(boost::format(xmlFilename) % name));
    wrp->setMetadata("DICOM_XML", realXmlFilename.string());
	wrp->setMetadata("DICOM_XML_PATTERN", xmlFilename);
    wrp->setMetadata("TRIAL_NAME", trialName);
    //core::VariantPtr wrp = wrapper->clone();
    auto serie = visualizer->createSerie(wrp);
    auto layeredSerie = dynamic_cast<LayeredSerie*>(serie->innerSerie());
    UTILS_ASSERT(layeredSerie);
    layeredSerie->setName(path.toStdString());
                        
    ILayeredImagePtr img = layeredSerie->getImage();
    //LayeredImagePtr img = utils::const_pointer_cast<LayeredImage>(cimg);
    bool localAdded = false;

    for (auto itXml = layers.begin(); itXml != layers.end(); ++itXml) {
        LayersVectorConstPtr layersVector = (*itXml)->get();
        std::string xmlUser = "unknown";
        if ((*itXml)->getMetadata("core/source", xmlUser)) {
            // TODO zrobic to regexem
            xmlUser = fs::Path(xmlUser).stem().string();
            xmlUser = xmlUser.substr(xmlUser.find_last_of(".") + 1);
        }
        if (name == xmlUser) {
            layersVector = resolveLocalXml(realXmlFilename, layersVector);
            localAdded = true;
        }

		// TODO - sprawdzic, jakie sa warunki przy nullptr i czy nie wynika to z jakiegos bledu
		if (!layersVector) {
			continue;
		}
		bool bFound = false;
		bool iFound = false;
		bool fFound = false;
		bool jFound = false;
		bool imgFound = false;

		for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {

			switch((*layerIt)->getAdnotationIdx()){
			case dicom::annotations::bloodLevel:
				bFound = true;
				break;


			case dicom::annotations::inflammatoryLevel:
				iFound = true;
				break;

			case dicom::annotations::jointType:
				jFound = true;
				break;

			case dicom::annotations::fingerType:
				fFound = true;
				break;

			case dicom::annotations::imageType:
				imgFound = true;
				break;
			}
		}		

		if(iFound == false){
			auto l = dicom::ILayerItemPtr(new InflammatoryLevelLayer(dicom::annotations::inflammatoryLevel, dicom::annotations::unknownInflammatoryLevel));
			l->setName(QObject::tr("Inflammatory level"));			
			img->addLayer(l, xmlUser);
		}

		if(bFound == false && img->isPowerDoppler() == true){
			auto l = dicom::ILayerItemPtr(new BloodLevelLayer(dicom::annotations::bloodLevel, dicom::annotations::unknownBloodLevel));
			l->setName(QObject::tr("Blood level"));			
			img->addLayer(l, xmlUser);
		}

		if(fFound == false){
			auto l = dicom::ILayerItemPtr(new FingerTypeLayer(dicom::annotations::fingerType, dicom::annotations::unknownFinger));
			l->setName(QObject::tr("Finger type"));			
			img->addLayer(l, xmlUser);
		}

		if(jFound == false){
			auto l = dicom::ILayerItemPtr(new JointTypeLayer(dicom::annotations::jointType, dicom::annotations::unknownJoint));
			l->setName(QObject::tr("Joint type"));			
			img->addLayer(l, xmlUser);
		}

		if(imgFound == false){
			auto l = dicom::ILayerItemPtr(new ImageQualityLayer(dicom::annotations::imageType, dicom::annotations::different));
			l->setName(QObject::tr("Image"));			
			img->addLayer(l, xmlUser);
		}

        for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
            img->addLayer(*layerIt, xmlUser);
        }
    }

    // nie bylo konfliktu danych lokalnych vs sciagnietych, ale mozliwe, ze sa tylko lokalne
    if (!localAdded) {

		bool bFound = false;
		bool iFound = false;
		bool fFound = false;
		bool jFound = false;
		bool imgFound = false;

        auto layersVector = resolveLocalXml(realXmlFilename);
        if (layersVector) {
            for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
                
				switch((*layerIt)->getAdnotationIdx()){
				case dicom::annotations::bloodLevel:
					bFound = true;
					break;


				case dicom::annotations::inflammatoryLevel:
					iFound = true;
					break;

				case dicom::annotations::jointType:
					jFound = true;
					break;

				case dicom::annotations::fingerType:
					fFound = true;
					break;

				case dicom::annotations::imageType:
					imgFound = true;
					break;
				}
            }
        }

		if(iFound == false){
			auto l = dicom::ILayerItemPtr(new InflammatoryLevelLayer(dicom::annotations::inflammatoryLevel, dicom::annotations::unknownInflammatoryLevel));
			l->setName(QObject::tr("Inflammatory level"));			
			img->addLayer(l, name);
		}

		if(bFound == false && img->isPowerDoppler() == true){
			auto l = dicom::ILayerItemPtr(new BloodLevelLayer(dicom::annotations::bloodLevel, dicom::annotations::unknownBloodLevel));
			l->setName(QObject::tr("Blood level"));			
			img->addLayer(l, name);
		}

		if(fFound == false){
			auto l = dicom::ILayerItemPtr(new FingerTypeLayer(dicom::annotations::fingerType, dicom::annotations::unknownFinger));
			l->setName(QObject::tr("Finger type"));			
			img->addLayer(l, name);
		}

		if(jFound == false){
			auto l = dicom::ILayerItemPtr(new JointTypeLayer(dicom::annotations::jointType, dicom::annotations::unknownJoint));
			l->setName(QObject::tr("Joint type"));			
			img->addLayer(l, name);
		}

		if(imgFound == false){
			auto l = dicom::ILayerItemPtr(new ImageQualityLayer(dicom::annotations::imageType, dicom::annotations::different));
			l->setName(QObject::tr("Image"));			
			img->addLayer(l, name);
		}

		if (layersVector) {
			for (auto layerIt = layersVector->cbegin(); layerIt != layersVector->cend(); ++layerIt) {
				img->addLayer(*layerIt, name);				
			}
		}
    }

    for (int row = img->getNumTags() - 1; row >= 0; --row) {
        std::string tag = img->getTag(row);
        img->setTagVisible(tag, tag == name);
    }
    layeredSerie->refresh();
    LayeredImageVisualizerView* view = dynamic_cast<LayeredImageVisualizerView*>(visualizer->getOrCreateWidget());
    UTILS_ASSERT(view);
    view->refresh();
    visualizer->setActiveSerie(serie);
    series.push_back(serie);    
}

dicom::DicomHelper::DicomHelper( const core::VariantConstPtr & wrapper, const core::ConstVariantsList& layers,
    const std::string& xmlFilename, const std::string& trialName) :
    WrappedItemHelper(wrapper),
    layers(layers),
    xmlFilename(xmlFilename),
    trialName(trialName)
{

}

void dicom::DicomMultiHelper::createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series)
{
	auto item = sessionItem.lock();
	if (item) {
		auto helpers = getHelpers(item.get());
		QProgressDialog progress("Task in progress...", "Cancel", 0, helpers.size() + 1, visualizer->getWidget());
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);
		progress.setValue(0);
		progress.setLabelText(QObject::tr("Processing..."));
		QApplication::processEvents();
		progress.setValue(1);
		for (auto& helper : helpers) {
			if (progress.wasCanceled()) {
				break;
			}
			helper->createSeries(visualizer, path, series);
			progress.setValue(progress.value() + 1);
			progress.setLabelText(QObject::tr("Processing: ") + QString::fromStdString(helper->getTrialName()));
		}
		
	}
}

core::VisualizerPtr dicom::DicomMultiHelper::createVisualizer(core::IVisualizerManager* manager)
{
	auto item = sessionItem.lock();
	if (item) {
		auto helpers = getHelpers(item.get());
		return helpers[0]->createVisualizer(manager);
	}

	throw std::runtime_error("Unable to create visualizer");
}

std::vector<utils::TypeInfo> dicom::DicomMultiHelper::getTypeInfos() const
{
	auto item = sessionItem.lock();
	if (item) {
		auto helpers = getHelpers(item.get());
		return helpers[0]->getTypeInfos();
	}

	throw std::runtime_error("Unable to get type infos");
}

//dicom::DicomMultiHelper::DicomMultiHelper(const std::vector<DicomHelperPtr>& helpers)
//{
//	this->helpers = helpers;
//	UTILS_ASSERT(helpers.empty() == false);
//}

dicom::DicomMultiHelper::DicomMultiHelper(core::IHierarchyItemWeakPtr item) : 
	sessionItem(item)
{

}

std::vector<dicom::DicomHelperPtr> dicom::DicomMultiHelper::getHelpers(core::IHierarchyItem* item) const
{
	std::vector<DicomHelperPtr> ret;
	auto count = item->getNumChildren();
	for (auto i = 0; i < count; i++) {
		auto data = dynamic_cast<const core::IHierarchyDataItem*>(item->getChild(i).get());
		auto helpers = data->getHelpers();
		for (auto& h : helpers) {
			auto dh = utils::dynamic_pointer_cast<DicomHelper>(h);
			if (dh) {
				ret.push_back(dh);
			}
		}

	}
	return ret;
}
