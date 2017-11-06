/* OpenSceneGraph example, osgmovie.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include <boost/format.hpp>
#include <sqliteUtils/SQLiteDB.h>
#include <sqliteUtils/SQLitePreparedStatement.h>
#include <sqliteUtils/SQLiteBLOB.h>
#include <sqliteUtils/SQLiteBLOBStreamBufferT.h>
#include <sqliteUtils/WrapperT.h>
#include <vidlib/FFmpegVideoStream.h>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/ReadFile>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/TextureCubeMap>
#include <osg/TexMat>
#include <osg/CullFace>
#include <osg/ImageStream>
#include <osg/io_utils>

#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/EventVisitor>

#include <iostream>

#include <vidlib/osg/ReaderWriterVidLib.h>

using vidlib::FFmpegVideoStream;
USE_OSGPLUGIN_VIDLIB();

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
	sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, sqlCheckTable), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
		const auto count = sqlite3_column_int(query, 0);
		if (count >= 1){
			ret = true;
		}
	}

	return ret;
}

void openStorage()
{
	std::string path("/home/wojtek/.local/share/PJWSTK/MDE/db/localStorage.db");
	std::string key("P,j.W/s<T>k2:0\"1;2");

	auto maxSqliteExecTries = 2400;
	auto sqliteExecWaitMS = 50;
	auto sqlite = sqliteUtils::SQLiteDB::open(path, key, SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE);
	auto close = sqliteUtils::SQLiteDB::Close(maxSqliteExecTries, sqliteExecWaitMS);
	sqliteUtils::SQLiteDB::Wrapper db(sqlite, close);
	if (db != nullptr){
		if (checkStructure(db,maxSqliteExecTries, sqliteExecWaitMS) == false){
			db.reset();
		}
	}

	std::string filename("2011-02-24-B0001-S01-T01.59461898.avi");
	//pobieramy dane z bazy
	const std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % filename).str());
	sqliteUtils::SQLitePreparedStatement::Wrapper query(sqliteUtils::SQLitePreparedStatement::prepare(db, fileQuery), sqliteUtils::SQLitePreparedStatement::Finalizer(maxSqliteExecTries, sqliteExecWaitMS));

	if (query != nullptr && sqliteUtils::SQLitePreparedStatement::step(query) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(query, 0);
		sqliteUtils::SQLiteBLOB::Wrapper blob(sqliteUtils::SQLiteBLOB::open(db, "files_table", "file", rowID, 0), sqliteUtils::SQLiteBLOB::Close(maxSqliteExecTries, sqliteExecWaitMS));
		std::auto_ptr<sqliteUtils::SQLiteBLOBStreamBufferT<>> buf(new sqliteUtils::SQLiteBLOBStreamBufferT<>(path, "files_table", "file", rowID, sqlite3_blob_bytes(blob), "main", key));
		typedef utils::shared_ptr<std::iostream> IOStreamPtr;
		auto stream = IOStreamPtr(new StreamWrapper<std::iostream, sqliteUtils::SQLiteBLOBStreamBufferT<>>(buf.release()));
		unsigned char* buffer = new unsigned char(256);
		try {
		stream->readsome(buffer, 256);
		std::auto_ptr<FFmpegVideoStream> innerStream(new vidlib::FFmpegVideoStream(stream, filename));
		} catch (std::exception& e) {
			std::string s(e.what());
			int a =0;
		}
	}


   	/*core::IStreamDataManager::StreamGrabberPtr sg(new StorageStreamGrabber(dataContext_->storage(), fileName));
	streamTransaction->addStream(sg);
	core::ConstVariantsList oList;
	streamTransaction->getObjects(sg->name(), oList);


   	utils::shared_ptr<std::istream> stream = utils::make_shared<std::ifstream>(arguments[i]);
   	// tworzymy strumień ffmpeg
	std::auto_ptr<FFmpegVideoStream> innerStream(new vidlib::FFmpegVideoStream(stream, std::string(arguments[i])));
	// UTILS_ASSERT(!innerStream->getLastError());
    */
}


int main(int argc, char** argv)
{
   	openStorage();
	// podpinay go pod adapter strumienia
	//VideoStreamPtr realStream(new ::VideoStream(innerStream.release()));
	//realStream->setTime(0);
	//adapter->set(realStream);
	//TODO
	//VideoChannelPtr channel(new VideoChannel(realStream));
	//localData[0]->set(channel);
	//data.swap(localData);
}



