#include "PCH.h"
#include <plugins/imuCostume/CostumeProfile.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <boost/lexical_cast.hpp>

using namespace IMU;

Skeleton::Skeleton(const Skeleton & skeleton)
	: kinematic::Skeleton(skeleton), id(skeleton.id), name(skeleton.name)
{

}

Skeleton::Skeleton(Skeleton && skeleton)
	: kinematic::Skeleton(skeleton), id(skeleton.id), name(std::move(skeleton.name))
{
	
}

Skeleton::Skeleton(const core::UniqueID id, const std::string & name,
	const kinematic::Skeleton & skeleton) : kinematic::Skeleton(skeleton),
	id(id), name(name)
{

}

Skeleton::Skeleton(const core::UniqueID id, const std::string & name,
	kinematic::Skeleton && skeleton) : kinematic::Skeleton(std::move(skeleton)),
	id(id), name(name)
{

}

Skeleton::~Skeleton()
{

}

const core::UniqueID Skeleton::ID() const
{
	return id;
}

CostumeProfile * CostumeProfile::clone() const
{
	std::unique_ptr<CostumeProfile> ret(new CostumeProfile);

	ret->name = name;

	if (skeleton != nullptr){
		ret->skeleton.reset(new Skeleton(*skeleton));
	}

	if (calibrationAlgorithm != nullptr){
		ret->calibrationAlgorithm.reset(calibrationAlgorithm->create());
	}

	if (motionEstimationAlgorithm != nullptr){
		ret->motionEstimationAlgorithm.reset(motionEstimationAlgorithm->create());
	}

	for (const auto & sd : sensorsDescriptions)
	{
		SensorDescription locsd;
		locsd.jointName = sd.second.jointName;
		locsd.jointIdx = sd.second.jointIdx;
		locsd.adjustment.offset = sd.second.adjustment.offset;
		locsd.adjustment.preMulRotation = sd.second.adjustment.preMulRotation;
		locsd.adjustment.postMulRotation = sd.second.adjustment.postMulRotation;
		if (sd.second.orientationEstimationAlgorithm != nullptr){
			locsd.orientationEstimationAlgorithm.reset(sd.second.orientationEstimationAlgorithm->create());
		}

		ret->sensorsDescriptions.insert(SensorsDescriptions::value_type(sd.first, locsd));
	}

	ret->activeJoints = activeJoints;

	return ret.release();
}

SerializableCostumeProfile SerializableCostumeProfile::pack(const CostumeProfile & profile)
{
	SerializableCostumeProfile ret;
	ret.name = profile.name;
	ret.skeletonID = profile.skeleton == nullptr ? boost::uuids::nil_uuid() : profile.skeleton->ID();
	ret.calibrationAlgorithmID = profile.calibrationAlgorithm == nullptr ? boost::uuids::nil_uuid() : profile.calibrationAlgorithm->ID();
	ret.motionEstimationAlgorithmID = profile.motionEstimationAlgorithm == nullptr ? boost::uuids::nil_uuid() : profile.motionEstimationAlgorithm->ID();

	for (const auto & sd : profile.sensorsDescriptions)
	{
		SensorDescription local;
		local.jointName = sd.second.jointName;
		local.offset = sd.second.adjustment.offset;
		local.preMulRotation = sd.second.adjustment.preMulRotation;
		local.postMulRotation = sd.second.adjustment.postMulRotation;
		local.orientationEstimationAlgorithmID = sd.second.orientationEstimationAlgorithm == nullptr ? boost::uuids::nil_uuid() : sd.second.orientationEstimationAlgorithm->ID();
		ret.sensorsDescriptions.insert(SensorsDescriptions::value_type(sd.first, local));
	}

	return ret;
}

CostumeProfile SerializableCostumeProfile::unpack(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds)
{
	auto skeletons = ds->skeletonModels();
	auto motionEstimationAlgos = ds->motionEstimationAlgorithms();
	auto calibrationAlgos = ds->calibrationAlgorithms();
	auto orientationEstimationAlgos = ds->orientationEstimationAlgorithms();


	CostumeProfile ret;
	ret.name = profile.name;

	kinematic::LinearizedSkeleton::LocalMapping localMapping;

	//szkielet
	{
		auto sIT = skeletons.find(profile.skeletonID);

		if (sIT != skeletons.end()){
			ret.skeleton = utils::make_shared < IMU::Skeleton >(*sIT->second);
			localMapping = kinematic::LinearizedSkeleton::createNonLeafMapping(*ret.skeleton);
		}
	}

	//motion estimation algorithm
	{
		auto mIT = motionEstimationAlgos.find(profile.motionEstimationAlgorithmID);

		if (mIT != motionEstimationAlgos.end()){
			ret.motionEstimationAlgorithm.reset(mIT->second->create());
		}
	}

	//kalibracja
	{
		auto cIT = calibrationAlgos.find(profile.calibrationAlgorithmID);

		if (cIT != calibrationAlgos.end()){
			ret.calibrationAlgorithm.reset(cIT->second->create());
		}
	}

	IMU::IMUCostumeCalibrationAlgorithm::SensorsDescriptions sds;	

	//konfiguracja sensorów
	for (const auto & ea : profile.sensorsDescriptions)
	{
		CostumeProfile::SensorDescription local;
		local.jointName = ea.second.jointName;
		local.jointIdx = -1;

		auto it = localMapping.data().right.find(local.jointName);

		if (it != localMapping.data().right.end()){
			local.jointIdx = it->get_left();
		}
		
		local.adjustment.offset = ea.second.offset;
		local.adjustment.preMulRotation = ea.second.preMulRotation;
		local.adjustment.postMulRotation = ea.second.postMulRotation;

		auto eIT = orientationEstimationAlgos.find(ea.second.orientationEstimationAlgorithmID);
		if (eIT != orientationEstimationAlgos.end()){
			local.orientationEstimationAlgorithm.reset(eIT->second->create());
		}

		ret.sensorsDescriptions.insert(CostumeProfile::SensorsDescriptions::value_type(ea.first, local));
		sds.insert({ ea.first, local });
	}

	if (ret.motionEstimationAlgorithm != nullptr){

		ret.activeJoints = ret.motionEstimationAlgorithm->activeJoints(ret.skeleton, sds);

	}

	return ret;
}

SerializableCostumeProfile::VerificationStatus SerializableCostumeProfile::verify(const SerializableCostumeProfile & profile, IMU::IIMUDataSource * ds)
{
	if (profile.name.empty() == true){
		return Failed;
	}

	if (profile.sensorsDescriptions.empty() == true
		|| profile.skeletonID.is_nil() == true
		|| profile.calibrationAlgorithmID.is_nil() == true
		|| profile.motionEstimationAlgorithmID.is_nil() == true){

		return Incomplete;
	}	

	{
		auto skeletons = ds->skeletonModels();
		auto sIT = skeletons.find(profile.skeletonID);

		if (sIT == skeletons.end()){
			return Incomplete;
		}
	}

	{
		auto motionEstimationAlgos = ds->motionEstimationAlgorithms();
		auto mIT = motionEstimationAlgos.find(profile.motionEstimationAlgorithmID);

		if (mIT == motionEstimationAlgos.end()){
			return Incomplete;
		}
	}

	{
		auto calibrationAlgos = ds->calibrationAlgorithms();
		auto cIT = calibrationAlgos.find(profile.calibrationAlgorithmID);

		if (cIT == calibrationAlgos.end()){
			return Incomplete;
		}
	}

	auto orientationEstimationAlgos = ds->orientationEstimationAlgorithms();

	for (const auto & ea : profile.sensorsDescriptions)
	{
		auto eIT = orientationEstimationAlgos.find(ea.second.orientationEstimationAlgorithmID);

		if (eIT == orientationEstimationAlgos.end()){
			return Incomplete;
		}
	}

	return Ok;
}

void SerializableCostumeProfile::serialize(std::ostream & stream, const SerializableCostumeProfile & sProfile)
{
	stream << "ProfileName: " << sProfile.name << std::endl;
	stream << "SkeletonID: " << sProfile.skeletonID << std::endl;
	stream << "SensorsConfiguration: " << sProfile.sensorsDescriptions.size() << std::endl;

	for (const auto & m : sProfile.sensorsDescriptions)
	{		
		std::string sID = std::to_string(m.first);

		stream << sID << ": " << m.second.jointName << " " << m.second.offset[0] << " " << m.second.offset[1] << " " << m.second.offset[2]
			<< " " << m.second.preMulRotation[0] << " " << m.second.preMulRotation[1] << " " << m.second.preMulRotation[2] << " " << m.second.preMulRotation[3]
			<< " " << m.second.postMulRotation[0] << " " << m.second.postMulRotation[1] << " " << m.second.postMulRotation[2] << " " << m.second.postMulRotation[3]
			<< " " << m.second.orientationEstimationAlgorithmID << std::endl;
	}

	stream << "CalibrationAlgoID: " << sProfile.calibrationAlgorithmID << std::endl;
	stream << "EstimationAlgoID: " << sProfile.motionEstimationAlgorithmID;
}

SerializableCostumeProfile SerializableCostumeProfile::deserialize(std::istream & stream)
{
	SerializableCostumeProfile ret;

	std::set<std::string> profileTokens;

	profileTokens.insert("ProfileName");
	profileTokens.insert("SkeletonID");	
	profileTokens.insert("SensorsConfiguration");
	profileTokens.insert("CalibrationAlgoID");
	profileTokens.insert("EstimationAlgoID");

	std::string line;

	bool finish = false;

	while (finish == false){

		if (profileTokens.empty() == true){
			break;
		}

		std::getline(stream, line);

		auto pos = line.find(':');

		if (pos != std::string::npos){
			auto token = line.substr(0, pos);

			if (profileTokens.find(token) == profileTokens.end()){
				finish = true;
			}
			else{

				std::string valueToExtract = line.substr(pos + 2);

				if (valueToExtract.empty() == true){
					finish = true;
				}
				else{

					profileTokens.erase(token);

					finish = profileTokens.empty();

					if (token == "ProfileName"){
						ret.name = valueToExtract;
					}
					else if (token == "SkeletonID"){
						std::stringstream ss;
						ss << valueToExtract;
						ss >> ret.skeletonID;
					}
					else if (token == "SensorsConfiguration"){
						unsigned int count = 0;
						std::stringstream ss;
						ss << valueToExtract;
						ss >> count;

						for (unsigned int i = 0; i < count; ++i){
							std::getline(stream, line);
							pos = line.find(':');

							if (pos != std::string::npos){
								token = line.substr(0, pos);
								imuCostume::Costume::SensorID sID = std::stoul(token);

								valueToExtract = line.substr(pos + 2);
								std::stringstream ss;
								ss << valueToExtract;
								SensorDescription sd;
								
								ss >> sd.jointName;
								ss >> sd.offset[0];
								ss >> sd.offset[1];
								ss >> sd.offset[2];
								ss >> sd.preMulRotation[0];
								ss >> sd.preMulRotation[1];
								ss >> sd.preMulRotation[2];
								ss >> sd.preMulRotation[3];
								ss >> sd.postMulRotation[0];
								ss >> sd.postMulRotation[1];
								ss >> sd.postMulRotation[2];
								ss >> sd.postMulRotation[3];
								ss >> sd.orientationEstimationAlgorithmID;

								ret.sensorsDescriptions.insert(SensorsDescriptions::value_type(sID, sd));
							}
							else{
								finish = true;
								break;
							}
						}
					}
					else if (token == "CalibrationAlgoID"){
						std::stringstream ss;
						ss << valueToExtract;
						ss >> ret.calibrationAlgorithmID;
					}
					else if (token == "EstimationAlgoID"){
						std::stringstream ss;
						ss << valueToExtract;
						ss >> ret.motionEstimationAlgorithmID;						
					}

					//std::getline(stream, line);
				}
			}
		}
		else{
			finish = true;
		}
	}

	return ret;
}