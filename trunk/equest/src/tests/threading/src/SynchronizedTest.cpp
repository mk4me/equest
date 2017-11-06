#include "SynchronizedTest.h"
#include <boost/type_traits.hpp>
using namespace utils;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( SynchronizedTest );

class EmptyClass
{
public:

    EmptyClass() {};
    ~EmptyClass() {};

    int getX() const
    {
        return 3;
    }
};

void SynchronizedTest::testInharitanceOfClass()
{
    typedef Synchronized<EmptyClass, utils::StrictSyncPolicy> SynchEC;
    bool isDerived = boost::is_base_of<EmptyClass, SynchEC>::value;
        
    CPPUNIT_ASSERT(isDerived == true);

    SynchEC x;
    EmptyClass * ptr = dynamic_cast<EmptyClass*>(&x);
    CPPUNIT_ASSERT(ptr != nullptr);
    CPPUNIT_ASSERT(ptr->getX() == 3);
    CPPUNIT_ASSERT(x.getX() == ptr->getX());
}

void SynchronizedTest::testPrimitivesPacking()
{
    typedef Synchronized<int, utils::StrictSyncPolicy> SynchInt;
    CPPUNIT_ASSERT(boost::is_scalar<SynchInt>::value == false);

    SynchInt x;
    x = 5;
    x *= 3;

    CPPUNIT_ASSERT(x == 15);
    
}

void SynchronizedTest::testLock()
{
    typedef Synchronized<int, utils::StrictSyncPolicy> SynchInt;

    SynchInt x;
    x = 0;

    TestClass<int> testThreadInvoker(x);

    testThreadInvoker.start();
    testThreadInvoker.join();

    CPPUNIT_ASSERT(testThreadInvoker.getTryLockResult() == true);
    CPPUNIT_ASSERT(testThreadInvoker.getLockedResult() == true);

}

void SynchronizedTest::testTryLock()
{
    typedef Synchronized<int, utils::StrictSyncPolicy> SynchInt;

    SynchInt x;
    x = 0;

    TestClass<int> testThreadInvoker(x);

    //LOCK(x){
    //    testThreadInvoker.start();
    //    testThreadInvoker.join();
    //}

    CPPUNIT_ASSERT(testThreadInvoker.getTryLockResult() == true);
    CPPUNIT_ASSERT(testThreadInvoker.getLockedResult() == false);

}
