#include <sqliteUtils/SQLitePreparedStatement.h>
#include <thread>

using namespace sqliteUtils;

SQLitePreparedStatement::Finalizer::Finalizer(const unsigned int retriesCount,
	const unsigned int stepWaitTime) : retriesCount(retriesCount),
	stepWaitTime(stepWaitTime)
{

}

bool SQLitePreparedStatement::Finalizer::operator()(sqlite3_stmt * statment)
{
	unsigned int tries = 0;
	bool retry = false;
	do{

		retry = !SQLitePreparedStatement::finalize(statment);

		if ((retry == true) && (retriesCount > 0) && (stepWaitTime > 0)){
			std::this_thread::sleep_for(std::chrono::milliseconds(stepWaitTime));
		}

	} while ((tries++ <= retriesCount) && (retry == true));

	return !retry;
}

sqlite3_stmt * SQLitePreparedStatement::prepare(sqlite3 * db, const std::string & sql)
{
	sqlite3_stmt * ret = nullptr;
	auto rc = sqlite3_prepare_v2(db, sql.c_str(), (int)sql.size(), &ret, nullptr);
	if (rc != SQLITE_OK){
		finalize(ret);
		ret = nullptr;
	}

	return ret;
}

bool SQLitePreparedStatement::finalize(sqlite3_stmt * statement)
{
	return sqlite3_finalize(statement) == SQLITE_OK;
}

int SQLitePreparedStatement::step(sqlite3_stmt * statement)
{
	return sqlite3_step(statement);
}