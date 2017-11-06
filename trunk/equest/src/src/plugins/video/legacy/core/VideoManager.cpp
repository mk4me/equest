#include "../VideoPCH.h"
#include "VMPrivate.h"
#include "VideoManager.h"
#include "FFmpegVideoStream.h"
#include <plugins/video/core/BufferedVideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

VideoManager* VideoManager::instance = NULL;

VideoManager::VideoManager() :
prefferedFormat(vm::PixelFormatRGB24), enableBuffering(false)
{
}

VideoManager::~VideoManager()
{
}


VideoManager* VideoManager::getInstance()
{
  if ( !instance ) {
    instance = new VideoManager();
  }
  return instance;
}

void VideoManager::destroyInstance()
{
  delete instance;
}

bool VideoManager::init()
{
  return true;
}

VideoStream* VideoManager::createStream( const std::string& path )
{
  FFmpegVideoStream * result = new FFmpegVideoStream(path);
  if ( this->enableBuffering ) {
    return new BufferedVideoStream(result);
  } else {
    return result;
  }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////
