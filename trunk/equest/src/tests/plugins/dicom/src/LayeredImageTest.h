/********************************************************************
	created:	2014/04/24
	created:	24:4:2014   13:41
	filename: 	LayeredImageTest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM_TEST__LAYEREDIMAGETEST_H__
#define HEADER_GUARD_DICOM_TEST__LAYEREDIMAGETEST_H__


#include <cppunit/extensions/HelperMacros.h>

class LayeredImageTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(LayeredImageTest);
        CPPUNIT_TEST(testMerge);
    CPPUNIT_TEST_SUITE_END();

public:

    virtual void setUp();

    void testMerge();
};

#endif
