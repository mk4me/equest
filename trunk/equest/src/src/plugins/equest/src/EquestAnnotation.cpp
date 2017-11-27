#include "EquestPCH.h"
#include "EquestAnnotation.h"
#include "boost/serialization/level.hpp"

using namespace equest;
BOOST_CLASS_IMPLEMENTATION(EquestAnnotations, object_serializable);
BOOST_CLASS_IMPLEMENTATION(EquestAnnotations::Entry, object_serializable)

void equest::EquestAnnotations::save(const utils::Filesystem::Path& path, const EquestAnnotations& annotations)
{
	unsigned int flags = boost::archive::no_header | boost::archive::no_codecvt | boost::archive::no_xml_tag_checking | boost::archive::no_tracking;
	std::ofstream ofs(path.string());
	boost::archive::xml_oarchive oa(ofs, flags);
	oa << BOOST_SERIALIZATION_NVP(annotations);
}

equest::EquestAnnotations equest::EquestAnnotations::load(const utils::Filesystem::Path& path)
{
	EquestAnnotations annotations;
	unsigned int flags = boost::archive::no_header | boost::archive::no_codecvt | boost::archive::no_xml_tag_checking | boost::archive::no_tracking;
	std::ifstream ifs(path.string());
	boost::archive::xml_iarchive ia(ifs, flags);
	ia >> BOOST_SERIALIZATION_NVP(annotations);
	return annotations;
}
