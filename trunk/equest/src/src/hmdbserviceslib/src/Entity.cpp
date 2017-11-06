#include <hmdbserviceslib/Entity.h>
#include <boost/algorithm/string.hpp>
#include "XMLHelper.h"
#include <utils/Debug.h>
#include <hmdbserviceslib/DateTimeUtils.h>

using namespace hmdbServices;
using namespace hmdbServices::xmlWsdl;

Entity::Type Entity::convert(const std::string & entity)
{
	Entity::Type ret = Entity::Unspecified;

	std::string e = boost::algorithm::to_lower_copy(entity);

	if (e == "session"){
		ret = Entity::Session;
	}
	else if (e == "trial"){
		ret = Entity::Trial;
	}
	else if (e == "file"){
		ret = Entity::File;
	}
	else if (e == "measurement_configuration"){
		ret = Entity::MeasurementConfiguration;
	}
	else if (e == "performer"){
		ret = Entity::Performer;
	}
	else if (e == "performer_configuration"){
		ret = Entity::PerformerConfiguration;
	}

	return ret;
}

std::string Entity::convert(const Type & entity)
{
	std::string ret("unspecified");

	switch (entity){
	case Entity::Session:
		ret = "session";
		break;

	case Entity::Trial:
		ret = "trial";
		break;

	case Entity::File:
		ret = "file";
		break;

	case Entity::MeasurementConfiguration:
		ret = "measurement_conf";
		break;

	case Entity::Performer:
		ret = "performer";
		break;

	case Entity::PerformerConfiguration:
		ret = "performer_conf";
		break;
	}

	return ret;
}

Gender::Type Gender::convert(const std::string & gender)
{
	Gender::Type ret = Gender::Unspecified;

	std::string e = boost::algorithm::to_lower_copy(gender);

	if (e == "man" || e == "male" || e == "m"){
		ret = Gender::Male;
	}
	else if (e == "woman" || e == "female" || e == "f" || e == "w" || e == "k"){
		ret = Gender::Female;
	}

	return ret;
}

std::string Gender::convert(const Type & gender)
{
	std::string ret("unspecified");

	switch (gender){
	case Gender::Male:
		ret = "m";
		break;

	case Gender::Female:
		ret = "k";
		break;
	}

	return ret;
}

AttributeType::Type AttributeType::convert(const std::string & attributeType)
{
	AttributeType::Type ret = AttributeType::Unspecified;

	std::string e = boost::algorithm::to_lower_copy(attributeType);

	if (e == "int"){
		ret = AttributeType::Int;
	}
	else if (e == "decimal"){
		ret = AttributeType::Decimal;
	}
	else if (e == "nonnegativeinteger"){
		ret = AttributeType::NonNegativeInteger;
	}
	else if (e == "nonnegativedecimal"){
		ret = AttributeType::NonNegativeDecimal;
	}
	else if (e == "datetime"){
		ret = AttributeType::DateTime;
	}
	else if (e == "date"){
		ret = AttributeType::Date;
	}
	else if (e == "timecode"){
		ret = AttributeType::TimeCode;
	}
	else if (e == "float"){
		ret = AttributeType::Float;
	}

	return ret;
}

std::string AttributeType::convert(const Type & attributeType)
{
	std::string ret("unspecified");

	switch (attributeType){
	case AttributeType::Int:
		ret = "int";
		break;

	case AttributeType::Decimal:
		ret = "decimal";
		break;

	case AttributeType::NonNegativeInteger:
		ret = "nonNegativeInteger";
		break;

	case AttributeType::NonNegativeDecimal:
		ret = "nonNegativeDecimal";
		break;

	case AttributeType::DateTime:
		ret = "dateTime";
		break;

	case AttributeType::Date:
		ret = "date";
		break;

	case AttributeType::TimeCode:
		ret = "TIMECODE";
		break;

	case AttributeType::Float:
		ret = "float";
		break;
	}

	return ret;
}

BooleanType::Type BooleanType::convert(const std::string & booleanType)
{
	BooleanType::Type ret = BooleanType::Unspecified;

	std::string e = boost::algorithm::to_lower_copy(booleanType);

	if (e == "true"){
		ret = BooleanType::True;
	}
	else if (e == "false"){
		ret = BooleanType::False;
	}

	return ret;
}

std::string BooleanType::convert(const Type & booleanType)
{
	std::string ret("unspecified");

	switch (booleanType){
	case BooleanType::True:
		ret = "true";
		break;

	case BooleanType::False:
		ret = "false";
		break;
	}

	return ret;
}

AnnotationStatus::Type AnnotationStatus::convert(const std::string & statusType)
{
	Type ret = Unspecified;

	try{
		int e = boost::lexical_cast<int>(statusType);
		if (e > -1 && e < 5){
			ret = static_cast<Type>(e);
		}
	}
	catch (...){
	}

	return ret;
}

std::string AnnotationStatus::convert(const Type & statusType)
{
	std::string ret("unspecified");

	switch (statusType){
	case UnderConstruction:
		ret = "1";
		break;

	case ReadyForReview:
		ret = "2";
		break;

	case UnderReview:
		ret = "3";
		break;

	case Approved:
		ret = "4";
		break;

	case Rejected:
		ret = "0";
		break;
	}

	return ret;
}

AnnotationsList hmdbServices::xmlWsdl::parseAnnotations(const std::string & xmlResponse)
{
	AnnotationsList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}		

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();		
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("Annotation");

	while (element != nullptr) {
		Annotation a;

		XMLHelper::extractAnnotation(element, a);

		ret.push_back(a);

		element = element->NextSiblingElement();
	}

	return ret;
}

UserGroupsList hmdbServices::xmlWsdl::parseUserGroups(const std::string & xmlResponse)
{
	UserGroupsList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("UserGroup");

	while (element != nullptr) {
		UserGroup ug;

		XMLHelper::extractUserGroup(element, ug);

		ret.push_back(ug);

		element = element->NextSiblingElement();
	}

	return ret;
}

UsersList hmdbServices::xmlWsdl::parseUsersList(const std::string & xmlResponse)
{
	UsersList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("UserDetails");

	while (element != nullptr) {
		UserDetails ud;

		XMLHelper::_extractUserDetails(element, ud);

		ret.push_back(ud);

		element = element->NextSiblingElement();
	}

	return ret;
}

User hmdbServices::xmlWsdl::parseUser(const std::string & xmlResponse)
{
	User ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();

	if (element == nullptr) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractUserDetails(element, ret);

	return ret;
}

AttributeGroupDefinitionList hmdbServices::xmlWsdl::parseAttributeGroupDefinitions(const std::string & xmlResponse)
{
	AttributeGroupDefinitionList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("AttributeGroupDefinition");	

	while (element != nullptr) {
		AttributeGroupDefinition agd;

		XMLHelper::extractTagValue(element, "AttributeGroupName", agd.name);
		XMLHelper::extractAndConvertTagValue<Entity>(element, "DescribedEntity", agd.describedEntity);		

		ret.push_back(agd);

		element = element->NextSiblingElement();
	}

	return ret;
}

PerformerList hmdbServices::xmlWsdl::parsePerfomers(const std::string & xmlResponse)
{
	PerformerList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("PerformerDetails");

	while (element != nullptr) {
		PerformerDetails pd;

		XMLHelper::extractTagValue(element, "PerformerID", pd.id);
		XMLHelper::extractTagValue(element, "FirstName", pd.firstName);
		XMLHelper::extractTagValue(element, "LastName", pd.lastName);		

		ret.push_back(pd);

		element = element->NextSiblingElement();
	}

	return ret;
}

PerformerDetailsWithAttributes hmdbServices::xmlWsdl::parsePerformer(const std::string & xmlResponse)
{
	PerformerDetailsWithAttributes ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	XMLHelper::extractPerformer(element, ret.performerDetails);
	XMLHelper::extractAttributes(element, ret.attributes);

	return ret;
}

PerformerWithAttributesList hmdbServices::xmlWsdl::parsePerfomersWithAttributes(const std::string & xmlResponse)
{
	PerformerWithAttributesList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	while (element != nullptr){
		PerformerDetailsWithAttributes pdwa;

		XMLHelper::extractPerformer(element, pdwa.performerDetails);
		XMLHelper::extractAttributes(element, pdwa.attributes);

		ret.push_back(pdwa);

		element = element->NextSiblingElement();
	}

	return ret;
}

SessionList hmdbServices::xmlWsdl::parseSessions(const std::string & xmlResponse)
{
	SessionList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("SessionDetails");	

	while (element != nullptr){
		SessionDetails sd;

		XMLHelper::extractAttributeValue(element, "SessionID", sd.id);
		XMLHelper::extractAttributeValue(element, "UserID", sd.userID);
		XMLHelper::extractAttributeValue(element, "LabID", sd.labID);
		XMLHelper::extractAttributeValue(element, "MotionKind", sd.motionKind);

		std::string sessionDate;
		XMLHelper::extractAttributeValue(element, "SessionDate", sessionDate);		
		sd.dateTime = toTime(sessionDate);

		XMLHelper::extractAttributeValue(element, "SessionDescription", sd.description);
		XMLHelper::extractAttributeValue(element, "SessionLabel", sd.label);

		ret.push_back(sd);

		element = element->NextSiblingElement();
	}

	return ret;
}

SessionDetailsWithAttributes hmdbServices::xmlWsdl::parseSession(const std::string & xmlResponse)
{
	SessionDetailsWithAttributes ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	XMLHelper::extractSession(element, ret.sessionDetails);
	XMLHelper::extractAttributes(element, ret.attributes);

	return ret;
}

SessionWithAttributesList hmdbServices::xmlWsdl::parseSessionsWithAttributes(const std::string & xmlResponse)
{
	SessionWithAttributesList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("SessionDetailsWithAttributes");

	while (element != nullptr){
		SessionDetailsWithAttributes sdwa;

		XMLHelper::extractSession(element, sdwa.sessionDetails);
		XMLHelper::extractAttributes(element, sdwa.attributes);

		ret.push_back(sdwa);

		element = element->NextSiblingElement();
	}

	return ret;
}

SessionGroupDefinitionList hmdbServices::xmlWsdl::parseSessionGroups(const std::string & xmlResponse)
{
	SessionGroupDefinitionList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("SessionGroupDefinition");	

	while (element != nullptr){
		SessionGroupDefinition sgd;

		XMLHelper::extractTagValue(element, "SessionGroupID", sgd.id);
		XMLHelper::extractTagValue(element, "SessionGroupName", sgd.name);

		ret.push_back(sgd);

		element = element->NextSiblingElement();
	}

	return ret;
}

MotionKindDefinitionList hmdbServices::xmlWsdl::parseMotionKinds(const std::string & xmlResponse)
{
	MotionKindDefinitionList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("MotionKindDefinition");	

	while (element != nullptr){
		MotionKindDefinition mkd;

		XMLHelper::extractTagValue(element, "MotionKindID", mkd.id);
		XMLHelper::extractTagValue(element, "MotionKindName", mkd.name);

		ret.push_back(mkd);

		element = element->NextSiblingElement();
	}

	return ret;
}

TrialList hmdbServices::xmlWsdl::parseTrials(const std::string & xmlResponse)
{
	TrialList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("TrialDetails");

	while (element != nullptr){
		TrialDetails td;

		XMLHelper::extractAttributeValue(element, "TrialID", td.id);
		XMLHelper::extractAttributeValue(element, "SessionID", td.sessionID);
		XMLHelper::extractAttributeValue(element, "TrialDescription", td.description);

		ret.push_back(td);

		element = element->NextSiblingElement();
	}

	return ret;
}

TrialDetailsWithAttributes hmdbServices::xmlWsdl::parseTrial(const std::string & xmlResponse)
{
	TrialDetailsWithAttributes ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}	

	XMLHelper::extractTrial(element, ret.trialDetails);
	XMLHelper::extractAttributes(element, ret.attributes);

	return ret;
}

TrialsWithAttributesList hmdbServices::xmlWsdl::parseTrialsWithAttributes(const std::string & xmlResponse)
{
	TrialsWithAttributesList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("TrialDetailsWithAttributes");	

	while (element != nullptr){
		TrialDetailsWithAttributes tdwa;

		XMLHelper::extractTrial(element, tdwa.trialDetails);
		XMLHelper::extractAttributes(element, tdwa.attributes);

		ret.push_back(tdwa);

		element = element->NextSiblingElement();
	}

	return ret;
}

MeasurementConfDetailsWithAttributes hmdbServices::xmlWsdl::parseMeasurementConf(const std::string & xmlResponse)
{
	MeasurementConfDetailsWithAttributes ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	XMLHelper::extractMeasurementConf(element, ret.measurementConfDetails);
	XMLHelper::extractAttributes(element, ret.attributes);

	return ret;
}

MeasurementConfWithAttributesList hmdbServices::xmlWsdl::parseMeasurementsConfWithAttributes(const std::string & xmlResponse)
{
	MeasurementConfWithAttributesList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("MeasurementConfDetailsWithAttributes");	

	while (element != nullptr){
		MeasurementConfDetailsWithAttributes mcwa;

		XMLHelper::extractMeasurementConf(element, mcwa.measurementConfDetails);
		XMLHelper::extractAttributes(element, mcwa.attributes);

		ret.push_back(mcwa);

		element = element->NextSiblingElement();
	}

	return ret;
}

PerformerConfDetailsWithAttributes hmdbServices::xmlWsdl::parsePerfomerConf(const std::string & xmlResponse)
{
	PerformerConfDetailsWithAttributes ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	XMLHelper::extractPerformerConf(element, ret.performerConfDetails);
	XMLHelper::extractAttributes(element, ret.attributes);

	return ret;
}

PerformerConfWithAttributesList hmdbServices::xmlWsdl::parsePerformersConfWithAttributes(const std::string & xmlResponse)
{
	PerformerConfWithAttributesList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("PerformerConfDetailsWithAttributes");	

	while (element != nullptr){
		PerformerConfDetailsWithAttributes pcwa;

		XMLHelper::extractPerformerConf(element, pcwa.performerConfDetails);
		XMLHelper::extractAttributes(element, pcwa.attributes);

		ret.push_back(pcwa);

		element = element->NextSiblingElement();
	}

	return ret;
}

FileList hmdbServices::xmlWsdl::parseFiles(const std::string & xmlResponse)
{
	FileList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("FileDetails");	

	while (element != nullptr){
		FileDetails fd;

		XMLHelper::extractFile(element, fd);

		ret.push_back(fd);

		element = element->NextSiblingElement();
	}

	return ret;
}

FileWithAttributeList hmdbServices::xmlWsdl::parseFilesWithAttributes(const std::string & xmlResponse)
{
	FileWithAttributeList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("FileDetailsWithAttributes");	

	while (element != nullptr){
		FileDetailsWithAttribute fdwa;

		XMLHelper::extractFile(element, fdwa.fileDetails);
		XMLHelper::extractAttributes(element, fdwa.attributes);

		ret.push_back(fdwa);

		element = element->NextSiblingElement();
	}

	return ret;
}

EnumValueList hmdbServices::xmlWsdl::parseEnumValues(const std::string & xmlResponse)
{
	EnumValueList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	XMLHelper::extractEnumValues(element, ret);

	return ret;
}

AttributeDefinitionList hmdbServices::xmlWsdl::parseAttributesDefinitions(const std::string & xmlResponse)
{
	AttributeDefinitionList ret;

	tinyxml2::XMLDocument doc;

	if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_NO_ERROR){
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		return ret;
	}

	auto element = doc.FirstChildElement();
	if (element != nullptr){
		element = element->FirstChildElement();
	}

	if (element == nullptr){
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
		return ret;
	}

	element = element->FirstChildElement("AttributeDefinition");	

	while (element != nullptr){
		AttributeDefinition ad;

		XMLHelper::extractAttribute(element, ad);
		XMLHelper::extractEnumValues(element, ad.enumValues);

		ret.push_back(ad);

		element = element->NextSiblingElement();
	}

	return ret;
}