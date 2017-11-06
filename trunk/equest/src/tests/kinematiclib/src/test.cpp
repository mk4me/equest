#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "JointTest.h"
#include "QuaternionManipulatorsTest.h"
#include "SkeletalDataTest.h"
#include "SkeletonStateTest.h"
#include "SkeletonTest.h"
#include "Viewer.h"

CPPUNIT_TEST_SUITE_REGISTRATION(JointTest);
CPPUNIT_TEST_SUITE_REGISTRATION(QuaternionManipulatorsTest);
CPPUNIT_TEST_SUITE_REGISTRATION(SkeletalDataTest);
CPPUNIT_TEST_SUITE_REGISTRATION(SkeletonStateTest);
CPPUNIT_TEST_SUITE_REGISTRATION(SkeletonTest);


int main(int argc, char* argv[])
{
	//kinematicTest::Viewer::start();
    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( suite );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
        std::cerr ) );
    // Run the tests.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.
    //system("pause");
    return wasSucessful ? 0 : 1;
}
