#include "PCH.h"
#include <plugins/imuCostume/CostumeIMUExtractor.h>
#include <corelib/IPlugin.h>

static double calculateDelta(const imuCostume::CostumeCANopenIO::Timestamp current,
	const imuCostume::CostumeCANopenIO::Timestamp previous)
{
	double deltaT = (current >= previous) ? (current - previous) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - previous + current);
	deltaT /= 1000.0;

	if (deltaT >= 1000)
	{
		PLUGIN_LOG_DEBUG("Probably wrong delta calculation -> current: " << current << " previous: " << previous);
	}

	return deltaT;
}

using namespace IMU;

CostumeCompleteDataFilter::CostumeCompleteDataFilter(CostumeCompleteDataFilter && Other)
	: sensorsIDs(std::move(Other.sensorsIDs))
{

}

CostumeCompleteDataFilter::CostumeCompleteDataFilter(const CostumeCompleteDataFilter & Other)
	: sensorsIDs(Other.sensorsIDs)
{

}

CostumeCompleteDataFilter::CostumeCompleteDataFilter(const imuCostume::Costume::SensorIDsSet & sensorsIDs)
	: sensorsIDs(sensorsIDs)
{

}

CostumeCompleteDataFilter::CostumeCompleteDataFilter(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration)
{
	auto it = sensorsConfiguration.find(imuCostume::Costume::IMU);
	if (it == sensorsConfiguration.end())
	{
		throw std::runtime_error("Empty IMU sensors set");
	}

	sensorsIDs = it->second;
}

CostumeCompleteDataFilter::~CostumeCompleteDataFilter()
{

}

bool CostumeCompleteDataFilter::operator()(const IMU::CostumeStream::value_type & streamData) const
{
	imuCostume::Costume::SensorIDsSet locSensorsIDs(sensorsIDs);

	IMU::SensorsData locData;

	for (const auto & d : streamData.sensorsData)
	{
		if (d->type() == imuCostume::Costume::IMU){
			auto imuData = utils::static_pointer_cast<const imuCostume::Costume::IMUSensor>(d);
			if (imuData->dataStatus() & 0x0F)
			{
				locSensorsIDs.erase(d->id());
			}
		}
	}

	return (locSensorsIDs.empty() == true);
}

CostumeIMUExtractor::CostumeIMUExtractor(CostumeIMUExtractor && Other)
	: completeDataFilter(std::move(Other.completeDataFilter)),
	currentData(std::move(Other.currentData))
{

}

CostumeIMUExtractor::CostumeIMUExtractor(const CostumeIMUExtractor & Other)
	: completeDataFilter(Other.completeDataFilter),
	currentData(Other.currentData)
{

}

CostumeIMUExtractor::CostumeIMUExtractor(const imuCostume::Costume::SensorIDsSet & sensorsIDs)
	: completeDataFilter(sensorsIDs)
{
	for (const auto & id : sensorsIDs)
	{
		SensorDataWithStatus isd;
		isd.status = 0;
		currentData.insert({ id, isd });
	}
}

CostumeIMUExtractor::CostumeIMUExtractor(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration)
	: completeDataFilter(sensorsConfiguration)
{
	auto it = sensorsConfiguration.find(imuCostume::Costume::IMU);

	if (it != sensorsConfiguration.end()){
		for (const auto & id : it->second)
		{
			SensorDataWithStatus isd;
			isd.status = 0;
			currentData.insert({ id, isd });
		}
	}
}

CostumeIMUExtractor::~CostumeIMUExtractor()
{

}

bool CostumeIMUExtractor::verify(const IMU::CostumeStream::value_type & streamData) const
{
	bool ret = false;
	
	for (const auto sd : streamData.sensorsData)
	{
		auto it = currentData.find(sd->id());

		if (it != currentData.end()){

			auto imuData = utils::static_pointer_cast<const imuCostume::Costume::IMUSensor>(sd);

			const auto status = imuData->dataStatus();

			if (status & imuCostume::Costume::IMUSensor::ACC_DATA){
				it->second.accelerometer = imuData->accelerometer();
			}

			if (status & imuCostume::Costume::IMUSensor::GYRO_DATA){
				it->second.gyroscope = imuData->gyroscope();
			}

			if (status & imuCostume::Costume::IMUSensor::MAG_DATA){
				it->second.magnetometer = imuData->magnetometer();
			}

			if (status & imuCostume::Costume::IMUSensor::ORIENT_DATA){
				it->second.orientation = imuData->orientation();
			}

			if((it->second.status |= status) & 0x0F){			
				ret = true;
			}
		}
	}

	return ret;
}

void CostumeIMUExtractor::extract(const IMU::CostumeStream::value_type & streamData, IMU::SensorsStreamData & sensorsData) const
{
	IMU::SensorsData locData;
	//PLUGIN_LOG_DEBUG("CostumeIMUExtractor");
	for (const auto & d : streamData.sensorsData)
	{
		auto it = currentData.find(d->id());

		if (it != currentData.end()){

			if (it->second.status | 0x0F){				

				locData.insert({ d->id(), IMU::SensorData(it->second) });
			}
		}
	}		
	
	sensorsData.timestamp = streamData.timestamp;
	sensorsData.sensorsData.swap(locData);	
}

OrientationEstimator::OrientationEstimator(IMU::CostumeProfilePtr profile)
	: profile(profile)
{

}

void OrientationEstimator::operator()(IMU::SensorsStreamData & data) const
{
	//PLUGIN_LOG_DEBUG("OrientationEstimator");
	for (auto & d : data.sensorsData)
	{
		auto oIT = profile->sensorsDescriptions.find(d.first);

		if (oIT != profile->sensorsDescriptions.end()){

			double deltaT = 0;
			auto tIT = lastUpdateTime.find(d.first);

			if (tIT == lastUpdateTime.end()){
				lastUpdateTime.insert({ d.first, data.timestamp });
			}
			else{				
				deltaT = calculateDelta(data.timestamp, tIT->second);				
				tIT->second = data.timestamp;
			}

			d.second.orientation = oIT->second.orientationEstimationAlgorithm->estimate(d.second.accelerometer, d.second.gyroscope, d.second.magnetometer, deltaT, d.second.orientation);
		}
	}
}

OrientationEstimator::~OrientationEstimator()
{

}

ArrayExtractor::ArrayExtractor(const std::size_t idx)
	: idx(idx)
{

}

ArrayExtractor::ArrayExtractor(const ArrayExtractor & Other)
	: idx(Other.idx)
{

}

ArrayExtractor::~ArrayExtractor()
{

}

//! \param idx Index obiketu który chcemy wypakowywaæ z wektora
CompoundArrayExtractor::CompoundArrayExtractor(const std::size_t idx)
	: idx(idx)
{

}
//! \param Other Inny kopiowany extractor
CompoundArrayExtractor::CompoundArrayExtractor(const CompoundArrayExtractor & Other)
	: idx(Other.idx)
{

}
//! Destruktor
CompoundArrayExtractor::~CompoundArrayExtractor()
{

}

OrientationExtractor::OrientationExtractor(const unsigned int idx)
	: idx(idx)
{

}

OrientationExtractor::OrientationExtractor(const OrientationExtractor & Other)
	: idx(Other.idx)
{

}

OrientationExtractor::~OrientationExtractor()
{

}

bool OrientationExtractor::verify(const IMU::MotionStream::value_type & a) const
{
	return true;
}

void OrientationExtractor::extract(const IMU::MotionStream::value_type & a, osg::Quat & ret) const
{
	ret = a.second.data().orientations[idx];
}

CANopenDataExtractor::CANopenDataExtractor()
{

}

CANopenDataExtractor::~CANopenDataExtractor()
{

}

bool CANopenDataExtractor::verify(const IMU::CANopenFramesStream::value_type & a)
{
	return true;
}

void CANopenDataExtractor::extract(const IMU::CANopenFramesStream::value_type & a, IMU::CostumeStream::value_type & ret) const
{
	//PLUGIN_LOG_ERROR("CANopenDataExtractor");
	ret = imuCostume::Costume::convert(a);
}

ExtractCostumeMotion::ExtractCostumeMotion(
	IMU::CostumeProfilePtr profile)
	: profile(profile),	previousTime(0), skeleton(*profile->skeleton)
{

}

ExtractCostumeMotion::ExtractCostumeMotion(ExtractCostumeMotion&& other)
	: skeleton(std::move(other.skeleton)), profile(std::move(other.profile)),
	previousTime(std::move(other.previousTime))
{

}

ExtractCostumeMotion::~ExtractCostumeMotion()
{

}

bool ExtractCostumeMotion::verify(const IMU::SensorsStreamData & input) const
{
	return true;
}

void ExtractCostumeMotion::extract(const IMU::SensorsStreamData & input, IMU::MotionStream::value_type & output) const
{
	double deltaTime = calculateDelta(input.timestamp, previousTime);	

	if (previousTime == 0)
	{
		deltaTime = 0;
	}

	previousTime = input.timestamp;

	kinematic::SkeletonState::RigidCompleteStateLocal motionState;
	motionState.data().position = osg::Vec3(0, 0, 0);

	std::map<kinematic::LinearizedSkeleton::NodeIDX, osg::Quat> jointsGlobalOrientations;

	for (const auto & i : input.sensorsData)
	{
		auto it = profile->sensorsDescriptions.find(i.first);
		if (it != profile->sensorsDescriptions.end()){
			const auto & sd = it->second;
			//jointsGlobalOrientations[it->second.jointIdx] = i.second.orientation * adj.rotation.inverse();
			jointsGlobalOrientations[it->second.jointIdx] = sd.adjustment.preMulRotation * i.second.orientation * sd.adjustment.postMulRotation;
		}
	}
	
	auto it = jointsGlobalOrientations.begin();

	kinematic::LinearizedSkeleton::Visitor::localIndexedVisitNonLeaf(skeleton,
		[&motionState, &jointsGlobalOrientations, &it]
	(kinematic::Skeleton::JointPtr joint, const kinematic::LinearizedSkeleton::NodeIDX idx)
	{		
		if (it != jointsGlobalOrientations.end()){
			if (it->first == idx){
				joint->value().setGlobal(it->second);
				++it;
			}
		}

		motionState.data().orientations.insert(motionState.data().orientations.end(), joint->value().localOrientation());
	});

	try{
		output.second = profile->motionEstimationAlgorithm->estimate(motionState, input.sensorsData, deltaTime);
		//output.first = (double)previousTime / 1000.0;
		output.first = previousTime;
	}
	catch (...){

	}
}

KinematicStreamExtractor::KinematicStreamExtractor(kinematic::Skeleton && skeleton)
	: skeleton_(std::move(skeleton))
{

}

KinematicStreamExtractor::KinematicStreamExtractor(KinematicStreamExtractor&& other)
	: skeleton_(std::move(other.skeleton_))
{

}

KinematicStreamExtractor::~KinematicStreamExtractor()
{

}

bool KinematicStreamExtractor::verify(const IMU::MotionStream::value_type & input) const
{
	return true;
}

void KinematicStreamExtractor::extract(const IMU::MotionStream::value_type & input, IMU::SkeletonStateStream::value_type & output) const
{
	output.first = input.first;
	skeleton_.root()->value().setGlobal(input.second.data().position);
	IMU::SkeletonStateStream::value_type::second_type locOutput;	

	kinematic::SkeletonState::applyState(skeleton_, input.second);

	kinematic::LinearizedSkeleton::Visitor::visit(skeleton_,
		[&locOutput](kinematic::Skeleton::JointPtr joint)
	{
		locOutput.data().push_back({ joint->value().localPosition(), joint->value().localOrientation() });
	});

	output.second = locOutput;
}

const kinematic::Skeleton & KinematicStreamExtractor::skeleton() const
{
	return skeleton_;
}

RawToCANopenExtractor::RawToCANopenExtractor()
{

}

RawToCANopenExtractor::~RawToCANopenExtractor()
{

}

bool RawToCANopenExtractor::verify(const IMU::RawDataStream::value_type & val) const
{
	return true;
}

void RawToCANopenExtractor::extract(const IMU::RawDataStream::value_type & in, IMU::CANopenFramesStream::value_type & out) const
{
	//PLUGIN_LOG_DEBUG("RawToCANopenExtractor");
	out = imuCostume::CostumeCANopenIO::extractData(in.buffer.get() + sizeof(imuCostume::CostumeCANopenIO::Header), in.length - sizeof(imuCostume::CostumeCANopenIO::Header));
}
