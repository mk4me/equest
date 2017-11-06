#include "DicomPCH.h"
#include <fstream>
#include <plugins/dicom/Annotations.h>
#include <boost/serialization/shared_ptr.hpp>
#include "qstring_serialization.h"

dicom::annotations::BloodLevelsTypePtr dicom::annotations::getDefaultBloodLevels()
{
	BloodLevelsTypePtr adn = utils::make_shared<BloodLevelsType>();
	adn->insert(BloodLevelsType::value_type(unknownBloodLevel, QObject::tr("unknown blood level"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel0, QObject::tr("level 0"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel1, QObject::tr("level 1"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel2, QObject::tr("level 2"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel3, QObject::tr("level 3"))); 	
	return adn;
}

dicom::annotations::BloodLevelsTypePtr dicom::annotations::instanceBloodLevels()
{
	return getDefaultBloodLevels();
}

dicom::annotations::InflammatoryLevelsTypePtr dicom::annotations::getDefaultInflammatoryLevels()
{
	InflammatoryLevelsTypePtr adn = utils::make_shared<InflammatoryLevelsType>();
	adn->insert(InflammatoryLevelsType::value_type(unknownInflammatoryLevel, QObject::tr("unknown inflammatory level"))); 
	adn->insert(InflammatoryLevelsType::value_type(inflammatoryLevel0, QObject::tr("level 0"))); 
	adn->insert(InflammatoryLevelsType::value_type(inflammatoryLevel1, QObject::tr("level 1"))); 
	adn->insert(InflammatoryLevelsType::value_type(inflammatoryLevel2, QObject::tr("level 2"))); 
	adn->insert(InflammatoryLevelsType::value_type(inflammatoryLevel3, QObject::tr("level 3")));	

	return adn;
}

dicom::annotations::InflammatoryLevelsTypePtr dicom::annotations::instanceInflammatoryLevels()
{
	return getDefaultInflammatoryLevels();
}

dicom::annotations::FingerTypePtr dicom::annotations::getDefaultFingerTypes()
{
	FingerTypePtr adn = utils::make_shared<FingerType>();
	adn->insert(FingerType::value_type(unknownFinger, QObject::tr("unknown finger"))); 
	adn->insert(FingerType::value_type(finger1, QObject::tr("Finger 1"))); 
	adn->insert(FingerType::value_type(finger2, QObject::tr("Finger 2"))); 
	adn->insert(FingerType::value_type(finger3, QObject::tr("Finger 3"))); 
	adn->insert(FingerType::value_type(finger4, QObject::tr("Finger 4")));	
	adn->insert(FingerType::value_type(finger5, QObject::tr("Finger 5")));	

	return adn;
}

dicom::annotations::FingerTypePtr dicom::annotations::instanceFingerTypes()
{
	return getDefaultFingerTypes();
}

dicom::annotations::JointTypePtr dicom::annotations::getDefaultJointTypes()
{
	JointTypePtr adn = utils::make_shared<JointType>();
	adn->insert(JointType::value_type(unknownJoint, QObject::tr("unknown joint"))); 
	adn->insert(JointType::value_type(LPIP, QObject::tr("LPIP"))); 
	adn->insert(JointType::value_type(RPIP, QObject::tr("RPIP"))); 
	adn->insert(JointType::value_type(LMCP, QObject::tr("LMCP"))); 
	adn->insert(JointType::value_type(RMCP, QObject::tr("RMCP")));

	return adn;
}

dicom::annotations::JointTypePtr dicom::annotations::instanceJointTypes()
{
	return getDefaultJointTypes();
}

dicom::annotations::ImageTypePtr dicom::annotations::getDefaultImageTypes()
{
	ImageTypePtr adn = utils::make_shared<ImageType>();
	adn->insert(ImageType::value_type(incorrect, QObject::tr("incorrect")));	
	adn->insert(ImageType::value_type(different, QObject::tr("different"))); 
	adn->insert(ImageType::value_type(identical, QObject::tr("identical"))); 

	return adn;
}

dicom::annotations::ImageTypePtr dicom::annotations::instanceImageTypes()
{
	return getDefaultImageTypes();
}

dicom::annotations::AdnotationsTypePtr dicom::annotations::getDefault()
{
    AdnotationsTypePtr adn(new AdnotationsType);
    adn->insert(AdnotationsType::value_type(unknownAnnotation, QObject::tr("unknown annotation"))); 
    adn->insert(AdnotationsType::value_type(otherAnnotation, QObject::tr("other"))); 
    adn->insert(AdnotationsType::value_type(skin, QObject::tr("skin"))); 
    adn->insert(AdnotationsType::value_type(bone, QObject::tr("bone"))); 
    adn->insert(AdnotationsType::value_type(tendon, QObject::tr("tendon"))); 
    adn->insert(AdnotationsType::value_type(joint, QObject::tr("joint"))); 
    adn->insert(AdnotationsType::value_type(inflammatory, QObject::tr("region inflammatory synovitis degree of inflammation / hyperplasia"))); 
    adn->insert(AdnotationsType::value_type(intensity, QObject::tr("the degree of intensity/ hyperaemia")));
    adn->insert(AdnotationsType::value_type(noise, QObject::tr("noise")));
	adn->insert(AdnotationsType::value_type(bloodLevel, QObject::tr("blood level")));
	adn->insert(AdnotationsType::value_type(inflammatoryLevel, QObject::tr("inflammatory level")));
	adn->insert(AdnotationsType::value_type(fingerType, QObject::tr("finger type")));
	adn->insert(AdnotationsType::value_type(jointType, QObject::tr("joint type")));
	adn->insert(AdnotationsType::value_type(imageType, QObject::tr("image status")));
	
    return adn;
}

dicom::annotations::AdnotationsTypePtr dicom::annotations::load( const utils::Filesystem::Path& p )
{
    AdnotationsTypePtr adnotations;
    std::ifstream ifs(p.c_str());
    if (ifs.good()) {
        adnotations = AdnotationsTypePtr(new AdnotationsType);
        boost::archive::xml_iarchive xmlIn(ifs);
        xmlIn >> BOOST_SERIALIZATION_NVP(adnotations);
        ifs.close();
    }

    return adnotations;
}

void dicom::annotations::save( const utils::Filesystem::Path& p, AdnotationsTypeConstPtr adnotations )
{
    std::ofstream ofs(p.c_str());
    if(ofs.good()) {
        boost::archive::xml_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(adnotations);
        ofs.close();
    }
}

dicom::annotations::AdnotationsTypePtr dicom::annotations::instance()
{
	static annotations::AdnotationsTypePtr adn;

	if (!adn) {
		adn = annotations::load(adnotationsFile());
		if (!adn) {
			adn = annotations::getDefault();
		}
	}

	return adn;

}

void dicom::annotations::autoSave()
{
    save(adnotationsFile(), instance());
}

utils::Filesystem::Path dicom::annotations::adnotationsFile()
{
    return plugin::getUserApplicationDataPath("DicomAdnotations.xml");
}
