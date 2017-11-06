/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:14
	filename: VideoError.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOERROR_H__
#define HEADER_GUARD_VIDLIB__VIDEOERROR_H__

#include <stdexcept>
#include <vidlib/Config.h>

namespace vidlib {

//! Wyjątek używany przez bibliotekę.
class VIDLIB_EXPORT VideoError : public std::exception
{
private:
    //! Wg. standradu std::exception NIE ma konstruktora przyjmującego
    //! const char*, z tego powodu trzymamy ręczną kopię.
    char* msgCopy;

protected:
    //! Konstruktor zerujący.
    VideoError();

public:
    //! \param msg Wiadomość o błędzie.
    //! \return id ID błędu.
    VideoError(const char* msg);
    //! \param msg Wiadomość o błędzie.
    //! \return id ID błędu.
    VideoError(const std::string& msg);
    //! \param error Wyjątek źródłowy.
    VideoError(const VideoError& error);

    //! Polimorficzny destruktor.
    virtual ~VideoError() throw();

    //! \return Klon obiektu.
    virtual VideoError * clone() const;
    //! \return Komunikat błędu.
    virtual const char* what() const throw();

protected:
    //! Kopiuje wiadomość do wewnętrznego bufora. Specjalnie niewirtualna.
    void setMessage( const char* msg, size_t length );
};

} // namespace vidlib

#endif  // HEADER_GUARD_VIDLIB__VIDEOERROR_H__
