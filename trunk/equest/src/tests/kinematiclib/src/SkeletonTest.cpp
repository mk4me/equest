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
#include <list>
#include <boost/filesystem.hpp>
#include "SkeletonTest.h"
#include <kinematiclib/Skeleton.h>
#include <acclaimformatslib/AsfParser.h>


void SkeletonTest::test1()
{
	//acclaim::AsfParser parser;

	std::ifstream file("./testFiles/test.asf");
	acclaim::Skeleton acclaimSkeleton = acclaim::AsfParser::parse(file, true);
	kinematic::Skeleton skeleton;
	kinematic::Skeleton::convert(acclaimSkeleton, skeleton);
	//CPPUNIT_ASSERT_EQUAL(std::string("VICON"), skeleton.name);
	CPPUNIT_ASSERT_EQUAL(3, (int)skeleton.root()->children().size());
}

SkeletonTest::SkeletonTest(void)
{

}

SkeletonTest::~SkeletonTest(void)
{

}
