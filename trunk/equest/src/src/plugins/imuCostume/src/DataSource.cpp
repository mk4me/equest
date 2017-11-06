#include "PCH.h"
#include "DataSource.h"
#include <boost/lexical_cast.hpp>
#include <utils/Filesystem.h>
#include <QtCore/QCoreApplication>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <corelib/IFileDataManager.h>
#include "IMUCostumeListWidget.h"
#include <QtCore/QCoreApplication>
#include <corelib/IDataHierarchyManager.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmmlib/TreeItemHelper.h>
#include <iosfwd>
#include <fstream>
#include "IMUPerspective.h"
#include "CostumeSkeletonMotionHelper.h"
#include <corelib/IVisualizerManager.h>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <plugins/newChart/INewChartVisualizer.h>
#include <loglib/Exceptions.h>
#include <plugins/imuCostume/Streams.h>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <plugins/c3d/C3DChannels.h>
#include <osgutils/QuatUtils.h>
#include <utils/Utils.h>
#include <dataaccessorlib/Wrappers.h>

struct PrecalculatedJointFrame
{
	osg::Vec3d localPosition;
	osg::Vec3d globalPosition;
	osg::Vec3d localOrientation;
	osg::Vec3d globalOrientation;
};

MEMBER_EXTRACTOR(localPosition);
MEMBER_EXTRACTOR(globalPosition);
MEMBER_EXTRACTOR(localOrientation);
MEMBER_EXTRACTOR(globalOrientation);

using PrecalculatedSkeletonFrame = std::vector < PrecalculatedJointFrame > ;
using PrecalculatedSkeletonFrames = std::vector < PrecalculatedSkeletonFrame >;

struct MotionDescription
{
	PrecalculatedSkeletonFrames frames;
	float frameTime;
	float duration;
};

std::string vectorParameterName(const unsigned int idx)
{
	switch (idx)
	{
	case 0:	return "x";
	case 1:	return "y";
	case 2:	return "z";
	case 3:	return "w";
	default: return QObject::tr("Unknown").toStdString();
	}
}

std::string bodyPlaneName(const unsigned int idx)
{
	switch (idx)
	{
	case 0:	return QObject::tr("sagittal plane").toStdString();
	case 1:	return QObject::tr("coronal plane").toStdString();
	case 2:	return QObject::tr("traverse plane").toStdString();	
	default: return QObject::tr("Unknown body plane").toStdString();
	}
}

class JointStreamExtractor
{
public:
	JointStreamExtractor(JointStreamExtractor && other) : joint(std::move(other.joint)) {}
	JointStreamExtractor(const JointStreamExtractor & other) : joint(other.joint) {}
	JointStreamExtractor(kinematic::Skeleton::JointConstPtr joint) : joint(joint) {}

	template<typename T>
	inline static bool verify(const T &) { return true; }

	template<typename T>
	void extract(const T & src, IMU::JointStream::value_type & jointData) const {
		jointData.first = src.first;
		jointData.second.globalOrientation = joint->value().globalOrientation();
		jointData.second.localOrientation = joint->value().localOrientation();
		jointData.second.globalPosition = joint->value().globalPosition();
		jointData.second.localPosition = joint->value().localPosition();		
	}

private:
	kinematic::Skeleton::JointConstPtr joint;
};

TIMEMEMBER_EXTRACTOR(globalOrientation);
TIMEMEMBER_EXTRACTOR(localOrientation);
TIMEMEMBER_EXTRACTOR(globalPosition);
TIMEMEMBER_EXTRACTOR(localPosition);

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class NewStreamVector3ItemHelper : public core::WrappedItemHelper
{
public:
	//! Konstruktor pobiera obiekt z wrapperem wektora 3D
	//! \param wrapper 
	core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager)
	{
		core::IVisualizerManager::VisualizerPrototypes prototypes;
		manager->getVisualizerPrototypes(typeid(ScalarStream), prototypes, true);
		core::VisualizerPtr visualizer(prototypes.front()->create());

		QWidget * visWidget = visualizer->getOrCreateWidget();
		visWidget->layout()->setContentsMargins(2, 0, 2, 2);
		INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
		if (!chart) {
			UTILS_ASSERT(false);
			throw loglib::runtime_error("Wrong visualizer type!");
		}
		else {			
			chart->setTitle(name + (units.isEmpty() == false ? " ["+units+"]" : ""));
			chart->setAxisScale(INewChartVisualizer::AxisX, 0, 5);
			chart->setAxisScale(INewChartVisualizer::AxisY, yMin, yMax);
		}
		return visualizer;
	}

	void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series)
	{
		static int number = 0;
		// hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
		//std::string suffix = boost::lexical_cast<std::string>(number++);
		//std::string p = path.toStdString();

		visualizer->getOrCreateWidget();

		auto serieX = visualizer->createSerie(xwrapper);
		//serieX->serie()->setName(xAxisName.isEmpty() == false ? xAxisName.toStdString() : "X_" + suffix);
		auto serieY = visualizer->createSerie(ywrapper);
		//serieY->serie()->setName(yAxisName.isEmpty() == false ? yAxisName.toStdString() : "Y_" + suffix);
		auto serieZ = visualizer->createSerie(zwrapper);
		//serieZ->serie()->setName(zAxisName.isEmpty() == false ? zAxisName.toStdString() : "Z_" + suffix);

		INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->innerSerie());
		INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->innerSerie());
		INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->innerSerie());

		chartSerieX->setColor(QColor(255, 0, 0));
		chartSerieY->setColor(QColor(0, 255, 0));
		chartSerieZ->setColor(QColor(0, 0, 255));

		series.push_back(serieX);
		series.push_back(serieY);
		series.push_back(serieZ);
	}

	NewStreamVector3ItemHelper(const core::VariantConstPtr& wrapper,
		const core::VariantConstPtr& xwrapper,
		const core::VariantConstPtr& ywrapper,
		const core::VariantConstPtr& zwrapper,
		const QString & name, const QString & units,
		const double yMin, const double yMax,
		const QString & xAxisName = QString(),
		const QString & yAxisName = QString(),
		const QString & zAxisName = QString()) :
		WrappedItemHelper(wrapper), xwrapper(xwrapper), ywrapper(ywrapper),
		zwrapper(zwrapper), name(name), units(units), yMin(yMin), yMax(yMax),
		xAxisName(xAxisName), yAxisName(yAxisName), zAxisName(zAxisName)
	{
	}

	std::vector<utils::TypeInfo> getTypeInfos() const
	{
		std::vector<utils::TypeInfo> ret;
		ret.push_back(typeid(ScalarStream));
		return ret;
	}

private:
	const QString name;
	const QString units;
	const core::VariantConstPtr xwrapper;
	const core::VariantConstPtr ywrapper;
	const core::VariantConstPtr zwrapper;
	const double yMin;
	const double yMax;
	const QString xAxisName;
	const QString yAxisName;
	const QString zAxisName;
};

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class NewStreamQuaternionItemHelper : public core::WrappedItemHelper
{
public:
	//! Konstruktor pobiera obiekt z wrapperem wektora 3D
	//! \param wrapper 
	core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager)
	{
		core::IVisualizerManager::VisualizerPrototypes prototypes;
		manager->getVisualizerPrototypes(typeid(ScalarStream), prototypes, true);
		core::VisualizerPtr visualizer(prototypes.front()->create());

		QWidget * visWidget = visualizer->getOrCreateWidget();
		visWidget->layout()->setContentsMargins(2, 0, 2, 2);
		INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
		if (!chart) {
			UTILS_ASSERT(false);
			throw loglib::runtime_error("Wrong visualizer type!");
		}
		else {
			chart->setTitle(name);
			chart->setAxisScale(INewChartVisualizer::AxisX, 0, 5);
			chart->setAxisScale(INewChartVisualizer::AxisY, yMin, yMax);
		}
		return visualizer;
	}

	void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series)
	{
		static int number = 0;
		// hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
		std::string suffix = boost::lexical_cast<std::string>(number++);
		//std::string p = path.toStdString();

		visualizer->getOrCreateWidget();

		auto serieX = visualizer->createSerie(xwrapper);
		serieX->innerSerie()->setName("X_" + suffix);
		auto serieY = visualizer->createSerie(ywrapper);
		serieY->innerSerie()->setName("Y_" + suffix);
		auto serieZ = visualizer->createSerie(zwrapper);
		serieZ->innerSerie()->setName("Z_" + suffix);
		auto serieW = visualizer->createSerie(wwrapper);
		serieZ->innerSerie()->setName("W_" + suffix);

		INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->innerSerie());
		INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->innerSerie());
		INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->innerSerie());
		INewChartSerie* chartSerieW = dynamic_cast<INewChartSerie*>(serieW->innerSerie());

		chartSerieX->setColor(QColor(255, 0, 0));
		chartSerieY->setColor(QColor(0, 255, 0));
		chartSerieZ->setColor(QColor(0, 0, 255));
		chartSerieW->setColor(QColor(0, 0, 0));

		series.push_back(serieX);
		series.push_back(serieY);
		series.push_back(serieZ);
		series.push_back(serieW);
	}

	NewStreamQuaternionItemHelper(const core::VariantConstPtr& wrapper,
		const core::VariantConstPtr& xwrapper,
		const core::VariantConstPtr& ywrapper,
		const core::VariantConstPtr& zwrapper,
		const core::VariantConstPtr& wwrapper,
		const QString & name, const double yMin, const double yMax) :
		WrappedItemHelper(wrapper), xwrapper(xwrapper), ywrapper(ywrapper),
		zwrapper(zwrapper), wwrapper(wwrapper), name(name), yMin(yMin), yMax(yMax)
	{
	}

	std::vector<utils::TypeInfo> getTypeInfos() const
	{
		std::vector<utils::TypeInfo> ret;
		ret.push_back(typeid(ScalarStream));
		return ret;
	}

private:
	const QString name;	
	const core::VariantConstPtr xwrapper;
	const core::VariantConstPtr ywrapper;
	const core::VariantConstPtr zwrapper;
	const core::VariantConstPtr wwrapper;
	const double yMin;
	const double yMax;
};

template<typename StreamType, typename SensorDataType, typename Extractor>
core::HierarchyDataItemPtr extracImuStreamVector(core::VariantsList & domainData,
	utils::shared_ptr<StreamType> stream,
	SensorDataType & sd,
	const std::string & paramName,
	const QString & units, const double minY, const double maxY,
	const std::string & prefix = std::string())
{
	auto vec3Stream = utils::make_shared<threadingUtils::StreamAdapterT<typename StreamType::value_type, IMU::Vec3Stream::value_type, Extractor>>(stream);

	auto ow = core::Variant::wrap<IMU::Vec3Stream>(vec3Stream);
	ow->setMetadata("core/name", paramName);
	sd.domainData.push_back(ow);
	domainData.push_back(ow);

	std::vector<core::VariantConstPtr> vecDataObjects;
	//std::list<core::IHierarchyItemPtr> hierarchyItems;

	for (unsigned int j = 0; j < 3; ++j){
		auto scalarStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::Vec3Stream::value_type, ScalarStream::value_type, IMU::CompoundArrayExtractor>>(vec3Stream, IMU::CompoundArrayExtractor(j));

		auto ow = core::Variant::wrap<ScalarStream>(scalarStream);
		ow->setMetadata("core/name", vectorParameterName(j));
		sd.domainData.push_back(ow);
		domainData.push_back(ow);
		vecDataObjects.push_back(ow);

		//hierarchyItems.push_back(utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(vectorParameterName(j)), QString::fromStdString(vectorParameterName(j))));
	}

	auto helper = utils::make_shared<NewStreamVector3ItemHelper>(ow, vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], QString::fromStdString(prefix + paramName), units, minY, maxY);

	auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(paramName), QString::fromStdString(paramName), helper);

	/*for (auto & hi : hierarchyItems)
	{
		imuElement->appendChild(hi);
	}*/

	return imuElement;
}

class GeneralDiffPolicy
{
public:
	template<typename T>
	static T diff(const T & prev, const T & current, const double dt)
	{
		return (current - prev) / dt;
	}
};

template<>
osg::Quat GeneralDiffPolicy::diff<osg::Quat>(const osg::Quat & prev, const osg::Quat & current, const double dt)
{
	const auto prevInverse = prev.inverse();
	const auto quatDiff = current * prevInverse;
	//http://gamedev.stackexchange.com/questions/30926/quaternion-dfference-time-angular-velocity-gyroscope-in-physics-library
	auto diff = osg::QuatUtils::exp(osg::QuatUtils::log(quatDiff) / dt) * 2.0 * prevInverse;

	return osg::QuatUtils::normalize(diff);
	//return diff;
}

static std::string serialize(const osg::Quat & orient)
{
	std::stringstream ss;
	ss << orient.x() << " " << orient.y() << " " << orient.z() << " " << orient.w();
	return ss.str();
}

static std::string serialize(const osg::Vec3 & vec)
{
	std::stringstream ss;
	ss << vec.x() << " " << vec.y() << " " << vec.z();
	return ss.str();
}

template<typename ValueExtractor, typename StreamType, typename DiffPolicy = GeneralDiffPolicy>
class DiffAdapter
{
public:

	DiffAdapter(const ValueExtractor & ve = ValueExtractor()) : ve(ve), ret(false) {}	
	~DiffAdapter() {}
	
	template<typename SrcType>
	inline bool verify(const SrcType & src) const
	{
		static int level = 0;
		if (ve.verify(src) == true){
			level = (level + 1) % 2;
			typename StreamType::value_type current;
			ve.extract(src, current);

			double timeDiff = current.first - prev.first;

			if (timeDiff == 0){
				return false;
			}

			if (ret == true){

				timeDiff /= 1000.0;

				val.second = DiffPolicy::diff(prev.second, current.second, timeDiff);
				val.first = src.first;
				//PLUGIN_LOG_DEBUG("Level: " << level+1 << " time diff: " << timeDiff << " prev: [" << serialize(prev.second) << "] current: [" << serialize(current.second) << "] diff: [" << serialize(val.second) << "]");
				prev = current;
				return true;
			}
			else{
				prev = current;
				ret = true;
				return false;
			}			
		}

		return false;
	}
	
	template<typename SrcType>
	inline void extract(const SrcType & src, typename StreamType::value_type & dest) const {
		dest = val;
	}

private:
	mutable ValueExtractor ve;
	mutable typename StreamType::value_type prev;
	mutable typename StreamType::value_type val;
	mutable bool ret;
};

template<typename ValueExtractor>
class EulerAngleAdapter
{
public:

	EulerAngleAdapter(const ValueExtractor & ve = ValueExtractor()) : ve(ve) {}

	template<typename SrcType>
	inline bool verify(const SrcType & src) const { return ve.verify(src); }

	template<typename SrcType>
	inline void extract(const SrcType & src, IMU::Vec3Stream::value_type & dest) const{
		IMU::QuatStream::value_type val;
		ve.extract(src, val);
		dest.first = val.first;
		dest.second = kinematicUtils::toDegrees(kinematicUtils::convertXYZ(val.second));
	}

private:

	mutable ValueExtractor ve;
};

template<typename StreamType, typename SensorDataType, typename Extractor,
	template<typename> class AngleExtracotr = EulerAngleAdapter>
core::HierarchyDataItemPtr extracImuStreamLocalEuler(core::VariantsList & domainData,
	utils::shared_ptr<StreamType> stream,
	SensorDataType & sd,
	const QString & paramName,
	const QString & unit,
	const QString & prefix = QString())
{
	auto orientStream = utils::shared_ptr<IMU::Vec3Stream>(new threadingUtils::StreamAdapterT<StreamType::value_type,
		IMU::Vec3Stream::value_type, AngleExtracotr<Extractor>> (stream));

	auto ow = core::Variant::wrap<IMU::Vec3Stream>(orientStream);
	ow->setMetadata("core/name", paramName.toStdString());
	sd.domainData.push_back(ow);
	domainData.push_back(ow);

	std::vector<core::VariantConstPtr> vecDataObjects;
	//std::list<core::IHierarchyItemPtr> hierarchyItems;

	for (unsigned int j = 0; j < 3; ++j){
		auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<IMU::Vec3Stream::value_type, ScalarStream::value_type, IMU::CompoundArrayExtractor>(orientStream, IMU::CompoundArrayExtractor(j)));
		auto pName = bodyPlaneName(j);
		auto ow = core::Variant::wrap<ScalarStream>(scalarStream);
		ow->setMetadata("core/name", pName);
		sd.domainData.push_back(ow);
		domainData.push_back(ow);
		vecDataObjects.push_back(ow);

		//hierarchyItems.push_back(utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(pName), QString::fromStdString(pName)));
	}

	auto helper = utils::make_shared<NewStreamVector3ItemHelper>(ow, vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], prefix + paramName, unit, -180.0, 180.0);

	auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), paramName, paramName, helper);

	/*for (auto & hi : hierarchyItems)
	{
		imuElement->appendChild(hi);
	}*/

	return imuElement;
}

template<typename StreamType, typename SensorDataType, typename Extractor,
	template<typename> class AngleAdapter = EulerAngleAdapter>
core::HierarchyDataItemPtr extracImuStreamGlobalEuler(core::VariantsList & domainData,
	utils::shared_ptr<StreamType> stream,
	SensorDataType & sd,
	const QString & paramName,
	const QString & unit,
	const QString & prefix = QString())
{
	auto orientStream = utils::shared_ptr<IMU::Vec3Stream>(new threadingUtils::StreamAdapterT<StreamType::value_type,
		IMU::Vec3Stream::value_type, AngleAdapter<Extractor >> (stream));

	auto ow = core::Variant::wrap<IMU::Vec3Stream>(orientStream);
	ow->setMetadata("core/name", paramName.toStdString());
	sd.domainData.push_back(ow);
	domainData.push_back(ow);

	std::vector<core::VariantConstPtr> vecDataObjects;
	//std::list<core::IHierarchyItemPtr> hierarchyItems;

	for (unsigned int j = 0; j < 3; ++j){
		auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<IMU::Vec3Stream::value_type, ScalarStream::value_type, IMU::CompoundArrayExtractor>(orientStream, IMU::CompoundArrayExtractor(j)));
		auto pName = vectorParameterName(j);
		auto ow = core::Variant::wrap<ScalarStream>(scalarStream);
		ow->setMetadata("core/name", pName);
		sd.domainData.push_back(ow);
		domainData.push_back(ow);
		vecDataObjects.push_back(ow);

		//hierarchyItems.push_back(utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(pName), QString::fromStdString(pName)));
	}

	auto helper = utils::make_shared<NewStreamVector3ItemHelper>(ow, vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], prefix + paramName, unit, -180.0, 180.0);

	auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), paramName, paramName, helper);

	/*for (auto & hi : hierarchyItems)
	{
	imuElement->appendChild(hi);
	}*/

	return imuElement;
}

template<typename StreamType, typename SensorDataType, typename Extractor>
core::HierarchyDataItemPtr extracImuStreamQuat(core::VariantsList & domainData,
	utils::shared_ptr<StreamType> stream,
	SensorDataType & sd,
	const std::string & paramName,
	const std::string & prefix = std::string())
{
	auto orientStream = utils::shared_ptr<IMU::QuatStream>(new threadingUtils::StreamAdapterT<StreamType::value_type, IMU::QuatStream::value_type, Extractor>(stream));

	auto ow = core::Variant::wrap<IMU::QuatStream>(orientStream);
	ow->setMetadata("core/name", paramName);
	sd.domainData.push_back(ow);
	domainData.push_back(ow);

	std::vector<core::VariantConstPtr> vecDataObjects;
	//std::list<core::IHierarchyItemPtr> hierarchyItems;

	for (unsigned int j = 0; j < 4; ++j){
		auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<IMU::QuatStream::value_type, ScalarStream::value_type, IMU::CompoundArrayExtractor>(orientStream, IMU::CompoundArrayExtractor(j)));

		auto ow = core::Variant::wrap<ScalarStream>(scalarStream);
		ow->setMetadata("core/name", vectorParameterName(j));
		sd.domainData.push_back(ow);
		domainData.push_back(ow);
		vecDataObjects.push_back(ow);

		//hierarchyItems.push_back(utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(vectorParameterName(j)), QString::fromStdString(vectorParameterName(j))));
	}

	auto helper = utils::make_shared<NewStreamQuaternionItemHelper>(ow, vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], vecDataObjects[3], QString::fromStdString(prefix + paramName), -1, 1);

	auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(paramName), QString::fromStdString(paramName), helper);

	/*for (auto & hi : hierarchyItems)
	{
	imuElement->appendChild(hi);
	}*/

	return imuElement;
}

utils::SamplesStatus::StatusMap createStatusMap()
{
	utils::SamplesStatus::StatusMap ret;

	ret.insert(utils::SamplesStatus::StatusMap::value_type(0.0, IMU::IIMUDataSource::ConnectionStatus::OFFLINE));
	ret.insert(utils::SamplesStatus::StatusMap::value_type(0.35, IMU::IIMUDataSource::ConnectionStatus::CONNECTION_PROBLEMS));
	ret.insert(utils::SamplesStatus::StatusMap::value_type(0.5, IMU::IIMUDataSource::ConnectionStatus::ONLINE));

	return ret;
}

typedef threadingUtils::StreamAdapterT<IMU::SensorsStreamData, IMU::MotionStream::value_type, IMU::ExtractCostumeMotion> RealMotionStream;

enum
{
	AccIdx = 0,
	GyroIdx = 1,
	MagIdx = 2,
	OrientationIdx = 3
};

using namespace IMU;

IMUCostumeDataSource::IMUCostumeDataSource()
	: memoryDM(nullptr), fileDM(nullptr), hierarchyDM(nullptr), finish(false)
{

}

IMUCostumeDataSource::~IMUCostumeDataSource()
{
}

void IMUCostumeDataSource::init(core::IDataManager * memoryDM,
	core::IStreamDataManager * streamDM,
	core::IFileDataManager * fileDM,
	core::IDataHierarchyManager * hierarchyDM)
{
	this->memoryDM = memoryDM;
	this->fileDM = fileDM;
	this->hierarchyDM = hierarchyDM;
	refreshThread = plugin::getThreadPool()->get(name(), "Costumes data reader");
}

void updateStatus(volatile IIMUDataSource::ConnectionStatus & destStatus,
	const utils::SamplesStatus & srcStatus)
{
	destStatus = (IIMUDataSource::ConnectionStatus)srcStatus.status();
}

void IMUCostumeDataSource::resfreshCostumesData()
{
	while (finish == false){
		CostumesData locCostumesData;
		{
			std::lock_guard<std::recursive_mutex > lock(synch);
			locCostumesData = costumesData;
		}

		if (locCostumesData.empty() == true){
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else{
			std::set<imuCostume::CostumeRawIO::CostumeAddress> costumes;
			imuCostume::CostumeCANopenIO::Frame frame;
			uint16_t length;

			CostumesDataFrame costumesDataFrame;

			for (auto & cd : locCostumesData)
			{				
				try{
					bool success = cd.second.rawCostume->receive(frame.buffer, length, 50);

					if (success == true){

						{
							std::lock_guard<std::recursive_mutex > lock(synch);
							cd.second.samplesStatus->positiveSample();
						}						

						if (length < 3 || frame.structure.header.structure.type != 0x80){
							continue;
						}

						imuCostume::ProtocolSendBufferHelper::Buffer streamBuffer;
						streamBuffer.buffer.reset(new uint8_t[length], utils::array_deleter<uint8_t>());
						streamBuffer.length = length;
						std::memcpy(streamBuffer.buffer.get(), frame.buffer.data(), length);
						//raw data
						//PLUGIN_LOG_DEBUG("Raw push");
						cd.second.rawDataStream->pushData(streamBuffer);
						//PLUGIN_LOG_DEBUG("After raw push");

						//potencjalne dane do zapisu przy nagrywaniu
						CostumeID id;
						id.ip = cd.second.rawCostume->ip();
						id.port = cd.second.rawCostume->port();
						
						costumes.insert(id);

						//konwertuje dane do czujnikow kompletnych

						updateSensorsStatus(frame, length, cd.second);

						//PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " data received");												
					}
					else{

						std::lock_guard<std::recursive_mutex > lock(synch);
						cd.second.samplesStatus->negativeSample();

						//nie ma danych to sensory tez maja false probke
						for (auto & sd : cd.second.sensorsData)
						{
							sd.second.samplesStatus->negativeSample();
						}

						PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " timeout on data receive");
					}					
				}
				catch (...){
					PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " failed to receive data");
				}
			}

			tryRecord(costumes);

		}
	}
}

bool IMUCostumeDataSource::lateInit()
{
	refreshThread.run(&IMUCostumeDataSource::resfreshCostumesData, this);

	try{
		auto p = plugin::getApplicationDataPath() / "profiles.pro";

		if (utils::Filesystem::pathExists(p) == true){
			std::ifstream f(p.string());

			//próbujemy czytać profile
			if (f.is_open() == true){

				while (f.eof() == false){

					auto pro = SerializableCostumeProfile::deserialize(f);

					if (SerializableCostumeProfile::verify(pro, this) != SerializableCostumeProfile::Failed){
						auto upro = utils::make_shared<IMU::CostumeProfile>(SerializableCostumeProfile::unpack(pro, this));
						costumesProfiles_.insert(CostumesProfiles::value_type(upro->name, upro));
					}
				}
			}			
		}
	}
	catch (...){
		PLUGIN_LOG_ERROR("Failed to read profiles");
	}

	return true;
}

void IMUCostumeDataSource::finalize()
{
	finish = true;

	if (refreshThread.joinable() == true){
		refreshThread.join();
		refreshThread = core::Thread();
	}

	if (costumesProfiles_.empty() == false){
		auto p = plugin::getApplicationDataPath() / "profiles.pro";
		PLUGIN_LOG_INFO("Trying to write " << costumesProfiles_.size() <<" profiles to " << p.string());
		try{
		
			std::ofstream f(p.string());

			//próbujemy zapisać profile
			if (f.is_open() == true){

				for (const auto & pro : costumesProfiles_){

					auto sPro = SerializableCostumeProfile::pack(*pro.second);
					SerializableCostumeProfile::serialize(f, sPro);
					f << std::endl;
				}
					
				f.close();
			}			
		}
		catch (...){
			PLUGIN_LOG_ERROR("Failed to write profiles");
		}
	}


	OrientationEstimationAlgorithms().swap(orientationEstimationAlgorithms_);
	CostumeCalibrationAlgorithms().swap(calibrationAlgorithms_);
	CostumeMotionEstimationAlgorithms().swap(motionEstimationAlgorithms_);
	SkeletonModels().swap(skeletonModels_);
	CostumesProfiles().swap(costumesProfiles_);
}

void IMUCostumeDataSource::update(double deltaTime)
{
}

QWidget* IMUCostumeDataSource::getWidget()
{
	return new IMUCostumeWidget(this);
}

QWidget* IMUCostumeDataSource::getControlWidget()
{
	return nullptr;
}

QWidget* IMUCostumeDataSource::getSettingsWidget()
{
	return nullptr;
}

void IMUCostumeDataSource::getOfferedTypes(utils::TypeInfoList & offeredTypes) const
{	
	offeredTypes.push_back(typeid(RawDataStream));
	offeredTypes.push_back(typeid(CANopenFramesStream));
	offeredTypes.push_back(typeid(CostumeStream));
	offeredTypes.push_back(typeid(IMUStream));
	offeredTypes.push_back(typeid(Vec3Stream));
	offeredTypes.push_back(typeid(ScalarStream));
}

bool IMUCostumeDataSource::addCostume(const CostumeID & id)
{
	static const uint8_t MaxSamplesCount = 125;
	std::lock_guard<std::recursive_mutex > lock(synch);
	auto it = costumesData.find(id);
	if (it == costumesData.end()){
		try{
			CostumeData cData;
			cData.rawCostume.reset(new imuCostume::CostumeRawIO(id.ip, id.port));
			cData.rawCostume->setSamplingDelay(10);
			cData.rawDataStream.reset(new RawDataStream);
			cData.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, createStatusMap());
			cData.samplesStatus->positiveSample();
			configureCostume(cData);
			innerRefreshCostumeSensorsConfiguration(cData, MaxSamplesCount);

			if (cData.sensorsConfiguration.empty() == true){
				return false;
			}

			costumesData.insert(CostumesData::value_type(id, cData));
		}
		catch (...){
			return false;
		}
	}

	return it == costumesData.end();
}

const bool IMUCostumeDataSource::refreshCostumes()
{
	static const uint8_t MaxSamplesCount = 125;

	auto costumes = imuCostume::CostumeRawIO::listAvailableCostumes();
	if (costumes.empty() == true){
		costumes = imuCostume::CostumeRawIO::listAvailableCostumes();
	}

	std::set<CostumeID> refreshedIPs;
	std::set<CostumeID> currentIPs;

	for (const auto & c : costumes){
		refreshedIPs.insert(c);
	}

	std::lock_guard<std::recursive_mutex > lock(synch);

	for (const auto & c : costumesData){
		currentIPs.insert(c.first);
	}

	std::vector<CostumeID> toAdd(costumes.size());
	std::vector<CostumeID> toRemove(costumesData.size());

	auto removeIT = std::set_difference(currentIPs.begin(), currentIPs.end(), refreshedIPs.begin(), refreshedIPs.end(), toRemove.begin());
	auto addIT = std::set_difference(refreshedIPs.begin(), refreshedIPs.end(), currentIPs.begin(), currentIPs.end(), toAdd.begin());

	toAdd.resize(addIT - toAdd.begin());
	toRemove.resize(removeIT - toRemove.begin());

	/*
	auto it = toRemove.begin();
	//TODO
	//infinit loop
	while (it != toRemove.end()){
		auto cIT = costumesData.find(*it);
		if (cIT != costumesData.end()){
			//TODO - unload jak był załadowany
			
			//if (cIT->second.domainData.empty() == false){
			//	it = toRemove.erase(it);
			//}
			//else{
			//	++it;
			//}
			
		}
		else{
			it = toRemove.erase(it);
		}
	}
	*/

	CostumesData additionalCostumesData;

	for (const auto & id : toAdd){
		try{
			CostumeData cData;
			cData.rawCostume.reset(new imuCostume::CostumeRawIO(id.ip, id.port));
			cData.rawCostume->setSamplingDelay(10);
			cData.rawDataStream.reset(new RawDataStream);
			cData.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, createStatusMap());
			cData.samplesStatus->positiveSample();
			configureCostume(cData);
			innerRefreshCostumeSensorsConfiguration(cData, MaxSamplesCount);		
			additionalCostumesData.insert(CostumesData::value_type(id, cData));
		}
		catch (...){
			//TODO
			//info o nieudanej probie załadowania kostiumu
		}
	}

	for (const auto & id : toRemove){		
		costumesData.erase(id);
	}

	costumesData.insert(additionalCostumesData.begin(), additionalCostumesData.end());

	return costumes.empty() == false;
}

void IMUCostumeDataSource::configureCostume(CostumeDescription & cd)
{
	imuCostume::ProtocolSendBufferHelper psbh;
	imuCostume::CostumeCANopenIO::prepareCostumeConfiguration(psbh);
	imuCostume::Costume::prepareCostumeConfiguration(psbh);

	while (psbh.empty() == false)
	{
		const auto buff = psbh.createBuffer();
		cd.rawCostume->send(buff.buffer.get(), buff.length);
	}
}

bool IMUCostumeDataSource::innerRefreshCostumeSensorsConfiguration(CostumeData & data, const uint8_t MaxSamplesCount)
{	
	bool ret = false;
	imuCostume::CostumeCANopenIO::Frame frame = { 0 };
	uint16_t length = 0;	

	for (unsigned int i = 0; i < MaxSamplesCount; ++i){		
		if (data.rawCostume->receive(frame.buffer, length, 100) == true){
			if (length > 2 && frame.structure.header.structure.type == 0x80){
				auto sc = imuCostume::Costume::sensorsConfiguration(frame.structure.data.data(), length - sizeof(imuCostume::CostumeCANopenIO::Header));
				ret = true;
				for (const auto & st : sc){
					auto it = data.sensorsConfiguration.find(st.first);
					if (it == data.sensorsConfiguration.end()){
						data.sensorsConfiguration.insert(st);
					}
					else{
						it->second.insert(st.second.begin(), st.second.end());
					}

					for (const auto & sid : st.second){						
						auto ret = data.sensorsData.insert(std::map<imuCostume::Costume::SensorID, SensorData>::value_type(sid, SensorData()));
						if (ret.second == true){
							ret.first->second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, createStatusMap());
						}
					}
				}
			}
		}
	}	

	return ret;
}

const unsigned int IMUCostumeDataSource::costumesCout() const
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	return costumesData.size();
}

bool IMUCostumeDataSource::costumeLoaded(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	auto it = costumesData.find(id);

	if (it != costumesData.end()){
		return !it->second.domainData.empty();
	}

	return false;
}

std::string sensorParameterName(const unsigned int idx)
{
	switch (idx)
	{
	case AccIdx:			return QObject::tr("Accelerometer").toStdString();
	case GyroIdx:			return QObject::tr("Gyroscope").toStdString();
	case MagIdx:			return QObject::tr("Magnetometer").toStdString();
	case OrientationIdx:	return QObject::tr("Orientation").toStdString();
	default:				return QObject::tr("Unknown").toStdString();
	}
}


core::HierarchyItemPtr IMUCostumeDataSource::fillRawCostumeData(CostumeData & cData)
{
	auto hierarchyTransaction = hierarchyDM->transaction();
	//core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>("Raw Data", "Raw communication data", QIcon());	
	//hierarchyTransaction->addRoot(root);

	//const auto & sc = cData.sensorsConfiguration;
	auto ow = core::Variant::create<RawDataStream>();
	ow->setMetadata("core/name", QObject::tr("Raw Data").toStdString());

	static_assert((!std::is_const<typename utils::pointed_type<decltype(cData.rawDataStream)>::type>::value), "Nalezy zapisywac dane bez modyfikatora const");


	ow->set(cData.rawDataStream);
	cData.domainData.push_back(ow);

	//core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Raw stream"), QObject::tr("Raw Ethernet costume frames"));
	//root->appendChild(item);

	cData.CANopenStream.reset(new threadingUtils::StreamAdapterT<RawDataStream::value_type, CANopenFramesStream::value_type, RawToCANopenExtractor>(cData.rawDataStream, RawToCANopenExtractor()));
	ow = core::Variant::create<CANopenFramesStream>();
	ow->setMetadata("core/name", QObject::tr("CANopen Data").toStdString());
	ow->set(cData.CANopenStream);
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("CANopen stream"), QObject::tr("Extracted CANopen frames stream"));
	//root->appendChild(item);

	cData.costumeStream.reset(new threadingUtils::StreamAdapterT<CANopenFramesStream::value_type, CostumeStream::value_type, CANopenDataExtractor>(cData.CANopenStream, CANopenDataExtractor()));
	ow = core::Variant::create<CostumeStream>();
	ow->setMetadata("core/name", QObject::tr("Costume Data").toStdString());
	ow->set(cData.costumeStream);
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Sensors stream"), QObject::tr("Grouped sensors data and status stream"));
	//root->appendChild(item);

	//return root;
	return core::HierarchyItemPtr();
}

void IMUCostumeDataSource::loadRawCostume(const CostumeID & id)
{
	std::lock_guard<std::recursive_mutex > lock(synch);	
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		if (it->second.domainData.empty() == false){
			throw std::runtime_error("Costume already loaded");
		}
	}	
	
	CostumeData cData = it->second;

	fillRawCostumeData(cData);

	try{
		auto t = memoryDM->transaction();
		for (const auto & ow : cData.domainData){
			t->addData(ow);
		}

		it->second = cData;

		//auto rootItem = utils::make_shared<core::HierarchyItem>(QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port), "", QIcon());
		//rootItem->appendChild(hItem);		
		//memoryDM->hierarchyTransaction()->addRoot(rootItem);
		///it->second.hierarchyRootItem = rootItem;
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());		
	}
	catch (...){
		PLUGIN_LOG_ERROR("Unknown error while loading raw costume data");
	}
}
void IMUCostumeDataSource::loadCalibratedCostume(const CostumeID & id,
	CostumeProfilePtr profile)
{
	std::lock_guard<std::recursive_mutex > lock(synch);	
	
	auto it = costumesData.find(id);

	if (it == costumesData.end()){
		throw std::runtime_error("Costume not found");
	}
	
	if (it->second.domainData.empty() == false){
		throw std::runtime_error("Costume already loaded");	
	}

	unsigned int imusCount = 0;	

	auto sIT = it->second.sensorsConfiguration.find(imuCostume::Costume::IMU);
	if (sIT != it->second.sensorsConfiguration.end()){
		imusCount = sIT->second.size();
	}

	if (profile->sensorsDescriptions.size() > imusCount)
	{
		throw std::runtime_error("Mismatch in number of loaded sensors orientation estimation algorithms and costume configuration");
	}

	CostumeData cData = it->second;

	fillRawCostumeData(cData);

	auto root = utils::make_shared<core::HierarchyItem>(QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port), "", QIcon());
	//core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(QObject::tr("Unpacked Data"), QObject::tr("Unpacked IMU sensors data"), QIcon());

	//adapter na pełne dane z kostiumu napędzające strumień z estymacją
	cData.completeImuStream.reset(new ExtractedCostumeStreamAdapter(cData.costumeStream, CostumeIMUExtractor(cData.sensorsConfiguration)));

	auto ow = core::Variant::create<IMU::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Complete unpacked IMU data").toStdString());
	ow->set(IMU::SensorsStreamPtr(cData.completeImuStream));
	cData.domainData.push_back(ow);

	core::IHierarchyItemPtr item = utils::make_shared<core::HierarchyItem>(QObject::tr("IMU stream"), QObject::tr("IMU sensors data stream"));
	root->appendChild(item);

	unpackSensorsStream(cData.completeImuStream, cData.sensorsData, item, cData.domainData, profile);

	auto estimatedData = utils::make_shared<threadingUtils::StreamProcessorT<IMU::SensorsStreamData, OrientationEstimator>>(cData.completeImuStream,
		OrientationEstimator(profile));

	ow = core::Variant::create<IMU::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Filtered IMU stream").toStdString());
	ow->set(IMU::SensorsStreamPtr(estimatedData));
	cData.domainData.push_back(ow);

	item = utils::make_shared<core::HierarchyItem>(QObject::tr("Filtered IMU stream"), QObject::tr("IMU filtered orientation stream"));
	root->appendChild(item);

	unpackSensorsStream(estimatedData, cData.sensorsData, item, cData.domainData, profile);

	cData.skeletonMotion = utils::make_shared<IMU::CostumeSkeletonMotion>();
	ow = core::Variant::create<IMU::CostumeSkeletonMotion>();
	ow->setMetadata("core/name", QObject::tr("Skeleton").toStdString());
	ow->set(cData.skeletonMotion);
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Skeleton"), QObject::tr("Calibrated and filtrated skeleton"));
	//root->appendChild(item);

	//mapowanie pozycji wektora do nazwy jointa w szkielecie i stanie

	cData.skeletonMotion->mapping = kinematic::LinearizedSkeleton::createCompleteMapping(*profile->skeleton);	

	cData.skeletonMotion->skeleton = profile->skeleton;
	cData.skeletonMotion->stream.reset(new RealMotionStream(estimatedData,
		ExtractCostumeMotion(profile)));

	ow = core::Variant::create<MotionStream>();
	ow->setMetadata("core/name", QObject::tr("Skeleton stream").toStdString());
	ow->set(MotionStreamPtr(cData.skeletonMotion->stream));
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Skeleton stream"), QObject::tr("Calibrated and filtrated skeleton stream"));
	//root->appendChild(item);

	{
		//dane dla kinematic
		auto kStream = utils::make_shared<SkeletonWithStreamData>();
		kStream->skeleton = profile->skeleton;
		kStream->nodesMapping = cData.skeletonMotion->mapping;
		//SkeletonStateStream

		auto skeleton = kinematic::Skeleton(*profile->skeleton);

		std::list<kinematic::Skeleton::JointConstPtr> joints;

		kinematic::LinearizedSkeleton::Visitor::visit(skeleton, [&joints](kinematic::Skeleton::JointConstPtr joint)
		{
			joints.push_back(joint);
		});

		auto sStateStream = utils::make_shared<threadingUtils::StreamAdapterT<MotionStream::value_type, IMU::SkeletonStateStream::value_type, KinematicStreamExtractor>>(cData.skeletonMotion->stream, KinematicStreamExtractor(std::move(skeleton)));

		cData.domainData.push_back(core::Variant::wrap<IMU::SkeletonStateStream>(sStateStream));

		kStream->states.reset(new threadingUtils::StreamAdapterT<IMU::SkeletonStateStream::value_type, ::SkeletonStateStream::value_type, IMU::TimeRemoverExtractor>(sStateStream));

		auto kow = core::Variant::create<SkeletonWithStreamData>();
		kow->setMetadata("core/name", QObject::tr("Kinematic stream").toStdString());
		kow->set(kStream);

		item = utils::make_shared<core::HierarchyDataItem>(kow, QIcon(), QObject::tr("Kinematic stream"), QObject::tr("Kinematic 3D stream"));
		root->appendChild(item);

		auto ajitem = utils::make_shared<core::HierarchyItem>(QObject::tr("Dynamic joints"), QObject::tr("Joints with sensors"));
		item->appendChild(ajitem);
		//TODO
		//dodać gałęzie z jointami i ich danymi: lokalny kąt w szkielecie, globalny kąt w świecie, lokalna pozycja względem rodzica, globalna pozycja

		for (const auto & sd : profile->sensorsDescriptions)
		{
			auto it = std::find_if(joints.begin(), joints.end(), [&sd](kinematic::Skeleton::JointConstPtr joint)
			{
				if (joint->value().name() == sd.second.jointName){
					return true;
				}

				return false;
			});

			if (it != joints.end()){

				auto sIT = cData.sensorsData.find(sd.first);

				//TODO
				//dodać gałąź jointa active!!
				auto jname = (*it)->value().name();
				auto stream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::SkeletonStateStream::value_type, JointStream::value_type, JointStreamExtractor>>(sStateStream, JointStreamExtractor(*it));
				cData.domainData.push_back(core::Variant::wrap<JointStream>(stream));
				auto jitem = utils::make_shared<core::HierarchyItem>(QString::fromStdString(jname), QString::fromStdString(jname));
				ajitem->appendChild(jitem);

				//global orient
				{
					jitem->appendChild(extracImuStreamGlobalEuler<JointStream, IMU::IMUCostumeDataSource::SensorData,
						TimeMemberExtractorglobalOrientation>(cData.domainData, stream,
					//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalOrientation>(cData.domainData, stream,
						sIT->second, QObject::tr("Global orientation"), QObject::tr("deg"), QString::fromStdString(jname + "\n")));
				}

				//local orient
				{
					jitem->appendChild(extracImuStreamLocalEuler<JointStream, IMU::IMUCostumeDataSource::SensorData,
						TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
						//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
						sIT->second, QObject::tr("Local orientation"), QObject::tr("deg"), QString::fromStdString(jname + "\n")));					
				}

				//angular velocity
				{
					jitem->appendChild(extracImuStreamLocalEuler<JointStream, IMU::IMUCostumeDataSource::SensorData,
						DiffAdapter<TimeMemberExtractorlocalOrientation, IMU::QuatStream>>(cData.domainData, stream,
						//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
						sIT->second, QObject::tr("Angular velocity"), QObject::tr("deg/s"), QString::fromStdString(jname + "\n")));
				}

				//angular acceleration
				{
					typedef DiffAdapter<TimeMemberExtractorlocalOrientation, IMU::QuatStream> DA;
					jitem->appendChild(extracImuStreamLocalEuler<JointStream, IMU::IMUCostumeDataSource::SensorData,
						DiffAdapter<DA, IMU::QuatStream>> (cData.domainData, stream,
						//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
						sIT->second, QObject::tr("Angular acceleration"), QObject::tr("deg/s^2"), QString::fromStdString(jname + "\n")));
				}

				//local pos
				{
					jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalPosition>(cData.domainData, stream,
					//jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalPosition>(cData.domainData, stream,
						sIT->second, QObject::tr("Local position").toStdString(), QString(), -1, 1, jname + "\n"));
				}

				//global pos
				{
					jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalPosition>(cData.domainData, stream,
					//jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalPosition>(cData.domainData, stream,
					sIT->second, QObject::tr("Global position").toStdString(), QString(), -1, 1, jname + "\n"));
				}

				//linear velocity
				{
					jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, DiffAdapter<TimeMemberExtractorglobalPosition, IMU::Vec3Stream>>(cData.domainData, stream,
						//jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalPosition>(cData.domainData, stream,
						sIT->second, QObject::tr("Linear velocity").toStdString(), QObject::tr("m/s"), -2, 2, jname + "\n"));
				}

				//linear acceleration
				{
					typedef DiffAdapter<TimeMemberExtractorglobalPosition, IMU::Vec3Stream> DA;
					jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, DiffAdapter<DA, IMU::Vec3Stream>>(cData.domainData, stream,
						//jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalPosition>(cData.domainData, stream,
						sIT->second, QObject::tr("Linear acceleration").toStdString(), QObject::tr("m/s^2"), -10, 10, jname + "\n"));
				}

				joints.erase(it);
			}
		}

		ajitem = utils::make_shared<core::HierarchyItem>(QObject::tr("Static joints"), QObject::tr("Joints with no sensors attached"));
		item->appendChild(ajitem);

		SensorData sd;

		for (const auto & joint : joints){
			auto jname = joint->value().name();
			auto stream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::SkeletonStateStream::value_type, JointStream::value_type, JointStreamExtractor>>(sStateStream, JointStreamExtractor(joint));
			cData.domainData.push_back(core::Variant::wrap<JointStream>(stream));
			auto jitem = utils::make_shared<core::HierarchyItem>(QString::fromStdString(jname), QString::fromStdString(jname));
			ajitem->appendChild(jitem);

			//global orient
			{
				jitem->appendChild(extracImuStreamGlobalEuler<JointStream, IMU::IMUCostumeDataSource::SensorData,
					TimeMemberExtractorglobalOrientation>(cData.domainData, stream,
					//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalOrientation>(cData.domainData, stream,
					sd, QObject::tr("Global orientation"), QObject::tr("deg"), QString::fromStdString(jname + "\n")));				
			}

			//local orient
			{
				jitem->appendChild(extracImuStreamLocalEuler<JointStream, IMU::IMUCostumeDataSource::SensorData,
					TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
					//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
					sd, QObject::tr("Local orientation"), QObject::tr("deg"), QString::fromStdString(jname + "\n")));				
			}

			//angular velocity
			{
				jitem->appendChild(extracImuStreamLocalEuler < JointStream, IMU::IMUCostumeDataSource::SensorData,
					DiffAdapter < TimeMemberExtractorlocalOrientation, IMU::QuatStream >> (cData.domainData, stream,
					//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
					sd, QObject::tr("Angular velocity"), QObject::tr("deg/s"), QString::fromStdString(jname + "\n")));
			}

			//angular acceleration
			{
				typedef DiffAdapter<TimeMemberExtractorlocalOrientation, IMU::QuatStream> DA;
				jitem->appendChild(extracImuStreamLocalEuler < JointStream, IMU::IMUCostumeDataSource::SensorData,
					DiffAdapter < DA, IMU::QuatStream >> (cData.domainData, stream,
					//jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
					sd, QObject::tr("Angular acceleration"), QObject::tr("deg/s^2"), QString::fromStdString(jname + "\n")));
			}

			//local pos
			{
				jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalPosition>(cData.domainData, stream,
					sd, QObject::tr("Local position").toStdString(), QString(), -1, 1, jname + "\n"));
			}

			//global pos
			{
				jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalPosition>(cData.domainData, stream,
					sd, QObject::tr("Global position").toStdString(), QString(), -1, 1, jname + "\n"));
			}

			//linear velocity
			{
				jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, DiffAdapter<TimeMemberExtractorglobalPosition, IMU::Vec3Stream>>(cData.domainData, stream,
					sd, QObject::tr("Linear velocity").toStdString(), QObject::tr("m/s"), -2, 2, jname + "\n"));
			}

			//linear acceleration
			{
				typedef DiffAdapter<TimeMemberExtractorglobalPosition, IMU::Vec3Stream> DA;
				jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, DiffAdapter<DA, IMU::Vec3Stream>>(cData.domainData, stream,
					sd, QObject::tr("Linear acceleration").toStdString(), QObject::tr("m/s^2"), -10, 10, jname + "\n"));
			}
		}

		cData.domainData.push_back(kow);
	}

	try{
		{
			auto t = memoryDM->transaction();
			for (const auto & ow : cData.domainData){
				t->addData(ow);
			}
		}

		cData.profile = profile;
		it->second = cData;
		
		//auto troot = utils::make_shared<core::HierarchyItem>(QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port), "", QIcon());
		//troot->appendChild(hItem);
		//troot->appendChild(root);
		//memoryDM->hierarchyTransaction()->addRoot(troot);
		hierarchyDM->transaction()->addRoot(root);

		//it->second.hierarchyRootItem = troot;
		it->second.hierarchyRootItem = root;
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());
	}
	catch (...){
		PLUGIN_LOG_ERROR("Unknown error while loading raw costume data");
	}
}

void IMUCostumeDataSource::unloadCostume(const CostumeID & id)
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		auto transaction = memoryDM->transaction();

		for (const auto & d : it->second.domainData){
			transaction->tryRemoveData(d);
		}

		hierarchyDM->transaction()->removeRoot(it->second.hierarchyRootItem);

		it->second.hierarchyRootItem.reset();
		it->second.CANopenStream.reset();
		it->second.completeImuStream.reset();
		it->second.costumeStream.reset();
		core::VariantsList().swap(it->second.domainData);
		it->second.profile.reset();
		it->second.skeletonMotion.reset();
		
		for (auto & sd : it->second.sensorsData)
		{
			core::VariantsList().swap(sd.second.domainData);
		}
	}
	else{
		throw std::runtime_error("Costume not loaded");
	}
}

unsigned int IMUCostumeDataSource::loadedCostumesCount() const
{
	std::lock_guard<std::recursive_mutex > lock(synch);

	unsigned int ret = 0;

	for (const auto & val : costumesData)
	{
		if (val.second.domainData.empty() == false)
		{
			++ret;
		}
	}

	return ret;
}

core::ConstVariantsList IMUCostumeDataSource::costumeData(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex > lock(synch);

	core::ConstVariantsList ret;
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		for (const auto & d : it->second.domainData){
			ret.push_back(d);
		}
	}

	return ret;
}

void IMUCostumeDataSource::startRecording(RecordingConfigurationPtr recording)
{
	if (recording != nullptr){
		std::lock_guard<std::recursive_mutex > lock(synch);
		recordings.insert(recording);
	}
}

void IMUCostumeDataSource::stopRecording(RecordingConfigurationPtr recording)
{
	if (recording != nullptr){
		std::lock_guard<std::recursive_mutex > lock(synch);
		recordings.erase(recording);
	}
}

//core::HierarchyItemPtr IMUCostumeDataSource::createRootItem()
//{
//	return utils::make_shared<core::HierarchyItem>(QObject::tr("Costumes data"), QObject::tr("Costumes data"), QIcon());
//}
//
//core::HierarchyItemPtr IMUCostumeDataSource::createStreamItem()
//{
//	return utils::make_shared<core::HierarchyItem>(QObject::tr("Streams"), QObject::tr("Costumes data streams"), QIcon());
//}
//
//core::HierarchyItemPtr IMUCostumeDataSource::createRecordItem()
//{
//	return utils::make_shared<core::HierarchyItem>(QObject::tr("Recordings"), QObject::tr("Costumes data recordings"), QIcon());
//}
//
//core::HierarchyItemPtr IMUCostumeDataSource::createIMUsItem()
//{
//	return utils::make_shared<core::HierarchyItem>(QObject::tr("IMUs"), QObject::tr("IMUs data"), QIcon());
//}

//void IMUCostumeDataSource::tryCreateRootItem()
//{
//	if (root == nullptr){
//		root = createRootItem();
//		auto hierarchyTransaction = hierarchyDM->transaction();
//		hierarchyTransaction->addRoot(root);
//	}
//}
//
//void IMUCostumeDataSource::tryCreateStreamItem()
//{
//	if (streamItems == nullptr){
//		streamItems = createStreamItem();
//		tryCreateRootItem();
//		root->appendChild(streamItems);
//	}
//}
//
//void IMUCostumeDataSource::tryCreateRecordedItem()
//{
//	if (recordedItems == nullptr){
//		recordedItems = createRecordItem();
//		tryCreateRootItem();
//		root->appendChild(recordedItems);
//	}
//}

void IMUCostumeDataSource::registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithmPtr algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (algorithm == nullptr){
		throw std::runtime_error("Uninitialized orientation estimation algorithm");
	}

	if (orientationEstimationAlgorithms_.find(algorithm->ID()) == orientationEstimationAlgorithms_.end()){
		orientationEstimationAlgorithms_.insert(OrientationEstimationAlgorithms::value_type(algorithm->ID(), IIMUOrientationEstimationAlgorithmConstPtr(algorithm)));
	}
	else{
		throw std::runtime_error("Orientation estimation algorithm already registered");
	}
}

void IMUCostumeDataSource::registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithmPtr algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (algorithm == nullptr){
		throw std::runtime_error("Uninitialized costume calibration algorithm");
	}

	if (calibrationAlgorithms_.find(algorithm->ID()) == calibrationAlgorithms_.end()){
		calibrationAlgorithms_.insert(CostumeCalibrationAlgorithms::value_type(algorithm->ID(), IMUCostumeCalibrationAlgorithmConstPtr(algorithm)));
	}
	else{
		throw std::runtime_error("Costume calibration algorithm already registered");
	}
}

void IMUCostumeDataSource::registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithmPtr algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (algorithm == nullptr){
		throw std::runtime_error("Uninitialized motion estimation algorithm");
	}

	if (motionEstimationAlgorithms_.find(algorithm->ID()) == motionEstimationAlgorithms_.end()){
		motionEstimationAlgorithms_.insert(CostumeMotionEstimationAlgorithms::value_type(algorithm->ID(), IMUCostumeMotionEstimationAlgorithmConstPtr(algorithm)));
	}
	else{
		throw std::runtime_error("Motion estimation algorithm already registered");
	}
}

void IMUCostumeDataSource::registerSkeletonModel(SkeletonPtr skeleton)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (skeleton == nullptr){
		throw std::runtime_error("Uninitialized skeleton model");
	}

	auto it = skeletonModels_.find(skeleton->id);	

	if (it == skeletonModels_.end()){
		skeletonModels_.insert(SkeletonModels::value_type(skeleton->id, skeleton));
	}
	else{
		throw std::runtime_error("Skeleton model already registered");
	}
}	

bool verifyProfile(const CostumeProfile & profile)
{
	bool ret = true;

	if (profile.name.empty() == true || profile.calibrationAlgorithm == nullptr ||
		profile.motionEstimationAlgorithm == nullptr ||
		profile.skeleton == nullptr || profile.sensorsDescriptions.empty() == true){

		ret = false;
	}

	return ret;
}

void IMUCostumeDataSource::registerCostumeProfile(CostumeProfilePtr profile)
{
	if (verifyProfile(*profile) == false){
		throw std::runtime_error("Incomplete profile");
	}

	std::lock_guard<std::recursive_mutex> lock(synch);

	if (costumesProfiles_.find(profile->name) != costumesProfiles_.end()){
		throw std::runtime_error("Profile already registered");
	}

	//produjemy rejestrowac brakujace elementy profilu

	if (profile->skeleton != nullptr){
		if (skeletonModels_.find(profile->skeleton->id) == skeletonModels_.end()){
			skeletonModels_.insert(SkeletonModels::value_type(profile->skeleton->id, profile->skeleton));
		}
	}

	//algorytm kalibracji
	if (calibrationAlgorithms_.find(profile->calibrationAlgorithm->ID()) == calibrationAlgorithms_.end())
	{
		calibrationAlgorithms_.insert(CostumeCalibrationAlgorithms::value_type(profile->calibrationAlgorithm->ID(), profile->calibrationAlgorithm));
	}

	//algorytm estymacji ruchu
	if (motionEstimationAlgorithms_.find(profile->motionEstimationAlgorithm->ID()) == motionEstimationAlgorithms_.end())
	{
		motionEstimationAlgorithms_.insert(CostumeMotionEstimationAlgorithms::value_type(profile->motionEstimationAlgorithm->ID(), profile->motionEstimationAlgorithm));
	}

	// algorytmy estymacji
	for (const auto & ea : profile->sensorsDescriptions)
	{
		if (orientationEstimationAlgorithms_.find(ea.second.orientationEstimationAlgorithm->ID()) == orientationEstimationAlgorithms_.end()){
			orientationEstimationAlgorithms_.insert(OrientationEstimationAlgorithms::value_type(ea.second.orientationEstimationAlgorithm->ID(), ea.second.orientationEstimationAlgorithm));
		}
	}

	//rejestrujemy profil
	costumesProfiles_.insert(CostumesProfiles::value_type(profile->name, profile));
}

IMUCostumeDataSource::OrientationEstimationAlgorithms IMUCostumeDataSource::orientationEstimationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return orientationEstimationAlgorithms_;
}

IMUCostumeDataSource::CostumeCalibrationAlgorithms IMUCostumeDataSource::calibrationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return calibrationAlgorithms_;
}

IMUCostumeDataSource::CostumeMotionEstimationAlgorithms IMUCostumeDataSource::motionEstimationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return motionEstimationAlgorithms_;
}

IMUCostumeDataSource::SkeletonModels IMUCostumeDataSource::skeletonModels() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return skeletonModels_;
}

IMUCostumeDataSource::CostumesProfiles IMUCostumeDataSource::costumesProfiles() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return costumesProfiles_;
}

hmdbCommunication::IHMDBShallowCopyContextPtr IMUCostumeDataSource::selectUploadContext() const
{
	hmdbCommunication::IHMDBShallowCopyContextPtr ret;

	auto source = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (source != nullptr){
		auto contextsCount = source->shallowContextsCount();
		std::map<QString, hmdbCommunication::IHMDBShallowCopyContextPtr> mapping;

		for (unsigned int i = 0; i < contextsCount; ++i){
			auto context = source->shallowContext(i);
			if (context != nullptr){
				if (context->shallowCopyRemoteContext() != nullptr){
					auto session = context->shallowCopyRemoteContext()->remoteContext()->session();
					mapping.insert(std::map<QString, hmdbCommunication::IHMDBShallowCopyContextPtr>::value_type(QString("%1@%2").arg(QString::fromStdString(session->motionServicesUrl())).arg(QString::fromStdString(session->userName())), context));
				}
			}
		}

		if (mapping.empty() != true){
			if (mapping.size() == 1){
				ret = mapping.begin()->second;
			}
			else{
				bool ok = true;
				QStringList items;

				for (const auto m : mapping){
					items << m.first;
				}

				auto item = QInputDialog::getItem(nullptr, QObject::tr("Select upload context"), QObject::tr("Database connection"), items, 0, false, &ok);
				if (ok == true){
					ret = mapping.find(item)->second;
				}
			}
		}
	}

	return ret;
}

IMUCostumeDataSource::CostumeDescription IMUCostumeDataSource::costumeDescription(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		return it->second;
	}

	return CostumeDescription();
}

const bool IMUCostumeDataSource::costumesEmpty() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return costumesData.empty();
}

bool IMUCostumeDataSource::refreshCostumeSensorsConfiguration(const CostumeID & id, const uint8_t samplesCount)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		return innerRefreshCostumeSensorsConfiguration(it->second, samplesCount);
	}

	return false;
}

void IMUCostumeDataSource::resetCostumeStatus(const CostumeID & id)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){		
		const auto sm = createStatusMap();
		it->second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, sm);		

		for (auto & ss : it->second.sensorsData)
		{
			ss.second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, sm);			
		}		
	}
}

void IMUCostumeDataSource::resetSensorStatus(const CostumeID & costumeID,
	const imuCostume::Costume::SensorID sensorID)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(costumeID);
	if (it != costumesData.end()){		

		auto sIT = it->second.sensorsData.find(sensorID);
		if (sIT != it->second.sensorsData.end()){
			sIT->second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, createStatusMap());			
		}
	}
}

IMUCostumeDataSource::CostumeStatus IMUCostumeDataSource::innerCreateCostumeStatus(const CostumeData & cData)
{
	CostumeStatus cs;
	cs.status = cData.samplesStatus != nullptr ? (ConnectionStatus)cData.samplesStatus->status() : UNKNOWN;

	for (const auto & ss : cData.sensorsData)
	{
		cs.sensorsStatus.insert(std::map<imuCostume::Costume::SensorID, ConnectionStatus>::value_type(ss.first, ss.second.samplesStatus != nullptr ? (ConnectionStatus)ss.second.samplesStatus->status() : UNKNOWN));
	}

	return cs;
}

IMUCostumeDataSource::CostumeStatus IMUCostumeDataSource::costumeStatus(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		return innerCreateCostumeStatus(it->second);
	}

	return CostumeStatus();
}

IMUCostumeDataSource::CostumeDetails IMUCostumeDataSource::costumeDetails(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		CostumeDetails cd;
		cd.descriptionDetails = it->second;
		cd.statusDetails = innerCreateCostumeStatus(it->second);
		return cd;
	}

	return CostumeDetails();
}

IMUCostumeDataSource::CostumesDescriptions IMUCostumeDataSource::costumesDescriptions() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	CostumesDescriptions ret;

	for (const auto & cd : costumesData)
	{
		ret.insert(CostumesDescriptions::value_type(cd.first, cd.second));
	}

	return ret;
}

IMUCostumeDataSource::CostumesStatus IMUCostumeDataSource::costumesStatus() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	CostumesStatus ret;

	for (const auto & cd : costumesData)
	{
		ret.insert(CostumesStatus::value_type(cd.first, innerCreateCostumeStatus(cd.second)));
	}

	return ret;
}

IMUCostumeDataSource::CostumesDetails IMUCostumeDataSource::costumesDetails() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	CostumesDetails ret;

	for (const auto & cd : costumesData)
	{
		CostumeDetails c;
		c.descriptionDetails = cd.second;
		c.statusDetails = innerCreateCostumeStatus(cd.second);
		ret.insert(CostumesDetails::value_type(cd.first, c));
	}

	return ret;
}

void IMUCostumeDataSource::unpackSensorsStream(IMU::SensorsStreamPtr stream,
	SensorsData & sensorsData,
	core::IHierarchyItemPtr root,
	core::VariantsList & domainData,
	CostumeProfileConstPtr profile)
{
	for (auto & sd : sensorsData)
	{		
		if (profile->sensorsDescriptions.find(sd.first) == profile->sensorsDescriptions.end()){
			continue;
		}

		auto imuStream = utils::shared_ptr<IMUStream>(new threadingUtils::StreamAdapterT<IMU::SensorsStreamData, IMU::IMUStream::value_type, IMU::IMUExtractor>(stream, IMU::IMUExtractor(sd.first)));

		auto ow = core::Variant::create<IMUStream>();
		auto jname = profile->sensorsDescriptions.find(sd.first)->second.jointName;
		auto name = QObject::tr("IMU %1 Data (joint: %2)").arg(sd.first).arg(jname.c_str());
		ow->setMetadata("core/name", name.toStdString());
		ow->set(imuStream);
		sd.second.domainData.push_back(ow);
		domainData.push_back(ow);

		auto imuItem = utils::make_shared<core::HierarchyItem>(name, name);
		root->appendChild(imuItem);		

		//ACC
		{
			imuItem->appendChild(extracImuStreamVector<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractoraccelerometer>(domainData, imuStream,
				sd.second, sensorParameterName(0), QString("m/s^2"), -15.0, 15.0));
		}

		//GYRO
		{
			imuItem->appendChild(extracImuStreamVector<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractorgyroscope>(domainData, imuStream,
				sd.second, sensorParameterName(1), QString("rad/s"), -0.3, 0.3));
		}

		//MAG
		{
			imuItem->appendChild(extracImuStreamVector<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractormagnetometer>(domainData, imuStream,
				sd.second, sensorParameterName(2), "", -2.5, 2.5));
		}

		//ORIENT
		{
			imuItem->appendChild(extracImuStreamQuat<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractororientation>(domainData, imuStream,
				sd.second, QObject::tr("Orientation").toStdString()));
		}	
	}
}

void IMUCostumeDataSource::uploadSession(const utils::Filesystem::Path & configuration,
	const utils::Filesystem::FilesList & recordings)
{

}



//TODO
//do wygeneralizowania
template<typename Extractor>
class PrecalculatedDataAdapter : public c3dlib::VectorChannelReaderInterface
{
public:

	PrecalculatedDataAdapter(utils::shared_ptr<MotionDescription> motionDescription,
		const kinematic::LinearizedSkeleton::NodeIDX jointIdx)
		: motionDescription(motionDescription), jointIdx(jointIdx)
	{

	}

	virtual ~PrecalculatedDataAdapter() {}

	//! \param idx Indeks próbki
	//! \return Wartość czasu dla danego indeksu
	virtual argument_type argument(const size_type idx) const override
	{
		return motionDescription->frameTime * idx;
	}

	//! \param idx Indeks próbki
	//! \return Wartość próbki dla danego indeksu
	virtual value_type value(const size_type idx) const override
	{
		return Extractor::extract(motionDescription->frames[idx][jointIdx]);
	}

	//! \param idx Indeks próbki
	//! \return Próbka dla danego indeksu
	virtual sample_type sample(const size_type idx) const override
	{
		return sample_type(argument(idx), value(idx));
	}

	//! \return Ilość próbek w kanale
	virtual size_type size() const override
	{
		return motionDescription->frames.size();
	}

	//! \return Czy kanał nie zawiera danych
	virtual bool empty() const override
	{
		return motionDescription->frames.empty();
	}

private:
	const kinematic::LinearizedSkeleton::NodeIDX jointIdx;
	utils::shared_ptr<MotionDescription> motionDescription;
};

void IMUCostumeDataSource::loadRecordedData(const utils::Filesystem::Path & asfFile,
	const utils::Filesystem::Path & amcFile,
	const utils::Filesystem::Path & configFile)
{
	auto transaction = fileDM->transaction();	
	transaction->addFile(amcFile);
	transaction->tryAddFile(asfFile);
	core::ConstVariantsList oList;

	transaction->getObjects(amcFile, oList);
	auto wrp1 = *oList.begin();
	transaction->getObjects(asfFile, oList);
	auto wrp2 = *(++oList.begin());

	acclaim::MotionDataConstPtr data = wrp1->get();
	acclaim::SkeletonConstPtr model = wrp2->get();
	kinematic::SkeletonPtr skeleton = utils::make_shared<kinematic::Skeleton>();
	if (!kinematic::Skeleton::convert(*model, *skeleton)) {
		throw std::runtime_error("Unable to convert skeleton");
	}
	SkeletonStatesPtr states = utils::make_shared<SkeletonStates>();
	states->frameTime = data->frameTime;

	const auto amapping = acclaim::Skeleton::createMapping(model->bones);	
	const auto activeMapping = kinematic::SkeletonState::createAcclaimActiveMappingLocal(*skeleton, model->bones);
	const auto helperData = acclaim::Skeleton::helperMotionData(*model);

	const auto size = skeleton->root()->size();

	auto precalculatedFrames = utils::make_shared<MotionDescription>();
	precalculatedFrames->frameTime = data->frameTime;
	precalculatedFrames->duration = data->frameTime * (float)data->frames.size();
	precalculatedFrames->frames.reserve(data->frames.size());

	kinematic::Skeleton helperSkeleton(*skeleton);

	for (auto& frame : data->frames) {
		PrecalculatedSkeletonFrame sf;
		sf.reserve(size);
		auto sChange = kinematic::SkeletonState::convert(model->bones, amapping, frame.bonesData, activeMapping, helperData, model->units.isAngleInRadians());
		states->frames.push_back(sChange);

		kinematic::SkeletonState::applyState(helperSkeleton, sChange);
		kinematic::LinearizedSkeleton::Visitor::visit(helperSkeleton, [&sf](kinematic::Skeleton::JointConstPtr joint)
		{
			PrecalculatedJointFrame jf;
			jf.localOrientation = kinematicUtils::toDegrees(kinematicUtils::convertXYZ(joint->value().localOrientation()));
			jf.globalOrientation = kinematicUtils::toDegrees(kinematicUtils::convertXYZ(joint->value().globalOrientation()));
			jf.localPosition = joint->value().localPosition();
			jf.globalPosition = joint->value().globalPosition();
			sf.push_back(jf);
		});

		precalculatedFrames->frames.push_back(sf);
	}

	PLUGIN_LOG_DEBUG("precalculatedFrames->frameTime = " << precalculatedFrames->frameTime);
	PLUGIN_LOG_DEBUG("precalculatedFrames->duration = " << precalculatedFrames->duration);
	PLUGIN_LOG_DEBUG("precalculatedFrames->frames.size = " << precalculatedFrames->frames.size());

	auto sws = utils::make_shared<SkeletonWithStates>();
	sws->skeleton = skeleton;
	sws->states = states;
	sws->nodesMapping = kinematic::LinearizedSkeleton::createCompleteMapping(*skeleton);
	auto object = core::Variant::create<SkeletonWithStates>();
	object->set(sws);

	//próbujemy dodac opis na bazie cfg
	QString desc;

	if (configFile.empty() == false && utils::Filesystem::pathExists(configFile) == true){
		//deserialize ccfg file and generate description
		QFile f(configFile.string().c_str());
		if (f.open(QFile::ReadOnly | QFile::Text) == true){
			QTextStream in(&f);
			desc = in.readAll();
		}
	}

	bool addItem = false;

	if (recordedItems == nullptr){
		addItem = true;
		recordedItems = utils::make_shared<core::HierarchyItem>(QObject::tr("Recorded data"), "", QIcon());
	}

	auto idx = recordedItems->getNumChildren();

	auto item = utils::make_shared<core::HierarchyItem>(QObject::tr("Recording %1").arg(idx + 1), desc, QIcon());
	recordedItems->appendChild(item);

	core::HierarchyDataItemPtr ditem = utils::make_shared<core::HierarchyDataItem>(object, QIcon(), QObject::tr("Kinematic skeleton motion"), "");
	item->appendChild(ditem);

	kinematic::LinearizedSkeleton::NodeIDX nidx = 0;

	auto ff = dataaccessor::FunctionFeature::feature(true);

	//adaptery dla jointów lokalnych, wyliczyć globale orientacje i pozycje
	//wygenerować hierarchię	

	kinematic::LinearizedSkeleton::Visitor::visit(*skeleton, [&ff, &nidx, &ditem, &precalculatedFrames](kinematic::Skeleton::JointPtr joint)
	{
		auto item = utils::make_shared<core::HierarchyItem>(QString::fromStdString(joint->value().name()), "", QIcon());

		//local orientatrion
		{
			auto data = utils::make_shared<PrecalculatedDataAdapter<MEMBER_EXTRACTOR_NAME(localOrientation)>>(precalculatedFrames, nidx);
			data->attachFeature(ff);
			typedef decltype(data)::element_type channelT;
			data->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Local orientation", "Deg", "s")));
			//	joint->value().name() + "\n" + QObject::tr("Local orientation").toStdString(), QObject::tr("s").toStdString(), QObject::tr("deg").toStdString(), nidx);
			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(data);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Local orientation"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), QString::fromStdString(bodyPlaneName(0)), QString::fromStdString(bodyPlaneName(1)), QString::fromStdString(bodyPlaneName(2))));
			item->appendChild(di);
		}


		//global orientation
		{			
			auto data = utils::make_shared<PrecalculatedDataAdapter<MEMBER_EXTRACTOR_NAME(globalOrientation)>>(precalculatedFrames, nidx);
			data->attachFeature(ff);
			typedef decltype(data)::element_type channelT;
			data->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Global orientation", "Deg", "s")));
			//joint->value().name() + "\n" + QObject::tr("Global orientation").toStdString(), QObject::tr("s").toStdString(), QObject::tr("deg").toStdString(), nidx);
			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(data);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Global orientation"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), "x", "y", "z"));
			item->appendChild(di);
		}

		const auto cFT = precalculatedFrames->frameTime;
		const auto sPS = 1.0 / cFT;
	
		dataaccessor::UniformArgumentsGenerator<float> uag(sPS, precalculatedFrames->frames.size());

		utils::shared_ptr<c3dlib::VectorChannelReaderInterface> av;
		//angular velocity
		{
			//auto data = utils::make_shared<c3dlib::VectorChannel>(sPS);
			std::vector<c3dlib::VectorChannelReaderInterface::value_type> data;
			data.reserve(precalculatedFrames->frames.size());
			//data->setName(joint->value().name() + "\n" + QObject::tr("Angular velocity").toStdString());
			//data->setTimeBaseUnit(QObject::tr("s").toStdString());
			//data->setValueBaseUnit(QObject::tr("deg").toStdString());
			
			//zasilamy danymi			
			data.push_back(c3dlib::VectorChannelReaderInterface::value_type());
			for (std::size_t idx = 1; idx < precalculatedFrames->frames.size(); ++idx)
			{
				auto diff = GeneralDiffPolicy::diff(kinematicUtils::convertXYZ(kinematicUtils::toRadians(precalculatedFrames->frames[idx - 1][nidx].localOrientation)),
					kinematicUtils::convertXYZ(kinematicUtils::toRadians(precalculatedFrames->frames[idx][nidx].localOrientation)), cFT);
				data.push_back(kinematicUtils::toDegrees(kinematicUtils::convertXYZ(diff)));
			}

			PLUGIN_LOG_DEBUG("angular velocity.size = " << data.size());
			
			av = dataaccessor::wrap(std::move(data), uag);
			av->attachFeature(ff);
			typedef decltype(av)::element_type channelT;
			av->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Angular velocity", "deg/s", "s")));
			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(av);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Angular velocity"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), "x", "y", "z"));
			item->appendChild(di);			
		}
		
		//angular acceleration
		{
			std::vector<c3dlib::VectorChannelReaderInterface::value_type> data;
			data.reserve(precalculatedFrames->frames.size());
			//auto data = utils::make_shared<c3dlib::VectorChannel>(sPS);
			//data->setName(joint->value().name() + "\n" + QObject::tr("Angular acceleration").toStdString());
			//data->setTimeBaseUnit(QObject::tr("s^2").toStdString());
			//data->setValueBaseUnit(QObject::tr("deg").toStdString());

			//zasilamy danymi			
			data.push_back(osg::Vec3());			
			for (std::size_t idx = 1; idx < precalculatedFrames->frames.size(); ++idx)
			{
				auto diff = GeneralDiffPolicy::diff(kinematicUtils::convertXYZ(kinematicUtils::toRadians(av->value(idx-1))),
					kinematicUtils::convertXYZ(kinematicUtils::toRadians(av->value(idx))), cFT);
				data.push_back(kinematicUtils::toDegrees(kinematicUtils::convertXYZ(diff)));
			}

			PLUGIN_LOG_DEBUG("angular acceleration.size = " << data.size());

			auto av = dataaccessor::wrap(std::move(data), uag);
			av->attachFeature(ff);
			typedef decltype(av)::element_type channelT;
			av->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Angular acceleration", "deg/s^2", "s")));
			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(av);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Angular acceleration"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), "x", "y", "z"));
			item->appendChild(di);
		}

		//local position
		{
			auto data = utils::make_shared<PrecalculatedDataAdapter<MEMBER_EXTRACTOR_NAME(localPosition)>>(precalculatedFrames, nidx);
			data->attachFeature(ff);
			typedef decltype(data)::element_type channelT;
			data->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Local position", "m", "s")));
				//joint->value().name() + "\n" + QObject::tr("Local position").toStdString(), QObject::tr("s").toStdString(), "", nidx);
			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(data);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Local position"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), "x", "y", "z"));
			item->appendChild(di);
		}

		//global position
		{
			auto data = utils::make_shared<PrecalculatedDataAdapter<MEMBER_EXTRACTOR_NAME(globalPosition)>>(precalculatedFrames, nidx);
			data->attachFeature(ff);
			typedef decltype(data)::element_type channelT;
			data->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Global position", "m", "s")));
				//joint->value().name() + "\n" + QObject::tr("Global position").toStdString(), QObject::tr("s").toStdString(), "", nidx);
			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(data);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Global position"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), "x", "y", "z"));
			item->appendChild(di);
		}
		
		//linear velocity
		utils::shared_ptr<c3dlib::VectorChannelReaderInterface> lv;
		{
			std::vector<c3dlib::VectorChannelReaderInterface::value_type> data;
			data.reserve(precalculatedFrames->frames.size());
			//data->setName(joint->value().name() + "\n" + QObject::tr("Linear velocity").toStdString());
			//data->setTimeBaseUnit(QObject::tr("s").toStdString());
			//data->setValueBaseUnit(QObject::tr("m").toStdString());

			//zasilamy danymi			
			data.push_back(osg::Vec3());
			for (std::size_t idx = 1; idx < precalculatedFrames->frames.size(); ++idx)
			{
				auto diff = GeneralDiffPolicy::diff(precalculatedFrames->frames[idx - 1][nidx].globalPosition,
					precalculatedFrames->frames[idx][nidx].globalPosition, cFT);
				data.push_back(diff);
			}

			PLUGIN_LOG_DEBUG("linear velocity.size = " << data.size());
			lv = dataaccessor::wrap(std::move(data), uag);
			lv->attachFeature(ff);

			typedef decltype(lv)::element_type channelT;
			lv->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Linear velocity", "m/s", "s")));

			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(lv);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Linear velocity"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), "x", "y", "z"));
			item->appendChild(di);			
		}
		
		//linear acceleration
		{
			std::vector<c3dlib::VectorChannelReaderInterface::value_type> data;
			data.reserve(precalculatedFrames->frames.size());
			//data->setName(joint->value().name() + "\n" + QObject::tr("Linear acceleration").toStdString());
			//data->setTimeBaseUnit(QObject::tr("s^2").toStdString());
			//data->setValueBaseUnit(QObject::tr("m").toStdString());

			//zasilamy danymi			
			data.push_back(osg::Vec3());
			for (std::size_t idx = 1; idx < precalculatedFrames->frames.size(); ++idx)
			{
				auto diff = GeneralDiffPolicy::diff(lv->value(idx - 1), lv->value(idx), cFT);
				data.push_back(diff);
			}

			PLUGIN_LOG_DEBUG("linear acceleration.size = " << data.size());

			auto av = dataaccessor::wrap(std::move(data), uag);
			av->attachFeature(ff);
			typedef decltype(av)::element_type channelT;
			av->attachFeature(dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<channelT::value_type, channelT::argument_type >("Linear acceleration", "m/s^2", "s")));

			auto ow = core::Variant::wrap<c3dlib::VectorChannelReaderInterface>(av);
			core::HierarchyDataItemPtr di = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Linear acceleration"), "", utils::make_shared<NewVector3ItemHelper>(ow,
				c3dlib::EventsCollectionConstPtr(), "x", "y", "z"));
			item->appendChild(di);
		}

		ditem->appendChild(item);

		++nidx;

	});

	auto hierarchyTransaction = hierarchyDM->transaction();
	
	if (addItem == true){
		hierarchyTransaction->addRoot(recordedItems);
	}
	else{
		hierarchyTransaction->updateRoot(recordedItems);
	}
}

void IMU::IMUCostumeDataSource::updateSensorsStatus(const imuCostume::CostumeCANopenIO::Frame &frame,
	const uint16_t length, const CostumeData &cd)
{
	auto s = imuCostume::Costume::extractSensorsData(frame.structure.data.data(), length - sizeof(imuCostume::CostumeCANopenIO::Header));

	std::map<imuCostume::Costume::SensorID, imuCostume::Costume::SensorDataPtr> sdat;

	for (auto ss : s)
	{
		sdat.insert({ ss->id(), ss });
	}

	{
		std::lock_guard<std::recursive_mutex > lock(synch);

		//nie ma danych to sensory tez maja false probke
		for (auto & sd : cd.sensorsData)
		{
			auto it = sdat.find(sd.first);
			if (it != sdat.end()){
				if (it->second->type() == imuCostume::Costume::IMU){
					auto ptr = utils::dynamic_pointer_cast<imuCostume::Costume::IMUSensor>(it->second);
					if (ptr->dataStatus() >= 0x07){
						sd.second.samplesStatus->positiveSample();
					}
					else if (ptr->dataStatus() > 0x00 && ptr->dataStatus() < 0x08){
						sd.second.samplesStatus->unspecifiedSample();
					}
					else{
						sd.second.samplesStatus->negativeSample();
					}
				}
				else{
					sd.second.samplesStatus->positiveSample();
				}
			}
			else{
				sd.second.samplesStatus->negativeSample();
			}
		}
	}
}

void IMU::IMUCostumeDataSource::tryRecord(std::set<imuCostume::CostumeRawIO::CostumeAddress> &costumes)
{
	if (costumes.empty() == false && recordings.empty() == false){
		std::lock_guard<std::recursive_mutex > lock(synch);
		for (auto & rec : recordings)
		{
			CostumesDataFrame cdf;
			for (const auto & ctr : rec->costumesToRecord)
			{
				auto it = costumes.find(ctr.first);
				if (it != costumes.end()){
					RecordedCostumeData rcd;

					auto cIT = costumesData.find(ctr.first);

					utils::shared_ptr<SkeletonWithStreamData> kinematicStream;
					cIT->second.domainData.back()->tryGet(kinematicStream);
					::SkeletonStateStream::value_type sdata;
					kinematicStream->states->data(sdata);

					IMU::SensorsStreamData cssd;
					cIT->second.completeImuStream->data(cssd);
					rcd.timestamp = cssd.timestamp;

					//zapis danych sensorów tego kostiumu
					for (const auto & sID : ctr.second)
					{
						RecordedSensorData rsd;
						auto sdIT = cIT->second.sensorsData.find(sID);
						auto ow = sdIT->second.domainData.front();
						utils::shared_ptr<IMUStream> imuStream;
						ow->tryGet(imuStream);
						IMUStream::value_type data;
						imuStream->data(data);

						rsd.accelerometer = data.second.accelerometer;
						rsd.gyroscope = data.second.gyroscope;
						rsd.magnetometer = data.second.magnetometer;
						rsd.orientation = data.second.orientation;

						auto it = sdIT->second.domainData.begin();
						std::advance(it, 18);

						ow = *it;
						ow->tryGet(imuStream);
						imuStream->data(data);

						rsd.estimatedOrientation = data.second.orientation;

						rcd.sensorsData.insert({ sID, rsd });
					}

					//zapis stanu szkieletu
					//TODO
					//globalny stan
					for (const auto & sd : sdata.data())
					{
						rcd.skeletonData.push_back({ sd, kinematic::SkeletonState::NonRigidJointState() });
					}

					cdf.insert({ ctr.first, rcd });
				}
			}

			rec->costumesDataBuffer.pushData(std::move(cdf));
		}
	}
}
