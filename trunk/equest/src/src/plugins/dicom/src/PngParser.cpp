#include "DicomPCH.h"
#include "PngParser.h"
#include <utils/Filesystem.h>
#include "LayeredImage.h"
#include <plugins/dicom/Dicom.h>
#include <boost/make_shared.hpp>

using namespace dicom;

PngParser::PngParser()
{

}

PngParser::~PngParser()
{

}


void PngParser::parse( const std::string & source  )
{
	image = utils::ObjectWrapper::create<LayeredImage>();
    LayeredImagePtr l = boost::make_shared<LayeredImage>(source);
    image->set(l);
}

void PngParser::reset()
{
	image.reset();
}

plugin::IParser* PngParser::create() const
{
    return new PngParser();
}

void PngParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "Png format";

	expDesc.objectsTypes.push_back(typeid(LayeredImage));
    expressions.insert(Expressions::value_type(".*\\.png$", expDesc));
}

void PngParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(image);	
}


