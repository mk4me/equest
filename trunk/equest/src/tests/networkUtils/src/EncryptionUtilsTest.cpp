#include "EncryptionUtilsTest.h"
#include <networkUtils/EncryptionUtils.h>
#include <cppunit/TestAssert.h>

using namespace networkUtils::EncryptionUtils;

EncryptionUtilsTest::EncryptionUtilsTest(void)
{

}

EncryptionUtilsTest::~EncryptionUtilsTest(void)
{

}

void EncryptionUtilsTest::encryptTest()
{
	QString s("test");
	EncryptionKey key1A = { "key1key1key1key1" };
	EncryptionKey key2A = { "key2key2key2key2" };
	EncryptionKey key1B = { "key1key1key1key1" };
	EncryptionKey key2B = { "key2key2key2key2" };

	CPPUNIT_ASSERT(encrypt(s, key1A, key1B) != encrypt(s, key2A, key2B));
	CPPUNIT_ASSERT(encrypt(s, key1A, key1B) != encrypt(s, key1A, key2B));
	CPPUNIT_ASSERT(encrypt(s, key1A, key1B) != encrypt(s, key2A, key1B));
}

void EncryptionUtilsTest::decryptTest()
{

	QString s("test");
	EncryptionKey key1A = { "key1key1key1key1" };
	EncryptionKey key2A = { "key2key2key2key2" };
	EncryptionKey key1B = { "key1key1key1key1" };
	EncryptionKey key2B = { "key2key2key2key2" };

	CPPUNIT_ASSERT(decrypt(s, key1A, key1B) != decrypt(s, key2A, key2B));
	CPPUNIT_ASSERT(decrypt(s, key1A, key1B) != decrypt(s, key1A, key2B));
	CPPUNIT_ASSERT(decrypt(s, key1A, key1B) != decrypt(s, key2A, key1B));
}

void EncryptionUtilsTest::encryptDecryptTest()
{
	QString s("test");
	EncryptionKey key1 = {"key1key1key1key1"};
	EncryptionKey key2 = {"key2key2key2key2"};

	CPPUNIT_ASSERT(s == decrypt(encrypt(s)));
	CPPUNIT_ASSERT(s == decrypt(encrypt(s, key1, key2), key1, key2));
	s = "12345";	 
	CPPUNIT_ASSERT(s == decrypt(encrypt(s)));
	CPPUNIT_ASSERT(s == decrypt(encrypt(s, key1, key2), key1, key2));
	s = "A";		 
	CPPUNIT_ASSERT(s == decrypt(encrypt(s)));
	CPPUNIT_ASSERT(s == decrypt(encrypt(s, key1, key2), key1, key2));
}

void EncryptionUtilsTest::invalidArgumentsTest()
{
	CPPUNIT_ASSERT_THROW(encrypt(QString()), std::invalid_argument);
	CPPUNIT_ASSERT_THROW(decrypt(QString()), std::invalid_argument);
}
