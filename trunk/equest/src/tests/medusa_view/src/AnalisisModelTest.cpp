#include "AnalisisModelTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( AnalisisModelTest );

void AnalisisModelTest::setUp()
{
    model = utils::make_shared<AnalisisModel>();
}

void AnalisisModelTest::tearDown()
{
    model = AnalisisModelPtr();
}

void AnalisisModelTest::testTree()
{
    model->applyFilter(core::IFilterCommandPtr());
    CPPUNIT_ASSERT(true);
}


