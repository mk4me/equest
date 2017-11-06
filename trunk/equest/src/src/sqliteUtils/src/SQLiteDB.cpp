#include <sqliteUtils/SQLiteDB.h>
#include <sqliteUtils/SQLitePreparedStatement.h>
#include <thread>

using namespace sqliteUtils;

SQLiteDB::Close::Close(const unsigned int maxStatements,
	const unsigned int retriesCount, const unsigned int stepWaitTime)
	: maxStatements(maxStatements), retriesCount(retriesCount),
	stepWaitTime(stepWaitTime)
{

}

bool SQLiteDB::Close::operator()(sqlite3 * db)
{
	bool ret = SQLiteDB::close(db);
	if (ret == false){

		unsigned int tries = 0;		
		
		while (tries++ < retriesCount && SQLiteDB::finalizeStatements(db, maxStatements, retriesCount, stepWaitTime) == true && sqlite3_next_stmt(db, nullptr) != nullptr) {}

		ret = SQLiteDB::close(db);
	}

	return ret;
}

bool SQLiteDB::close(sqlite3 * db)
{
	bool ret = false;
	if (db != nullptr){
		ret = (sqlite3_close(db) == SQLITE_OK);
	}

	return ret;
}

bool SQLiteDB::finalizeStatements(sqlite3 * db, const unsigned int maxStatements,
	const unsigned int retriesCount, const unsigned int stepWaitTime)
{
	unsigned int tries = 0;
	bool done = true;
	sqlite3_stmt * stmt = nullptr;
	SQLitePreparedStatement::Finalizer f(retriesCount, stepWaitTime);
	while ((stmt = sqlite3_next_stmt(db, nullptr)) != nullptr && tries++ < maxStatements && done == true)
	{
		// don't trap, can't handle it anyway
		done = f(stmt);
	}

	return done;
}

int SQLiteDB::exec(sqlite3 * db, const std::string & query, unsigned int retriesCount, unsigned int stepWaitTime)
{
	auto rc = SQLITE_OK;
	unsigned int tries = 0;
	do{
		rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);

		if (rc != SQLITE_DONE && rc != SQLITE_OK && rc != SQLITE_ROW && retriesCount > 0 && stepWaitTime > 0){
			std::this_thread::sleep_for(std::chrono::milliseconds(stepWaitTime));
		}

	} while (rc != SQLITE_DONE && rc != SQLITE_OK && rc != SQLITE_ROW && (tries++ < retriesCount));

	return rc;
}

bool SQLiteDB::verify(sqlite3 * db)
{
	return exec(db, "SELECT count(*) FROM sqlite_master;", 0, 0) == SQLITE_ROW;
}

sqlite3 * SQLiteDB::open(const std::string & path, const std::string & key,
	const int flags)
{
	sqlite3 * db = open(path, flags);	

	if (db != nullptr && key.empty() == false){
		auto rc = sqlite3_key(db, key.c_str(), (int)key.size());
		if (rc != SQLITE_OK){
			close(db);
			db = nullptr;
		}
	}

	return db;
}

sqlite3 * SQLiteDB::open(const std::string & path, const int flags)
{
	sqlite3 * db = nullptr;

	auto rc = sqlite3_open_v2(path.c_str(), &db, flags, nullptr);

	if (rc != SQLITE_OK){
		close(db);
		db = nullptr;
	}

	return db;
}
