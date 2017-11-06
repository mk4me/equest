#ifndef __AVINOUT_FFMPEGEXCEPTION_H__
#define __AVINOUT_FFMPEGEXCEPTION_H__

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

//! Wyjątek używany przez bibliotekę.
class FFmpegError : public VideoError
{
public:
  FFmpegError(const std::string& msg, int id = VideoError::UnknownID) : VideoError(msg, id)
  {}

  FFmpegError(const FFmpegError& error) : VideoError(error)
  {}

  //! \return Klon obiektu.
  virtual VideoError * clone() const
  {
    return new FFmpegError(*this);
  }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_FFMPEGEXCEPTION_H__
