/********************************************************************
	created:  2014/07/31	08:37:11
	filename: XMLHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBSERVICES__XMLHELPER_H__
#define __HEADER_GUARD_HMDBSERVICES__XMLHELPER_H__

#include <string>
#include <tinyxml2.h>
#include <boost/lexical_cast.hpp>
#include <hmdbserviceslib/Entity.h>
#include <hmdbserviceslib/ShallowCopy.h>

namespace hmdbServices
{
	namespace XMLHelper
	{
		template<class T>
		bool extractTagValue(const tinyxml2::XMLElement * root, const std::string & tag, T & value)
		{
			auto element = root->FirstChildElement(tag.c_str());

			if (element != nullptr){
				try{
					value = boost::lexical_cast<T>(element->GetText());
					return true;
				}
				catch (...){
					return false;
				}
			}

			return false;
		}

		template<class T>
		bool extractAttributeValue(const tinyxml2::XMLElement * root, const std::string & attribute, T & value)
		{
			auto attrib = root->FindAttribute(attribute.c_str());
			if (attrib != nullptr){
				try{
					value = boost::lexical_cast<T>(std::string(attrib->Value()));
					return true;
				}
				catch (...){
					return false;
				}
			}

			return false;
		}

		template<>
		bool extractTagValue<std::string>(const tinyxml2::XMLElement * root, const std::string & tag, std::string & value);

		template<>
		bool extractAttributeValue<std::string>(const tinyxml2::XMLElement * root, const std::string & attribute, std::string & value);

		template<typename T>
		bool extractAndConvertAttributeValue(const tinyxml2::XMLElement * root, const std::string & attribute, typename T::Type & value)
		{
			bool ret = false;

			std::string sValue;
			if (extractAttributeValue(root, attribute, sValue) == true){
				value = T::convert(sValue);
				ret = true;
			}

			return ret;
		}

		template<typename T>
		bool extractAndConvertTagValue(const tinyxml2::XMLElement * root, const std::string & tag, typename T::Type & value)
		{
			bool ret = false;

			std::string sValue;
			if (extractTagValue(root, tag, sValue) == true){
				value = T::convert(sValue);
				ret = true;
			}

			return ret;
		}

		bool extractAttributes(const tinyxml2::XMLElement * root, xmlWsdl::Attributes & attributes);

		bool extractEnumValues(const tinyxml2::XMLElement * root, xmlWsdl::EnumValueList & enumValues);

		void extractAnnotation(const tinyxml2::XMLElement * element, xmlWsdl::Annotation & annotation);

		void extractUserGroup(const tinyxml2::XMLElement * element, xmlWsdl::UserGroup & userGroup);

		void extractUserDetails(const tinyxml2::XMLElement * element, xmlWsdl::UserDetails & userDetails);

		//! TODO
		//! ta metoda albo poprzednia musi zniknąć po ujednoliceniu opisu usera w odpowiedzi usług
		void _extractUserDetails(const tinyxml2::XMLElement * element, xmlWsdl::UserDetails & userDetails);

		void extractUserData(const tinyxml2::XMLElement * element, xmlWsdl::User & user);

		void extractPerformer(const tinyxml2::XMLElement * element, xmlWsdl::PerformerDetails & performer);

		void extractSession(const tinyxml2::XMLElement * element, xmlWsdl::SessionDetails & session);

		void extractTrial(const tinyxml2::XMLElement * element, xmlWsdl::TrialDetails & trial);

		void extractFile(const tinyxml2::XMLElement * element, xmlWsdl::FileDetails & file);

		void extractMeasurementConf(const tinyxml2::XMLElement * element, xmlWsdl::MeasurementConfDetails & mcDetails);

		void extractPerformerConf(const tinyxml2::XMLElement * element, xmlWsdl::PerformerConfDetails & pcDetails);

		void extractAttribute(const tinyxml2::XMLElement * element, xmlWsdl::AttributeDefinition & attributeDef);

		void extractAttributeGroup(const tinyxml2::XMLElement * element, xmlWsdl::AttributeGroupDefinition & attributeGroupDef);

		void extractSessionGroup(const tinyxml2::XMLElement * element, xmlWsdl::SessionGroupDefinition & sessionGroupDef);

		void extractMotionKind(const tinyxml2::XMLElement * element, xmlWsdl::MotionKindDefinition & motionKind);

		void extractShallowCopyAttributes(const tinyxml2::XMLElement * element, MotionShallowCopy::Attrs & attributes);

		void extractShallowCopyFile(const tinyxml2::XMLElement * element, MotionShallowCopy::File & file);
	};
}

#endif	// __HEADER_GUARD_HMDBSERVICES__XMLHELPER_H__
