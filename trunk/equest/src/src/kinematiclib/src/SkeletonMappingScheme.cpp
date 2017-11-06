#include <kinematiclib/SkeletonMappingScheme.h>
#include <tinyxml2.h>
#include <utils/Debug.h>
#include <utils/StreamTools.h>

using namespace kinematic;

SkeletonMappingScheme::MappingDictionary readMappingNode(tinyxml2::XMLNode* node)
{
	SkeletonMappingScheme::MappingDictionary result;	
	for (auto child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
		if (node->ToElement()) {
			auto element = child ? child->ToElement() : nullptr;
			auto pAttrib = element ? element->FirstAttribute() : nullptr;
			std::string name;
			std::string val;
			while (pAttrib != nullptr) {
				if (std::strcmp(pAttrib->Name(), "hanim") == 0) {
					name = pAttrib->Value();
				}
				else if (std::strcmp(pAttrib->Name(), "name") == 0) {
					val = pAttrib->Value();
				}
				else {
					UTILS_ASSERT(false, "Wrong file");
				}
				pAttrib = pAttrib->Next();
			}

			UTILS_ASSERT(val.size() > 0 && name.size() > 0, "Wrong file");
			result[val] = name;
		}
		else {
			UTILS_ASSERT(false, "wrong dictionary file");
		}
	}
	return result;
}

void readNode(tinyxml2::XMLNode* node, std::list<SkeletonMappingScheme::MappingDictionary> & result)
{
	if (!node) {
		return;
	}

	if (node->ToUnknown() != nullptr) {
		UTILS_ASSERT(false, "Unable to load dictionary");
	}
	else if (node->ToElement() != nullptr) {
		auto element = node->ToElement();
		if (element && strcmp(element->Value(), "SkeletonScheme") == 0) {
			SkeletonMappingScheme::MappingDictionary dict = readMappingNode(element);
			if (dict.size() > 0) {
				result.push_back(dict);
			}
			return;
		}
	}
	
	for (auto pChild = node->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		readNode(pChild, result);
	}
}

void SkeletonMappingScheme::loadFromXML(std::istream& in, std::list<SkeletonMappingScheme::MappingDictionary>& outSchemes)
{
	auto content = utils::StreamTools::read(in);
	tinyxml2::XMLDocument doc;
	if (doc.Parse(content.c_str()) == tinyxml2::XML_SUCCESS) {
		readNode(&doc, outSchemes);
	} else {
		throw std::runtime_error("Unable to parse stream");
	}
}

SkeletonMappingScheme::MappingDictionary createDefaultAcclaimToHAnimMappingDict()
{
	SkeletonMappingScheme::MappingDictionary ret;
	ret["root0"] = "vl5";
	ret["root1"] = "l_hip";
	ret["root2"] = "r_hip";
	ret["ToSpine"] = "vl4";
	ret["Spine"] = "vt12";
	ret["Spine1"] = "vt1";
	ret["Spine10"] = "vc7";
	ret["Spine11"] = "l_sternoclavicular";
	ret["Spine12"] = "r_sternoclavicular";
	ret["Neck"] = "vc1";
	ret["Head"] = "skullbase";
	ret["LeftShoulder"] = "l_shoulder";
	ret["LeftArm"] = "l_elbow";
	ret["LeftForeArm"] = "l_wrist";
	ret["LeftHand"] = "l_middle0";
	ret["RightShoulder"] = "r_shoulder";
	ret["RightArm"] = "r_elbow";
	ret["RightForeArm"] = "r_wrist";
	ret["RightHand"] = "r_middle0";
	ret["LeftUpLeg"] = "l_knee";
	ret["LeftLeg"] = "l_ankle";
	ret["LeftFoot"] = "l_subtalar";
	ret["LeftToeBase"] = "l_metatarsal";
	ret["RightUpLeg"] = "r_knee";
	ret["RightLeg"] = "r_ankle";
	ret["RightFoot"] = "r_subtalar";
	ret["RightToeBase"] = "r_metatarsal";
	//ret.mappingDict("dictionary.txt");
	ret["root"] = "HumanoidRoot";
	ret["lhipjoint"] = "l_hip";
	ret["lfemur"] = "l_knee";
	ret["ltibia"] = "l_ankle";
	ret["lfoot"] = "l_subtalar";
	ret["ltoes"] = "l_metatarsal";
	ret["rhipjoint"] = "r_hip";
	ret["rfemur"] = "r_knee";
	ret["rtibia"] = "r_ankle";
	ret["rfoot"] = "r_subtalar";
	ret["rtoes"] = "r_metatarsal";
	ret["lowerback"] = "vl5";
	ret["upperback"] = "vt12";
	ret["thorax"] = "vt1";
	ret["lowerneck"] = "vc7";
	ret["upperneck"] = "vc1";
	ret["head"] = "skullbase";
	ret["lclavicle"] = "l_sternoclavicular";
	ret["lhumerus"] = "l_shoulder";
	ret["lradius"] = "l_elbow";
	ret["lwrist"] = "l_wrist";
	ret["lhand"] = "l_middle0";
	ret["lfingers"] = "l_middle1";
	ret["lthumb"] = "l_thumb1";
	ret["rclavicle"] = "r_sternoclavicular";
	ret["rhumerus"] = "r_shoulder";
	ret["rradius"] = "r_elbow";
	ret["rwrist"] = "r_wrist";
	ret["rhand"] = "r_middle0";
	ret["rfingers"] = "r_middle1";
	ret["rthumb"] = "r_thumb1";


	ret["root"] = "HumanoidRoot";
	ret["LeftUpLeg_dum"] = "l_hip";
	ret["RightUpLeg_dum"] = "r_hip";
	ret["Spine_dum"] = "vl5";
	ret["LeftUpLeg"] = "l_knee";
	ret["LeftLeg"] = "l_ankle";
	ret["LeftFoot"] = "l_subtalar";
	ret["LeftToeBase"] = "l_metatarsal";
	ret["RightUpLeg"] = "r_knee";
	ret["RightLeg"] = "r_ankle";
	ret["RightFoot"] = "r_subtalar";
	ret["RightToeBase"] = "r_metatarsal";
	ret["Spine"] = "vt12";
	ret["Spine1"] = "vt1";
	ret["Spine1_dum1"] = "vc7";
	ret["Spine1_dum2"] = "l_sternoclavicular";
	ret["Spine1_dum3"] = "r_sternoclavicular";
	ret["Neck"] = "vc1";
	ret["Head"] = "skullbase";
	ret["LeftShoulder"] = "l_acromioclavicular";
	ret["LeftArm"] = "l_shoulder";
	ret["LeftForeArm"] = "l_elbow";
	ret["LeftHand"] = "l_wrist";
	ret["LeftHand_dum2"] = "l_middle0";
	ret["LeftHand_dum1"] = "l_thumb1";
	ret["LeftHandThumb"] = "l_thumb2";
	ret["RightShoulder"] = "r_acromioclavicular";
	ret["RightArm"] = "r_shoulder";
	ret["RightForeArm"] = "r_elbow";
	ret["RightHand"] = "r_wrist";
	ret["RightHand_dum2"] = "r_middle0";
	ret["RightHand_dum1"] = "r_thumb1";
	ret["RightHandThumb"] = "r_thumb2";

	ret["lshoulderjoint"] = "l_sternoclavicular";
	ret["rshoulderjoint"] = "r_sternoclavicular";

	return ret;
}
const SkeletonMappingScheme::MappingDictionary & SkeletonMappingScheme::defaultAcclaimToHAnimMappingScheme()
{
	static const auto defaultMappingDict = createDefaultAcclaimToHAnimMappingDict();
	return defaultMappingDict;
}