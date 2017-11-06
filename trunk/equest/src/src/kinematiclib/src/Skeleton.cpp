#include <kinematiclib/Skeleton.h>
#include <acclaimformatslib/DegreeOfFreedom.h>
#include <kinematicUtils/RotationConverter.h>
#include <set>
#include <deque>
#include <stdexcept>
#include <utils/Debug.h>
#include <treecontainerlib/NodeT.h>
#include <treecontainerlib/TreeT.h>
#include <osg/PositionAttitudeTransform>
#include "loglib/Logger.h"
#include "loglib/Exceptions.h"

using namespace kinematic;

osg::Vec3d correctNegativeZero(const osg::Vec3 & value)
{
	auto ret(value);
	for (unsigned char i = 0; i < 3; ++i)
	{
		ret[i] = utils::correctNegativeZero(ret[i]);
	}

	return ret;
}

class Skeleton::JointData::JointDataImpl
{
private:

	static osg::Matrix PATMatrix(osg::PositionAttitudeTransform * pat)
	{
		UTILS_ASSERT(pat != nullptr);
		osg::Matrix ret;
		ret.makeIdentity();
		ret.makeTranslate(pat->getPosition());
		ret.makeRotate(pat->getAttitude());
		ret.makeScale(1, 1, 1);

		return ret;
	}

	static osg::Matrix nodeGlobalMatrix(osg::Node * node)
	{
		osg::Matrix ret;

		UTILS_ASSERT(node != nullptr);

		auto wm = node->getWorldMatrices();

		UTILS_ASSERT(wm.size() < 2);

		if (wm.empty() == false){
			ret = wm[0];
		}
		else{
			auto pat = dynamic_cast<osg::PositionAttitudeTransform*>(node);
			UTILS_ASSERT(pat != nullptr);
			ret = PATMatrix(pat);
		}

		return ret;
	}

	osg::Matrix parentGlobalMatrix() const
	{
		osg::Matrix ret;

		UTILS_ASSERT(node != nullptr);
		UTILS_ASSERT(node->getNumParents() < 2);

		if (node->getNumParents() == 1){
			ret = nodeGlobalMatrix(node->getParent(0));
		}
		else{
			ret.makeRotate(osg::Quat(0, 0, 0, 1));
			ret.makeTranslate(0, 0, 0);
			ret.makeScale(1, 1, 1);
		}

		return ret;
	}

	osg::Matrix myGlobalMatrix() const
	{
		UTILS_ASSERT(node != nullptr);
		return nodeGlobalMatrix(node);
	}

	osg::Quat parentGlobalOrientation() const
	{
		return parentGlobalMatrix().getRotate();
	}

	osg::Vec3d parentGlobalPosition() const
	{
		return parentGlobalMatrix().getTrans();
	}

public:

	JointDataImpl(const JointDataImpl & other)
		: node(new osg::PositionAttitudeTransform(*(other.node), osg::CopyOp::DEEP_COPY_ALL))
	{

	}

	JointDataImpl(const std::string & name, const osg::Vec3d & translation,
		const osg::Quat & rotation) : node(new osg::PositionAttitudeTransform)
	{
		node->setName(name);
		node->setPosition(translation);
		node->setAttitude(rotation);
		node->setScale(osg::Vec3(1, 1, 1));
	}

	JointDataImpl(JointDataImpl & parent, const std::string & name,
		const osg::Vec3d & translation, const osg::Quat & rotation) :
		JointDataImpl(name, translation, rotation)
	{
		parent.node->addChild(node);
	}

	JointDataImpl(JointDataImpl & parent, const JointDataImpl & other)
		: node(new osg::PositionAttitudeTransform(*(other.node), osg::CopyOp::DEEP_COPY_ALL))
	{
		parent.node->addChild(node);
	}

	~JointDataImpl()
	{

	}

	std::string name() const
	{
		return node->getName();
	}

	osg::Vec3d localPosition() const
	{
		return node->getPosition();
	}

	osg::Vec3d globalPosition() const
	{
		return myGlobalMatrix().getTrans();
	}

	osg::Quat localOrientation() const
	{
		return node->getAttitude();
	}

	//! \return Globalna orientacja
	osg::Quat globalOrientation() const
	{
		return myGlobalMatrix().getRotate();
	}

	//! \param translation Lokalne przesunięcie
	void localUpdate(const osg::Vec3d & translation)
	{
		node->setPosition(node->getPosition() + translation);
	}

	void localUpdate(const osg::Quat & rotation)
	{
		node->setAttitude(rotation);
	}

	void globalUpdate(const osg::Vec3d & translation)
	{
		setGlobal(translation);
	}

	void globalUpdate(const osg::Quat & rotation)
	{
		setGlobal(rotation);
	}

	//! \param position Globalna pozycja
	void setGlobal(const osg::Vec3d & position)
	{
		node->setPosition(position - parentGlobalPosition());
	}

	//! \param position Globalna pozycja
	void setLocal(const osg::Vec3d & position)
	{
		node->setPosition(position);
	}

	//! \param translation Lokalne przesunięcie
	//! \param rotation Lokalna rotacja
	void localUpdate(const osg::Vec3d & translation, const osg::Quat & rotation)
	{
		localUpdate(translation);
		localUpdate(rotation);
	}

	//! \param orientation Globalna orientacja
	void setGlobal(const osg::Quat & orientation)
	{
		node->setAttitude(orientation * parentGlobalOrientation().inverse());
		//TODO weryfikacja w testach ze to dziala
		//UTILS_ASSERT(std::fabs((orientation * parentGlobalOrientation().inverse()).w()) < osg::DegreesToRadians(0.05));
	}
	//! \param orientation Globalna orientacja
	void setLocal(const osg::Quat & orientation)
	{
		node->setAttitude(orientation);
	}

	//! \param translation Lokalne przesunięcie
	//! \param rotation Lokalna rotacja
	void globalUpdate(const osg::Vec3d & translation, const osg::Quat & rotation)
	{
		globalUpdate(translation);
		globalUpdate(rotation);
	}
	//! \param position Globalna pozycja
	//! \param orientation Globalna orientacja
	void setGlobal(const osg::Vec3d & position, const osg::Quat & orientation)
	{
		setGlobal(position);
		setGlobal(orientation);
	}

	//! \param position Globalna pozycja
	//! \param orientation Globalna orientacja
	void setLocal(const osg::Vec3d & position, const osg::Quat & orientation)
	{
		setLocal(position);
		setLocal(orientation);
	}
private:
	//! Węzeł osg
	osg::ref_ptr<osg::PositionAttitudeTransform> node;
};

Skeleton::JointData::JointData(const JointData & other)
	: impl(new JointDataImpl(*(other.impl)))
{

}

Skeleton::JointData::JointData(const std::string & name, const osg::Vec3d & translation, const osg::Quat & rotation)
	: impl(new JointDataImpl(name, translation, rotation))
{

}
			
Skeleton::JointData::JointData(JointData & parent, const std::string & name,
	const osg::Vec3d & translation, const osg::Quat & rotation)
	: impl(new JointDataImpl(*(parent.impl), name, translation, rotation))
{
	
}

Skeleton::JointData::JointData(JointData & parent, const JointData & other)
	: impl(new JointDataImpl(*(parent.impl), *(other.impl)))
{

}

Skeleton::JointData::JointData(JointData && Other) : impl(std::move(Other.impl))
{

}

Skeleton::JointData::~JointData()
{

}

std::string Skeleton::JointData::name() const
{
	return impl->name();
}

osg::Vec3d Skeleton::JointData::localPosition() const
{
	return impl->localPosition();
}

osg::Vec3d Skeleton::JointData::globalPosition() const
{
	return impl->globalPosition();
}
void Skeleton::JointData::localUpdate(const osg::Vec3d & translation, const osg::Quat & rotation)
{
	impl->localUpdate(translation, rotation);
}

void Skeleton::JointData::globalUpdate(const osg::Vec3d & translation, const osg::Quat & rotation)
{
	impl->globalUpdate(translation, rotation);
}

osg::Quat Skeleton::JointData::localOrientation() const
{
	return impl->localOrientation();
}

osg::Quat Skeleton::JointData::globalOrientation() const
{
	return impl->globalOrientation();
}

void Skeleton::JointData::localUpdate(const osg::Vec3 & translation)
{
	impl->localUpdate(translation);
}

void Skeleton::JointData::globalUpdate(const osg::Vec3d & translation)
{
	impl->globalUpdate(translation);
}

void Skeleton::JointData::setGlobal(const osg::Vec3d & translation)
{
	impl->setGlobal(translation);
}

void Skeleton::JointData::setLocal(const osg::Vec3d & position)
{
	impl->setLocal(position);
}

void Skeleton::JointData::localUpdate(const osg::Quat & rotation)
{
	impl->localUpdate(rotation);
}

void Skeleton::JointData::globalUpdate(const osg::Quat & rotation)
{
	impl->localUpdate(rotation);
}

void Skeleton::JointData::setGlobal(const osg::Quat & orientation)
{
	impl->setGlobal(orientation);
}

void Skeleton::JointData::setLocal(const osg::Quat & orientation)
{
	impl->setLocal(orientation);
}

void Skeleton::JointData::setGlobal(const osg::Vec3d & position, const osg::Quat & orientation)
{
	impl->setGlobal(position, orientation);
}

void Skeleton::JointData::setLocal(const osg::Vec3d & position, const osg::Quat & orientation)
{
	impl->setLocal(position, orientation);
}

Skeleton::Skeleton()
{

}

Skeleton::JointPtr createJoint(Skeleton::JointPtr parent, Skeleton::JointConstPtr joint)
{
	auto ret = Skeleton::Joint::create(Skeleton::JointData(parent->value(), joint->value()));

	for (const auto & c : joint->children())
	{
		Skeleton::Joint::add(ret, createJoint(ret, c));
	}

	return ret;
}

Skeleton::Skeleton(const Skeleton & other)
	: root_(Joint::create(other.root_->value()))
{
	for (const auto & c : other.root()->children())
	{
		Joint::add(root_, createJoint(root_, c));
	}
}

Skeleton::Skeleton(Skeleton && other)
	: root_(std::move(other.root_))
{

}

Skeleton::Skeleton(JointPtr && root) : root_(std::move(root))
{

}

Skeleton::~Skeleton()
{

}

Skeleton::JointPtr Skeleton::root()
{
	return root_;
}

Skeleton::JointConstPtr Skeleton::root() const
{
	return root_;
}

Skeleton & Skeleton::operator = (Skeleton && Other)
{
	root_ = std::move(Other.root_);
	return *this;
}

Skeleton & Skeleton::operator=(const Skeleton & Other)
{
	if (this != &Other){
		root_ = Skeleton(Other).root_;
	}
	return *this;
}

bool verify(const Skeleton & skeleton)
{
	if (skeleton.root() == nullptr){
		return true;
	}

	std::set<std::string> uniqueNames;
	auto fun = [&uniqueNames](Skeleton::JointConstPtr joint)
		{
			if (joint == nullptr){
				return false;
			}

			return uniqueNames.insert(joint->value().name()).second;
		};
	return treeContainer::VisitPolicies::Node::PreOrder::visitWhile(skeleton.root(), fun);
}

void createJoint(Skeleton::JointPtr parentJoint, TopologyNodeConstPtr topologyJoint)
{
	for (const auto & tn : topologyJoint->children())
	{
		auto rj = Skeleton::Joint::add(parentJoint, Skeleton::JointData(parentJoint->value(), tn->value().name));
		createJoint(rj, tn);
	}
}

bool Skeleton::convert(TopologyNodeConstPtr topology, Skeleton & skeleton)
{
	Skeleton localSkeleton;

	if (topology != nullptr){
		localSkeleton.root_ = Joint::create(topology->value().name);
		createJoint(localSkeleton.root_, topology);
	}

	bool ret = verify(localSkeleton);

	if (ret == true){
		skeleton = std::move(localSkeleton);
	}

	return ret;
}

bool createJoint(Skeleton::JointPtr parentJoint, const acclaim::Skeleton & skeleton,
	const osg::Vec3& parentPos,	const acclaim::Bone& currentBone,
	std::set<std::string> & names)
{
	if (names.find(currentBone.name) != names.end()){
		return false;
	}

	bool ret = true;

	names.insert(currentBone.name);	

	auto joint = Skeleton::Joint::add(parentJoint, Skeleton::JointData(parentJoint->value(), currentBone.name, parentPos));
	auto range = skeleton.hierarchy.left.equal_range(currentBone.id);	
	const auto nextPos = currentBone.direction * currentBone.length;
	//taka jest konwencja ASF - podajemy przesuniecie rodzica, dla wyjaśnienia spojrzeć na tworzenie liści hierarchi
	for (auto it = range.first; (it != range.second) && (ret == true); ++it){
		ret = createJoint(joint, skeleton, nextPos, skeleton.bones.find(it->second)->second, names);
	}
	//przerywamy rekurencje tworzenia hierarchi
	static const int TERMINATOR = -0xdead;
	if ((ret == true) && (range.first == range.second) && (currentBone.id != TERMINATOR)) {
		acclaim::Bone b;
		b.id = TERMINATOR;
		b.name = currentBone.name + "_leaf";
		ret = createJoint(joint, skeleton, nextPos, b, names);
	}

	return ret;
}

bool Skeleton::convert(const acclaim::Skeleton & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.bones.empty()){
		return false;
	}

	bool ret = true;

	std::set<std::string> names;

	//root
	Skeleton::JointData jointData("root", srcSkeleton.position, kinematicUtils::convert(srcSkeleton.orientation, srcSkeleton.axisOrder));
	names.insert(jointData.name());

	auto joint = Skeleton::Joint::create(jointData);
	auto range = srcSkeleton.hierarchy.left.equal_range(srcSkeleton.root);
	for (auto it = range.first; (it != range.second) && (ret == true); ++it) {
		//ret = createJoint(joint, srcSkeleton, srcSkeleton.position, srcSkeleton.bones.find(it->second)->second, names);
		ret = createJoint(joint, srcSkeleton, osg::Vec3d(0,0,0), srcSkeleton.bones.find(it->second)->second, names);
	}

	if (joint->isLeaf() == true || ret == false){
		return false;
	}
				
	destSkeleton.root_ = joint;
		
	return true;
}

kinematicUtils::AxisOrder::Type findJointAxis(const std::string & jointName,
	const Skeleton::JointsAxis & jointsAxis, kinematicUtils::AxisOrder::Type defaultJointAxis)
{
	auto it = jointsAxis.find(jointName);
	if (it != jointsAxis.end()){
		return it->second;
	}

	return defaultJointAxis;
}

bool Skeleton::convert(const Skeleton & srcSkeleton, acclaim::Skeleton & destSkeleton,
	const kinematicUtils::AxisOrder::Type axisOrder,
	const kinematicUtils::AngleUnitType angleUnit,
	const JointsAxis & jointsAxis)
{
	if (srcSkeleton.root_ == nullptr){
		return false;
	}

	try{
		acclaim::Skeleton localSkeleton;
		localSkeleton.name = "kinematicSkeleton";
		localSkeleton.axisOrder = axisOrder;
		localSkeleton.units.setAngleType(angleUnit);
		localSkeleton.position = correctNegativeZero(srcSkeleton.root_->value().localPosition());
		localSkeleton.orientation = kinematicUtils::convert(srcSkeleton.root_->value().localOrientation(), axisOrder);

		if (angleUnit == kinematicUtils::Deg){
			localSkeleton.orientation = kinematicUtils::toDegrees(localSkeleton.orientation);
		}

		localSkeleton.orientation = correctNegativeZero(localSkeleton.orientation);

		acclaim::Bone::ID id = 0;

		acclaim::Bone bone;
		bone.id = id;
		bone.name = "root";
		bone.axis = localSkeleton.orientation;
		bone.axisOrder = localSkeleton.axisOrder;
		bone.direction = localSkeleton.position;
		bone.direction.normalize();
		bone.length = 0.0;
		
		localSkeleton.bones.insert({ bone.id, bone }).first;
		localSkeleton.root = bone.id;

		for (const auto & c : srcSkeleton.root_->children()){
			auto vis = [&id, &localSkeleton,
						&jointsAxis, axisOrder, angleUnit](Skeleton::JointPtr joint)
					{
						if (joint == nullptr){
							throw std::runtime_error("Uninitialized joint");
						}

						if (joint->isLeaf() == true){
							return;
						}

						auto parentID = localSkeleton.root;

						acclaim::Bone bone;
						bone.name = joint->value().name();
						bone.axisOrder = findJointAxis(bone.name, jointsAxis, axisOrder);

						const auto pc = joint->parent()->children();

						if (pc.size() > 1){

							auto it = pc.begin();

							const auto pos = (*it)->value().localPosition();
							++it;

							bool ok = true;

							for (; it != pc.end(); ++it)
							{
								if (pos != (*it)->value().localPosition()){
									ok = false;
									break;
								}
							}

							//sprawdzamy czy nie trzeba dodać dummy bone!
							//tylko wtedy kiedy nasza pozycja jest inna niż pozycja rodzica a ma on więcej dzieci
							if (ok == false){

								acclaim::Bone dummyBone;
								dummyBone.name = "dummy_" + bone.name;
								dummyBone.axis = osg::Vec3d(0, 0, 0);
								dummyBone.axisOrder = findJointAxis(dummyBone.name, jointsAxis, axisOrder);
								dummyBone.direction = joint->value().localPosition();
								dummyBone.length = dummyBone.direction.length();
								dummyBone.direction.normalize();
								dummyBone.id = id + 1;

								localSkeleton.bones.insert(localSkeleton.bones.end(), { dummyBone.id, dummyBone });

								if (joint->parent()->isRoot() == false){
									auto parentName = joint->parent()->value().name();
									auto it = std::find_if(localSkeleton.bones.begin(), localSkeleton.bones.end(),
										[&parentName](const acclaim::Skeleton::Bones::value_type & bd)
									{
										return bd.second.name == parentName;
									});

									if (it == localSkeleton.bones.end()){
										throw std::runtime_error("Could not find parent bone");
									}

									parentID = it->first;
								}

								localSkeleton.hierarchy.left.insert({ parentID, dummyBone.id });
								id = dummyBone.id;

								parentID = dummyBone.id;
							}

						}
						
						if (joint->children().size() == 1){
							bone.direction = correctNegativeZero(joint->children().front()->value().localPosition());
							//bone.direction = correctNegativeZero(joint->value().localPosition());
							bone.length = bone.direction.length();
							bone.direction.normalize();
						}
						else{
							bone.length = 0.0;
						}						

						//TODO
						/*
						bone.axis = osg::Vec3(0, 0, 0);
						bone.axis = kinematicUtils::convert(joint->value().orientation, bone.axisOrder);
						if (angleUnit == kinematicUtils::Deg){
							bone.axis = kinematicUtils::toDegrees(bone.axis);
						}*/
						bone.id = id + 1;

						localSkeleton.bones.insert(localSkeleton.bones.end(), { bone.id, bone });

						if (parentID == localSkeleton.root){

							if (joint->parent()->isRoot() == false){
								auto parentName = joint->parent()->value().name();
								auto it = std::find_if(localSkeleton.bones.begin(), localSkeleton.bones.end(),
									[&parentName](const acclaim::Skeleton::Bones::value_type & bd)
								{
									return bd.second.name == parentName;
								});

								if (it == localSkeleton.bones.end()){
									throw std::runtime_error("Could not find parent bone");
								}

								parentID = it->first;
							}
						}

						localSkeleton.hierarchy.left.insert({ parentID, bone.id });
						id = bone.id;
					};
			treeContainer::VisitPolicies::Node::PreOrder::visit(c, vis);
		}

		destSkeleton = localSkeleton;
	}
	catch (...){
		return false;
	}

	return true;
}

bool createJoint(Skeleton::JointPtr parentJoint, const biovision::JointPtr & srcJoint,
	std::set<std::string> & names)
{
	if (names.find(srcJoint->name) != names.end()){
		return false;
	}

	bool ret = true;

	auto joint = Skeleton::Joint::add(parentJoint, Skeleton::JointData(srcJoint->name, srcJoint->offset));
	names.insert(srcJoint->name);

	for (auto j : srcJoint->joints)
	{
		if ((ret = createJoint(joint, j, names)) == false){
			break;
		}
	}

	return ret;
}

bool Skeleton::convert(const biovision::Skeleton & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.root == nullptr){
		return false;
	}

	std::set<std::string> names;

	Skeleton::JointData jointData("root");

	names.insert(jointData.name());

	auto root = Skeleton::Joint::create(jointData);

	if (createJoint(root, srcSkeleton.root, names) == true){
		destSkeleton.root_ = root;
		return true;
	}

	return false;
}

bool Skeleton::convert(const hAnim::Humanoid & srcSkeleton, Skeleton & destSkeleton)
{
	if (srcSkeleton.joints.empty() == true){
		return false;
	}	

	/*
	//TODO
	*/

	return true;
}
