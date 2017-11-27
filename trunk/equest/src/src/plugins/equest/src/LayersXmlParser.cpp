#include "EquestPCH.h"
#include "LayersXmlParser.h"
#include "LayeredImage.h"
#include <utils/Filesystem.h>
#include <plugins/dicom/Dicom.h>
#include "DicomService.h"
#include "DicomLoader.h"

using namespace dicom;

LayersXmlParser::LayersXmlParser()
{

}

LayersXmlParser::~LayersXmlParser()
{

}


void LayersXmlParser::parse( const std::string & source  )
{
	layers = utils::ObjectWrapper::create<LayersVector>();
    LayersVectorPtr layers = dicom::DicomLoader::loadLayers(source);
    this->layers->set(layers);
}

plugin::IParser* LayersXmlParser::create() const
{
    return new LayersXmlParser();
}

void LayersXmlParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "Layers Xml";

	expDesc.objectsTypes.push_back(typeid(LayersVector));
    expressions.insert(Expressions::value_type(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}-T[0-9]{4}\\.xml$", expDesc));
    expressions.insert(Expressions::value_type(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}-T[0-9]{4}\\..*\\.xml$", expDesc));
}

void LayersXmlParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(layers);	
}

void LayersXmlParser::reset()
{
	layers.reset();
}


