#include <sqliteUtils/SQLiteBLOB.h>
#include <thread>

using namespace sqliteUtils;

SQLiteBLOB::Close::Close(const unsigned int retriesCount,
	const unsigned int stepWaitTime)
	: retriesCount(retriesCount), stepWaitTime(stepWaitTime)
{

}

bool SQLiteBLOB::Close::operator()(sqlite3_blob * blob)
{
	unsigned int tries = 0;
	bool retry = false;
	do{

		retry = !SQLiteBLOB::close(blob);

		if ((retry == true) && (retriesCount > 0) && (stepWaitTime > 0)){
			std::this_thread::sleep_for(std::chrono::milliseconds(stepWaitTime));			
		}

	} while ((tries++ <= retriesCount) && (retry == true));

	return !retry;
}

sqlite3_blob * SQLiteBLOB::open(sqlite3 * db, const std::string & tableName,
	const std::string & columnName, const sqlite3_int64 rowID,
	const int flags, const std::string & dbName)
{
	sqlite3_blob * ret = nullptr;	
	int rc = sqlite3_blob_open(db, dbName.c_str(), tableName.c_str(),
			columnName.c_str(), rowID, flags, &ret);

	if (rc != SQLITE_OK){
		close(ret);
		ret = nullptr;
	}

	return ret;
}

bool SQLiteBLOB::close(sqlite3_blob * blob)
{
	return sqlite3_blob_close(blob) == SQLITE_OK;
}