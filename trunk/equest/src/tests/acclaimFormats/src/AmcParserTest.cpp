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
#include <acclaimformatslib/AmcParser.h>
#include <list>
#include "AmcParserTest.h"
#include "acclaimformatslib/AsfParser.h"

const double epsilon = 0.00001;

AmcParserTest::AmcParserTest(void)
{
}

AmcParserTest::~AmcParserTest(void)
{

}

void AmcParserTest::testLoad()
{
	std::ifstream file("./testFiles/test.amc");
	acclaim::MotionData::FramesData frames = acclaim::AmcParser::parse(file);
	CPPUNIT_ASSERT_EQUAL(529, (int)frames.size());
	acclaim::MotionData::FrameData& fd = frames[0];
	CPPUNIT_ASSERT_EQUAL(1u, fd.id);
	CPPUNIT_ASSERT_EQUAL(25, (int)fd.bonesData.size());
	acclaim::MotionData::BoneData& bd = fd.bonesData[0];
	CPPUNIT_ASSERT_EQUAL(std::string("root"), bd.name);
	CPPUNIT_ASSERT_EQUAL(6, (int)bd.channelValues.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(9.60681  , (double)bd.channelValues[0], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.21614  , (double)bd.channelValues[1], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.253612, (double)bd.channelValues[2], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(93.8026  , (double)bd.channelValues[3], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-81.76	  , (double)bd.channelValues[4], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(94.1662  , (double)bd.channelValues[5], epsilon);

	acclaim::MotionData::BoneData& bd2 = *fd.bonesData.rbegin();
	CPPUNIT_ASSERT_EQUAL(std::string("ltoes"), bd2.name);
	CPPUNIT_ASSERT_EQUAL(3, (int)bd2.channelValues.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(7.4237e-014, (double)bd2.channelValues[0], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(7.57012e-014, (double)bd2.channelValues[1], epsilon);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(8.39191e-014, (double)bd2.channelValues[2], epsilon);
	
	{
		acclaim::MotionData::FrameData& fd = *frames.rbegin();
		CPPUNIT_ASSERT_EQUAL(529u, fd.id);
		CPPUNIT_ASSERT_EQUAL(25, (int)fd.bonesData.size());
		acclaim::MotionData::BoneData& bd = fd.bonesData[0];
		CPPUNIT_ASSERT_EQUAL(std::string("root"), bd.name);
		CPPUNIT_ASSERT_EQUAL(6, (int)bd.channelValues.size());
		     
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-7.58606, (double)bd.channelValues[0], epsilon);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.3173, (double)bd.channelValues[1], epsilon);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.476158, (double)bd.channelValues[2], epsilon);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(94.0948, (double)bd.channelValues[3], epsilon);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-83.513, (double)bd.channelValues[4], epsilon);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(94.5303, (double)bd.channelValues[5], epsilon);

		acclaim::MotionData::BoneData& bd2 = fd.bonesData[2];
		CPPUNIT_ASSERT_EQUAL(std::string("upperback"), bd2.name);
		CPPUNIT_ASSERT_EQUAL(3, (int)bd2.channelValues.size());
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.56871, (double)bd2.channelValues[0], epsilon);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1118, (double)bd2.channelValues[1], epsilon);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.670541, (double)bd2.channelValues[2], epsilon);
	}
}

/*
void AmcParserTest::testLoadNotExisting() {
	acclaim::AmcParser amc;
	acclaim::MotionData d;
    amc.parse(d, "NotExist.amc");
}
*/
