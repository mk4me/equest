#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main( int argc, char **argv)
{
	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest( registry.makeTest() );

	// Change the default outputter to a compiler error format outputter
	runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
		std::cerr ) );

	bool wasSuccessful = runner.run();
	return wasSuccessful == true ? 0 : 1;
}
