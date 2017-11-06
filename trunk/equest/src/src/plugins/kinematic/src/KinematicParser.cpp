#include "PCH.h"
#include <utils/Filesystem.h>
#include "KinematicParser.h"
#include <plugins/kinematic/Wrappers.h>
#include <acclaimformatslib/AmcParser.h>
#include <acclaimformatslib/MotionData.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "acclaimformatslib/AsfParser.h"


KinematicParser::KinematicParser()
{

}

KinematicParser::~KinematicParser()
{

}

void KinematicParser::parse( const std::string & source)
{
	skeletonData = utils::ObjectWrapper::create<acclaim::MotionData>();
	utils::Filesystem::Path path(source);
	using namespace kinematic;
	using acclaim::AsfParser;

	acclaim::MotionDataPtr dataPtr = utils::make_shared<acclaim::MotionData>();

	//if(utils::Filesystem::fileExtension(path).compare(".amc") == 0) {		
		std::ifstream amcFile(path.string());
		dataPtr->frames = acclaim::AmcParser::parse(amcFile);
				
	//} 
	// bvh chwilowo wylaczone
	/*else if (utils::Filesystem::fileExtension(path).compare(".bvh") == 0)  {
		BvhParser bvh;
		bvh.parse(modelPtr, dataPtr, path.string());
	}*/
	
	if (  dataPtr && !dataPtr->frames.empty()) {
		skeletonData->set(dataPtr);
	}
}

void KinematicParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(skeletonData);
}

void KinematicParser::reset()
{
	skeletonData.reset();
}

plugin::IParser* KinematicParser::create() const
{
    return new KinematicParser();
}

void KinematicParser::acceptedExpressions(Expressions & extensions) const
{
    plugin::IParser::ExpressionDescription expDesc;

    expDesc.description = "Acclaim Motion Capture format";
	expDesc.objectsTypes.push_back(typeid(acclaim::MotionData));
    extensions[".*\\.amc$"] = expDesc;

    //expDesc.description = "Biovision Hierarchical Data format";
    //extensions["bvh"] = expDesc;
}