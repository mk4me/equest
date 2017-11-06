/********************************************************************
    created:  2015/12/07 20:14
    filename: EncryptionUtilsTest.h
    author:   Wojciech Knieæ
    
    purpose: 
*********************************************************************/
#ifndef HEADER_GUARD_NETWORKUTILS_TEST__EncryptionUtilsTest_h__
#define HEADER_GUARD_NETWORKUTILS_TEST__EncryptionUtilsTest_h__

#include <cppunit/extensions/HelperMacros.h>


class EncryptionUtilsTest : public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(EncryptionUtilsTest);
	CPPUNIT_TEST(encryptTest);
	CPPUNIT_TEST(decryptTest);
	CPPUNIT_TEST(encryptDecryptTest);
	CPPUNIT_TEST(invalidArgumentsTest);
	CPPUNIT_TEST_SUITE_END();
public:
	EncryptionUtilsTest(void);
	virtual ~EncryptionUtilsTest(void);

public:
	void encryptTest();
	void decryptTest();
	void encryptDecryptTest();
	void invalidArgumentsTest();
};
#endif
