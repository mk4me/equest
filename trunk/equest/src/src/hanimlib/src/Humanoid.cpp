#include <hanimlib/Humanoid.h>
#include <utils/Debug.h>

using namespace hAnim;

Humanoid::Humanoid()
	: bboxCenter(0, 0, 0), bboxSize(-1, -1, -1),
	center(0, 0, 0), rotation(0, 0, 0, 1), scale(1, 1, 1),
	scaleOrientation(0, 0, 0, 1), translation(0, 0, 0),
	version("undefined")
{

}

Humanoid::Humanoid(const Humanoid & Other)
	: bboxCenter(Other.bboxCenter),
	bboxSize(Other.bboxSize), center(Other.center),
	rotation(Other.rotation), scale(Other.scale),
	scaleOrientation(Other.scaleOrientation),
	translation(Other.translation),
	info(Other.info), joints(Other.joints),
	segments(Other.segments), sites(Other.sites),
	skeleton(Other.skeleton), skin(Other.skin),
	skinCoord(Other.skinCoord), skinNormal(Other.skinNormal),
	version(Other.version),
	viewpoints(Other.viewpoints)
{

}

Humanoid::Humanoid(Humanoid && Other)
	: bboxCenter(Other.bboxCenter),
	bboxSize(Other.bboxSize), center(Other.center),
	rotation(Other.rotation), scale(Other.scale),
	scaleOrientation(Other.scaleOrientation),
	translation(Other.translation),
	info(std::move(Other.info)), joints(std::move(Other.joints)),
	segments(std::move(Other.segments)), sites(std::move(Other.sites)),
	skeleton(std::move(Other.skeleton)), skin(std::move(Other.skin)),
	skinCoord(std::move(Other.skinCoord)), skinNormal(std::move(Other.skinNormal)),
	version(std::move(Other.version)),
	viewpoints(std::move(Other.viewpoints))
{
	Other.bboxCenter = osg::Vec3(0, 0, 0);
	Other.bboxSize = osg::Vec3(-1, -1, -1);
	Other.center = osg::Vec3(0, 0, 0);
	Other.rotation = osg::Quat(0, 0, 0, 1);
	Other.scale = osg::Vec3(1, 1, 1);
	Other.scaleOrientation = osg::Quat(0, 0, 0, 1);
	Other.translation = osg::Vec3(0, 0, 0);
}

void backJointAndSegment(std::string jointName, std::string segmentName,
	sequence<Joint>::value_type & joint,
	sequence<Segment>::value_type & segment,
	Humanoid & humanoid)
{
	auto jIT = std::find_if(humanoid.joints.begin(), humanoid.joints.end(), [&jointName](const sequence<Joint>::value_type & j) { return (*j).name == jointName; });
	auto sIT = std::find_if(humanoid.segments.begin(), humanoid.segments.end(), [&segmentName](const sequence<Segment>::value_type & j) { return (*j).name == segmentName; });

	UTILS_ASSERT(jIT != humanoid.joints.end());
	UTILS_ASSERT(sIT != humanoid.segments.end());

	joint = *jIT;
	segment = *sIT;
}

void createJointAndSegment(std::string jointName, std::string segmentName,
	sequence<Joint>::value_type & parentJoint,
	sequence<Segment>::value_type & parentSegment,
	Humanoid & humanoid)
{
	auto joint = utils::make_shared<Joint>();
	joint->name = jointName;
	auto segment = utils::make_shared<Segment>();
	segment->name = segmentName;
	if (parentSegment != nullptr) {
		parentSegment->children.push_back(joint);
	}
	joint->children.push_back(segment);

	humanoid.joints.push_back(joint);
	humanoid.segments.push_back(segment);

	parentJoint = joint;
	parentSegment = segment;
}

Humanoid createDefaultHumanoid()
{
	Humanoid ret;
	ret.version = "200x";

	sequence<Joint>::value_type joint;
	sequence<Segment>::value_type segment;

	createJointAndSegment("HumanoidRoot", "sacrum", joint, segment, ret);
	createJointAndSegment("sacroiliac", "pelvis", joint, segment, ret);
	createJointAndSegment("l_hip", "l_thigh", joint, segment, ret);
	createJointAndSegment("l_knee", "l_calf", joint, segment, ret);
	createJointAndSegment("l_ankle", "l_hindfoot", joint, segment, ret);
	createJointAndSegment("l_subtalar", "l_midproximal", joint, segment, ret);
	createJointAndSegment("l_midtarsal", "l_middistal", joint, segment, ret);
	createJointAndSegment("l_metatarsal", "l_forefoot", joint, segment, ret);
	backJointAndSegment("sacroiliac", "pelvis", joint, segment, ret);
	createJointAndSegment("r_hip", "r_thigh", joint, segment, ret);
	createJointAndSegment("r_knee", "r_calf", joint, segment, ret);
	createJointAndSegment("r_ankle", "r_hindfoot", joint, segment, ret);
	createJointAndSegment("r_subtalar", "r_midproximal", joint, segment, ret);
	createJointAndSegment("r_midtarsal", "r_middistal", joint, segment, ret);
	createJointAndSegment("r_metatarsal", "r_forefoot", joint, segment, ret);
	backJointAndSegment("HumanoidRoot", "sacrum", joint, segment, ret);
	createJointAndSegment("vl5", "l5", joint, segment, ret);
	createJointAndSegment("vl4", "l4", joint, segment, ret);
	createJointAndSegment("vl3", "l3", joint, segment, ret);
	createJointAndSegment("vl2", "l2", joint, segment, ret);
	createJointAndSegment("vl1", "l1", joint, segment, ret);
	createJointAndSegment("vt12", "t12", joint, segment, ret);
	createJointAndSegment("vt11", "t11", joint, segment, ret);
	createJointAndSegment("vt10", "t10", joint, segment, ret);
	createJointAndSegment("vt9", "t9", joint, segment, ret);
	createJointAndSegment("vt8", "t8", joint, segment, ret);
	createJointAndSegment("vt7", "t7", joint, segment, ret);
	createJointAndSegment("vt6", "t6", joint, segment, ret);
	createJointAndSegment("vt5", "t5", joint, segment, ret);
	createJointAndSegment("vt4", "t4", joint, segment, ret);
	createJointAndSegment("vt3", "t3", joint, segment, ret);
	createJointAndSegment("vt2", "t2", joint, segment, ret);
	createJointAndSegment("vt1", "t1", joint, segment, ret);
	/*|*/createJointAndSegment("vc7", "c7", joint, segment, ret);
	createJointAndSegment("vc6", "c6", joint, segment, ret);
	createJointAndSegment("vc5", "c5", joint, segment, ret);
	createJointAndSegment("vc4", "c4", joint, segment, ret);
	createJointAndSegment("vc3", "c3", joint, segment, ret);
	createJointAndSegment("vc2", "c2", joint, segment, ret);
	createJointAndSegment("vc1", "c1", joint, segment, ret);
	createJointAndSegment("skullbase", "skull", joint, segment, ret);
	createJointAndSegment("l_eyelid_joint", "l_eyelid", joint, segment, ret);
	backJointAndSegment("skullbase", "skull", joint, segment, ret);
	createJointAndSegment("r_eyelid_joint", "r_eyelid", joint, segment, ret);
	backJointAndSegment("skullbase", "skull", joint, segment, ret);
	createJointAndSegment("l_eyeball_joint", "l_eyeball", joint, segment, ret);
	backJointAndSegment("skullbase", "skull", joint, segment, ret);
	createJointAndSegment("r_eyeball_joint", "r_eyeball", joint, segment, ret);
	backJointAndSegment("skullbase", "skull", joint, segment, ret);
	createJointAndSegment("l_eyebrow_joint", "l_eyebrow", joint, segment, ret);
	backJointAndSegment("skullbase", "skull", joint, segment, ret);
	createJointAndSegment("r_eyebrow_joint", "r_eyebrow", joint, segment, ret);
	backJointAndSegment("skullbase", "skull", joint, segment, ret);
	createJointAndSegment("temporomandibular", "jaw", joint, segment, ret);
	backJointAndSegment("vt1", "t1", joint, segment, ret);
	/*|*/createJointAndSegment("l_sternoclavicular", "l_clavicle", joint, segment, ret);
	createJointAndSegment("l_acromioclavicular", "l_scapula", joint, segment, ret);
	createJointAndSegment("l_shoulder", "l_upperarm", joint, segment, ret);
	createJointAndSegment("l_elbow", "l_forearm", joint, segment, ret);
	createJointAndSegment("l_wrist", "l_hand", joint, segment, ret);
	createJointAndSegment("l_thumb1", "l_thumb_metacarpal", joint, segment, ret);
	createJointAndSegment("l_thumb2", "l_thumb_proximal", joint, segment, ret);
	createJointAndSegment("l_thumb3", "l_thumb_distal", joint, segment, ret);
	backJointAndSegment("l_wrist", "l_hand", joint, segment, ret);
	createJointAndSegment("l_index0", "l_index_metacarpal", joint, segment, ret);
	createJointAndSegment("l_index1", "l_index_proximal", joint, segment, ret);
	createJointAndSegment("l_index2", "l_index_middle", joint, segment, ret);
	createJointAndSegment("l_index3", "l_index_distal", joint, segment, ret);
	backJointAndSegment("l_wrist", "l_hand", joint, segment, ret);
	createJointAndSegment("l_middle0", "l_middle_metacarpal", joint, segment, ret);
	createJointAndSegment("l_middle1", "l_middle_proximal", joint, segment, ret);
	createJointAndSegment("l_middle2", "l_middle_middle", joint, segment, ret);
	createJointAndSegment("l_middle3", "l_middle_distal", joint, segment, ret);
	backJointAndSegment("l_wrist", "l_hand", joint, segment, ret);
	createJointAndSegment("l_ring0", "l_ring_metacarpal", joint, segment, ret);
	createJointAndSegment("l_ring1", "l_ring_proximal", joint, segment, ret);
	createJointAndSegment("l_ring2", "l_ring_middle", joint, segment, ret);
	createJointAndSegment("l_ring3", "l_ring_distal", joint, segment, ret);
	backJointAndSegment("l_wrist", "l_hand", joint, segment, ret);
	createJointAndSegment("l_pinky0", "l_pinky_metacarpal", joint, segment, ret);
	createJointAndSegment("l_pinky1", "l_pinky_proximal", joint, segment, ret);
	createJointAndSegment("l_pinky2", "l_pinky_middle", joint, segment, ret);
	createJointAndSegment("l_pinky3", "l_pinky_distal", joint, segment, ret);
	backJointAndSegment("vt1", "t1", joint, segment, ret);
	/*|*/createJointAndSegment("r_sternoclavicular", "r_clavicle", joint, segment, ret);
	createJointAndSegment("r_acromioclavicular", "r_scapula", joint, segment, ret);
	createJointAndSegment("r_shoulder", "r_upperarm", joint, segment, ret);
	createJointAndSegment("r_elbow", "r_forearm", joint, segment, ret);
	createJointAndSegment("r_wrist", "r_hand", joint, segment, ret);
	createJointAndSegment("r_thumb1", "r_thumb_metacarpal", joint, segment, ret);
	createJointAndSegment("r_thumb2", "r_thumb_proximal", joint, segment, ret);
	createJointAndSegment("r_thumb3", "r_thumb_distal", joint, segment, ret);
	backJointAndSegment("r_wrist", "r_hand", joint, segment, ret);
	createJointAndSegment("r_index0", "r_index_metacarpal", joint, segment, ret);
	createJointAndSegment("r_index1", "r_index_proximal", joint, segment, ret);
	createJointAndSegment("r_index2", "r_index_middle", joint, segment, ret);
	createJointAndSegment("r_index3", "r_index_distal", joint, segment, ret);
	backJointAndSegment("r_wrist", "r_hand", joint, segment, ret);
	createJointAndSegment("r_middle0", "r_middle_metacarpal", joint, segment, ret);
	createJointAndSegment("r_middle1", "r_middle_proximal", joint, segment, ret);
	createJointAndSegment("r_middle2", "r_middle_middle", joint, segment, ret);
	createJointAndSegment("r_middle3", "r_middle_distal", joint, segment, ret);
	backJointAndSegment("r_wrist", "r_hand", joint, segment, ret);
	createJointAndSegment("r_ring0", "r_ring_metacarpal", joint, segment, ret);
	createJointAndSegment("r_ring1", "r_ring_proximal", joint, segment, ret);
	createJointAndSegment("r_ring2", "r_ring_middle", joint, segment, ret);
	createJointAndSegment("r_ring3", "r_ring_distal", joint, segment, ret);
	backJointAndSegment("r_wrist", "r_hand", joint, segment, ret);
	createJointAndSegment("r_pinky0", "r_pinky_metacarpal", joint, segment, ret);
	createJointAndSegment("r_pinky1", "r_pinky_proximal", joint, segment, ret);
	createJointAndSegment("r_pinky2", "r_pinky_middle", joint, segment, ret);
	createJointAndSegment("r_pinky3", "r_pinky_distal", joint, segment, ret);

	return ret;
}

const Humanoid & Humanoid::defaultHumanHumanoid()
{
	static const Humanoid ret = createDefaultHumanoid();
	return ret;
}

Humanoid::Hierarchy createDefaultHumanHierarchy()
{
	Humanoid::Hierarchy ret;
	const auto humanoid = Humanoid::defaultHumanHumanoid();

	for (auto j : humanoid.joints)
	{
		auto segments = Object::filter<Segment>(j->children);
		for (auto s : segments)
		{
			ret.insert({ j->name, s->name });
		}
	}

	for (auto s : humanoid.segments)
	{
		auto joints = Object::filter<Joint>(s->children);
		for (auto j : joints)
		{
			ret.insert({ s->name, j->name });
		}
	}

	return ret;
}

const Humanoid::Hierarchy & Humanoid::defaultHumanHierarchy()
{
	static const Hierarchy ret = createDefaultHumanHierarchy();
	return ret;
}