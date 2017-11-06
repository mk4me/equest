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

using namespace PluginSubject;
core::IHierarchyItemPtr TreeBuilder::createTree(const QString& rootItemName, const core::ConstVariantsList& sessions)
{
    SubjectHierarchyFilterPtr nullFilter;
    return TreeBuilder::createTree(rootItemName, sessions, nullFilter);
}


core::IHierarchyItemPtr TreeBuilder::createTree( const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject, const SubjectHierarchyFilterPtr & dataFilter )
{
    core::ConstVariantsList sessions;
    subject->getSessions(sessions);
    return createTree(rootItemName, sessions, dataFilter);
}


core::IHierarchyItemPtr TreeBuilder::createTree( const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject )
{
    return TreeBuilder::createTree(rootItemName, subject, SubjectHierarchyFilterPtr());
}



core::IHierarchyItemPtr  TreeBuilder::createTree(const QString& rootItemName, const core::ConstVariantsList& sessions, const SubjectHierarchyFilterPtr & dataFilter)
{
    core::IHierarchyItemPtr rootItem(new core::HierarchyItem(rootItemName, QString()));
    core::ConstVariantsList filteredSessions;
    if(dataFilter != nullptr){
        dataFilter->filterSessions(sessions, filteredSessions);
    }else {
        filteredSessions = sessions;
    }

    for (auto it = filteredSessions.crbegin(); it != filteredSessions.crend(); ++it) {
        
        std::string emgConfigName;
        (*it)->getMetadata("EMGConf", emgConfigName);
        
        core::ConstVariantsList filteredMotions;

        if(dataFilter != nullptr){
            dataFilter->filterSessionMotions(*it, filteredMotions);
        }else {
            PluginSubject::SessionConstPtr s = (*it)->get();
            s->getMotions(filteredMotions);
        }

        for(auto motionOW : filteredMotions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
           
            QString label(QString::fromUtf8(motion->getLocalName().c_str()));

            std::string metaLabel;
            if(motionOW->getMetadata("label", metaLabel)){
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
                    analogItem->appendChild(createEMGBranch(motion, QObject::tr("EMG"), getRootEMGIcon(), getEMGIcon(),emgConfigName));
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
                // do rozwiniecia - potrzeba parsować pliki vsk i interpretować strukture kinamatyczna tak jak to robi Vicon
                if (!powers.empty()) {
                    kineticItem->appendChild(createPowersBranch(motion, QObject::tr("Powers"), getRootPowersIcon(), getPowersIcon(), std::string()));
                }
            }

			bool hasMarkers = motion->hasObject(typeid(c3dlib::MarkerCollection), false);
			bool hasJoints = motion->hasObject(typeid(SkeletonWithStates), false);
			bool hasAngles = motion->hasObject(typeid(c3dlib::AngleCollection), false);
            if (hasMarkers || hasJoints || hasAngles) {
                core::IHierarchyItemPtr kinematicItem;
                QString kinematicName = QObject::tr("Kinematic data");
                 
				//TODO
				//czemu tu jest na sztywno false? Automatycznie dyskryminuje ta galez
                if (false && hasJoints || hasMarkers || hasGrf) {
                    Multiserie3DPtr multi(new Multiserie3D(motion));
                    kinematicItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(QIcon(), kinematicName, desc, multi));
                } else {
                    kinematicItem = core::IHierarchyItemPtr(new core::HierarchyItem(kinematicName, desc));
                }
                motionItem->appendChild(kinematicItem);
                if (hasMarkers) {
                    kinematicItem->appendChild(createMarkersBranch(motion, QObject::tr("Markers"), getRootMarkersIcon(), getMarkersIcon(), std::string()));

                }
                if (hasAngles || hasJoints) {
                    kinematicItem->appendChild(createJointsBranch(motion, QObject::tr("Joints"), getRootJointsIcon(), getJointsIcon(), std::string()));
                }
            }

			if (motion->hasObject(typeid(VideoChannel), false)) {
                motionItem->appendChild(createVideoBranch(motion, QObject::tr("Videos"), QIcon(), getVideoIcon(), std::string()));
            }
        }
    }

    return rootItem;
}

core::IHierarchyItemPtr TreeBuilder::createEMGBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & conf )
{
    QString desc = createDescription(motion);
    core::IHierarchyItemPtr emgItem = utils::make_shared<core::HierarchyItem>(rootName, desc, rootIcon);
    core::ConstVariantsList emgs;
	motion->getObjects(emgs, typeid(c3dlib::EMGCollection), false);

	c3dlib::EMGCollectionConstPtr collection = emgs.front()->get();

    //auto measurements = Measurements::get();
    //MeasurementConfigConstPtr config;
    ////próbuje pobrać metadane
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
    for(auto it = emgs.begin(); it != emgs.end(); ++it) {	
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

core::IHierarchyItemPtr TreeBuilder::createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    QString desc = createDescription(motion);
    core::ConstVariantsList grfCollections;
	motion->getObjects(grfCollections, typeid(c3dlib::GRFCollection), false);
    //TreeWrappedItemHelperPtr grfCollectionHelper(new TreeWrappedItemHelper(grfCollections.front()));
    core::IHierarchyItemPtr grfItem(new core::HierarchyDataItem(grfCollections.front(), rootIcon, rootName, desc));
    
    core::ConstVariantsList grfs;
	motion->getObjects(grfs, typeid(c3dlib::GRFChannel), false);
    for(auto it = grfs.begin(); it != grfs.end(); ++it) {
		c3dlib::GRFChannelConstPtr c = (*it)->get();
        if (c) {
            std::string l("UNKNOWN");
            (*it)->getMetadata("core/name", l);
            core::HierarchyHelperPtr channelHelper(new NewVector3ItemHelper(*it));
            // todo w jakim celu ustawiany byl motion
            //channelHelper->setMotion(motion);
            core::IHierarchyItemPtr channelItem( new core::HierarchyDataItem(*it, itemIcon,  QString::fromStdString(l), desc, channelHelper));	
            grfItem->appendChild(channelItem);			
        }
    }
    return grfItem;
}


core::IHierarchyItemPtr TreeBuilder::createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
    QString desc = createDescription(motion);
    core::IHierarchyItemPtr videoItem(new core::HierarchyItem(rootName, desc, rootIcon));
   
    core::ConstVariantsList videos;
	motion->getObjects(videos, typeid(VideoChannel), false);
    for(auto it = videos.begin(); it != videos.end(); ++it) {			
        std::string l("UNKNOWN");
        (*it)->getMetadata("core/name", l);		
        core::IHierarchyItemPtr channelItem(new core::HierarchyDataItem(*it, itemIcon, QString::fromStdString(l), desc));	
        videoItem->appendChild(channelItem);
    }

    return videoItem;
}

core::IHierarchyItemPtr TreeBuilder::createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
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
    } else {
        skeletonItem = core::IHierarchyItemPtr(new core::HierarchyItem(rootName, desc, rootIcon));
    }

    try {
        core::ConstVariantsList aCollections;
		motion->getObjects(aCollections, typeid(c3dlib::AngleCollection), false);
        core::VariantConstPtr angleCollection = aCollections.front();
		c3dlib::AngleCollectionConstPtr m = angleCollection->get();
		tryAddVectorToTree<c3dlib::AngleChannel>(motion, m, "Angle collection", itemIcon, skeletonItem, false);
    } catch (...) {

    }

    return skeletonItem;
}

core::IHierarchyItemPtr TreeBuilder::createMarkersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
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

core::IHierarchyItemPtr TreeBuilder::createForcesBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
	return createTBranch<c3dlib::ForceChannel, c3dlib::ForceCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr TreeBuilder::createMomentsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
	return createTBranch<c3dlib::MomentChannel, c3dlib::MomentCollection>(motion, rootName, rootIcon, itemIcon);
}

core::IHierarchyItemPtr TreeBuilder::createPowersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & )
{
	return createTBranch<c3dlib::PowerChannel, c3dlib::PowerCollection>(motion, rootName, rootIcon, itemIcon);
}

template <class Channel, class Collection>
core::IHierarchyItemPtr TreeBuilder::createTBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
{
    typedef typename utils::ObjectWrapperTraits<Collection>::Ptr CollectionPtr;
	typedef typename utils::ObjectWrapperTraits<Collection>::ConstPtr CollectionConstPtr;
    //typedef typename Collection::ChannelType Channel;
    QString desc = createDescription(motion);

    core::ConstVariantsList collection;
	motion->getObjects(collection, typeid(Collection), false);
    core::IHierarchyItemPtr rootItem (new core::HierarchyItem(rootName, desc, rootIcon));
    CollectionConstPtr m = collection.front()->get();
    tryAddVectorToTree<Channel>(motion, m, "Collection", itemIcon, rootItem, false);
    return rootItem;
}

template <class Channel, class CollectionPtr>
void TreeBuilder::tryAddVectorToTree( const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, core::IHierarchyItemPtr parentItem, bool createContainerItem /*= true */ )
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
        } else {
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
            core::IHierarchyItemPtr channelItem (new core::HierarchyDataItem(wrappers[i], childIcon, QString::fromStdString(c->getOrCreateFeature<dataaccessor::DescriptorFeature>()->name()), desc, helpers));
            collectionItem->appendChild(channelItem);
        }
    }
}

core::HierarchyHelperPtr TreeBuilder::allTFromSession( const std::string& channelName, PluginSubject::SessionConstPtr s, int channelNo )
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

core::HierarchyHelperPtr TreeBuilder::createNormalized( core::VariantConstPtr wrapper, PluginSubject::MotionConstPtr motion, c3dlib::C3DParser::IEvent::Context context )
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
    } else if (context == c3dlib::C3DParser::IEvent::Right) {
        text = QObject::tr("Multichart/Normalized right from motion");
    } else {
        UTILS_ASSERT(false);
    }
    multi->setText(text);
    return multi;
}

core::HierarchyHelperPtr  TreeBuilder::createNormalizedFromAll( const std::string& channelName, SessionConstPtr s, c3dlib::C3DParser::IEvent::Context context )
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
                    QColor colorX(r + 55,g , b);
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
							//na bazie info z reader zasilić wrapowany akcesor

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
        } else if (context == c3dlib::C3DParser::IEvent::Right) {
            text = QObject::tr("Multichart/Normalized right from session");
        } else {
            UTILS_ASSERT(false);
        }
        multi->setText(text);
        return multi;
    } 

    //UTILS_ASSERT(false);
    return NewMultiserieHelperPtr();
}

QString TreeBuilder::createDescription( PluginSubject::MotionConstPtr motion )
{
    QString text;
    PluginSubject::SessionConstPtr session = motion->getUnpackedSession();
    PluginSubject::SubjectConstPtr subject = session->getUnpackedSubject();
    
    text += QObject::tr("Motion: ") + QString::fromStdString(motion->getLocalName()) + "\n";
    text += QObject::tr("Session: ") + QString::fromStdString(session->getLocalName()) + "\n";
    text += QObject::tr("Subject: ") + QString::fromStdString(subject->getName()) + "\n";
       
    try {

		std::string groupName, groupID;

		if(motion->getSession()->getMetadata("groupName", groupName) == true) {
			text += QObject::tr("Owner: %1").arg(groupName.c_str()); 
			if (motion->getSession()->getMetadata("groupID", groupID) == true) {
				text += QString("(%1)").arg(groupID.c_str());
			}
			text += "\n";
		}

		std::string date;
		if(motion->getSession()->getMetadata("data", date) == true) {
			text += QObject::tr("Date: %1\n").arg(date.c_str());
		}
    } catch (...) {
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

c3dlib::EventsCollectionConstPtr TreeBuilder::getEvents(const PluginSubject::MotionConstPtr & motion)
{
	c3dlib::EventsCollectionConstPtr events;
	if (motion->hasObject(typeid(c3dlib::C3DEventsCollection), false)) {
        core::ConstVariantsList m;
		motion->getObjects(m, typeid(c3dlib::C3DEventsCollection), false);
        events = m.front()->get();
    }
    return events;
}

core::IHierarchyItemPtr MotionPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
    if (hasValidData(subject)) {
        return TreeBuilder::createTree("SUB", subject);
    }

    return core::IHierarchyItemPtr();
}

bool MotionPerspective::hasValidData(PluginSubject::SubjectPtr subject)
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

        for(auto motionOW : motions) {	

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
