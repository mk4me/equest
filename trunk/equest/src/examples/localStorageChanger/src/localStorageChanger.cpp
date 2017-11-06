#include <boost/format.hpp>
#include <sqliteUtils/SQLiteDB.h>
#include <sqliteUtils/SQLitePreparedStatement.h>
#include <sqliteUtils/SQLiteBLOB.h>
#include <sqliteUtils/SQLiteBLOBStreamBufferT.h>
#include <sqliteUtils/WrapperT.h>
#include <boost/program_options.hpp>

#include <iostream>
#include <iosfwd>
#include <fstream>

const int maxSqliteExecTries = 2400;
const int sqliteExecWaitMS = 50;

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
		auto toDelete = Base::rdbuf();
		Base::rdbuf(nullptr);
		delete toDelete;
	}
};


const bool checkStructure(sqlite3 * db, int maxSqliteExecTries, int sqliteExecWaitMS)
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

sqliteUtils::UniqueWrapperT<sqlite3> openStorage(const std::string& path, const std::string& key)
{
	//std::string path("/home/wojtek/.local/share/PJWSTK/MDE/db/localStorage.db");
	//std::string path("C:/Users/Wojciech/AppData/Roaming/PJWSTK/MDE/db/localStorage.db");
	//std::string key("P,j.W/s<T>k2:0\"1;2");

	
	auto sqlite = sqliteUtils::SQLiteDB::open(path, key, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE);
	//auto close = sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS);
	sqliteUtils::UniqueWrapperT<sqlite3> db(sqlite);
	if (db != nullptr) {
		if (checkStructure(db, maxSqliteExecTries, sqliteExecWaitMS)) {
			return std::move(db);
		} else {
			db.reset();
		}
	} 

	throw std::runtime_error(std::string("Unable to open storage: ") + path);
}

void do_Stuff()
{
	////std::string filename("2011-02-24-B0001-S01-T01.60981847.avi");
	//std::string filename("2011-02-24-B0001-S01-T01.59461898.avi");
	////pobieramy dane z bazy
	//const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % filename).str());
	//sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	//if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
	//	const auto rowID = sqlite3_column_int64(query, 0);
	//	
	//	std::ifstream file("E:\\testEksport.avi", std::ios::binary);
	//	file.seekg(0, std::ios::end);
	//	std::streamsize size = file.tellg();
	//	file.seekg(0, std::ios::beg);

	//	std::vector<char> buffer(size);
	//	if (file.read(buffer.data(), size)) {
	//		/* worked! */
	//	}
	//	file.close();
	//	{
	//		std::string query = (boost::format("UPDATE files_table SET file = ? WHERE file_name = '%1%';") % filename).str();

	//		sqliteUtils::SQLitePreparedStatement::Wrapper store(sqliteUtils::SQLitePreparedStatement::prepare(db, query), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	//		if (store == nullptr) {
	//			int a = 0;
	//		}
	//		auto rc = sqlite3_bind_zeroblob(store, 1, size);
	//		rc = sqlite3_step(store);
	//	}

	//	{
	//		sqliteUtils::SQLiteBLOB::Wrapper blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowID, 1), sqliteUtils::SQLiteBLOB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	//		int errorCode = sqlite3_blob_write(blob, buffer.data(), size, 0);//(sqlite3_blob *, const void *z, int n, int iOffset);
	//		//sqlite3_blob_close(blob);
	//	}


	//	/*	{
	//			sqliteUtils::SQLiteBLOB::Wrapper blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowID, 0), sqliteUtils::SQLiteBLOB::Close(maxSqliteExecTries, sqliteExecWaitMS));
	//			std::auto_ptr<sqliteUtils::SQLiteBLOBStreamBufferT<>> buf(new sqliteUtils::SQLiteBLOBStreamBufferT<>(path, "files_table", "file", rowID, sqlite3_blob_bytes(blob), "main", key));
	//			typedef utils::shared_ptr<std::iostream> IOStreamPtr;
	//			auto stream = IOStreamPtr(new StreamWrapper<std::iostream, sqliteUtils::SQLiteBLOBStreamBufferT<>>(buf.release()));
	//			const auto tmpPath = "E:\\testEksport.avi";
	//			std::ofstream fle(tmpPath, std::ifstream::out | std::ifstream::binary);
	//			fle << stream->rdbuf();
	//			fle.close();
	//			}*/
	//}


 //  	/*core::IStreamDataManager::StreamGrabberPtr sg(new StorageStreamGrabber(dataContext_->storage(), fileName));
	//streamTransaction->addStream(sg);
	//core::ConstVariantsList oList;
	//streamTransaction->getObjects(sg->name(), oList);


 //  	utils::shared_ptr<std::istream> stream = utils::make_shared<std::ifstream>(arguments[i]);
 //  	// tworzymy strumień ffmpeg
	//std::auto_ptr<FFmpegVideoStream> innerStream(new vidlib::FFmpegVideoStream(stream, std::string(arguments[i])));
	//// UTILS_ASSERT(!innerStream->getLastError());
 //   */
}


void extractFile(const sqliteUtils::UniqueWrapperT<sqlite3> & db, const std::string& filename, const std::string & outFile,
					const std::string& dbPath, const std::string& dbKey, const std::vector<std::string>& availible)
{
	auto it = std::find(availible.begin(), availible.end(), filename);
	if (it == availible.end()) {
		throw std::runtime_error(std::string("Given file: ") + filename + std::string(", not present in db"));
	}

	//pobieramy dane z bazy
	const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % filename).str());
	sqliteUtils::UniqueWrapperT<sqlite3_stmt> query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(query, 0);
		std::unique_ptr<sqliteUtils::SQLiteBLOBStreamBufferT<char>> buf(new sqliteUtils::SQLiteBLOBStreamBufferT<char>(utils::make_unique<sqliteUtils::FixedBufferPolicyT<char>>(), db, "files_table", "file", rowID, "main"));
		typedef utils::shared_ptr<std::iostream> IOStreamPtr;
		auto stream = IOStreamPtr(new StreamWrapper<std::iostream, sqliteUtils::SQLiteBLOBStreamBufferT<char>>(buf.release()));
		std::ofstream fle(outFile, std::ifstream::out | std::ifstream::binary);
		fle << stream->rdbuf();
		fle.close();
	} else {
		throw std::runtime_error(std::string("Unable to extract file: ") + filename);
	}
}


std::vector<std::string> listFiles(const sqliteUtils::UniqueWrapperT<sqlite3>& db)
{
	std::vector<std::string> files;
		
	char *sql = "SELECT file_name FROM files_table";

	sqlite3_stmt *result;
	auto rc = sqlite3_prepare_v2(db, sql, -1, &result, 0);

	if (rc != SQLITE_OK) {
		throw std::runtime_error(std::string("Failed to execute statement: ") + sqlite3_errmsg(db));
	}

	while (sqlite3_step(result) == SQLITE_ROW) {
			files.push_back(reinterpret_cast<const char*>(sqlite3_column_text(result, 0)));
	}
	sqlite3_finalize(result);
	return files;
}

void updateFile(const sqliteUtils::UniqueWrapperT<sqlite3>& db, const std::string& fileInBase, const std::string& pathToFile, const std::string& path, const std::string& key, std::vector<std::string> available)
{
	auto it = std::find(available.begin(), available.end(), fileInBase);
	if (it == available.end()) {
		throw std::runtime_error(std::string("Given file: ") + fileInBase + std::string(", not present in db"));
	}

	const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % fileInBase).str());
	sqliteUtils::UniqueWrapperT<sqlite3_stmt> query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(query, 0);
		
		std::ifstream file(pathToFile, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Unable to open file: " + pathToFile);
		}
		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> buffer(size);
		if (!file.read(buffer.data(), size)) {
			file.close();
			throw std::runtime_error(std::string("Unable to read file: ") + pathToFile);
		}
		file.close();
		{
			std::string query = (boost::format("UPDATE files_table SET file = ? WHERE file_name = '%1%';") % fileInBase).str();

			sqliteUtils::UniqueWrapperT<sqlite3_stmt> store(sqliteUtils::SQLitePreparedStatement::prepare(db, query));

			if (store == nullptr) {
				throw std::runtime_error("Unable to prepare statement : " + query);
			}
			sqlite3_bind_zeroblob(store, 1, size);
			sqlite3_step(store);
		}

		{
			sqliteUtils::UniqueWrapperT<sqlite3_blob> blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowID, 1));
			sqlite3_blob_write(blob, buffer.data(), size, 0);//(sqlite3_blob *, const void *z, int n, int iOffset);
			//sqlite3_blob_close(blob);
		}
	}
}

std::pair<std::string, std::string> splitPaths(const std::string& te)
{
	auto idx = te.find('|');
	if (idx != std::string::npos && idx != 0 && idx != te.size()) {
		return std::make_pair(std::string(te.begin(), te.begin() + idx), std::string(te.begin() + idx + 1, te.end()));
	} else {
		throw std::runtime_error(te + "is in wrong format.");
	}
}

void extractFiles(const std::string& path, const std::string& key, const std::vector<std::string>& toExtract)
{
	auto db = openStorage(path, key);
	auto files = listFiles(db);
	for (auto& te : toExtract) {
		std::cout << "Extracting " << te << "... ";
		extractFile(db, te, te, path, key, files);
		std::cout << "done." << std::endl;
	}
}

void doStuff(const boost::program_options::variables_map& vm, const std::string& path, const std::string& key)
{
	if (vm.count("list")) {
		auto db = openStorage(path, key);
		auto files = listFiles(db);
		std::copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
	} 

	if (vm.count("extract-all")) {
		
		std::vector<std::string> toExtract;
		{
			auto db = openStorage(path, key);
			toExtract = listFiles(db);
		}
		extractFiles(path, key, toExtract);

	}

	if (vm.count("extract")) {
		std::vector<std::string> toExtract = vm["extract"].as< std::vector<std::string> >();
		extractFiles(path, key, toExtract);
	}


	if (vm.count("update")) {
		auto toExtract = vm["update"].as< std::vector<std::string> >();
		auto db = openStorage(path, key);
		auto files = listFiles(db);
		for (auto& te : toExtract) {
			std::cout << "Updating " << te << "... ";
			auto pair = splitPaths(te);
			updateFile(db, pair.first, pair.second, path, key, files);
			std::cout << "done." << std::endl;
		}
	}

	
}

int main(int argc, char** argv)
{
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("storage,s", po::value<std::string>(), "path to a local storage")
		("password,p", po::value<std::string>(), "password to a local storage")
		("list,l", "lists all file names present in storage")
		("extract-all,a", "extracts all files from storage")
		("extract,e", po::value< std::vector<std::string> >()->composing(), "extracts given files from storage")
		("update,u", po::value< std::vector<std::string> >()->composing(), "update files in storage")
		;

	po::variables_map vm;

	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	} catch (po::error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 2;
	}

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	std::string path;// ("C:/Users/Wojciech/AppData/Roaming/PJWSTK/MDE/db/localStorage.db");
	if (vm.count("storage")) {
		path = vm["storage"].as<std::string>();
	} else {
		std::cerr << "Storage path was not set.\n";
		return 2;
	}

	std::string key;// ("P,j.W/s<T>k2:0\"1;2");
	if (vm.count("password")) {
		key = vm["password"].as<std::string>();
	} else {
		std::cerr << "Storage password was not set.\n" << std::endl;
		return 2;
	}

	try {
		doStuff(vm, path, key);
	}
	catch (std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 2;
	}
	catch (...) {
		std::cerr << "Unknown error" << std::endl;
		return 2;
	}

	return 0;

}



