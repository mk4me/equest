#include "CorePCH.h"
#include "ParserManager.h"
#include "RegisteredDataTypesManager.h"
#include "ApplicationCommon.h"
#include <loglib/Exceptions.h>
#include <boost/lexical_cast.hpp>

using namespace core;

void ParserManager::parsers(ParserPrototypes & parserPrototypes) const
{
	for (auto it = parsers_.begin(); it != parsers_.end(); ++it){
		parserPrototypes.push_back(it->second.parser);
	}
}

const bool ParserManager::verifyExpression(const std::string & source,
	const std::regex & expression)
{
	bool ret = false;
	std::cmatch what;
	if (std::regex_match(source.c_str(), what, expression)){
		ret = true;
	}

	return ret;
}

const bool ParserManager::verifyParserExpressions(const std::string & source,
	const ParserManager::Expressions & expressions)
{
	bool ret = false;
	for (auto eIT = expressions.begin(); eIT != expressions.end(); ++eIT){
		if (verifyExpression(source, eIT->second.regularExpression) == true){
			ret = true;
			break;
		}
	}

	return ret;
}

void ParserManager::parsers(const std::string & source, ParserPrototypes & parserPrototypes) const
{
	for (auto it = parsers_.begin(); it != parsers_.end(); ++it){
		if (verifyParserExpressions(source, it->second.expressions) == true){
			parserPrototypes.push_back(it->second.parser);
		}
	}
}

void ParserManager::sourceParsers(const std::string & source, ParserPrototypes & parserPrototypes) const
{
	for (auto it = parsers_.begin(); it != parsers_.end(); ++it){
		if (it->second.sourceParser && verifyParserExpressions(source, it->second.expressions) == true){
			parserPrototypes.push_back(it->second.parser);
		}
	}
}

void ParserManager::streamParsers(const std::string & source, ParserPrototypes & parserPrototypes) const
{
	for (auto it = parsers_.begin(); it != parsers_.end(); ++it){
		if (it->second.streamParser && verifyParserExpressions(source, it->second.expressions) == true){
			parserPrototypes.push_back(it->second.parser);
		}
	}
}

const bool ParserManager::sourceIsAccepted(const std::string & source) const
{
	for (auto it = parsers_.begin(); it != parsers_.end(); ++it){
		if (verifyParserExpressions(source, it->second.expressions) == true){
			return true;
		}
	}

	return false;
}

void ParserManager::sourcePossibleTypes(const std::string & source, utils::TypeInfoSet & types) const
{
	for (auto it = parsers_.begin(); it != parsers_.end(); ++it){
		for (auto eIT = it->second.expressions.begin(); eIT != it->second.expressions.end(); ++eIT){
			if (verifyExpression(source, eIT->second.regularExpression) == true){
				types.insert(eIT->second.supportedTypes.begin(), eIT->second.supportedTypes.end());
			}
		}
	}
}

const ParserManager::TypesMap ParserManager::parserTypes(const UniqueID parserID, const std::string & source) const
{
	auto it = parsers_.find(parserID);

	if (it == parsers_.end()){
		return TypesMap();
	}

	for (auto eIT = it->second.expressions.begin(); eIT != it->second.expressions.end(); ++eIT){
		if (verifyExpression(source, eIT->second.regularExpression) == true){
			return eIT->second.typesMap;
		}
	}

	return TypesMap();
}

void ParserManager::registerParser(const plugin::IParserPtr & parser)
{
	//! Weryfikujemy ID parsera
	UTILS_ASSERT((parser != nullptr), "Pusty parser");

	if (parsers_.find(parser->ID()) != parsers_.end()){
		CORE_LOG_NAMED_WARNING("parser", "Parser with given ID: " + boost::lexical_cast<std::string>(parser->ID()) + " already registered - skipping registration");
		throw loglib::runtime_error("Parser with similar ID already registered");
	}

	//! Weryfikujemy możliwości parsowania parsera
	ParserData pData;
	pData.sourceParser = dynamic_cast<plugin::ISourceParser*>(parser.get());
	pData.streamParser = dynamic_cast<plugin::IStreamParser*>(parser.get());

	if (!(pData.sourceParser || pData.streamParser)){
		CORE_LOG_NAMED_WARNING("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->ID()) + " does not support any of known capabilities to parse");
		throw loglib::runtime_error("Parser does not support any known parse capabilities");
	}

	//! Weryfikujemy wspierane wyrażenia
	plugin::IParser::Expressions expressions;
	parser->acceptedExpressions(expressions);

	if (expressions.empty() == true){
		CORE_LOG_NAMED_WARNING("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->ID()) + " does not support any sources");
		throw loglib::runtime_error("Parser does not support any sources");
	}

	auto typesHierarchy = getRegisteredDataTypesManager();

	//! Weryfikujemy oferowane typy - zawężamy jeśli coś jest nieznane
	for (auto it = expressions.begin(); it != expressions.end(); ++it){
		ExpressionDescription desc;
		desc.objectsTypes = it->second.objectsTypes;
		for (unsigned int i = 0; i < desc.objectsTypes.size(); ++i){
			auto t = desc.objectsTypes[i];
			if (typesHierarchy->isRegistered(t)){
				desc.supportedTypes.insert(t);
				desc.typesMap[i] = t;
			}
		}

		//! Weryfikacja czy wyrażenie wspiera jakieś znane typy
		if (desc.supportedTypes.empty() == true){
			CORE_LOG_NAMED_WARNING("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->ID()) + " does not provide any known type for expression: " + it->first);
		}
		else{
			desc.description = it->second.description;
			desc.regularExpression = std::regex(it->first);
			pData.expressions.insert(Expressions::value_type(it->first, desc));
		}
	}

	//! Weryfikacja czy mamy jakies wyrażenia wspierające znane typy
	if (pData.expressions.empty() == true){
		CORE_LOG_NAMED_WARNING("parser", "Parser ID: " + boost::lexical_cast<std::string>(parser->ID()) + " doest not provide any known type for any offered expression - skiping registration");
	}
	else{
		pData.parser = parser;
		parsers_[parser->ID()] = pData;
		std::string capabilities;
		if (pData.sourceParser && pData.streamParser){
			capabilities = "custom I/O capabilities and stream capabilities";
		}
		else if (pData.sourceParser){
			capabilities = "custom I/O capabilities";
		}
		else{
			capabilities = "stream capabilities";
		}
		CORE_LOG_NAMED_INFO("parser", "Parser ID: " + boost::lexical_cast<std::string>(parser->ID()) + " (" << parser->shortName() << ") successfully registered offering: " + capabilities);
	}
}
