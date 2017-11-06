#include "DicomPCH.h"
#include "DicomLoader.h"
#include <fstream>
#include <boost/archive/xml_iarchive.hpp>
#include "DicomInternalStruct.h"
#include "BackgroundLayer.h"
#include "PointsLayer.h"


namespace dicom {

LayersVectorPtr DicomLoader::loadLayers(const utils::Filesystem::Path &p)
{
	std::ifstream ifs(p.string());
	boost::archive::xml_iarchive xmlIn(ifs);

	xmlIn.register_type<BackgroundLayer>();
	//xmlIn.register_type<CircleLayer>();
	xmlIn.register_type<PointsLayer>();
	xmlIn.register_type<BloodLevelLayer>();
	xmlIn.register_type<InflammatoryLevelLayer>();
	xmlIn.register_type<FingerTypeLayer>();
	xmlIn.register_type<JointTypeLayer>();
	xmlIn.register_type<ImageQualityLayer>();
	LayersVectorPtr layers = boost::make_shared<LayersVector>();
	//xmlIn >> BOOST_SERIALIZATION_NVP(layers);
	xmlIn >> boost::serialization::make_nvp("layers", *layers);
	ifs.close();

	return layers;
}

DicomInternalStructPtr DicomLoader::load(const utils::Filesystem::Path& from)
{
	DicomInternalStructPtr inter = boost::make_shared<DicomInternalStruct>();
	std::ifstream ifs(from.c_str());
	if (ifs.good()) {
		boost::archive::xml_iarchive xmlIn(ifs);
		xmlIn >> BOOST_SERIALIZATION_NVP(inter);
		ifs.close();
	}

	return inter;
}

}
