/********************************************************************
    created:  2011/05/13
    created:  13:5:2011   8:53
    filename: SynchronizedTest.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__SYNCHRONIZEDTEST_H__
#define HEADER_GUARD_UTILS__SYNCHRONIZEDTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <threading/Synchronized.h>
#include <OpenThreads/Thread>

class SynchronizedTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SynchronizedTest);
    CPPUNIT_TEST(testInharitanceOfClass);
    CPPUNIT_TEST(testPrimitivesPacking);
    CPPUNIT_TEST(testLock);
    CPPUNIT_TEST(testTryLock);
    CPPUNIT_TEST_SUITE_END();

public:

    void testInharitanceOfClass();
    void testPrimitivesPacking();
    void testLock();
    void testTryLock();

private:

    template<class T>
    class TestClass : public OpenThreads::Thread {

    public:

        TestClass(utils::Synchronized<T, utils::StrictSyncPolicy> & obj) : object(&obj),
            triedLock(false), locked(false) {}

        ~TestClass() {}

        bool getTryLockResult() const
        {
            return triedLock;
        }

        bool getLockedResult() const
        {
            return locked;
        }

        virtual void run()
        {
            triedLock = false;
            locked = false;
            //TRY_LOCK(*object){
            //    locked = true;
            //}

            triedLock = true;
        }

    private:

        utils::Synchronized<T, utils::StrictSyncPolicy> * object;
        bool triedLock;
        bool locked;
    };
};

#endif  //  HEADER_GUARD_UTILS__SYNCHRONIZEDTEST_H__
