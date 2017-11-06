#include <sqliteUtils/SQLiteDB.h>
#include <string>
#include <utils/Macros.h>
#include <utils/Utils.h>
#include <stdio.h>
#include "SqlCipherTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( NetworkUtilsTest );

void NetworkUtilsTest::testOpen()
{
   sqlite3 *db;   
   int rc;

   rc = sqlite3_open("test.db", &db);

   CPPUNIT_ASSERT(  rc == SQLITE_DONE || rc == SQLITE_OK );
   sqlite3_close(db);
}

void NetworkUtilsTest::testCrypto()
{
    const unsigned int sqliteExecWaitMS = 50;
   const unsigned int maxSqliteExecTries = 2400;

   std::string path = "testCrypto.db";
   std::string key = "P,j.W/s<T>k2:0\"1;2";
   std::remove(path.c_str());
   sqliteUtils::UniqueWrapperT<sqlite3> db(sqliteUtils::SQLiteDB::open(path, key, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE));// , sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS));
   CPPUNIT_ASSERT( db != nullptr );
   auto ret = sqliteUtils::SQLiteDB::exec(db, "CREATE TABLE files_table (file_name TEXT PRIMARY KEY, file BLOB);", maxSqliteExecTries, sqliteExecWaitMS);
   CPPUNIT_ASSERT( ret == SQLITE_DONE || ret == SQLITE_OK );
   auto keyRet = sqlite3_key(db, key.c_str(), key.size());
   CPPUNIT_ASSERT( keyRet == SQLITE_OK);

   CPPUNIT_ASSERT(sqlite3_exec(db, "SELECT count(*) FROM sqlite_master;", NULL, NULL, NULL) == SQLITE_OK);

}

