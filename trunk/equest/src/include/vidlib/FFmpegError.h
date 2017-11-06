#ifndef HEADER_GUARD_VIDLIB__FFMPEGERROR_H__
#define HEADER_GUARD_VIDLIB__FFMPEGERROR_H__

#include <vidlib/VideoError.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! Wyjątek używany przez bibliotekę.
class VIDLIB_EXPORT FFmpegError : public VideoError
{
public:
    enum {
        //! Bład posiada nieznane ID.
        UnknownID = -1
    };

private:
    //! Wewnętrzne ID błędu.
    const int id;

public:
    FFmpegError(const char* msg, int id = UnknownID);
    FFmpegError(const std::string& msg, int id = UnknownID);
    FFmpegError(const FFmpegError& error);

    //! \return Wewnętrzne ID błędu.
    inline const int getID()
    {
        return id;
    }

    //! \return Klon obiektu.
    virtual VideoError * clone() const;

protected:
    //! Specjalnie niewirtualna!
    void setMessage(const char* msg, size_t length);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__FFMPEGERROR_H__
