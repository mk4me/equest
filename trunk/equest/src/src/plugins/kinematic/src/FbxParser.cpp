#include "PCH.h"
#include "FbxParser.h"
#include <utils/Filesystem.h>
#include <plugins/kinematic/Wrappers.h>


FbxParser::FbxParser()
{

}

FbxParser::~FbxParser()
{

}


void FbxParser::parse(const std::string & source)
{
	fbxWrapper = utils::ObjectWrapper::create<osg::PositionAttitudeTransform>();
	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(source);
	osg::ref_ptr<osg::PositionAttitudeTransform> paModel = new osg::PositionAttitudeTransform;
	paModel->addChild(model);
	fbxWrapper->set(paModel);
}

void FbxParser::reset()
{
	fbxWrapper.reset();
}

plugin::IParser* FbxParser::create() const
{
	return new FbxParser();
}

void FbxParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "Fbx format";

	expDesc.objectsTypes.push_back(typeid(osg::PositionAttitudeTransform));
	expressions.insert(Expressions::value_type(".*\\.fbx$", expDesc));
}

void FbxParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(fbxWrapper);
}


