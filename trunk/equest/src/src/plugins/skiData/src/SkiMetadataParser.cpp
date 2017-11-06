#include "SkiMetadataParser.h"
#include "SkiMetadata.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <tinyxml2.h>

using namespace skidata;

SkiMetadataParser::SkiMetadataParser()
{

}

SkiMetadataParser::~SkiMetadataParser()
{

}

void SkiMetadataParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "Ski metadata format";
	expDesc.objectsTypes.push_back(typeid(SkiMetadata));
	expressions.insert(Expressions::value_type(".*\\.xml$", expDesc));
}

plugin::IParser* SkiMetadataParser::create() const
{
	return new SkiMetadataParser();
}

void SkiMetadataParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(data);
}

void SkiMetadataParser::reset()
{
	data.reset();
}

MoviesDelays unpackMoviesDelays(const tinyxml2::XMLElement * element)
{
	MoviesDelays ret;

	auto md_element = element->FirstChildElement("MovieDelay");
	while (md_element != nullptr) {
		std::string file;
		float delay;
		bool ok = true;
		auto e = md_element->FirstChildElement("File");
		if (e != nullptr) {
			file = std::string(e->GetText());
		}
		else {
			ok = false;
		}

		e = md_element->FirstChildElement("Delay");
		if (e != nullptr) {
			std::string val(e->GetText());
			boost::replace_all(val, ",", ".");
			delay = boost::lexical_cast<float>(val);
		}
		else {
			ok = false;
		}

		if (ok == true) {
			ret.insert({ file, delay });
		}

		md_element = md_element->NextSiblingElement();
	}

	return ret;
}

void SkiMetadataParser::parse(const std::string & source)
{
	tinyxml2::XMLDocument xmlDocument;

	if (xmlDocument.LoadFile(source.c_str())) {
		throw std::runtime_error("Blad wczytania pliku metadanych dla nart");
	}

	auto element = xmlDocument.FirstChildElement("SkiMetadataRoot");
	if (element == nullptr) {
		throw std::runtime_error("B³êdna struktura metadanych");
	}

	bool ok = false;
	SkiMetadata localData;
	localData.dataFrequency = 50.0f;
	localData.samplesCount = 0;
	
	auto e = element->FirstChildElement("Name");
	if (e != nullptr) {	
		localData.name = std::string(e->GetText());
		ok = true;
	}

	e = element->FirstChildElement("Description");
	if (e != nullptr) {
		localData.description = std::string(e->GetText());
		ok = true;
	}

	e = element->FirstChildElement("Frequency");
	if (e != nullptr) {
		std::string val(e->GetText());
		boost::replace_all(val, ",", ".");
		localData.dataFrequency = boost::lexical_cast<float>(val);
		ok = true;
	}

	e = element->FirstChildElement("SamplesCount");
	if (e != nullptr) {		
		localData.samplesCount = boost::lexical_cast<std::size_t>(std::string(e->GetText()));
		if (localData.samplesCount > 0) {
			ok = true;
		}
	}

	e = element->FirstChildElement("MoviesDelays");

	if(e != nullptr){
		localData.moviesDelays = unpackMoviesDelays(e);
		ok = true;
	}

	if (ok == true) {
		this->data = utils::ObjectWrapper::wrap(utils::make_shared<SkiMetadata>(localData));
	}
}