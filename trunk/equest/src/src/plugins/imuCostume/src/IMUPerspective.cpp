#include "PCH.h"
#include "IMUPerspective.h"
#include <corelib/HierarchyItem.h>

#include <corelib/IDataManagerReader.h>
#include <corelib/DataAccessors.h>
#include <plugins/subject/IMotion.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/kinematic/JointsItemHelper.h>
#include <plugins/subject/ISubject.h>
#include <plugins/hmdbCommunication/IPatient.h>
#include <corelib/PluginCommon.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <corelib/ISourceManager.h>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <boost/format.hpp>
#include <QtCore/QObject>
#include "IMUCFGParser.h"
#include "IMUDatParser.h"
//#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/Skeleton.h>
#include <kinematicUtils/RotationConverter.h>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <plugins/c3d/C3DChannels.h>
#include <dataaccessorlib/Wrappers.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>
#include <dataaccessorlib/DescriptorFeature.h>

typedef utils::Filesystem fs;

core::IHierarchyItemPtr IMU::IMUPerspective::getPerspective( PluginSubject::SubjectPtr subject )
{
    bool hasData = false;
    auto rootItem = utils::make_shared<core::HierarchyItem>(QString(), QString(), QIcon());
    core::ConstVariantsList sessions;
    subject->getSessions(sessions);
	
    for (auto& sessionWrp :sessions) {
		
        core::ConstVariantsList motions;
		PluginSubject::SessionConstPtr s = sessionWrp->get();
		core::ConstVariantsList configs;
		IMU::IMUConfigConstPtr config = utils::make_shared<IMUConfig>();
		if (s->hasObject(typeid(IMU::IMUConfig), false)) {
			s->getObjects(configs, typeid(IMU::IMUConfig), false);
			UTILS_ASSERT(configs.size() == 1);
			(*configs.begin())->tryGet(config);
		}
		
        QString label(QString::fromUtf8(s->getLocalName().c_str()));
        QString desc("");
        
        core::HierarchyItemPtr sessionItem(new core::HierarchyItem(label, desc));
        rootItem->appendChild(sessionItem);
        s->getMotions(motions);

        for(auto motionOW : motions) {	

            PluginSubject::MotionConstPtr motion = motionOW->get();
            std::ostringstream ss;
			ss << "Trial" << std::setw(5) << std::setfill('0') << motion->getID();
            
			core::HierarchyItemPtr motionItem(new core::HierarchyItem(QString::fromStdString(ss.str()), QString()));
			sessionItem->appendChild(motionItem);
	if (motion->hasObject(typeid(c3dlib::VectorChannelCollection), false)) {
                core::ConstVariantsList vectors;
				hasData = true;
				motion->getObjects(vectors, typeid(c3dlib::VectorChannelCollection), false);
				createIMUBranch(vectors, motionItem);
            }

			if (motion->hasObject(typeid(IMU::Frames), false)) {
				core::ConstVariantsList framesV;
				hasData = true;
				motion->getObjects(framesV, typeid(IMU::Frames), false);

				createIMUBranch(framesV, config, s->getName(), motionItem, memoryDataManager);

			}

			if (motion->hasObject(typeid(SkeletonStates), false) &&
				motion->hasObject(typeid(kinematic::Skeleton), false)) {
				
				core::ConstVariantsList sdl, sml;
				motion->getObjects(sdl, typeid(SkeletonStates), false);
				motion->getObjects(sml, typeid(kinematic::Skeleton), false);

				SkeletonStatesConstPtr sd = sdl.front()->get();
				kinematic::SkeletonConstPtr sm = sml.front()->get();

				//kinematic::JointAnglesCollectionPtr ja = utils::make_shared<kinematic::JointAnglesCollection>();
				//TODO - uzypełnić szkielet + dane
				//ja->setSkeletal(sm, *sd);
				//auto jaWrapper = utils::ObjectWrapper::create<kinematic::JointAnglesCollection>();
				//ja->setLengthRatio(0.1);
				//jaWrapper->set(ja);

				//core::VariantConstPtr joints = core::Variant::create(jaWrapper);
				
				//auto skeletonItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(joints, QIcon(), QObject::tr("Kinematic"), QString()));
				//generateAnglesChannelBranch(ja, skeletonItem);
				//motionItem->appendChild(skeletonItem);
			}

			if (motion->hasObject(typeid(SkeletonWithStates), false)) {			
			
				core::ConstVariantsList jCollections;
				motion->getObjects(jCollections, typeid(SkeletonWithStates), false);
				if (jCollections.size() != 1) {
					// error
				}

				core::VariantConstPtr joints = jCollections.front();

				JointsItemHelperPtr skeletonHelper(new JointsItemHelper(joints));

				auto skeletonItem = core::IHierarchyItemPtr(new core::HierarchyDataItem(joints, QIcon(), QString("3D"), desc, skeletonHelper));
				motionItem->appendChild(skeletonItem);
			}
        }
    }

    if (hasData) {
        return rootItem;
    }

    return core::IHierarchyItemPtr();
}

void IMU::IMUPerspective::createIMUBranch(core::ConstVariantsList &oList, core::HierarchyItemPtr root)
{
	int i = 0;
	for (auto it = oList.begin(); it != oList.end(); ++i, ++it) {
		c3dlib::VectorChannelCollectionConstPtr collection = (*it)->get();
		if (collection) {
			auto collectionItem = createImuCollectionItem(i, collection);
			root->appendChild(collectionItem);
		}
	}
}

void IMU::IMUPerspective::createIMUBranch(core::ConstVariantsList &framesV, IMU::IMUConfigConstPtr config, const std::string& sourceName, core::HierarchyItemPtr sessionItem, core::IDataManager * memoryDataManager)
{
	//auto accWrapper = utils::ObjectWrapper::create<c3dlib::VectorChannelCollection>();
	//auto magWrapper = utils::ObjectWrapper::create<c3dlib::VectorChannelCollection>();
	//auto gyrWrapper = utils::ObjectWrapper::create<c3dlib::VectorChannelCollection>();

	auto accelerations = utils::make_shared<c3dlib::VectorChannelCollection>();
	auto magnetometers = utils::make_shared<c3dlib::VectorChannelCollection>();
	auto gyroscopes = utils::make_shared<c3dlib::VectorChannelCollection>();

	// todo przeniesc te hz do configa?
	int hz = 50;

	IMU::FramesConstPtr frms = (*framesV.begin())->get();
	if (config->imusCount == -1 && !frms->empty()) {
		auto c = utils::const_pointer_cast<IMU::IMUConfig>(config);
		c->imusCount = frms->at(0).size();
	}
	/*for (int i = 0; i < config->imusCount; ++i) {
		accelerations->addAccessor(createChannel(hz, *config, i, "m/s^2"));
		magnetometers->addAccessor(createChannel(hz, *config, i));
		gyroscopes->addAccessor(createChannel(hz, *config, i, "rad/s"));
	}*/

	dataaccessor::UniformArgumentsGenerator<float> uag(1.0 / hz, 1);

	for (auto f : *frms) {		
		
		int count = config->imusCount;
		// na wypadek, gdyby jakies z ostatnich czujnikow byly uszkodzone
		if (count > f.size()) {
			count = f.size();
		}
		for (int i = 0; i < count; ++i) {

			std::vector<osg::Vec3> data;
			data.push_back(osg::Vec3(f[i].raw.acc_x, f[i].raw.acc_y, f[i].raw.acc_z) * (1 / 1024.0f));
			auto a = dataaccessor::wrap(std::move(data), uag);
			a->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<osg::Vec3, float>(generateChannelName(*config, i + 1), "m/s^2", "s")));
			uag.init(a.get());
			accelerations->addAccessor(a);

			data.push_back(osg::Vec3(f[i].raw.mag_x, f[i].raw.mag_y, f[i].raw.mag_z) * (1 / 2048.0f));
			a = dataaccessor::wrap(std::move(data), uag);
			a->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<osg::Vec3, float>(generateChannelName(*config, i + 1), "", "s")));
			uag.init(a.get());
			magnetometers->addAccessor(a);

			data.push_back(osg::Vec3(f[i].raw.rate_x, f[i].raw.rate_y, f[i].raw.rate_z) * (1 / 2048.0f));
			a = dataaccessor::wrap(std::move(data), uag);
			a->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<osg::Vec3, float>(generateChannelName(*config, i + 1), "rad/s", "s")));
			uag.init(a.get());
			gyroscopes->addAccessor(a);
		}
	}
	auto accWrapper = core::Variant::wrap(accelerations);
	auto magWrapper = core::Variant::wrap(magnetometers);
	auto gyrWrapper = core::Variant::wrap(gyroscopes);

	{
		core::VariantsList objects;

		auto _lambda = [&](c3dlib::VectorChannelCollectionPtr collection, const std::string& groupName)
		{
			int count = collection->getNumAccessors();
			for (int i = 0; i < count; ++i) {
				auto channel = collection->getAccessor(i);

				auto wrapper = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(channel);
				wrapper->setMetadata("core/name", channel->feature<dataaccessor::DescriptorFeature>()->name());
				wrapper->setMetadata("core/source", sourceName + std::string("/") + groupName);
				objects.push_back(wrapper);
			}
		};

		_lambda(accelerations, "accelerations");
		_lambda(magnetometers, "magnetometers");
		_lambda(gyroscopes, "gyroscopes");

		if (memoryDataManager) {
			auto memTransaction = memoryDataManager->transaction();
			for (auto it = objects.begin(); it != objects.end(); ++it){
				memTransaction->addData(*it);
			}
		}
	}

	for (int i = 0; i < config->imusCount; ++i) {
		auto collectionItem = utils::make_shared<core::HierarchyItem>(QString::fromStdString(generateChannelName(*config, i+1)), QString());
		sessionItem->appendChild(collectionItem);

		collectionItem->appendChild(createChannelItem(accelerations, i, QObject::tr("Acceleration")));
		collectionItem->appendChild(createChannelItem(magnetometers, i, QObject::tr("Magnetometer")));
		collectionItem->appendChild(createChannelItem(gyroscopes, i, QObject::tr("Gyroscope")));
	}

	/*auto accItem = createImuCollectionItem(0, accelerations);
	sessionItem->appendChild(accItem);

	auto magItem = createImuCollectionItem(1, magnetometers);
	sessionItem->appendChild(magItem);

	auto gyrItem = createImuCollectionItem(2, gyroscopes);
	sessionItem->appendChild(gyrItem);*/
}

core::IHierarchyItemPtr IMU::IMUPerspective::createImuCollectionItem(int i, c3dlib::VectorChannelCollectionConstPtr collection)
{
	QString name;
	switch (i) {
		case 0: name = QObject::tr("Accelerations"); break;
		case 1: name = QObject::tr("Magnetometers"); break;
		case 2: name = QObject::tr("Gyroscopes"); break;
		default: name = QObject::tr("Item"); break;
	}
	auto collectionItem = utils::make_shared<core::HierarchyItem>(name, QString());
	for (int j = 0; j < collection->getNumAccessors(); ++j) {
		auto c = collection->getAccessor(j);
		auto wrapper = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(utils::const_pointer_cast<c3dlib::VectorChannelReaderInterface>(c));
		static int number = 0;
		wrapper->setMetadata("name", "Serie_" + boost::lexical_cast<std::string>(number++));
		wrapper->setMetadata("source", "TreeBuilder");
		NewVector3ItemHelperPtr channelHelper = utils::make_shared<NewVector3ItemHelper>(wrapper);
		core::IHierarchyItemPtr channelItem = utils::make_shared<core::HierarchyDataItem>(wrapper, QIcon(), QString::fromStdString(c->feature<dataaccessor::DescriptorFeature>()->name()), QString(), channelHelper);
		collectionItem->appendChild(channelItem);
	}

	return collectionItem;
}

IMU::IMUPerspective::IMUPerspective(core::IDataManager * memoryDataManager) : 
	memoryDataManager(memoryDataManager)
{

}

std::string IMU::IMUPerspective::generateChannelName(const IMU::IMUConfig& conf, int idx)
{
	auto size = conf.joints.size();
	for (int i = 0; i < size; ++i) {
		if (conf.joints[i].idx == idx) {
			return conf.joints[i].name;
		}
	}
	QString unknown = QObject::tr("Sensor %1").arg(idx);
	return unknown.toStdString();
}

core::IHierarchyItemPtr IMU::IMUPerspective::createChannelItem(c3dlib::VectorChannelCollectionPtr collection, int i, const QString& name)
{
	auto c = collection->getAccessor(i);
	auto wrapper = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(utils::const_pointer_cast<c3dlib::VectorChannelReaderInterface>(c));	
	static int number = 0;
	wrapper->setMetadata("name", "Serie_" + boost::lexical_cast<std::string>(number++));
	wrapper->setMetadata("source", "TreeBuilder");
	NewVector3ItemHelperPtr channelHelper = utils::make_shared<NewVector3ItemHelper>(wrapper);
	core::IHierarchyItemPtr channelItem = utils::make_shared<core::HierarchyDataItem>(wrapper, QIcon(), name, QString(), channelHelper);
	return channelItem;
}

/*
void IMU::IMUPerspective::generateAnglesChannelBranch(kinematic::JointAnglesCollectionPtr ja, core::IHierarchyItemPtr skeletonItem)
{
	VectorChannelCollectionPtr angles = utils::make_shared<VectorChannelCollection>();
	int count = ja->getNumChannels();
	for (int i = 0; i < count; ++i) {

		kinematic::JointAngleChannelPtr channel = ja->getChannel(i);
		auto c = utils::make_shared<VectorChannel>(channel->getSamplesPerSecond());
		c->setName(channel->getName());
		c->setValueBaseUnit("rad");

		int size = channel->size();

		for (int idx = 0; idx < size; ++idx) {
			osg::Quat q = channel->value(idx);
			//TODO - weryfikacja konwencji konwersji euler->quat
			osg::Vec3 v = kinematicUtils::convertXYX(q);
			c->addPoint(v);
		}

		angles->addChannel(c);
	}
	
	for (int i = 0; i < count; ++i) {
		skeletonItem->appendChild(createChannelItem(angles, i, QString::fromStdString(ja->getChannel(i)->getName())));
	}
}*/

void IMU::IMUPerspectiveService::init(core::ISourceManager * sourceManager,
	core::IVisualizerManager * visualizerManager, core::IDataManager * memoryDataManager, 
	core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager,
	core::IDataHierarchyManager * hierarchyManager)
{
	this->sourceManager = sourceManager;
	this->memoryDataManager = memoryDataManager;
}

void IMU::IMUPerspectiveService::finalize()
{
}

void IMU::IMUPerspectiveService::update(double deltaTime)
{
}

QWidget* IMU::IMUPerspectiveService::getWidget()
{
	return nullptr;
}

QWidgetList IMU::IMUPerspectiveService::getPropertiesWidgets()
{
	return QWidgetList();
}

const bool IMU::IMUPerspectiveService::lateInit()
{
	auto comm = core::querySource<hmdbCommunication::IHMDBSource>(sourceManager);
	if (comm){
		comm->viewManager()->addHierarchyPerspective(utils::make_shared<IMUPerspective>(memoryDataManager));
		return true;
	}

	return false;
}
