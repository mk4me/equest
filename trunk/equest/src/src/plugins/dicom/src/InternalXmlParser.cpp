#include "DicomPCH.h"
#include "InternalXmlParser.h"
#include <utils/Filesystem.h>
#include "LayeredImage.h"
#include <plugins/dicom/Dicom.h>
#include "DicomService.h"
#include "DicomInternalStruct.h"
#include <regex>
#include "DicomLoader.h"

using namespace dicom;

InternalXmlParser::InternalXmlParser()
{
    
}

InternalXmlParser::~InternalXmlParser()
{

}


void InternalXmlParser::parse( const std::string & source  )
{
	sessionInternal = utils::ObjectWrapper::create<DicomInternalStruct>();
    DicomInternalStructPtr inter = dicom::DicomLoader::load(source);
	sessionInternal->set(inter);
    //LayeredImagePtr l = utils::make_shared<LayeredImage>(source);
    //image->set(l);
}

plugin::IParser* InternalXmlParser::create() const
{
    return new InternalXmlParser();
}

void InternalXmlParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "Internal Dicom Xml";

    expDesc.objectsTypes.push_back(typeid(DicomInternalStruct));
    expressions.insert(Expressions::value_type(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}\\.xml$", expDesc));

    //std::cmatch cm;
    //bool test = std::regex_match("C:\\Users\\Wojciech\\Documents\\PJWSTK\\MDE\\tmp\\hmdbCommunication\\data\\zxl2rjizxf56pzxl2rjizxf7i\\data\\2013-08-21-S0007\\2013-08-21-S0007.xml", cm,std::regex(".*[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}\.xml$"));
}

void InternalXmlParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(sessionInternal);	
}

void InternalXmlParser::reset()
{
	sessionInternal.reset();
}


