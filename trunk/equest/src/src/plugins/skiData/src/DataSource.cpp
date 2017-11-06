#include "DataSource.h"
#include <boost/algorithm/string.hpp>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include <corelib/IDataManager.h>
#include <corelib/IFileDataManager.h>
#include <corelib/IDataHierarchyManager.h>
#include <corelib/VariantsCollection.h>
#include <dataaccessorlib/UniformArgumentsGenerator.h>
#include <dataaccessorlib/Wrappers.h>
#include <utils/MemberExtractor.h>
#include "SkiMetadata.h"
#include "SkiData.h"
#include "SkiDataInitializerT.h"
#include <plugins/c3d/C3DChannels.h>
#include "DataSourceWidget.h"
#include <plugins/video/Wrappers.h>
#include <plugins/newChart/NewChartItemHelper.h>

using namespace skidata;

unsigned int SkiDataSource::runIdx = 0;

SkiDataSource::SkiDataSource()
{

}

SkiDataSource::~SkiDataSource()
{

}

void SkiDataSource::init(core::IDataManager * memoryDM,
	core::IStreamDataManager * streamDM,
	core::IFileDataManager * fileDM,
	core::IDataHierarchyManager * hierarchyDM)
{
	this->memoryDM = memoryDM;
	this->fileDM = fileDM;
	this->hierarchyDM = hierarchyDM;
}

bool SkiDataSource::lateInit()
{
	return true;
}

void SkiDataSource::finalize()
{

}

void SkiDataSource::update(double deltaTime)
{

}

QWidget* SkiDataSource::getWidget()
{
	return new DataSourceWidget(this);
}

QWidget* SkiDataSource::getControlWidget()
{
	return nullptr;
}

QWidget* SkiDataSource::getSettingsWidget()
{
	return nullptr;
}

void SkiDataSource::getOfferedTypes(utils::TypeInfoList & offeredTypes) const
{

}

std::list<utils::Filesystem::Path> filterRunFiles(const utils::Filesystem::Path & path)
{
	bool dataFound = false;
	bool metadataFound = false;
	auto files = utils::Filesystem::listFiles(path);
	std::list<utils::Filesystem::Path> filteredFiles;

	auto it = files.begin();
	while (it != files.end()) {
		auto ext = boost::to_lower_copy((*it).extension().string());
		if (ext == ".mp4") { filteredFiles.push_back(*it); }
		else
			if (ext == ".csv" && dataFound == false) { dataFound = true; filteredFiles.push_back(*it); }
		if (ext == ".xml" && metadataFound == false) { metadataFound = true; filteredFiles.push_back(*it); }
		++it;
	}

	if (dataFound == false || metadataFound == false) {
		filteredFiles.clear();
	}

	return filteredFiles;
}

unsigned int SkiDataSource::addRun(const utils::Filesystem::Path & path)
{
	if (utils::Filesystem::isDirectory(path) == false) {
		throw std::runtime_error("Not a ski run directory");
	}

	auto files = filterRunFiles(path);

	if (files.empty() == true) {
		throw std::runtime_error("Incomplete ski run data");
	}

	RunDescription rd;
	rd.loaded = false;
	rd.idx = runIdx++;

	for (const auto & f : files) {
		rd.files.push_back(f.filename().string());
	}

	//dodajemy do wpisów
	data.insert({ path, rd });

	return rd.idx;
}

void SkiDataSource::removeRun(const utils::Filesystem::Path & path)
{
	unloadRun(path);	
	data.erase(path);
}

std::list<utils::Filesystem::Path> SkiDataSource::currentRuns() const
{
	std::list<utils::Filesystem::Path> ret;

	for (const auto & r : data) { ret.push_back(r.first); }

	return ret;
}

core::IHierarchyItemPtr generateRunHierarchy(const core::VariantsList & input,
	const std::size_t idx,
	core::VariantsList & generatedData)
{
	return core::IHierarchyItemPtr();
}

MEMBER_EXTRACTOR(imu)

MEMBER_EXTRACTOR(leftSki)
MEMBER_EXTRACTOR(rightSki)
MEMBER_EXTRACTOR(chest)

	MEMBER_EXTRACTOR(accelerometer);
	MEMBER_EXTRACTOR(gyroscope);
	MEMBER_EXTRACTOR(magnetometer);
	MEMBER_EXTRACTOR(orientation);

MEMBER_EXTRACTOR(gps);
	MEMBER_EXTRACTOR(position);

		MEMBER_EXTRACTOR(longnitude);
		MEMBER_EXTRACTOR(latitude);
		MEMBER_EXTRACTOR(height);
		MEMBER_EXTRACTOR(heading);

	MEMBER_EXTRACTOR(speed);

		MEMBER_EXTRACTOR(speed2D);
		MEMBER_EXTRACTOR(speed3D);

MEMBER_EXTRACTOR(environment);

	MEMBER_EXTRACTOR(temperature);
	MEMBER_EXTRACTOR(preasure);	

#define SKI_EXTRACTORS(ski) \
	NESTED_MEMBER_EXTRACTOR(imu, ski, accelerometer) \
	NESTED_MEMBER_EXTRACTOR(imu, ski, gyroscope) \
	NESTED_MEMBER_EXTRACTOR(imu, ski, magnetometer) \
	NESTED_MEMBER_EXTRACTOR(imu, ski, orientation)


SKI_EXTRACTORS(leftSki)
SKI_EXTRACTORS(rightSki)
SKI_EXTRACTORS(chest)

NESTED_MEMBER_EXTRACTOR(gps, position, longnitude)
NESTED_MEMBER_EXTRACTOR(gps, position, latitude)
NESTED_MEMBER_EXTRACTOR(gps, position, height)
NESTED_MEMBER_EXTRACTOR(gps, position, heading)

NESTED_MEMBER_EXTRACTOR(gps, speed, speed2D)
NESTED_MEMBER_EXTRACTOR(gps, speed, speed3D)

NESTED_MEMBER_EXTRACTOR(environment, temperature)
NESTED_MEMBER_EXTRACTOR(environment, preasure)

template<std::size_t Idx>
class Vec3ElementInitializer : public core::IVariantInitializer
{
public:

	Vec3ElementInitializer(core::VariantConstPtr src) : src(src) {}
	virtual ~Vec3ElementInitializer() {}

	//! \param object Obiekt który inicjujemy
	virtual void initialize(core::Variant * object)
	{
		c3dlib::VectorChannelReaderInterfaceConstPtr srcData;
		if (src->tryGet(srcData) == true) {
			auto w = dataaccessor::StaticVector<Idx>::wrap(srcData);
			if (srcData->featureExists(dataaccessor::ArgumentBounded) == true) {
				w->attachFeature(srcData->feature(dataaccessor::ArgumentBounded));
			}

			if (srcData->featureExists(dataaccessor::Description) == true) {
				w->attachFeature(srcData->feature(dataaccessor::Description));
			}

			if (srcData->featureExists(dataaccessor::UniformArguments) == true) {
				w->attachFeature(srcData->feature(dataaccessor::UniformArguments));
			}

			object->set(w);
		}
	}

	//! \return Kopia inicjalizatora
	virtual IVariantInitializer * clone() const
	{
		return new Vec3ElementInitializer(src);
	}

	//! \param initializer Inicjalizator z ktorym się porównujemy
	//! \return Czy inicjalizatory dadzš takie same dane (częc wspólna)
	virtual bool isEqual(const IVariantInitializer & initializer) const { return false; }

private:

	const core::VariantConstPtr src;
};

class Vec3MagnitudeInitializer : public core::IVariantInitializer
{
public:

	Vec3MagnitudeInitializer(core::VariantConstPtr src) : src(src) {}
	virtual ~Vec3MagnitudeInitializer() {}

	//! \param object Obiekt który inicjujemy
	virtual void initialize(core::Variant * object)
	{
		c3dlib::VectorChannelReaderInterfaceConstPtr srcData;
		if (src->tryGet(srcData) == true) {
			std::vector<c3dlib::ScalarChannelReaderInterface::sample_type> data;
			data.reserve(srcData->size());
			for (std::size_t i = 0; i < srcData->size(); ++i) {
				auto sample = srcData->sample(i);
				data.push_back({sample.first, sample.second.length()});
			}

			auto w = dataaccessor::wrap(std::move(data));
			if (srcData->featureExists(dataaccessor::ArgumentBounded) == true) {
				w->attachFeature(srcData->feature(dataaccessor::ArgumentBounded));
			}

			if (srcData->featureExists(dataaccessor::Description) == true) {
				w->attachFeature(srcData->feature(dataaccessor::Description));
			}

			if (srcData->featureExists(dataaccessor::UniformArguments) == true) {
				w->attachFeature(srcData->feature(dataaccessor::UniformArguments));
			}

			object->set(w);
		}
	}

	//! \return Kopia inicjalizatora
	virtual IVariantInitializer * clone() const
	{
		return new Vec3MagnitudeInitializer(src);
	}

	//! \param initializer Inicjalizator z ktorym się porównujemy
	//! \return Czy inicjalizatory dadzš takie same dane (częc wspólna)
	virtual bool isEqual(const IVariantInitializer & initializer) const { return false; }

private:

	const core::VariantConstPtr src;
};

template<std::size_t Idx>
core::IHierarchyItemPtr generateVec3ElementSubBranch(core::VariantConstPtr data,
	const QString & name, const QString & description = QString())
{
	auto v = core::Variant::create<c3dlib::ScalarChannelReaderInterface>();
	v->setInitializer(utils::make_shared<Vec3ElementInitializer<Idx>>(data));
	v->copyMetadata(*data);
	v->setMetadata("core/name", name.toStdString());

	return utils::make_shared<core::HierarchyDataItem>(v, QIcon(), name, description, utils::make_shared<NewChartItemHelper>(v));
}

core::IHierarchyItemPtr generateVec3MagnitudeSubBranch(core::VariantConstPtr data,
	const QString & name, const QString & description = QString())
{
	auto v = core::Variant::create<c3dlib::ScalarChannelReaderInterface>();
	v->setInitializer(utils::make_shared<Vec3MagnitudeInitializer>(data));
	v->copyMetadata(*data);
	v->setMetadata("core/name", name.toStdString());

	return utils::make_shared<core::HierarchyDataItem>(v, QIcon(), name, description, utils::make_shared<NewChartItemHelper>(v));
}

void generateVec3SubBranches(core::VariantConstPtr data, core::IHierarchyItemPtr root)
{
	root->appendChild(generateVec3ElementSubBranch<0>(data, "x"));
	root->appendChild(generateVec3ElementSubBranch<1>(data, "y"));
	root->appendChild(generateVec3ElementSubBranch<2>(data, "z"));
}

core::IHierarchyItemPtr generateVec3Branch(core::VariantConstPtr data,
	const QString & name, const QString & description = QString())
{
	auto child = utils::make_shared<core::HierarchyDataItem>(data, QIcon(), name, description, utils::make_shared<NewVector3ItemHelper>(data));
	generateVec3SubBranches(data, child);
	return child;
}

core::IHierarchyItemPtr generateVec3CompleteBranch(core::VariantConstPtr data,
	const QString & name, const QString & description = QString())
{
	auto child = generateVec3Branch(data, name, description);
	child->appendChild(generateVec3MagnitudeSubBranch(data, "Magnitude"));
	return child;
}

#define GENERATE_IMU_BRANCH_INITIALIZERS(name) \
	auto initializerAcc = utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(imu, name, accelerometer)>>>(data, uag, all_acc_descriptions);\
	auto initializerGyro = utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(imu, name, gyroscope)>>>(data, uag, all_gyro_descriptions);\
	auto initializerMag = utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(imu, name, magnetometer)>>>(data, uag, all_mag_descriptions);\
	auto initializerOrient = utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(imu, name, orientation)>>>(data, uag, all_orient_descriptions);

template<typename ValueType = float>
dataaccessor::IFeaturePtr generateDescription(const QString & name, const QString & valueUnit)
{
	return dataaccessor::IFeaturePtr(dataaccessor::DescriptorFeature::create<ValueType, float>(name.toStdString(), valueUnit.toStdString(), "s"));
}

static const auto acc_description = generateDescription<osg::Vec3>("Acceleration", "m/s^2");
static const auto gyro_description = generateDescription<osg::Vec3>("Gyroscope", "deg/s");
static const auto mag_description = generateDescription<osg::Vec3>("Magnetometer", "mikroT");
static const auto orient_description = generateDescription<osg::Vec3>("Orientation", "deg");
static const auto heading_description = generateDescription<>("GPS Heading", "deg");
static const auto height_description = generateDescription<>("GPS Height", "m");
static const auto long_description = generateDescription<>("GPS Longitude", "deg");
static const auto lat_description = generateDescription<>("GPS Latitude", "deg");
static const auto speed2d_description = generateDescription<>("GPS Speed 2D", "m/s");
static const auto speed3d_description = generateDescription<>("GPS Speed 3D", "m/s");
static const auto temp_description = generateDescription<>("Temperature", "degC");
static const auto pressure_description = generateDescription<>("Pressure", "hPa");

#define GENERATE_IMU_BRANCH_DESCRIPTORS \
	std::vector<dataaccessor::IFeaturePtr> all_acc_descriptions; all_acc_descriptions.push_back(feature); all_acc_descriptions.push_back(acc_description); \
	std::vector<dataaccessor::IFeaturePtr> all_gyro_descriptions; all_gyro_descriptions.push_back(feature); all_gyro_descriptions.push_back(gyro_description); \
	std::vector<dataaccessor::IFeaturePtr> all_mag_descriptions; all_mag_descriptions.push_back(feature); all_mag_descriptions.push_back(mag_description); \
	std::vector<dataaccessor::IFeaturePtr> all_orient_descriptions; all_orient_descriptions.push_back(feature); all_orient_descriptions.push_back(orient_description);

#define GENERATE_GPS_BRANCH_DESCRIPTORS \
	std::vector<dataaccessor::IFeaturePtr> all_long_descriptions; all_long_descriptions.push_back(feature); all_long_descriptions.push_back(long_description); \
	std::vector<dataaccessor::IFeaturePtr> all_lat_descriptions; all_lat_descriptions.push_back(feature); all_lat_descriptions.push_back(lat_description); \
	std::vector<dataaccessor::IFeaturePtr> all_heading_descriptions; all_heading_descriptions.push_back(feature); all_heading_descriptions.push_back(heading_description); \
	std::vector<dataaccessor::IFeaturePtr> all_height_descriptions; all_height_descriptions.push_back(feature); all_height_descriptions.push_back(height_description); \
	std::vector<dataaccessor::IFeaturePtr> all_speed2d_descriptions; all_speed2d_descriptions.push_back(feature); all_speed2d_descriptions.push_back(speed2d_description); \
	std::vector<dataaccessor::IFeaturePtr> all_speed3d_descriptions; all_speed3d_descriptions.push_back(feature); all_speed3d_descriptions.push_back(speed3d_description); \

#define GENERATE_ENVIRONMENT_BRANCH_DESCRIPTORS \
	std::vector<dataaccessor::IFeaturePtr> all_temp_descriptions; all_temp_descriptions.push_back(feature); all_temp_descriptions.push_back(temp_description); \
	std::vector<dataaccessor::IFeaturePtr> all_pressure_descriptions; all_pressure_descriptions.push_back(feature); all_pressure_descriptions.push_back(pressure_description); \

core::IHierarchyItemPtr generateIMUSubBranch(core::VariantConstPtr data,
	const QString & name, const QString & description, core::VariantInitializerPtr iacc,
	core::VariantInitializerPtr igyro, core::VariantInitializerPtr imag, core::VariantInitializerPtr iorient)
{
	auto root = utils::make_shared<core::HierarchyItem>(name, description);

	auto v = core::Variant::create<c3dlib::VectorChannelReaderInterface>();
	v->setInitializer(iacc);
	v->copyMetadata(*data);
	v->setMetadata("core/name", "Accelerometer");
	root->appendChild(generateVec3CompleteBranch(v, "Accelerometer"));

	v = core::Variant::create<c3dlib::VectorChannelReaderInterface>();
	v->setInitializer(igyro);
	v->copyMetadata(*data);
	v->setMetadata("core/name", "Gyroscope");
	root->appendChild(generateVec3CompleteBranch(v, "Gyroscope"));

	v = core::Variant::create<c3dlib::VectorChannelReaderInterface>();
	v->setInitializer(imag);
	v->copyMetadata(*data);
	v->setMetadata("core/name", "Magnetometer");
	root->appendChild(generateVec3CompleteBranch(v, "Magnetometer"));

	v = core::Variant::create<c3dlib::VectorChannelReaderInterface>();
	v->setInitializer(iorient);
	v->copyMetadata(*data);
	v->setMetadata("core/name", "Orientation");
	//TODO - dodać moduł dla Eulerów na bazie kwaternionów - moduł kąta obrotu
	root->appendChild(generateVec3Branch(v, "Orientation"));

	return root;
}

core::IHierarchyItemPtr generateIMUBranch(core::VariantConstPtr data,
	const QString & description,
	const dataaccessor::UniformArgumentsGenerator<float> & uag,
	utils::shared_ptr<dataaccessor::UniformArgumentsFeature<float>> feature)
{
	GENERATE_IMU_BRANCH_DESCRIPTORS

	auto root = utils::make_shared<core::HierarchyItem>("IMU", description);
	{
		GENERATE_IMU_BRANCH_INITIALIZERS(leftSki)
		root->appendChild(generateIMUSubBranch(data, "Left ski", "", initializerAcc,
			initializerGyro, initializerMag, initializerOrient));
	}

	{
		GENERATE_IMU_BRANCH_INITIALIZERS(rightSki)
		root->appendChild(generateIMUSubBranch(data, "Right ski", "", initializerAcc,
			initializerGyro, initializerMag, initializerOrient));
	}

	{
		GENERATE_IMU_BRANCH_INITIALIZERS(chest)
		root->appendChild(generateIMUSubBranch(data, "Chest", "", initializerAcc,
			initializerGyro, initializerMag, initializerOrient));
	}

	return root;
}

core::IHierarchyItemPtr generateScalarBranch(core::VariantConstPtr data,
	const QString & name, const QString & description, core::VariantInitializerPtr initializer)
{
	auto v = core::Variant::create<c3dlib::ScalarChannelReaderInterface>();
	v->setInitializer(initializer);
	v->copyMetadata(*data);
	v->setMetadata("core/name", name.toStdString());	

	return utils::make_shared<core::HierarchyDataItem>(v, QIcon(), name, description, utils::make_shared<NewChartItemHelper>(v));
}

core::IHierarchyItemPtr generateGPSBranch(core::VariantConstPtr data,
	const QString & description,
	const dataaccessor::UniformArgumentsGenerator<float> & uag,
	utils::shared_ptr<dataaccessor::UniformArgumentsFeature<float>> feature)
{
	auto root = utils::make_shared<core::HierarchyItem>("GPS", description);

	GENERATE_GPS_BRANCH_DESCRIPTORS

	{
		auto child = utils::make_shared<core::HierarchyItem>("Position", description);
		{
			child->appendChild(generateScalarBranch(data, "Longitude", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(gps, position, longnitude)>>>(data, uag, all_long_descriptions)));
			child->appendChild(generateScalarBranch(data, "Latitude", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(gps, position, latitude)>>>(data, uag, all_lat_descriptions)));
			child->appendChild(generateScalarBranch(data, "Height", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(gps, position, height)>>>(data, uag, all_height_descriptions)));
			child->appendChild(generateScalarBranch(data, "Heading", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(gps, position, heading)>>>(data, uag, all_heading_descriptions)));
		}
		root->appendChild(child);
	}

	{
		auto child = utils::make_shared<core::HierarchyItem>("Speed", description);
		{
			child->appendChild(generateScalarBranch(data, "2D", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(gps, speed, speed2D)>>>(data, uag, all_speed2d_descriptions)));
			child->appendChild(generateScalarBranch(data, "3D", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(gps, speed, speed3D)>>>(data, uag, all_speed3d_descriptions)));
		}
		root->appendChild(child);
	}

	return root;
}

core::IHierarchyItemPtr generateEnvironmentBranch(core::VariantConstPtr data,
	const QString & description,
	const dataaccessor::UniformArgumentsGenerator<float> & uag,
	utils::shared_ptr<dataaccessor::UniformArgumentsFeature<float>> feature)
{
	auto root = utils::make_shared<core::HierarchyItem>("Environment", description);

	GENERATE_ENVIRONMENT_BRANCH_DESCRIPTORS
	
	root->appendChild(generateScalarBranch(data, "Temperature", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(environment, temperature)>>>(data, uag, all_temp_descriptions)));
	root->appendChild(generateScalarBranch(data, "Pressure", "", utils::make_shared<SkiDataInitializerT<dataaccessor::GeneralContainerValueExtractor<MEMBER_EXTRACTOR_NAME_N(environment, preasure)>>>(data, uag, all_pressure_descriptions)));

	return root;
}

core::IHierarchyItemPtr generateVideoBranch(const core::ConstVariantsList & video,
	const MoviesDelays & moviesDelays,
	const QString & description)
{
	auto root = utils::make_shared<core::HierarchyItem>("Video", description);

	for (const auto & v : video) {
		std::string name;
		v->getMetadata("core/source", name);
		name = utils::Filesystem::Path(name).filename().string();
		auto child = utils::make_shared<core::HierarchyDataItem>(v, QIcon(), QString::fromStdString(name), description);
		auto it = moviesDelays.find(name);
		if (it != moviesDelays.end()) {
			utils::const_pointer_cast<core::Variant>(v)->setMetadata("movieDelay", boost::lexical_cast<std::string>(it->second));
		}

		root->appendChild(child);
	}	

	return root;
}

core::IHierarchyItemPtr generateRunBranch(core::VariantConstPtr data,
	const core::ConstVariantsList & video,
	const MoviesDelays & moviesDelays,
	const QString & name,
	const QString & description,
	const dataaccessor::UniformArgumentsGenerator<float> & uag,
	utils::shared_ptr<dataaccessor::UniformArgumentsFeature<float>> feature)
{
	auto root = utils::make_shared<core::HierarchyItem>(name, description);

	root->appendChild(generateIMUBranch(data, "", uag, feature));
	root->appendChild(generateGPSBranch(data, "", uag, feature));
	root->appendChild(generateEnvironmentBranch(data, "", uag, feature));
	if (video.empty() == false) {
		root->appendChild(generateVideoBranch(video, moviesDelays, ""));
	}

	return root;
}

bool SkiDataSource::isLoaded(const utils::Filesystem::Path & path)
{
	auto it = data.find(path);

	if (it == data.end()) {
		throw std::runtime_error("Unmanaged run data");
	}

	return it->second.loaded;
}

void SkiDataSource::loadRun(const utils::Filesystem::Path & path)
{
	auto it = data.find(path);

	if (it == data.end()) {
		throw std::runtime_error("Unmanaged run data");
	}
	
	auto fdt = fileDM->transaction();

	core::ConstVariantsList vc;

	for (const auto & f : it->second.files) {
		const auto ff = path / f;
		fdt->addFile(ff);
		fdt->getObjects(ff, vc);
	}

	core::VariantConstPtr metav;
	core::VariantConstPtr datav;
	core::ConstVariantsList video;

	for (const auto & v : vc) {
		if (v->data()->isSupported(typeid(SkiMetadata)) == true) {
			metav = v;
		}
		else if (v->data()->isSupported(typeid(SkiData)) == true) {
			datav = v;
		}
		else if (v->data()->isSupported(typeid(VideoChannel)) == true) {
			video.push_back(v);
		}
	}

	utils::shared_ptr<const SkiMetadata> unpackedmd;

	if (metav != nullptr) {
		metav->tryGet(unpackedmd);
	}

	float dataFrequency = 50.0f;
	std::size_t samplesCount = 0;

	if (unpackedmd != nullptr) {
		float dataFrequency = unpackedmd->dataFrequency;
		std::size_t samplesCount = unpackedmd->samplesCount;
		if (samplesCount > 0 && dataFrequency > 0.0f) {
			dataaccessor::UniformArgumentsGenerator<float> uag(1.0f / dataFrequency, samplesCount);
			auto feature = utils::make_shared<dataaccessor::UniformArgumentsFeature<float>>(1.0f / dataFrequency);
			//TODO - dodać opis
			it->second.hierarchyItem = generateRunBranch(datav, video, unpackedmd->moviesDelays, QString("Run %1").arg(it->second.idx) + (unpackedmd->name.empty() == false ? QString(" - %1").arg(QString::fromStdString(unpackedmd->name)) : QString()), QString::fromStdString(unpackedmd->description), uag, feature);
			auto hdt = hierarchyDM->transaction();

			if (hierarchyRoot == nullptr) {
				hierarchyRoot = utils::make_shared<core::HierarchyItem>("Ski runs", "");
			}

			hierarchyRoot->appendChild(it->second.hierarchyItem);
			if (hierarchyRoot->getNumChildren() == 1) {
				hdt->addRoot(hierarchyRoot);
			}
			else {
				hdt->updateRoot(hierarchyRoot);
			}

			it->second.loaded = true;
		}
	}
	else {
		fdt->rollback();
	}
}

void SkiDataSource::unloadRun(const utils::Filesystem::Path & path)
{
	auto it = data.find(path);
	if (it != data.end()) {
		
		if (it->second.loaded == true) {
			{
				auto hdt = hierarchyDM->transaction();
				hierarchyRoot->removeChild(it->second.hierarchyItem);
				if (hierarchyRoot->getNumChildren() == 0) {
					hdt->removeRoot(hierarchyRoot);
				}
				else {
					hdt->updateRoot(hierarchyRoot);
				}
			}

			{
				auto fdt = fileDM->transaction();
				for (const auto & f : it->second.files) {
					const auto ff = path / f;
					fdt->tryRemoveFile(ff);
				}
			}

			{
				auto mdt = memoryDM->transaction();
				for (const auto & d : it->second.generatedData) {
					mdt->tryRemoveData(d);
				}
			}

			it->second.hierarchyItem.reset();
			//std::list<std::string>().swap(it->second.files);
			core::VariantsList().swap(it->second.generatedData);

			it->second.loaded = false;
		}
	}
}