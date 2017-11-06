#include "PCH.h"
#include <utils/Filesystem.h>
#include <plugins/kinematic/Wrappers.h>
//#include <kinematiclib/SkeletalModel.h>
//#include <kinematiclib/SkeletalParsers.h>
//#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "BvhParser.h"

BvhParser::BvhParser()
{

}

BvhParser::~BvhParser()
{

}

void BvhParser::parse( const std::string & source )
{
	//skeletalModel = utils::ObjectWrapper::create<BVHData>();
	//utils::Filesystem::Path path(source);  
 //   kinematic::SkeletalModelPtr modelPtr = utils::make_shared<kinematic::SkeletalModel>();
	//kinematic::SkeletalDataPtr dataPtr = utils::make_shared<kinematic::SkeletalData>();
 //   kinematic::BvhParser bvh;
 //   bvh.parse(modelPtr, dataPtr, path.string());

	//auto pairPtr = utils::make_shared<BVHData>(std::make_pair(modelPtr, dataPtr));
	//skeletalModel->set(pairPtr);
	
}

plugin::IParser* BvhParser::create() const
{
    return new BvhParser();
}

void BvhParser::acceptedExpressions(Expressions & expressions) const
{
 //   plugin::IParser::ExpressionDescription expDesc;
 //   expDesc.description = "Biovision Skeleton File format";
	//expDesc.objectsTypes.push_back(typeid(BVHData));
 //   expressions[".*\\.bvh$"] = expDesc;
}

void BvhParser::reset()
{
//	skeletalModel.reset();
}

void BvhParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	//object.set(skeletalModel);
}