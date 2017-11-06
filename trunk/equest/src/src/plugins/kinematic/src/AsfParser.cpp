#include "PCH.h"
#include <utils/Filesystem.h>
#include <plugins/kinematic/Wrappers.h>
#include <acclaimformatslib/AsfParser.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "AsfParser.h"

AsfParser::AsfParser()
{

}

AsfParser::~AsfParser()
{

}

void AsfParser::parse( const std::string & source )
{
	skeletalModel = utils::ObjectWrapper::create<acclaim::Skeleton>();
	utils::Filesystem::Path path(source);
	std::ifstream file(path.string());	 
	 auto skeleton = acclaim::AsfParser::parse(file, true);
	acclaim::SkeletonPtr modelPtr = utils::make_shared<acclaim::Skeleton>(std::move(skeleton));
    skeletalModel->set(modelPtr);
}

plugin::IParser* AsfParser::create() const
{
    return new AsfParser();
}

void AsfParser::acceptedExpressions(Expressions & expressions) const
{
    plugin::IParser::ExpressionDescription expDesc;
    expDesc.description = "Acclaim Skeleton File format";

	expDesc.objectsTypes.push_back(typeid(acclaim::Skeleton));

    expressions[".*\\.asf$"] = expDesc;
}

void AsfParser::reset()
{
	skeletalModel.reset();
}

void AsfParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(skeletalModel);
}