#include "../VideoPCH.h"
/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

#include "../core/VMPrivate.h"
#include "../core/FFmpegVideoStream.h"
#include "VideoImageStream.h"
#include <core/Log.h>

/**
 *	Wtyczka do OSG.
 */
class ReaderWriterVMPlugin : public osgDB::ReaderWriter
{
public:

  //! 
  ReaderWriterVMPlugin()
  {
    supportsProtocol("http","Read video/audio from http using ffmpeg.");
    supportsProtocol("rtsp","Read video/audio from rtsp using ffmpeg.");

    supportsExtension("video",     "");
    supportsExtension("avi",    "");
    supportsExtension("flv",    "Flash video");
    supportsExtension("mov",    "Quicktime");
    supportsExtension("ogg",    "Theora movie format");
    supportsExtension("mpg",    "Mpeg movie format");
    supportsExtension("mpv",    "Mpeg movie format");
    supportsExtension("wmv",    "Windows Media Video format");
    supportsExtension("mkv",    "Matroska");
    supportsExtension("mjpeg",  "Motion JPEG");
    supportsExtension("mp4",    "MPEG-4");
    supportsExtension("sav",    "MPEG-4");
    supportsExtension("3gp",    "MPEG-4");
    supportsExtension("sdp",    "MPEG-4");
    supportsExtension("m1v",    "MPEG-1");

    // teoretycznie to powinno znaleźć się gdzieś w kodzie FFmpegVideoStream,
    // ale nie mam pomysłu, jak to tam umieścić
    video::FFmpegVideoStream::setLockManager(lockManager);
    video::FFmpegVideoStream::setLogCallback(logCallback);
  }

  //! 
  virtual ~ReaderWriterVMPlugin()
  {
  }

  //! 
  virtual const char * className() const
  {
    return "ReaderWriterVMPlugin";
  }

  //! 
  //! \param filename
  //! \param options
  virtual ReadResult readImage(const std::string & filename, const osgDB::ReaderWriter::Options * options) const
  {
    const std::string ext = osgDB::getLowerCaseFileExtension(filename);
    
    if (ext=="video") { 
      return readImage(osgDB::getNameLessExtension(filename),options);
    }

    if (filename.compare(0, 5, "/dev/")==0) {
      return readImageStream(filename, options);
    }

    if (! acceptsExtension(ext)) {
      return ReadResult::FILE_NOT_HANDLED;
    }

    const std::string path = osgDB::containsServerAddress(filename) ?
      filename :
      osgDB::findDataFile(filename, options);

    if (path.empty()) {
      return ReadResult::FILE_NOT_FOUND;
    }

    return readImageStream(path, options);
  }
  
  //! 
  //! \param filename
  //! \param options
  ReadResult readImageStream(const std::string& filename, const osgDB::ReaderWriter::Options * options) const
  {
    OSG_INFO<< "ReaderWriterVMPlugin::readImage " << filename << std::endl;

    osg::ref_ptr<video::osgPlugin::VideoImageStream> image_stream(new video::osgPlugin::VideoImageStream);

    if (! image_stream->open(filename)) {
      return ReadResult::FILE_NOT_HANDLED;
    }

    return image_stream.release();
  }

private:
  //! 
  //! \param mutex
  //! \param op
  static int lockManager(void **mutex, video::FFmpegVideoStream::LockOp op)
  {    
    using namespace video;
    // pobieramy mutexa
    OpenThreads::Mutex **m=(OpenThreads::Mutex**)mutex;
    // operacja na mutexie
    switch (op) {
      case FFmpegVideoStream::LockOpCreate:
        *m=new OpenThreads::Mutex;
        return !*m;
      case FFmpegVideoStream::LockOpDestroy:
        delete *m;
        return 0;
      case FFmpegVideoStream::LockOpObtain:
        (*m)->lock();
        return 0;
      case FFmpegVideoStream::LockOpRelease:
        (*m)->unlock();
        return 0;
      default:
        return -1;
    }
  }

  //! 
  //! \param severity
  //! \param msg
  //! \param item
  //! \param parent
  static void logCallback(video::FFmpegVideoStream::LogSeverity severity, const char* msg, video::FFmpegVideoStream::FFmpegClass* item, video::FFmpegVideoStream::FFmpegClass* parent)
  {
    using namespace video;
    switch ( severity ) {
        case FFmpegVideoStream::LogSeverityQuiet:
            break;

        case FFmpegVideoStream::LogSeverityPanic:
        case FFmpegVideoStream::LogSeverityFatal:
        case FFmpegVideoStream::LogSeverityError:
            LOG_ERROR_STATIC_NAMED("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityWarning:
            LOG_WARNING_STATIC_NAMED("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityInfo:
        case FFmpegVideoStream::LogSeverityVerbose:
            LOG_INFO_STATIC_NAMED("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityDebug:
            LOG_DEBUG_STATIC_NAMED("ffmpeg", msg);
            break;

        default:
            UTILS_ASSERT(false, "Nieznany poziom FFmpeg. Wiadomość: %s", msg);
            break;
    }
  }

};

REGISTER_OSGPLUGIN(VideoManager, ReaderWriterVMPlugin)
