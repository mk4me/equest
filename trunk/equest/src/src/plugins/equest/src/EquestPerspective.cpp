#include "EquestPCH.h"
#include "plugins/equest/EquestPerspective.h"
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
#include <plugins/equest/Equest.h>
#include "EquestAnnotation.h"
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <corelib/ISourceManager.h>
#include <corelib/PluginCommon.h>
#include <QtWidgets/QApplication>
#include <corelib/ISourceManager.h>
#include <boost/format.hpp>
#include "plugins/hmdbCommunication/IHMDBShallowCopyContext.h"
#include <QtWidgets/QProgressDialog>
#include "DescriptionMaker.h"
#include "corelib/Variant.h"
#include "plugins/video/Wrappers.h"
#include "plugins/hmmlib/TreeItemHelper.h"
#include "dataaccessorlib/Wrappers.h"
#include "c3dlib/C3DTypes.h"

#include <corelib/IDataManagerReader.h>
#include <corelib/DataAccessors.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/subject/IMotion.h>
#include <plugins/kinematic/JointsItemHelper.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include "TreeBuilder.h"
#include <plugins/hmmlib/TreeItemHelper.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/video/Wrappers.h>
#include <plugins/subject/ISubject.h>
#include <plugins/hmdbCommunication/IPatient.h>
#include <corelib/PluginCommon.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <boost/lexical_cast.hpp>
#include <dataaccessorlib/Wrappers.h>

#include <plugins/newChart/NewChartItemHelper.h>
#include <corelib/IVisualizerManager.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <utils/Debug.h>
#include <loglib/Exceptions.h>
#include <boost/lexical_cast.hpp>
#include <dataaccessorlib/Wrappers.h>
#include <plugins/c3d/C3DChannels.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <corelib/Variant.h>
#include "QCoreApplication"
#include "EquestAnnotationVisualizer.h"


typedef utils::Filesystem fs;
using namespace PluginSubject;

//equest::LayersVectorConstPtr resolveLocalXml( const fs::Path& xmlPath, equest::LayersVectorConstPtr layersVector = equest::LayersVectorConstPtr() )
//{
//	throw std::runtime_error("NYI");
//    /*if (fs::pathExists(xmlPath)) {
//        equest::LayersVectorPtr layers = equest::EquestLoader::loadLayers(xmlPath);
//        if (layers->size()) {
//            return layers;
//        }
//    }
//    return layersVector;*/
//}

std::string equest::getUserName(const void * data)
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

std::string equest::getUserName(const core::VariantConstPtr data)
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

core::IHierarchyItemPtr equest::EquestPerspective::createTree(const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject)
{
	core::IHierarchyItemPtr rootItem(new core::HierarchyItem(rootItemName, QString()));
	core::ConstVariantsList filteredSessions;
	//if (dataFilter != nullptr) {
	//	dataFilter->filterSessions(sessions, filteredSessions);
	//}
	//else {
	//	filteredSessions = sessions;
	//}
	subject->getSessions(filteredSessions);

	for (auto it = filteredSessions.crbegin(); it != filteredSessions.crend(); ++it) {

		std::string emgConfigName;
		(*it)->getMetadata("EMGConf", emgConfigName);

		core::ConstVariantsList filteredMotions;

		//if (dataFilter != nullptr) {
		//	dataFilter->filterSessionMotions(*it, filteredMotions);
		//}
		//else {
			PluginSubject::SessionConstPtr s = (*it)->get();
			s->getMotions(filteredMotions);
		//}

		for (auto motionOW : filteredMotions) {

			PluginSubject::MotionConstPtr motion = motionOW->get();

			QString label(QString::fromUtf8(motion->getLocalName().c_str()));

			std::string metaLabel;
			if (motionOW->getMetadata("label", metaLabel)) {
				label = QString::fromStdString(metaLabel);
			}

			QString desc = createDescription(motion);

			core::IHierarchyItemPtr motionItem(new core::HierarchyItem(label, desc));
			rootItem->appendChild(motionItem);
			bool hasEmg = motion->hasObject(typeid(c3dlib::EMGChannel), false);
			bool hasGrf = motion->hasObject(typeid(c3dlib::GRFCollection), false);
			if (hasEmg || hasGrf) {
				core::IHierarchyItemPtr analogItem(new core::HierarchyItem(QObject::tr("Analog data"), desc));
				motionItem->appendChild(analogItem);
				if (hasEmg) {
					analogItem->appendChild(createEMGBranch(motion, QObject::tr("EMG"), getRootEMGIcon(), getEMGIcon(), emgConfigName));
				}

				if (hasGrf) {
					analogItem->appendChild(createGRFBranch(motion, QObject::tr("GRF"), getRootGRFIcon(), getGRFIcon(), std::string()));
				}
			}

			if (motion->hasObject(typeid(c3dlib::ForceCollection), false)
				|| motion->hasObject(typeid(c3dlib::MomentCollection), false)
				|| motion->hasObject(typeid(c3dlib::PowerCollection), false)) {
				core::IHierarchyItemPtr kineticItem(new core::HierarchyItem(QObject::tr("Kinetic data"), desc));
				motionItem->appendChild(kineticItem);
				core::ConstVariantsList forces;
				motion->getObjects(forces, typeid(c3dlib::ForceCollection), false);
				core::ConstVariantsList moments;
				motion->getObjects(moments, typeid(c3dlib::MomentCollection), false);
				core::ConstVariantsList powers;
				motion->getObjects(powers, typeid(c3dlib::PowerCollection), false);
				if (!forces.empty()) {
					kineticItem->appendChild(createForcesBranch(motion, QObject::tr("Forces"), getRootForcesIcon(), getForcesIcon(), std::string()));
				}
				if (!moments.empty()) {
					kineticItem->appendChild(createMomentsBranch(motion, QObject::tr("Moments"), getRootMomentsIcon(), getMomentsIcon(), std::string()));
				}
				// do rozwiniecia - potrzeba parsowaæ pliki vsk i interpretowaæ strukture kinamatyczna tak jak to robi Vicon
				if (!powers.empty()) {
					kineticItem->appendChild(createPowersBranch(motion, QObject::tr("Powers"), getRootPowersIcon(), getPowersIcon(), std::string()));
				}
			}

			bool hasMarkers = motion->hasObject(typeid(c3dlib::MarkerCollection), false);
			bool hasJoints = false;//motion->hasObject(typeid(SkeletonWithStates), false);
			bool hasAngles = motion->hasObject(typeid(c3dlib::AngleCollection), false);
			if (hasMarkers || hasJoints || hasAngles) {
				core::IHierarchyItemPtr kinematicItem;
				QString kinematicName = QObject::tr("Kinematic data");

				//TODO
				//czemu tu jest na sztywno false? Automatycznie dyskryminuje ta galez
				/*if (false && hasJoints || hasMarkers || hasGrf) {
					Multiserie3DPtr multi(new Multiserie3D(motion));
					kinematicItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(QIcon(), kinematicName, desc, multi));
				}
				else {*/
					kinematicItem = core::IHierarchyItemPtr(new core::HierarchyItem(kinematicName, desc));
				//}
				motionItem->appendChild(kinematicItem);
				if (hasMarkers) {
					kinematicItem->appendChild(createMarkersBranch(motion, QObject::tr("Markers"), getRootMarkersIcon(), getMarkersIcon(), std::string()));

				}
				/*if (hasAngles || hasJoints) {
					kinematicItem->appendChild(createJointsBranch(motion, QObject::tr("Joints"), getRootJointsIcon(), getJointsIcon(), std::string()));
				}*/
			}

			if (motion->hasObject(typeid(VideoChannel), false)) {
				motionItem->appendChild(createVideoBranch(motion, QObject::tr("Videos"), QIcon(), getVideoIcon(), std::string()));
			}

			auto username = equest::getUserName(subject.get());
			motionItem->appendChild(createAnnotationsBranch(motion, QObject::tr("Annotations"), QIcon(), getVideoIcon(), username, label.toStdString()));
			 
		}
	}

	return rootItem;
}

core::IHierarchyItemPtr equest::EquestPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
	if (hasValidData(subject)) {
		return createTree("SUB", subject);
	}

	return core::IHierarchyItemPtr();
	/*auto object = core::Variant::create<EquestAnnotations>();
	equest::EquestAnnotationsPtr ptr = utils::make_shared<equest::EquestAnnotations>();
	object->set(ptr);

	core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>("Annotation Tool", "Annotation Tool", QIcon());
	core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(object, QString());
	root->appendChild(item);

	return root;
	*/
	
    /*std::string name = getUserName(subject.get());

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
        if (s->hasObject(typeid(equest::EquestInternalStruct),false)) {
            core::ConstVariantsList inter;
			s->getObjects(inter, typeid(equest::EquestInternalStruct), false);
            sessionWrp = *inter.begin();
            std::string sessionName;
            if (sessionWrp->getMetadata("core/source", sessionName)) {
                fs::Path sp(sessionName);
                label = QString(sp.stem().string().c_str());
            }
            equest::EquestInternalStructConstPtr internalStruct = sessionWrp->get();
            auto session = internalStruct->tryGetSession(label.toStdString());
            if (session) {
                desc = equest::DescriptionMaker::createDesc(*session);
            }
            //EquestInternalStructConstPtr test = (*inter.begin())->get();
		} else {
			PLUGIN_LOG_ERROR("No session file found!");
			// nie znaleziono pliku sesji, takie dane nie moga miec informacji o power dopplerze
			return core::IHierarchyItemPtr(); // < ------------	return !
		}
        core::HierarchyDataItemPtr sessionItem(new core::HierarchyDataItem(QIcon(), label, desc));
	    
        rootItem->appendChild(sessionItem);
		std::vector<EquestHelperPtr> helpers;
        s->getMotions(motions);

        for(auto motionOW : motions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
            std::string trialName;
            motionOW->getMetadata("core/source", trialName);
            
			if (motion->hasObject(typeid(equest::ILayeredImage), false)) {
                core::ConstVariantsList images;
                core::ConstVariantsList layers;
                motion->getObjects(images, typeid(equest::ILayeredImage), false);	
                motion->getObjects(layers, typeid(equest::LayersVector), false);
                
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
                            equest::EquestInternalStructConstPtr internalStruct = sessionWrp->get();
                            auto internalImage = internalStruct->tryGetImage(imageFilename + ".png");
                            if (internalImage) {
								desc = equest::DescriptionMaker::createDesc(*internalImage);
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
                            icon = QIcon(":/equest/file_done.png");
                        } else if (!layers.empty()) {
                            icon = QIcon(":/equest/file_blue.png");
                        } else {
                            icon = QIcon(":/equest/file.png");
                        }
                        
                        EquestHelperPtr helper = EquestHelperPtr(new EquestHelper(wrapper, layers, xmlFilenamePattern, trialName));
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

		EquestMultiHelperPtr helper = utils::make_shared<EquestMultiHelper>(sessionItem);
		sessionItem->addHelper(helper);
    }

    if (hasData) {
        return rootItem;
    }

    return core::IHierarchyItemPtr();
	*/
	throw std::runtime_error("NYI");
}

core::HierarchyDataItemPtr equest::EquestPerspective::tryGetHierarchyItem( const std::string& filename ) const
{
    auto it = name2hierarchy.find(filename);
    if (it != name2hierarchy.end()) {
        return (it->second).lock();
    }

    return core::HierarchyDataItemPtr();
}


void equest::EquestHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series )
{
	throw std::runtime_error("NYI");
    /*UTILS_ASSERT(wrapper, "Item should be initialized");
    core::VariantPtr wrp = utils::const_pointer_cast<core::Variant>(wrapper);
    std::string name = getUserName(wrapper);
	fs::Path realXmlFilename(boost::str(boost::format(xmlFilename) % name));
    wrp->setMetadata("EQUEST_XML", realXmlFilename.string());
	wrp->setMetadata("EQUEST_XML_PATTERN", xmlFilename);
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
			case equest::annotations::bloodLevel:
				bFound = true;
				break;


			case equest::annotations::inflammatoryLevel:
				iFound = true;
				break;

			case equest::annotations::jointType:
				jFound = true;
				break;

			case equest::annotations::fingerType:
				fFound = true;
				break;

			case equest::annotations::imageType:
				imgFound = true;
				break;
			}
		}		

		if(iFound == false){
			auto l = equest::ILayerItemPtr(new InflammatoryLevelLayer(equest::annotations::inflammatoryLevel, equest::annotations::unknownInflammatoryLevel));
			l->setName(QObject::tr("Inflammatory level"));			
			img->addLayer(l, xmlUser);
		}

		if(bFound == false && img->isPowerDoppler() == true){
			auto l = equest::ILayerItemPtr(new BloodLevelLayer(equest::annotations::bloodLevel, equest::annotations::unknownBloodLevel));
			l->setName(QObject::tr("Blood level"));			
			img->addLayer(l, xmlUser);
		}

		if(fFound == false){
			auto l = equest::ILayerItemPtr(new FingerTypeLayer(equest::annotations::fingerType, equest::annotations::unknownFinger));
			l->setName(QObject::tr("Finger type"));			
			img->addLayer(l, xmlUser);
		}

		if(jFound == false){
			auto l = equest::ILayerItemPtr(new JointTypeLayer(equest::annotations::jointType, equest::annotations::unknownJoint));
			l->setName(QObject::tr("Joint type"));			
			img->addLayer(l, xmlUser);
		}

		if(imgFound == false){
			auto l = equest::ILayerItemPtr(new ImageQualityLayer(equest::annotations::imageType, equest::annotations::different));
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
				case equest::annotations::bloodLevel:
					bFound = true;
					break;


				case equest::annotations::inflammatoryLevel:
					iFound = true;
					break;

				case equest::annotations::jointType:
					jFound = true;
					break;

				case equest::annotations::fingerType:
					fFound = true;
					break;

				case equest::annotations::imageType:
					imgFound = true;
					break;
				}
            }
        }

		if(iFound == false){
			auto l = equest::ILayerItemPtr(new InflammatoryLevelLayer(equest::annotations::inflammatoryLevel, equest::annotations::unknownInflammatoryLevel));
			l->setName(QObject::tr("Inflammatory level"));			
			img->addLayer(l, name);
		}

		if(bFound == false && img->isPowerDoppler() == true){
			auto l = equest::ILayerItemPtr(new BloodLevelLayer(equest::annotations::bloodLevel, equest::annotations::unknownBloodLevel));
			l->setName(QObject::tr("Blood level"));			
			img->addLayer(l, name);
		}

		if(fFound == false){
			auto l = equest::ILayerItemPtr(new FingerTypeLayer(equest::annotations::fingerType, equest::annotations::unknownFinger));
			l->setName(QObject::tr("Finger type"));			
			img->addLayer(l, name);
		}

		if(jFound == false){
			auto l = equest::ILayerItemPtr(new JointTypeLayer(equest::annotations::jointType, equest::annotations::unknownJoint));
			l->setName(QObject::tr("Joint type"));			
			img->addLayer(l, name);
		}

		if(imgFound == false){
			auto l = equest::ILayerItemPtr(new ImageQualityLayer(equest::annotations::imageType, equest::annotations::different));
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
	*/
}

equest::EquestHelper::EquestHelper( const core::VariantConstPtr & wrapper, const core::ConstVariantsList& layers,
    const std::string& xmlFilename, const std::string& trialName) :
    WrappedItemHelper(wrapper),
    layers(layers),
    xmlFilename(xmlFilename),
    trialName(trialName)
{

}

void equest::EquestMultiHelper::createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series)
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

core::VisualizerPtr equest::EquestMultiHelper::createVisualizer(core::IVisualizerManager* manager)
{
	auto item = sessionItem.lock();
	if (item) {
		auto helpers = getHelpers(item.get());
		return helpers[0]->createVisualizer(manager);
	}

	throw std::runtime_error("Unable to create visualizer");
}

std::vector<utils::TypeInfo> equest::EquestMultiHelper::getTypeInfos() const
{
	auto item = sessionItem.lock();
	if (item) {
		auto helpers = getHelpers(item.get());
		return helpers[0]->getTypeInfos();
	}

	throw std::runtime_error("Unable to get type infos");
}

//equest::EquestMultiHelper::EquestMultiHelper(const std::vector<EquestHelperPtr>& helpers)
//{
//	this->helpers = helpers;
//	UTILS_ASSERT(helpers.empty() == false);
//}

equest::EquestMultiHelper::EquestMultiHelper(core::IHierarchyItemWeakPtr item) : 
	sessionItem(item)
{

}

std::vector<equest::EquestHelperPtr> equest::EquestMultiHelper::getHelpers(core::IHierarchyItem* item) const
{
	std::vector<EquestHelperPtr> ret;
	auto count = item->getNumChildren();
	for (auto i = 0; i < count; i++) {
		auto data = dynamic_cast<const core::IHierarchyDataItem*>(item->getChild(i).get());
		auto helpers = data->getHelpers();
		for (auto& h : helpers) {
			auto dh = utils::dynamic_pointer_cast<EquestHelper>(h);
			if (dh) {
				ret.push_back(dh);
			}
		}

	}
	return ret;
}


core::IHierarchyItemPtr equest::EquestPerspective::createEMGBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & conf)
{
	QString desc = createDescription(motion);
	core::IHierarchyItemPtr emgItem = utils::make_shared<core::HierarchyItem>(rootName, desc, rootIcon);
	core::ConstVariantsList emgs;
	motion->getObjects(emgs, typeid(c3dlib::EMGCollection), false);

	c3dlib::EMGCollectionConstPtr collection = emgs.front()->get();

	//auto measurements = Measurements::get();
	//MeasurementConfigConstPtr config;
	////próbuje pobraæ metadane
	//try{
	//    if(measurements != nullptr && conf.empty() == false) {
	//        QString confName = QString("EMG_") + QString::fromStdString(conf);
	//        if (measurements->hasConfig(confName)) {
	//            config = measurements->getConfig(confName);
	//        }
	//    }
	//} catch(...) {
	//    PLUGIN_LOG_WARNING("Problem with setting EMG names");
	//}

	core::ConstVariantsList().swap(emgs);

	motion->getObjects(emgs, typeid(c3dlib::EMGChannel), false);
	for (auto it = emgs.begin(); it != emgs.end(); ++it) {
		c3dlib::EMGChannelConstPtr c = (*it)->get();
		if (c) {
			std::string l("UNKNOWN");
			(*it)->getMetadata("core/name", l);

			/*core::IHierarchyItemPtr channelItem = utils::make_shared<core::HierarchyDataItem>(*it, itemIcon, QString::fromStdString(l), desc);
			emgItem->appendChild(channelItem);	*/

			EMGFilterHelperPtr channelHelper(new EMGFilterHelper(*it, getEvents(motion)));

			core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l), desc, channelHelper));
			emgItem->appendChild(channelItem);
		}
	}

	return emgItem;
}

core::IHierarchyItemPtr equest::EquestPerspective::createGRFBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &)
{
	QString desc = createDescription(motion);
	core::ConstVariantsList grfCollections;
	motion->getObjects(grfCollections, typeid(c3dlib::GRFCollection), false);
	//TreeWrappedItemHelperPtr grfCollectionHelper(new TreeWrappedItemHelper(grfCollections.front()));
	core::IHierarchyItemPtr grfItem(new core::HierarchyDataItem(grfCollections.front(), rootIcon, rootName, desc));

	core::ConstVariantsList grfs;
	motion->getObjects(grfs, typeid(c3dlib::GRFChannel), false);
	for (auto it = grfs.begin(); it != grfs.end(); ++it) {
		c3dlib::GRFChannelConstPtr c = (*it)->get();
		if (c) {
			std::string l("UNKNOWN");
			(*it)->getMetadata("core/name", l);
			core::HierarchyHelperPtr channelHelper(new NewVector3ItemHelper(*it));
			// todo w jakim celu ustawiany byl motion
			//channelHelper->setMotion(motion);
			core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l), desc, channelHelper));
			grfItem->appendChild(channelItem);
		}
	}
	return grfItem;
}


core::IHierarchyItemPtr equest::EquestPerspective::createVideoBranch(const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &)
{
	QString desc = createDescription(motion);
	core::IHierarchyItemPtr videoItem(new core::HierarchyItem(rootName, desc, rootIcon));

	core::ConstVariantsList videos;
	motion->getObjects(videos, typeid(VideoChannel), false);
	for (auto it = videos.begin(); it != videos.end(); ++it) {
		std::string l("UNKNOWN");
		(*it)->getMetadata("core/name", l);
		core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l), desc));
		videoItem->appendChild(channelItem);
	}

	return videoItem;
}

/*core::IHierarchyItemPtr equest::EquestPerspective::createJointsBranch(const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &)
{

	QString desc = createDescription(motion);
	bool hasJoints = motion->hasObject(typeid(SkeletonWithStates), false);
	core::IHierarchyItemPtr skeletonItem;
	if (hasJoints) {
		// todo setmotion
		//skeletonHelper->setMotion(motion);

		core::ConstVariantsList jCollections;
		motion->getObjects(jCollections, typeid(SkeletonWithStates), false);
		if (jCollections.size() != 1) {
			// error
		}

		core::VariantConstPtr joints = jCollections.front();

		JointsItemHelperPtr skeletonHelper(new JointsItemHelper(joints));

		skeletonItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(joints, rootIcon, rootName, desc, skeletonHelper));
	}
	else {
		skeletonItem = core::IHierarchyItemPtr(new core::HierarchyItem(rootName, desc, rootIcon));
	}

	try {
		core::ConstVariantsList aCollections;
		motion->getObjects(aCollections, typeid(c3dlib::AngleCollection), false);
		core::VariantConstPtr angleCollection = aCollections.front();
		c3dlib::AngleCollectionConstPtr m = angleCollection->get();
		tryAddVectorToTree<c3dlib::AngleChannel>(motion, m, "Angle collection", itemIcon, skeletonItem, false);
	}
	catch (...) {

	}

	return skeletonItem;
}*/

core::IHierarchyItemPtr equest::EquestPerspective::createMarkersBranch(const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &)
{
	QString desc = createDescription(motion);
	core::ConstVariantsList mCollections;
	motion->getObjects(mCollections, typeid(c3dlib::MarkerCollection), false);
	core::VariantConstPtr markerCollection = mCollections.front();
	core::WrappedItemHelperPtr markersHelper(new core::WrappedItemHelper(markerCollection));
	// todo setmotion
	//markersHelper->setMotion(motion);
	core::IHierarchyItemPtr markersItem(new core::HierarchyDataItem(rootIcon, rootName, desc, markersHelper));

	c3dlib::MarkerCollectionConstPtr m = markerCollection->get();
	tryAddVectorToTree<c3dlib::MarkerChannel>(motion, m, "Marker collection", itemIcon, markersItem, false);
	/*int count = markersItem->getNumChildren();
	for (int i = 0; i < count; ++i) {
	core::HierarchyDataItemConstPtr di = utils::dynamic_pointer_cast<const core::HierarchyDataItem>(markersItem->getChild(i));
	if (di) {
	core::HierarchyHelperPtr helperX = utils::make_shared<NewVector3ItemHelper>(di->getData());
	helperX->setText(QObject::tr("All x from session"));
	utils::const_pointer_cast<core::HierarchyDataItem>(di)->addHelper(helperX);

	core::HierarchyHelperPtr helperY = utils::make_shared<NewVector3ItemHelper>(di->getData());
	helperY->setText(QObject::tr("All y from session"));
	utils::const_pointer_cast<core::HierarchyDataItem>(di)->addHelper(helperY);

	core::HierarchyHelperPtr helperZ = utils::make_shared<NewVector3ItemHelper>(di->getData());
	helperZ->setText(QObject::tr("All z from session"));
	utils::const_pointer_cast<core::HierarchyDataItem>(di)->addHelper(helperZ);
	}
	}*/
	return markersItem;
}

core::IHierarchyItemPtr equest::EquestPerspective::createForcesBranch(const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &)
{
	return createTBranch<c3dlib::ForceChannel, c3dlib::ForceCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr equest::EquestPerspective::createMomentsBranch(const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &)
{
	return createTBranch<c3dlib::MomentChannel, c3dlib::MomentCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr equest::EquestPerspective::createPowersBranch(const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &)
{
	return createTBranch<c3dlib::PowerChannel, c3dlib::PowerCollection>(motion, rootName, rootIcon, itemIcon);
}

template <class Channel, class Collection>
core::IHierarchyItemPtr equest::EquestPerspective::createTBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon)
{
	typedef typename utils::ObjectWrapperTraits<Collection>::Ptr CollectionPtr;
	typedef typename utils::ObjectWrapperTraits<Collection>::ConstPtr CollectionConstPtr;
	//typedef typename Collection::ChannelType Channel;
	QString desc = createDescription(motion);

	core::ConstVariantsList collection;
	motion->getObjects(collection, typeid(Collection), false);
	core::IHierarchyItemPtr rootItem(new core::HierarchyItem(rootName, desc, rootIcon));
	CollectionConstPtr m = collection.front()->get();
	tryAddVectorToTree<Channel>(motion, m, "Collection", itemIcon, rootItem, false);
	return rootItem;
}

template <class Channel, class CollectionPtr>
void equest::EquestPerspective::tryAddVectorToTree(const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, core::IHierarchyItemPtr parentItem, bool createContainerItem /*= true */)
{
	QString desc = createDescription(motion);
	if (collection) {
		std::vector<core::VariantConstPtr> wrappers;
		for (int i = 0; i < collection->getNumAccessors(); ++i) {
			auto wrapper = core::Variant::create<Channel>();
			wrapper->set(utils::const_pointer_cast<Channel>(utils::dynamic_pointer_cast<const Channel>(collection->getAccessor(i))));

			static int number = 0;
			std::string();
			wrapper->setMetadata("name", "Serie_" + boost::lexical_cast<std::string>(number));
			wrapper->setMetadata("source", "TreeBuilder");
			wrappers.push_back(wrapper);
		}
		core::IHierarchyItemPtr collectionItem;
		if (createContainerItem) {
			collectionItem = core::IHierarchyItemPtr(new core::HierarchyItem(QString::fromStdString(name), desc));
			parentItem->appendChild(collectionItem);
		}
		else {
			collectionItem = parentItem;
		}
		int count = wrappers.size();

		c3dlib::EventsCollectionConstPtr events = getEvents(motion);

		for (int i = 0; i < count; ++i) {
			c3dlib::VectorChannelReaderInterfaceConstPtr c = wrappers[i]->get();
			std::string channelName = c->getOrCreateFeature<dataaccessor::DescriptorFeature>()->name();
			std::list<core::HierarchyHelperPtr> helpers;
			NewVector3ItemHelperPtr channelHelper(new NewVector3ItemHelper(wrappers[i], events));
			push_not_null(helpers, channelHelper);
			push_not_null(helpers, allTFromSession(channelName, motion->getUnpackedSession(), 0));
			push_not_null(helpers, allTFromSession(channelName, motion->getUnpackedSession(), 1));
			push_not_null(helpers, allTFromSession(channelName, motion->getUnpackedSession(), 2));
			push_not_null(helpers, createNormalized(wrappers[i], motion, c3dlib::C3DParser::IEvent::Left));
			push_not_null(helpers, createNormalized(wrappers[i], motion, c3dlib::C3DParser::IEvent::Right));
			push_not_null(helpers, createNormalizedFromAll(channelName, motion->getUnpackedSession(), c3dlib::C3DParser::IEvent::Left));
			push_not_null(helpers, createNormalizedFromAll(channelName, motion->getUnpackedSession(), c3dlib::C3DParser::IEvent::Right));
			core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(wrappers[i], childIcon, QString::fromStdString(c->getOrCreateFeature<dataaccessor::DescriptorFeature>()->name()), desc, helpers));
			collectionItem->appendChild(channelItem);
		}
	}
}

core::HierarchyHelperPtr equest::EquestPerspective::allTFromSession(const std::string& channelName, PluginSubject::SessionConstPtr s, int channelNo)
{
	NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
	core::ConstVariantsList motions;
	s->getMotions(motions);

	for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
		PluginSubject::MotionConstPtr m = (*itMotion)->get();
		core::ConstVariantsList wrappers;
		m->getObjects(wrappers, typeid(c3dlib::VectorChannelCollection), false);

		c3dlib::EventsCollectionConstPtr events;
		if (m->hasObject(typeid(c3dlib::C3DEventsCollection), false)) {
			core::ConstVariantsList e;
			m->getObjects(e, typeid(c3dlib::C3DEventsCollection), false);
			events = e.front()->get();
		}

		for (auto it = wrappers.begin(); it != wrappers.end(); ++it) {
			c3dlib::VectorChannelCollectionConstPtr collection = (*it)->get();
			int count = collection ? collection->getNumAccessors() : 0;
			for (int i = 0; i < count; ++i) {
				c3dlib::VectorChannelReaderInterfaceConstPtr channel = collection->getAccessor(i);
				auto df = channel->feature<dataaccessor::DescriptorFeature>();
				if (df != nullptr && df->name() == channelName) {
					c3dlib::ScalarChannelReaderInterfacePtr reader(dataaccessor::Vector::wrap(channel, channelNo));

					auto uaf = channel->feature<dataaccessor::UniformArgumentsFeature>();
					auto abf = channel->feature<dataaccessor::BoundedArgumentsFeature>();
					auto vbf = channel->feature<dataaccessor::BoundedValuesFeature>();

					if (abf != nullptr) {
						reader->attachFeature(abf);
					}

					if (vbf != nullptr) {
						auto lb = utils::ElementExtractor::extract(vbf->minValue(), channelNo);
						auto ub = utils::ElementExtractor::extract(vbf->maxValue(), channelNo);
						reader->attachFeature(utils::make_shared<dataaccessor::BoundedValuesFeature<decltype(reader->value(0))>>(lb, ub));
					}

					if (uaf != nullptr) {
						reader->attachFeature(uaf);
					}

					auto wrapper = core::Variant::wrap(reader);
					int no = toVisualize.size();
					std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
					wrapper->setMetadata("core/name", prefix + boost::lexical_cast<std::string>(no));
					std::string src;
					(*it)->getMetadata("core/source", src);
					wrapper->setMetadata("core/source", src + boost::lexical_cast<std::string>(no));
					toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(wrapper, events, m));
				}

			}

		}
	}
	NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
	QString text = QString("Multichart/All %1 from session").arg(channelNo == 0 ? "X" : (channelNo == 1 ? "Y" : "Z"));
	multi->setText(text);
	return multi;
}

core::HierarchyHelperPtr equest::EquestPerspective::createNormalized(core::VariantConstPtr wrapper, PluginSubject::MotionConstPtr motion, c3dlib::C3DParser::IEvent::Context context)
{
	NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
	//MotionConstPtr motion = helper->getMotion();
	c3dlib::EventsCollectionConstPtr events;
	std::vector<c3dlib::FloatPairPtr> segments;
	if (motion->hasObject(typeid(c3dlib::C3DEventsCollection), false)) {
		core::ConstVariantsList wrappers;
		motion->getObjects(wrappers, typeid(c3dlib::C3DEventsCollection), false);
		events = wrappers.front()->get();
		segments = getTimeSegments(events, context);
	}
	std::map<core::VariantConstPtr, QColor> colorMap;
	c3dlib::VectorChannelReaderInterfaceConstPtr channel = wrapper->get();

	for (int j = 0; j != segments.size(); ++j) {
		c3dlib::FloatPairPtr segment = segments[j];
		for (int channelNo = 0; channelNo <= 2; ++channelNo) {
			auto reader = dataaccessor::Vector::wrap(channel, channelNo);

			auto uaf = channel->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
			auto abf = channel->feature<dataaccessor::BoundedArgumentsFeature>();
			auto vbf = channel->feature<dataaccessor::BoundedValuesFeature>();

			if (abf != nullptr) {
				reader->attachFeature(abf);
			}

			if (vbf != nullptr) {
				auto lb = utils::ElementExtractor::extract(vbf->minValue(), channelNo);
				auto ub = utils::ElementExtractor::extract(vbf->maxValue(), channelNo);
				reader->attachFeature(utils::make_shared<dataaccessor::BoundedValuesFeature<decltype(reader->value(0))>>(lb, ub));
			}

			std::size_t start_idx = 0;
			std::size_t end_idx = 0;

			if (uaf == nullptr) {
				start_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->first).first;
				end_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->second).second;
			}
			else {
				reader->attachFeature(uaf);
				start_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->first, uaf->argumentsInterval()).first;
				end_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->second, uaf->argumentsInterval()).second;

			}

			auto normalized = dataaccessor::wrap(reader, end_idx - start_idx, start_idx);

			auto newWrapper = core::Variant::wrap<c3dlib::ScalarChannelReaderInterface>(normalized);
			int no = toVisualize.size();
			std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
			colorMap[newWrapper] = channelNo == 0 ? QColor(255, 0, 0) : (channelNo == 1 ? QColor(0, 255, 0) : QColor(0, 0, 255));
			newWrapper->setMetadata("core/name", prefix + ":" + boost::lexical_cast<std::string>(j));
			std::string src;
			wrapper->getMetadata("core/sources", src);
			newWrapper->setMetadata("core/sources", src + boost::lexical_cast<std::string>(no));
			toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(newWrapper, events, motion));
		}
	}
	NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
	multi->setColorStrategy(utils::make_shared<ColorMapMultiserieStrategy>(colorMap));

	QString text;
	if (context == c3dlib::C3DParser::IEvent::Left) {
		text = QObject::tr("Multichart/Normalized left from motion");
	}
	else if (context == c3dlib::C3DParser::IEvent::Right) {
		text = QObject::tr("Multichart/Normalized right from motion");
	}
	else {
		UTILS_ASSERT(false);
	}
	multi->setText(text);
	return multi;
}

core::HierarchyHelperPtr  equest::EquestPerspective::createNormalizedFromAll(const std::string& channelName, SessionConstPtr s, c3dlib::C3DParser::IEvent::Context context)
{
	NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
	//SessionConstPtr s = helper->getMotion()->getUnpackedSession();
	core::ConstVariantsList motions;
	s->getMotions(motions);

	std::map<core::VariantConstPtr, QColor> colorMap;
	for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
		PluginSubject::MotionConstPtr m = (*itMotion)->get();
		core::ConstVariantsList wrappers;
		m->getObjects(wrappers, typeid(dataaccessor::AccessorsCollection<c3dlib::VectorChannelReaderInterface>), false);

		c3dlib::EventsCollectionConstPtr events;
		std::vector<c3dlib::FloatPairPtr> segments;
		if (m->hasObject(typeid(c3dlib::C3DEventsCollection), false)) {
			core::ConstVariantsList e;
			m->getObjects(e, typeid(c3dlib::C3DEventsCollection), false);
			events = e.front()->get();
			segments = getTimeSegments(events, context);
		}

		for (auto it = wrappers.begin(); it != wrappers.end(); ++it) {
			c3dlib::VectorChannelCollectionConstPtr collection = (*it)->get();
			int count = collection ? collection->getNumAccessors() : 0;
			for (int i = 0; i < count; ++i) {
				auto channel = collection->getAccessor(i);
				if (channel->feature<dataaccessor::DescriptorFeature>()->name() == channelName) {

					int r = rand() % 200;
					int g = rand() % 200;
					int b = rand() % 200;
					QColor colorX(r + 55, g, b);
					QColor colorY(r, g + 55, b);
					QColor colorZ(r, g, b + 55);
					for (int j = 0; j != segments.size(); ++j) {
						c3dlib::FloatPairPtr segment = segments[j];

						for (int channelNo = 0; channelNo <= 2; ++channelNo) {
							auto reader = dataaccessor::Vector::wrap(channel, channelNo);

							auto uaf = channel->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
							auto abf = channel->feature<dataaccessor::BoundedArgumentsFeature>();
							auto vbf = channel->feature<dataaccessor::BoundedValuesFeature>();

							if (abf != nullptr) {
								reader->attachFeature(abf);
							}

							if (vbf != nullptr) {
								auto lb = utils::ElementExtractor::extract(vbf->minValue(), channelNo);
								auto ub = utils::ElementExtractor::extract(vbf->maxValue(), channelNo);
								reader->attachFeature(utils::make_shared<dataaccessor::BoundedValuesFeature<decltype(reader->value(0))>>(lb, ub));
							}

							std::size_t start_idx = 0;
							std::size_t end_idx = 0;

							if (uaf == nullptr) {
								start_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->first).first;
								end_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->second).second;
							}
							else {
								reader->attachFeature(uaf);
								start_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->first, uaf->argumentsInterval()).first;
								end_idx = dataaccessor::NearestArgumentsFinder::range(*reader, segment->second, uaf->argumentsInterval()).second;

							}

							//TODO
							//na bazie info z reader zasiliæ wrapowany akcesor

							auto wrapper = core::Variant::wrap<c3dlib::ScalarChannelReaderInterface>(dataaccessor::wrap(reader, end_idx - start_idx, start_idx));
							colorMap[wrapper] = channelNo == 0 ? colorX : (channelNo == 1 ? colorY : colorZ);
							int no = toVisualize.size();
							std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
							wrapper->setMetadata("core/name", prefix + boost::lexical_cast<std::string>(i) + ":" + boost::lexical_cast<std::string>(j));
							std::string src;
							(*it)->getMetadata("core/source", src);
							wrapper->setMetadata("core/source", src + boost::lexical_cast<std::string>(no));
							toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(wrapper, events, m));
						}
					}
				}

			}

		}
	}
	if (toVisualize.empty() == false) {
		NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
		multi->setColorStrategy(IMultiserieColorStrategyPtr(new ColorMapMultiserieStrategy(colorMap)));

		QString text;
		if (context == c3dlib::C3DParser::IEvent::Left) {
			text = QObject::tr("Multichart/Normalized left from session");
		}
		else if (context == c3dlib::C3DParser::IEvent::Right) {
			text = QObject::tr("Multichart/Normalized right from session");
		}
		else {
			UTILS_ASSERT(false);
		}
		multi->setText(text);
		return multi;
	}

	//UTILS_ASSERT(false);
	return NewMultiserieHelperPtr();
}

QString equest::EquestPerspective::createDescription(PluginSubject::MotionConstPtr motion)
{
	QString text;
	PluginSubject::SessionConstPtr session = motion->getUnpackedSession();
	PluginSubject::SubjectConstPtr subject = session->getUnpackedSubject();

	text += QObject::tr("Motion: ") + QString::fromStdString(motion->getLocalName()) + "\n";
	text += QObject::tr("Session: ") + QString::fromStdString(session->getLocalName()) + "\n";
	text += QObject::tr("Subject: ") + QString::fromStdString(subject->getName()) + "\n";

	try {

		std::string groupName, groupID;

		if (motion->getSession()->getMetadata("groupName", groupName) == true) {
			text += QObject::tr("Owner: %1").arg(groupName.c_str());
			if (motion->getSession()->getMetadata("groupID", groupID) == true) {
				text += QString("(%1)").arg(groupID.c_str());
			}
			text += "\n";
		}

		std::string date;
		if (motion->getSession()->getMetadata("data", date) == true) {
			text += QObject::tr("Date: %1\n").arg(date.c_str());
		}
	}
	catch (...) {
		PLUGIN_LOG_WARNING("Problem with summary window metadata");
	}

	if (session->hasObject(typeid(hmdbCommunication::IPatient), false)) {
		core::ConstVariantsList patients;
		session->getObjects(patients, typeid(hmdbCommunication::IPatient), false);
		hmdbCommunication::PatientConstPtr patient = patients.front()->get();
		text += QObject::tr("Patient: ") + QString::fromStdString(patient->getName()) + " " + QString::fromStdString(patient->getSurname()) + "\n";
	}

	if (session->hasObject(typeid(hmdbCommunication::AntropometricData), false)) {
		core::ConstVariantsList antropo;
		session->getObjects(antropo, typeid(hmdbCommunication::AntropometricData), false);
		utils::shared_ptr<const hmdbCommunication::AntropometricData> antro = antropo.front()->get();
		text += " ";
		text += QObject::tr("Weight: ") + QString("%1 ").arg(antro->bodyMass.first) + QString::fromStdString(antro->bodyMass.second) + "\n";
	}
	return text;
}

c3dlib::EventsCollectionConstPtr equest::EquestPerspective::getEvents(const PluginSubject::MotionConstPtr & motion)
{
	c3dlib::EventsCollectionConstPtr events;
	if (motion->hasObject(typeid(c3dlib::C3DEventsCollection), false)) {
		core::ConstVariantsList m;
		motion->getObjects(m, typeid(c3dlib::C3DEventsCollection), false);
		events = m.front()->get();
	}
	return events;
}


bool equest::EquestPerspective::hasValidData(PluginSubject::SubjectPtr subject)
{
	std::set<utils::TypeInfo> types;
	types.insert(typeid(c3dlib::C3DEventsCollection));
	types.insert(typeid(c3dlib::EMGChannel));
	types.insert(typeid(c3dlib::GRFCollection));
	types.insert(typeid(c3dlib::ForceCollection));
	types.insert(typeid(c3dlib::MomentCollection));
	types.insert(typeid(c3dlib::PowerCollection));
	types.insert(typeid(c3dlib::MarkerCollection));
	types.insert(typeid(SkeletonWithStates));
	types.insert(typeid(c3dlib::AngleCollection));
	types.insert(typeid(VideoChannel));
	types.insert(typeid(c3dlib::EMGCollection));
	types.insert(typeid(c3dlib::GRFChannel));

	core::ConstVariantsList sessions;
	subject->getSessions(sessions);
	for (auto it = sessions.crbegin(); it != sessions.crend(); ++it) {
		core::ConstVariantsList motions;
		PluginSubject::SessionConstPtr s = (*it)->get();
		s->getMotions(motions);

		for (auto motionOW : motions) {

			PluginSubject::MotionConstPtr motion = motionOW->get();
			for (auto it = types.begin(); it != types.end(); ++it) {
				if (motion->hasObject(*it, false)) {
					return true;
				}
			}
		}
	}

	return false;
}

core::IHierarchyItemPtr equest::EquestPerspective::createAnnotationsBranch(const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &userName, const std::string& label)
{

	QString desc = createDescription(motion);
	core::IHierarchyItemPtr annotationItem(new core::HierarchyItem(rootName, desc, rootIcon));

	bool wasUser = false;
	core::ConstVariantsList annotations;
	motion->getObjects(annotations, typeid(EquestAnnotations), false);
	for (auto it = annotations.begin(); it != annotations.end(); ++it) {
		auto metaName = getUserNameFromMetadata(*it);
		if (metaName == userName) {
			wasUser = true;
		}

		auto helper = utils::make_shared<EquestAnnotationItemHelper>(*it, motion, label);
		core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(itemIcon, QString::fromStdString(metaName), desc, helper));
		annotationItem->appendChild(channelItem);
	}

	if (!wasUser) {
		auto object = core::Variant::create<EquestAnnotations>();
		equest::EquestAnnotationsPtr ptr = utils::make_shared<equest::EquestAnnotations>();
		object->set(ptr);

		auto helper = utils::make_shared<EquestAnnotationItemHelper>(object, motion, label);
		core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(itemIcon, QString::fromStdString(userName), QString(), helper);
		annotationItem->appendChild(item);
	}

	return annotationItem;
}

std::string equest::getUserNameFromMetadata(core::VariantConstPtr variant)
{
	std::string l("UNKNOWN");
	variant->getMetadata("core/name", l);
	utils::Filesystem::Path p(l);
	auto name = p.stem().extension().string();
	if (!name.empty()) {
		if (name[0] == '.') {
			name = name.substr(1);
		}
		l = name;
	}
	return l;
}


equest::EquestAnnotationItemHelper::EquestAnnotationItemHelper(const core::VariantConstPtr& wrapper, PluginSubject::MotionConstPtr motion, const std::string& label) :
	WrappedItemHelper(wrapper),
	motion(motion),
	label(label)
{
}

/*
core::VisualizerPtr equest::EquestAnnotationItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(equest::EquestAnnotations), prototypes, true);
	core::VisualizerPtr visualizer(prototypes.front()->create());

	QWidget * visWidget = visualizer->getOrCreateWidget();
	INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
	if (!chart) {
		UTILS_ASSERT(false);
		throw loglib::runtime_error("Wrong visualizer type!");
	}
	else {
		std::string title;
		c3dlib::ScalarChannelReaderInterfaceConstPtr scalar = wrapper->get();

		auto df = scalar->feature<dataaccessor::DescriptorFeature>();

		if (df != nullptr) {

			title += df->name();
			title += " [";
			title += df->valueUnit();
			title += "]";

		}
		else {

		}

		chart->setTitle(processTitle(title));

	}

	return visualizer;
}
*/

void equest::EquestAnnotationItemHelper::createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series)
{
	auto serie = visualizer->createSerie(wrapper);
	typedef	equest::EquestAnnotationVisualizer::AnnotationSerie AS;
	auto as = dynamic_cast<AS*>(serie->innerSerie());
	as->setMotion(this->motion, this->label);
	series.push_back(serie);
}

//std::vector<utils::TypeInfo> equest::EquestAnnotationItemHelper::getTypeInfos() const
//{
//	std::vector<utils::TypeInfo> ret;
//	ret.push_back(typeid(equest::EquestAnnotations));
//	return ret;
//}


