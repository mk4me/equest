#include <limits>
#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <osg/Vec3d>
#include <osg/Quat>
#include <boost/shared_ptr.hpp>
#include <acclaimformatslib/AsfParser.h>
#include <list>
#include "AsfParserTest.h"
#include "acclaimformatslib/Bone.h"

const double epsilon = 0.0000001;

AsfParserTest::AsfParserTest(void) :
skeleton(nullptr)
{
}

AsfParserTest::~AsfParserTest(void)
{

}


void AsfParserTest::setUp()
{
	std::ifstream file("./testFiles/test.asf");
	skeleton.reset(new acclaim::Skeleton(acclaim::AsfParser::parse(file, true)));
}

void AsfParserTest::tearDown()
{

}

void AsfParserTest::testLoadMetadata()
{
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Asf name test", std::string("VICON"), skeleton->name);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Asf version test", std::string("1.10"), skeleton->version);
}

//void AsfParserTest::testLoadNotExisting() {
//    acclaim::AsfParser asf;
//    asf.parse("NotExist.asf");
//}

void AsfParserTest::testLoadUnits()
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, skeleton->units.defaultValues["mass"], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1, skeleton->units.defaultValues["length"], epsilon);
	CPPUNIT_ASSERT_EQUAL(std::string("deg"), skeleton->units.typeValueUnits["angle"]);
}

void AsfParserTest::testRoot()
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, skeleton->position.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, skeleton->position.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, skeleton->position.z(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, skeleton->orientation.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, skeleton->orientation.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, skeleton->orientation.z(), epsilon);
	CPPUNIT_ASSERT_EQUAL(kinematicUtils::AxisOrder::XYZ, skeleton->axisOrder);
	typedef kinematicUtils::ChannelType CT;
	std::vector<kinematicUtils::Channel> order{ CT::TX, CT::TY, CT::TZ, CT::RZ, CT::RY, CT::RX };
	CPPUNIT_ASSERT(order == skeleton->dataOrder);
}

void AsfParserTest::testHierarchy()
{
	auto& left = skeleton->hierarchy.left;
	auto it = left.find(skeleton->root);
	std::set<int> children;
	for (; it != left.end(); ++it) { children.insert(it->second); }
	CPPUNIT_ASSERT(children.count(1) == 1);
	CPPUNIT_ASSERT(children.count(6) == 1);
	CPPUNIT_ASSERT(children.count(11) == 1);
	CPPUNIT_ASSERT(skeleton->bones[1].name == "lhipjoint");
	CPPUNIT_ASSERT(skeleton->bones[6].name == "rhipjoint");
	CPPUNIT_ASSERT(skeleton->bones[11].name == "lowerback");

	it = left.find(9); // 9 = rfoot, 10 = rtoes
	CPPUNIT_ASSERT(it != left.end() && it->second == 10);

	auto& right = skeleton->hierarchy.right;
	auto itr = right.find(10); // 9 = rfoot, 10 = rtoes
	CPPUNIT_ASSERT(itr != right.end() && itr->second == 9);
						  
	itr = right.find(1);  
	CPPUNIT_ASSERT(itr != right.end() && itr->second == skeleton->root);
	itr = right.find(6);  
	CPPUNIT_ASSERT(itr != right.end() && itr->second == skeleton->root);
	itr = right.find(11); 
	CPPUNIT_ASSERT(itr != right.end() && itr->second == skeleton->root);
}

void AsfParserTest::testBones()
{
	CPPUNIT_ASSERT_EQUAL(acclaim::Skeleton::Bones::size_type(27), skeleton->bones.size());
}

void AsfParserTest::testSingleBone()
{
	auto it = skeleton->bones.find(4);
	CPPUNIT_ASSERT(it != skeleton->bones.end());
	acclaim::Bone b = it->second;
	CPPUNIT_ASSERT_EQUAL(4, b.id);
	CPPUNIT_ASSERT_EQUAL(std::string("lfoot"), b.name);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(7.05258e-012, b.direction.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.425936, b.direction.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.904753, b.direction.z(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.449453, b.length, epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-90, b.axis.x(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.73656e-015, b.axis.y(), epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10, b.axis.z(), epsilon);
	CPPUNIT_ASSERT_EQUAL(kinematicUtils::AxisOrder::XYZ, b.axisOrder);

	acclaim::DegreeOfFreedom x(kinematicUtils::ChannelType::RX, -180.0, 180.0);
	acclaim::DegreeOfFreedom y(kinematicUtils::ChannelType::RY, -180.0, 180.0);
	acclaim::DegreeOfFreedom z(kinematicUtils::ChannelType::RZ, -180.0, 180.0);
	std::vector<acclaim::DegreeOfFreedom> dofs{ x, y, z };
	CPPUNIT_ASSERT(dofs == b.dofs);

	//kinematicUtils::AxisOrder::Type rotationOrder;
}

