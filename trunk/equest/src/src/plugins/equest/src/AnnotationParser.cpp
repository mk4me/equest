#include "EquestPCH.h"
#include "AnnotationParser.h"
#include <utils/Filesystem.h>
#include <boost/make_shared.hpp>
#include "EquestAnnotation.h"
#include "utils/SmartPtr.h"
#include "plugins/equest/Equest.h"
using namespace equest;

AnnotationParser::AnnotationParser()
{

}

AnnotationParser::~AnnotationParser()
{

}


void AnnotationParser::parse( const std::string & source  )
{
	annotationsOW = utils::ObjectWrapper::create<EquestAnnotations>();
	equest::EquestAnnotationsPtr ptr = utils::make_shared<equest::EquestAnnotations>(equest::EquestAnnotations::load(source));
	annotationsOW->set(ptr);
}

void AnnotationParser::reset()
{
	annotationsOW.reset();
}

plugin::IParser* AnnotationParser::create() const
{
    return new AnnotationParser();
}

void AnnotationParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "Annotation format";
	expDesc.objectsTypes.push_back(typeid(EquestAnnotations));
    expressions.insert(Expressions::value_type(".*\\.xml$", expDesc));
}

void AnnotationParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(annotationsOW);
}


