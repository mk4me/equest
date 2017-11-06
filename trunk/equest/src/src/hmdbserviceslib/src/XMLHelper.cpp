#include "XMLHelper.h"
#include <hmdbserviceslib/DateTimeUtils.h>

using namespace hmdbServices;

bool XMLHelper::extractAttributes(const tinyxml2::XMLElement * root, xmlWsdl::Attributes & attributes)
{
	auto element = root->FirstChildElement("Attributes");
	if (element != nullptr){
		element = element->FirstChildElement("Attribute");
	}

	if (element == nullptr){
		return false;
	}

	bool ret = true;

	xmlWsdl::Attributes locAttributes;

	try{

		while (element != nullptr){
			xmlWsdl::Attribute attribute;

			extractAttributeValue(element, "Name", attribute.name);
			extractAttributeValue(element, "Value", attribute.value);
			extractAndConvertAttributeValue<xmlWsdl::Entity>(element, "Entity", attribute.entity);
			extractAttributeValue(element, "AttributeGroup", attribute.attributeGroup);
			extractAndConvertAttributeValue<xmlWsdl::AttributeType>(element, "Type", attribute.type);

			locAttributes.push_back(attribute);

			element = element->NextSiblingElement();
		}

		attributes.insert(attributes.end(), locAttributes.begin(), locAttributes.end());
	}
	catch (...){
		ret = false;
	}

	return ret;
}

bool XMLHelper::extractEnumValues(const tinyxml2::XMLElement * root, xmlWsdl::EnumValueList & enumValues)
{
	auto element = root->FirstChildElement("EnumValueList");
	if (element != nullptr){
		element = element->FirstChildElement("EnumValue");
	}

	if (element == nullptr){
		return false;
	}

	bool ret = true;

	xmlWsdl::EnumValueList locEnumValues;

	try{

		while (element != nullptr){
			xmlWsdl::EnumValue enumValue;

			enumValue = element->GetText();

			locEnumValues.push_back(enumValue);

			element = element->NextSiblingElement();
		}

		enumValues.insert(enumValues.end(), locEnumValues.begin(), locEnumValues.end());
	}
	catch (...){
		ret = false;
	}

	return ret;
}

void XMLHelper::extractAnnotation(const tinyxml2::XMLElement * element, xmlWsdl::Annotation & annotation)
{
	extractTagValue(element, "TrialID", annotation.trialID);
	extractTagValue(element, "UserID", annotation.userID);
	extractAndConvertTagValue<xmlWsdl::AnnotationStatus>(element, "Status", annotation.status);
	extractTagValue(element, "Comment", annotation.comment);
	extractTagValue(element, "Note", annotation.note);
}

void XMLHelper::extractUserGroup(const tinyxml2::XMLElement * element, xmlWsdl::UserGroup & userGroup)
{
	extractAttributeValue(element, "UserGroupID", userGroup.id);
	extractAttributeValue(element, "UserGroupName", userGroup.name);
}

void XMLHelper::extractUserDetails(const tinyxml2::XMLElement * element, xmlWsdl::UserDetails & userDetails)
{
	extractTagValue(element, "ID", userDetails.id);
	extractTagValue(element, "Login", userDetails.login);
	extractTagValue(element, "FirstName", userDetails.firstName);
	extractTagValue(element, "LastName", userDetails.lastName);
}

void XMLHelper::_extractUserDetails(const tinyxml2::XMLElement * element, xmlWsdl::UserDetails & userDetails)
{
	extractAttributeValue(element, "ID", userDetails.id);
	extractAttributeValue(element, "Login", userDetails.login);
	extractAttributeValue(element, "FirstName", userDetails.firstName);
	extractAttributeValue(element, "LastName", userDetails.lastName);
}

void XMLHelper::extractUserData(const tinyxml2::XMLElement * element, xmlWsdl::User & user)
{
	extractUserDetails(element, user);
	extractTagValue(element, "Email", user.email);
}

void XMLHelper::extractPerformer(const tinyxml2::XMLElement * element, xmlWsdl::PerformerDetails & performer)
{
	extractTagValue(element, "PerformerID", performer.id);
	extractTagValue(element, "FirstName", performer.firstName);
	extractTagValue(element, "LastName", performer.lastName);
}

void XMLHelper::extractSession(const tinyxml2::XMLElement * element, xmlWsdl::SessionDetails & session)
{
	extractTagValue(element, "SessionID", session.id);
	extractTagValue(element, "UserID", session.userID);
	extractTagValue(element, "LabID", session.labID);
	extractTagValue(element, "MotionKind", session.motionKind);

	std::string sessionDate;
	extractTagValue(element, "SessionDate", sessionDate);
	session.dateTime = toTime(sessionDate);

	extractTagValue(element, "SessionDescription", session.description);
	extractTagValue(element, "SessionLabel", session.label);
}

void XMLHelper::extractTrial(const tinyxml2::XMLElement * element, xmlWsdl::TrialDetails & trial)
{
	extractTagValue(element, "TrialID", trial.id);
	extractTagValue(element, "SessionID", trial.sessionID);
	extractTagValue(element, "TrialDescription", trial.description);
}

void XMLHelper::extractFile(const tinyxml2::XMLElement * element, xmlWsdl::FileDetails & file)
{
	extractTagValue(element, "FileID", file.id);
	extractTagValue(element, "FileName", file.name);
	extractTagValue(element, "FileDescription", file.description);
	extractTagValue(element, "SubdirPath", file.subdirPath);
	extractTagValue(element, "AttributeName", file.attributeName);
}

void XMLHelper::extractMeasurementConf(const tinyxml2::XMLElement * element, xmlWsdl::MeasurementConfDetails & mcDetails)
{
	extractTagValue(element, "MeasurementConfID", mcDetails.id);
	extractTagValue(element, "MeasurementConfName", mcDetails.name);
	extractTagValue(element, "MeasurementConfKind", mcDetails.kind);
	extractTagValue(element, "MeasurementConfDescription", mcDetails.description);
}

void XMLHelper::extractPerformerConf(const tinyxml2::XMLElement * element, xmlWsdl::PerformerConfDetails & pcDetails)
{
	extractTagValue(element, "PerformerConfID", pcDetails.id);
	extractTagValue(element, "SessionID", pcDetails.sessionID);
	extractTagValue(element, "PerformerID ", pcDetails.performerID);
}

void XMLHelper::extractAttribute(const tinyxml2::XMLElement * element, xmlWsdl::AttributeDefinition & attributeDef)
{
	extractTagValue(element, "AttributeName", attributeDef.name);
	extractAndConvertAttributeValue<xmlWsdl::AttributeType>(element, "AttributeType", attributeDef.type);
	extractTagValue(element, "AttributeEnum ", attributeDef.enumerate);
	extractTagValue(element, "AttributeGroupName ", attributeDef.groupName);
	extractTagValue(element, "Unit ", attributeDef.unit);
}

void XMLHelper::extractAttributeGroup(const tinyxml2::XMLElement * element, xmlWsdl::AttributeGroupDefinition & attributeGroupDef)
{
	extractTagValue(element, "AttributeGroupName", attributeGroupDef.name);
	extractAndConvertAttributeValue<xmlWsdl::Entity>(element, "DescribedEntity", attributeGroupDef.describedEntity);
}

void XMLHelper::extractSessionGroup(const tinyxml2::XMLElement * element, xmlWsdl::SessionGroupDefinition & sessionGroupDef)
{
	extractTagValue(element, "SessionGroupID", sessionGroupDef.id);
	extractTagValue(element, "SessionGroupName", sessionGroupDef.name);
}

void XMLHelper::extractMotionKind(const tinyxml2::XMLElement * element, xmlWsdl::MotionKindDefinition & motionKind)
{
	extractTagValue(element, "MotionKindID", motionKind.id);
	extractTagValue(element, "MotionKindName", motionKind.name);
}

void XMLHelper::extractShallowCopyAttributes(const tinyxml2::XMLElement * element, MotionShallowCopy::Attrs & attributes)
{
	//Attrs
	MotionShallowCopy::Attrs attrs;

	auto attrs_element = element->FirstChildElement("Attrs");
	if (attrs_element) {
		auto attr_element = attrs_element->FirstChildElement("A");
		while (attr_element != nullptr) {
			MotionShallowCopy::Attrs::key_type name;
			MotionShallowCopy::Attrs::mapped_type value;

			XMLHelper::extractAttributeValue(attr_element, "Name", name);
			XMLHelper::extractAttributeValue(attr_element, "Value", value);

			attrs.insert({ name, value });
			attr_element = attr_element->NextSiblingElement();
		}
	}

	attributes.insert(attrs.begin(), attrs.end());
}

void XMLHelper::extractShallowCopyFile(const tinyxml2::XMLElement * element, MotionShallowCopy::File & file)
{
	file.trial = nullptr;
	file.session = nullptr;	

	XMLHelper::extractAttributeValue(element, "FileName", file.fileName);
	XMLHelper::extractAttributeValue(element, "FileDescription", file.fileDescription);
	XMLHelper::extractAttributeValue(element, "SubdirPath", file.subdirPath);
	XMLHelper::extractAttributeValue(element, "Size", file.fileSize);
}


template<>
bool hmdbServices::XMLHelper::extractAttributeValue(const tinyxml2::XMLElement * root, const std::string & attribute, std::string & value)
{
	auto attrib = root->FindAttribute(attribute.c_str());

	if (attrib != nullptr){
		value = attrib->Value();
		return true;
	}

	return false;
}

template<>
bool hmdbServices::XMLHelper::extractTagValue(const tinyxml2::XMLElement * root, const std::string & tag, std::string & value)
{
	auto element = root->FirstChildElement(tag.c_str());

	if (element != nullptr){
		auto ret = element->GetText();
		if (ret != nullptr){
			value = std::string(ret);
		}

		return true;
	}

	return false;
}
