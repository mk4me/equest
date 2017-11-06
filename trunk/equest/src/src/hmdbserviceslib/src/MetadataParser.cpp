#include <hmdbserviceslib/MetadataParser.h>
#include <hmdbserviceslib/Entity.h>
#include <utils/Debug.h>
#include <utils/StreamTools.h>
#include "XMLHelper.h"

namespace hmdbServices
{
	bool MotionMetadataParser::parseFile(std::istream & document, MotionMetaData::MetaData & metadata)
	{
		auto s = utils::StreamTools::read(document);

		tinyxml2::XMLDocument xmlDocument;

		xmlDocument.Parse(s.c_str());

		if (xmlDocument.Error()) {
			UTILS_ASSERT(false, "Blad wczytania pliku MotionShallowCopy");
			return false;
		}

		auto element = xmlDocument.FirstChildElement();
		if (element == nullptr) {
			UTILS_ASSERT(false, "Blad wczytania z pliku MotionShallowCopy");
			return false;
		}

		//SessionGroups
		auto session_goups_element = element->FirstChildElement("SessionGroups");
		if (session_goups_element != nullptr) {
			auto session_goup_element = session_goups_element->FirstChildElement("SessionGroup");
			while (session_goup_element != nullptr) {
				MotionMetaData::SessionGroup sessionGroup;
				XMLHelper::extractAttributeValue(session_goup_element, "SessionGroupID", sessionGroup.sessionGroupID);
				XMLHelper::extractAttributeValue(session_goup_element, "SessionGroupName", sessionGroup.sessionGroupName);				

				metadata.sessionGroups[sessionGroup.sessionGroupID] = sessionGroup;
				session_goup_element = session_goup_element->NextSiblingElement();
			}
		}
		//MotionKinds
		auto motion_kinds_element = element->FirstChildElement("MotionKinds");
		if (motion_kinds_element != nullptr) {
			auto motion_kind_element = motion_kinds_element->FirstChildElement("MotionKind");
			while (motion_kind_element != nullptr) {
				MotionMetaData::MotionKind motionKind;
				XMLHelper::extractAttributeValue(motion_kind_element, "MotionKindName", motionKind.motionKindName);

				metadata.motionKinds.push_back(motionKind);
				motion_kind_element = motion_kind_element->NextSiblingElement();
			}
		}
		//Labs
		auto labs_element = element->FirstChildElement("Labs");
		if (labs_element != nullptr) {
			auto lab_element = labs_element->FirstChildElement("Lab");
			while (lab_element != nullptr) {
				MotionMetaData::Lab lab;

				XMLHelper::extractAttributeValue(lab_element, "LabID", lab.labID);
				XMLHelper::extractAttributeValue(lab_element, "LabName", lab.labName);				

				metadata.labs[lab.labID] = lab;
				lab_element = lab_element->NextSiblingElement();
			}
		}
		//AttributeGroups
		auto attribute_groups_element = element->FirstChildElement("AttributeGroups");
		if (attribute_groups_element != nullptr) {
			auto attribute_group_element = attribute_groups_element->FirstChildElement("AttributeGroup");
			while (attribute_group_element != nullptr) {
				MotionMetaData::AttributeGroup attributeGroup;
				XMLHelper::extractAttributeValue(attribute_group_element, "AttributeGroupID", attributeGroup.attributeGroupID);
				XMLHelper::extractAttributeValue(attribute_group_element, "AttributeGroupName", attributeGroup.attributeGroupName);
				XMLHelper::extractAndConvertAttributeValue<xmlWsdl::Entity>(attribute_group_element, "DescribedEntity", attributeGroup.describedEntity);

				//Attributes
				auto attrs_element = attribute_group_element->FirstChildElement("Attributes");
				if (attrs_element) {
					auto attr_element = attrs_element->FirstChildElement("Attribute");
					while (attr_element) {
						MotionMetaData::Attribute attribute;

						XMLHelper::extractAttributeValue(attr_element, "AttributeName", attribute.attributeName);						
						XMLHelper::extractAndConvertAttributeValue<xmlWsdl::AttributeType>(attr_element, "AttributeType", attribute.attributeType);						
						XMLHelper::extractAttributeValue(attr_element, "Unit", attribute.unit);

						//EnumValues
						/*auto enum_values_element = attr_element->FirstChildElement("EnumValues");
						if(enum_values_element) {
						auto enum_value_element = enum_values_element->FirstChildElement("Enumeration");
						while(enum_value_element) {
						communication::MotionMetaData::Enumeration enumeration;
						enum_value_element->QueryStringAttribute("EnumValue", &enumeration.enumValue);

						attribute.enumValues.push_back(enumeration);
						enum_value_element = enum_value_element->NextSiblingElement();
						}
						}*/
						//attributeGroup.attributes.push_back(attribute);
						attributeGroup.attributes[attribute.attributeName] = attribute;
						attr_element = attr_element->NextSiblingElement();
					}
				}

				metadata.attributeGroups[attributeGroup.attributeGroupID] = attributeGroup;
				attribute_group_element = attribute_group_element->NextSiblingElement();
			}
		}

		return true;
	}

	bool MedicalMetadataParser::parseFile(std::istream & document, MedicalMetaData::MetaData & metadata)
	{
		auto s = utils::StreamTools::read(document);

		tinyxml2::XMLDocument xmlDocument;

		xmlDocument.Parse(s.c_str());

		if (xmlDocument.Error()) {
			UTILS_ASSERT(false, "Blad wczytania pliku MotionShallowCopy");
			return false;
		}

		auto element = xmlDocument.FirstChildElement();
		if (element == nullptr) {
			UTILS_ASSERT(false, "Blad wczytania z pliku MotionShallowCopy");
			return false;
		}

		//ExamTypes
		auto exam_types_element = element->FirstChildElement("ExamTypes");
		if (exam_types_element != nullptr) {
			auto exam_type_element = exam_types_element->FirstChildElement("ExamType");
			while (exam_type_element != nullptr) {
				MedicalMetaData::ExamType examType;

				XMLHelper::extractAttributeValue(exam_type_element, "ExamTypeID", examType.examTypeID);
				XMLHelper::extractAttributeValue(exam_type_element, "ExamTypeName", examType.name);				

				metadata.examTypes[examType.examTypeID] = examType;
				exam_type_element = exam_type_element->NextSiblingElement();
			}
		}
		//Disorders
		auto disorders_element = element->FirstChildElement("Disorders");
		if (disorders_element != nullptr) {
			auto disorder_element = disorders_element->FirstChildElement("Disorder");
			while (disorder_element != nullptr) {
				MedicalMetaData::DisorderType disorderType;

				XMLHelper::extractAttributeValue(disorder_element, "DisorderID", disorderType.disorderTpeID);
				XMLHelper::extractAttributeValue(disorder_element, "DisorderName", disorderType.name);

				metadata.disorderTypes[disorderType.disorderTpeID] = disorderType;
				disorder_element = disorder_element->NextSiblingElement();
			}
		}

		return true;
	}
}
