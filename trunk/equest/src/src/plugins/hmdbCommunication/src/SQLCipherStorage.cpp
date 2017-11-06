#include "CommunicationPCH.h"
#include "SQLCipherStorage.h"
#include <streambuf>

#include <utils/SmartPtr.h>
#include <boost/format.hpp>
#include <sqlite3.h>
#include <sqliteUtils/BufferPolicyT.h>
#include <sqliteUtils/SQLiteBLOB.h>
#include <sqliteUtils/SQLiteDB.h>
#include <sqliteUtils/SQLitePreparedStatement.h>
#include <sqliteUtils/SQLiteBLOBStreamBufferT.h>
#include <loglib/Exceptions.h>
#include <threadingUtils/Macros.h>
#include <utils/StreamTools.h>

using namespace hmdbCommunication;

static const unsigned int sqliteExecWaitMS = 50;
static const unsigned int maxSqliteExecTries = 2400;

template<class Base, class Buffer>
class StreamWrapper : public Base
{
public:

	explicit StreamWrapper(Buffer *_Strbuf,
		bool _Isstd) : Base(_Strbuf, _Isstd)		
	{	// construct from stream buffer pointer		
	}

	explicit StreamWrapper(Buffer *_Strbuf)
		: Base(_Strbuf)
	{	// construct from stream buffer pointer
	}

	StreamWrapper(const StreamWrapper&) = delete;
	StreamWrapper& operator=(const StreamWrapper&) = delete;

	virtual ~StreamWrapper()
	{	// destroy the object
		auto toDelete = Base::rdbuf(nullptr);
		if (toDelete != nullptr){
			delete toDelete;
		}
	}
};

const bool initialize(sqlite3 * db)
{
	auto ret = sqliteUtils::SQLiteDB::exec(db, "CREATE TABLE files_table (file_name TEXT PRIMARY KEY, file BLOB);", maxSqliteExecTries, sqliteExecWaitMS);
	return ret == SQLITE_DONE || ret == SQLITE_OK;
}

const bool checkStructure(sqlite3 * db)
{
	static const std::string sqlCheckTable = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='files_table' ORDER BY name;";

	bool ret = false;	
	sqliteUtils::UniqueWrapperT<sqlite3_stmt> query(sqliteUtils::SQLitePreparedStatement::prepare(db, sqlCheckTable));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
		const auto count = sqlite3_column_int(query, 0);		
		if (count >= 1){
			ret = true;
		}
	}

	return ret;
}

class SQLCipherStorageTransaction : public threadingUtils::TransactionImplHelper<hmdbCommunication::IHMDBStorageOperations, SQLCipherStorage::TransactionSharedState>
{
private:

	sqliteUtils::UniqueWrapperT<sqlite3> db;

public:

	SQLCipherStorageTransaction(sqlite3 * db, SQLCipherStorage::SharedStatePtr sharedState)
		: ImplType(sharedState), db(db)
	{
		if (db == nullptr){
			throw std::runtime_error("Uninitialized db connection");
		}
	}
	virtual ~SQLCipherStorageTransaction() {}

	virtual const bool canStore(const unsigned long long size) const override
	{
		return utils::Filesystem::availableSpace(sharedState().path_) >= size;
	}

	virtual const bool shareDiskSpace(const utils::Filesystem::Path & path) const override
	{
		return sharedState().path_.root_path() == path.root_path();
	}

	virtual const bool exists(const std::string & key) const override
	{
		return rawExists(key, db);
	}

	virtual const IOStreamPtr get(const std::string & key) override
	{
		return rawGet(key);
	}

	virtual const IStreamPtr get(const std::string & key) const override
	{
		return rawGetReadOnly(key);
	}

	virtual const bool set(const std::string & key, IStreamPtr input) override
	{
		return rawSet(key, input, db);
	}

	virtual void set(const std::string & key, IStreamPtr input, hmdbCommunication::IHMDBStorageProgress * progress) override
	{
		rawSet(key, input, progress, db);
	}

	virtual const bool remove(const std::string & key) override
	{
		return rawRemove(key, db);
	}

	virtual const bool rename(const std::string & oldKey,
		const std::string & newKey, const bool overwrite = false) override
	{
		return rawRename(oldKey, newKey, overwrite, db);
	}

	virtual const Keys keys() const override
	{
		return rawKeys(db);
	}

private:

	static const bool rawExists(const std::string & key, sqlite3 * db)
	{
		bool ret = false;
		const std::string fileQuery((boost::format("SELECT count(*) FROM files_table WHERE file_name='%1%';") % key).str());

		sqliteUtils::UniqueWrapperT<sqlite3_stmt> query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

		if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
			const auto count = sqlite3_column_int(query, 0);
			if (count == 1){
				ret = true;
			}
		}

		return ret;
	}

	const IHMDBStorageOperations::IOStreamPtr rawGet(const std::string & key)
	{
		//pobieramy dane z bazy
		const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());
		sqliteUtils::UniqueWrapperT<sqlite3_stmt> query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

		if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
			const auto rowID = sqlite3_column_int64(query, 0);			
			return IHMDBStorageOperations::IOStreamPtr(new StreamWrapper<std::iostream, sqliteUtils::SQLiteBLOBStreamBufferT<char>>(
				new sqliteUtils::SQLiteBLOBStreamBufferT<char>(sqliteUtils::SQLiteBLOBStreamBufferT<char>::BufferPolicyPtr(new sqliteUtils::DynamicWriteBufferPolicy<char>()),
				db, "files_table", "file", rowID, "main")));
		}

		return IHMDBStorageOperations::IOStreamPtr();
	}

	const IHMDBStorageOperations::IStreamPtr rawGetReadOnly(const std::string & key) const
	{
		const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());
		sqliteUtils::UniqueWrapperT<sqlite3_stmt> query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

		if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
			const auto rowID = sqlite3_column_int64(query, 0);
			return IHMDBStorageOperations::IStreamPtr(new StreamWrapper<std::istream, sqliteUtils::SQLiteBLOBStreamBufferT<char>>(
				new sqliteUtils::SQLiteBLOBStreamBufferT<char>(sqliteUtils::SQLiteBLOBStreamBufferT<char>::BufferPolicyPtr(new sqliteUtils::DynamicWriteBufferPolicy<char>()),
				db, "files_table", "file", rowID, "main", std::ios_base::in)));
		}

		return IHMDBStorageOperations::IStreamPtr();
	}

	static const bool rawSet(const std::string & key, IHMDBStorageOperations::IStreamPtr input, sqlite3 * db)
	{
		if (input == nullptr){
			return false;
		}

		const auto streamSize = utils::StreamTools::size(*input);

		if (streamSize == 0){
			return false;
		}

		//wrzucam to do bazy danych
		std::string query;

		const bool existed = rawExists(key, db);

		//najpierw sprawdzam czy plik istnieje
		if (existed == false){
			//insert
			query = (boost::format("INSERT INTO files_table(file_name, file) VALUES('%1%', ?);") % key).str();
		}
		else{
			//update
			query = (boost::format("UPDATE files_table SET file = ? WHERE file_name = '%1%';") % key).str();
		}

	{
		sqliteUtils::UniqueWrapperT<sqlite3_stmt> store(sqliteUtils::SQLitePreparedStatement::prepare(db, query));

		if (store == nullptr){
			return false;
		}

		auto rc = sqlite3_bind_zeroblob(store, 1, streamSize);

		if (rc != SQLITE_OK || sqliteUtils::SQLitePreparedStatement::step(store) != SQLITE_DONE){
			rawRemove(key, db);
			return false;
		}
	}

	sqlite3_int64 rowid = -1;

	if (existed == false){
		rowid = sqlite3_last_insert_rowid(db);
	}
	else{
		const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name='%1%';") % key).str());
		sqliteUtils::UniqueWrapperT<sqlite3_stmt> selectQuery(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

		if (selectQuery != nullptr && sqliteUtils::SQLitePreparedStatement::step(selectQuery) == SQLITE_ROW){
			rowid = sqlite3_column_int64(selectQuery, 0);
		}
	}

	if (rowid == -1){
		rawRemove(key, db);
		return false;
	}

	// Getting here means we have a valid file handle, f, and a valid db handle, db
	//   Also, a blank row has been inserted with key rowid

	sqliteUtils::UniqueWrapperT<sqlite3_blob> blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowid, 1, "main"));

	if (blob == nullptr) {
		rawRemove(key, db);
		return false;
	}

	const auto readSize = std::min((int)streamSize, (int)sqliteUtils::MaxBufferSize);

	utils::unique_ptr<char[]> memblock(new char[readSize] {0});
	input->seekg(0, std::ios::beg);
	int offset = 0;
	const unsigned int fullReads = (streamSize / readSize);
	for (unsigned int i = 0; i < fullReads; ++i){
		input->read(memblock.get(), readSize);
		auto rc = sqlite3_blob_write(blob, memblock.get(), readSize, offset);
		if (rc != SQLITE_OK){
			blob.reset();
			rawRemove(key, db);
			return false;
		}

		offset += readSize;
	}

	const auto left = streamSize - offset;

	if (left > 0){
		input->read(memblock.get(), left);
		auto rc = sqlite3_blob_write(blob, memblock.get(), left, offset);
		if (rc != SQLITE_OK){
			blob.reset();
			rawRemove(key, db);
			return false;
		}
	}

	blob.reset();

	//TODO co jak zwalnianie bloba nie da rady?
	/*
	if (rc != SQLITE_OK){
	rawRemove(key, db);
	return false;
	}*/

	return true;
	}

	static void rawSet(const std::string & key, IHMDBStorageOperations::IStreamPtr input, hmdbCommunication::IHMDBStorageProgress * progress, sqlite3 * db)
	{
		if (input == nullptr){
			progress->setError("Uninitialized input");
			return;
		}

		const auto streamSize = utils::StreamTools::size(*input);

		if (streamSize == 0){
			progress->setError("Empty input stream");
			return;
		}

		//wrzucam to do bazy danych
		std::string query;

		const bool existed = rawExists(key, db);

		//najpierw sprawdzam czy plik istnieje
		if (existed == false){
			//insert
			query = (boost::format("INSERT INTO files_table(file_name, file) VALUES('%1%', ?);") % key).str();
		}
		else{
			//update
			query = (boost::format("UPDATE files_table SET file = ? WHERE file_name = '%1%';") % key).str();
		}

		{
			sqliteUtils::UniqueWrapperT<sqlite3_stmt> store(sqliteUtils::SQLitePreparedStatement::prepare(db, query));

			if (store == nullptr){
				progress->setError("Creation of db query failed");
				return;
			}

			auto rc = sqlite3_bind_zeroblob(store, 1, streamSize);

			if (rc != SQLITE_OK || sqliteUtils::SQLitePreparedStatement::step(store) != SQLITE_DONE){
				rawRemove(key, db);
				progress->setError("DB update failed");
				return;
			}
		}

		sqlite3_int64 rowid = -1;

		if (existed == false){
			rowid = sqlite3_last_insert_rowid(db);
		}
		else{
			const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name='%1%';") % key).str());
			sqliteUtils::UniqueWrapperT<sqlite3_stmt> selectQuery(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

			if (selectQuery != nullptr && sqliteUtils::SQLitePreparedStatement::step(selectQuery) == SQLITE_ROW){
				rowid = sqlite3_column_int64(selectQuery, 0);
			}
		}

		if (rowid == -1){
			rawRemove(key, db);
			progress->setError("Failed to grab DB record for modification");
			return;
		}

		// Getting here means we have a valid file handle, f, and a valid db handle, db
		//   Also, a blank row has been inserted with key rowid
		sqliteUtils::UniqueWrapperT<sqlite3_blob> blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowid, 1, "main"));

		if (blob == nullptr) {
			rawRemove(key, db);
			progress->setError("Failed to attach DB output for data");
			return;
		}

		const auto readSize = std::min((int)streamSize, (int)sqliteUtils::MaxBufferSize);

		utils::unique_ptr<char[]> memblock(new char[readSize] {0});
		input->seekg(0, std::ios::beg);
		int offset = 0;
		const unsigned int fullReads = (streamSize / readSize);

		const float progressPart = (float)readSize / (float)streamSize;
		for (unsigned int i = 0; i < fullReads && progress->aborted() == false; ++i){
			input->read(memblock.get(), readSize);
			auto rc = sqlite3_blob_write(blob, memblock.get(), readSize, offset);
			if (rc != SQLITE_OK){
				blob.reset();
				rawRemove(key, db);
				progress->setError("Failed to write to DB output");
				return;
			}

			offset += readSize;
			progress->setProgress(progressPart * (i + 1));
		}

		if (progress->aborted() == true){
			blob.reset();
			rawRemove(key, db);
			return;
		}

		const auto left = streamSize - offset;

		if (left > 0){
			input->read(memblock.get(), left);
			auto rc = sqlite3_blob_write(blob, memblock.get(), left, offset);
			if (rc != SQLITE_OK){
				blob.reset();
				rawRemove(key, db);
				progress->setError("Failed to write to DB output");
				return;
			}
		}

		progress->setProgress(1.0);

		//TODO
		//sprawdzac czy czasem nie bylo problemow ze zwolnieniem bloba!!
	}

	static const bool rawRemove(const std::string & key, sqlite3 * db)
	{
		const std::string fileDelete((boost::format("DELETE FROM files_table WHERE file_name = '%1%';") % key).str());
		if (sqliteUtils::SQLiteDB::exec(db, fileDelete) != SQLITE_DONE) {
			return false;
		}

		if (sqlite3_changes(db) == 0){
			return false;
		}

		return true;
	}

	const bool rawRename(const std::string & oldKey,
		const std::string & newKey, const bool overwrite, sqlite3 * db)
	{
		const std::string query((boost::format("UPDATE files_table SET file_name = %1% WHERE file_name = '%2%';") % newKey % oldKey).str());
		sqliteUtils::UniqueWrapperT<sqlite3_stmt> update(sqliteUtils::SQLitePreparedStatement::prepare(db, query));

		if (update != nullptr && rawExists(oldKey, db) == true){
			if (rawExists(newKey, db) == false){
				if (sqliteUtils::SQLitePreparedStatement::step(update) == SQLITE_DONE){
					return true;
				}
			}
			else if (overwrite == true){
				if (rawRemove(newKey, db) == false){
					return false;
				}

				if (sqliteUtils::SQLitePreparedStatement::step(update) == SQLITE_DONE){
					return true;
				}
			}
		}

		return false;
	}

	static const IHMDBStorageOperations::Keys rawKeys(sqlite3 * db)
	{
		IHMDBStorageOperations::Keys ret;

		const std::string fileQuery("SELECT file_name FROM files_table;");
		sqliteUtils::UniqueWrapperT<sqlite3_stmt> query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

		if (query != nullptr){
			int r = SQLITE_DONE;
			while ((r = sqliteUtils::SQLitePreparedStatement::step(query)) == SQLITE_ROW){
				const auto key = sqlite3_column_text(query, 0);

				ret.insert(std::string((const char *)key));
			}

			if (r == false){
				IHMDBStorageOperations::Keys().swap(ret);
			}
		}

		return ret;
	}
};

SQLCipherStorage::SQLCipherStorage()
{

}

SQLCipherStorage::~SQLCipherStorage()
{

}

void SQLCipherStorage::open(const utils::Filesystem::Path & path,
	const std::string & key)
{	
	if (sharedState != nullptr || utils::Filesystem::pathExists(path) == false){
		return;
	}
	
	sqliteUtils::UniqueWrapperT<sqlite3> db(sqliteUtils::SQLiteDB::open(path.string(), key, SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE));
	if (db != nullptr){
		if (checkStructure(db) == true){
			sharedState = utils::make_shared<TransactionSharedState>();
			sharedState->innerState.path_ = path;
			sharedState->innerState.key_ = key;
		}
	}
}

const bool SQLCipherStorage::create(const utils::Filesystem::Path & path,
	const std::string & key)
{
	if (utils::Filesystem::pathExists(path) == true){
		return false;
	}	

	sqliteUtils::UniqueWrapperT<sqlite3> db(sqliteUtils::SQLiteDB::open(path.string(), key, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE));
	if (db != nullptr){
		auto ret = initialize(db);
		if (ret == true) {
			if (sqlite3_key(db, key.c_str(), key.size()) == SQLITE_OK) {
				return ret;
			} else {
				return false;
			}
		}
		return ret;
	}

	return false;
}

const bool SQLCipherStorage::changeKey(const utils::Filesystem::Path & path,
	const std::string & oldKey, const std::string & newKey)
{
	if (utils::Filesystem::pathExists(path) == false || oldKey == newKey){
		return false;
	}	

	bool ret = false;

	sqliteUtils::UniqueWrapperT<sqlite3> db(sqliteUtils::SQLiteDB::open(path.string(), oldKey, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE));

	if (db != nullptr){
		//auto rc = sqlite3_rekey(db, newKey.c_str(), newKey.size());
		//if (rc == SQLITE_OK){
			db.reset(sqliteUtils::SQLiteDB::open(path.string(), newKey, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE));
			ret = (db != nullptr);
		//}
	}	

	return ret;
}

const bool SQLCipherStorage::setKey(const utils::Filesystem::Path & path,
	const std::string & key)
{
	if (utils::Filesystem::pathExists(path) == false){
		return false;
	}

	bool ret = false;

	sqliteUtils::UniqueWrapperT<sqlite3> db(sqliteUtils::SQLiteDB::open(path.string(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE));

	if (db != nullptr){
		//auto rc = sqlite3_rekey(db, key.c_str(), key.size());
		//if (rc == SQLITE_OK){
			ret = sqliteUtils::SQLiteDB::verify(db);
		//}
	}	

	return ret;
}

const bool SQLCipherStorage::verify(const utils::Filesystem::Path & path,
	const std::string & key)
{	
	sqliteUtils::UniqueWrapperT<sqlite3> db(sqliteUtils::SQLiteDB::open(path.string(), key, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE));

	if (db == nullptr){
		return checkStructure(db);
	}	

	return false;
}

const bool SQLCipherStorage::isOpened() const
{
	return sharedState != nullptr && sharedState->innerState.path_.empty() == false;
}

const std::string SQLCipherStorage::name() const
{
	return std::string("SQLCipher");
}

const std::string SQLCipherStorage::protocol() const
{
	return std::string("sqlcipher");
}

void SQLCipherStorage::close()
{
	sharedState.reset();	
}

IHMDBStorage::TransactionPtr SQLCipherStorage::transaction()
{
	return TransactionPtr(new SQLCipherStorageTransaction(sqliteUtils::SQLiteDB::open(sharedState->innerState.path_.string(), sharedState->innerState.key_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE), sharedState));
}

IHMDBStorage::TransactionConstPtr SQLCipherStorage::transaction() const
{
	return TransactionConstPtr(new SQLCipherStorageTransaction(sqliteUtils::SQLiteDB::open(sharedState->innerState.path_.string(), sharedState->innerState.key_, SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE), sharedState));
}