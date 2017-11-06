/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:15
	filename: VideoStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOSTREAM_H__
#define HEADER_GUARD_VIDLIB__VIDEOSTREAM_H__

#include <string>
#include <vidlib/Config.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/Picture.h>
#include <vidlib/VideoError.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class Converter;

enum {
    //! Nieprawidłowy czas
    INVALID_TIMESTAMP = -1
};


/**
 *	Abstrakcyjna klasa dla wejścia wideo.
 */
class VIDLIB_EXPORT VideoStream
{
public:
    //! Sygnatura callbacka.
    class VIDLIB_EXPORT Callback {
    protected:
        //! Chroniony destruktor - aby zaznaczyć, że strumień nie przemuje callbacka
        //! na własność (wskaźnika nie można skasować)
        virtual ~Callback() {}
    public:
        //! Callback wywoływany w momencie pominięcia pewnej ramki. Mechanizm ten
        //! umożliwa wydajniejsze buforowanie.
        //! \param picture
        //! \param layers
        //! \param timestamp
        virtual void frameRead(Picture * picture, PictureLayered * layers, double timestamp, double endTimestamp) = 0;
    };

protected:
    //!
    mutable Converter * converter;


private:
    //! Używane tylko w przypadku nieobsługiwania wyjątków.
    VideoError * lastError;
    //! Zbuforowana długość ramki.
    double frameDuration;
    //! Zbuforowana liczba klatek na sekundę.
    double frameRate;
    //! Zbuforowana długość strumienia.
    double duration;
    //!
    double aspectRatio;
    //! Format piksela.
    VIDLIB_PixelFormat format;
    //! Wysokość ramki.
    int width;
    //! Szerokość ramki.
    int height;
    //!
    Callback * callback;
    //! Zastosowany wzorzec "Pimpl" (żeby usunąć warningi przy budowie DLL)
    class VideoStreamImpl* impl;

protected:
    //! Konstruktor zerujący.
    VideoStream();

private:
    //! Brak konstruktora kopiującego.
    VideoStream(const VideoStream&);

public:
    //! Destruktor wirtualny.
    virtual ~VideoStream();

public:
    //! \return Klon bieżącej instancji.
    virtual VideoStream* clone() const = 0;

    //! Odczytuje następną klatkę.
    /** Przesuwa się w strumieniu na następną ramkę. Modyfikacji ulega
      * czas oraz timestamp (nadane im są te same wartości
      */
    virtual bool readNext() = 0;

    //! \retrun Pozycja w strumieniu.
    virtual double getTime() const = 0;
    //! \param time Pozycja w źródle
    virtual bool setTime(double time) = 0;
    //! \return Prawdziwy timestamp ramki.
    virtual double getFrameTimestamp() const = 0;
    //! \return Prawdziwy timestamp kolejnej ramki.
    virtual double getNextFrameTimestamp() const = 0;

    //! Jeżeli strumień jest w stanie zaprezentować ramkę w postaci spakowanej
    //! powinien przeładować tę metodę.
    //! \param dst
    virtual bool getData(Picture & /*dst*/) const
    { 
        return false; 
    }

    //! Jeżeli strumień jest w stanie zaprezentować ramkę w postaci zdjęcia
    //! powinien przeładować tę metodę.
    //! \param dst
    virtual bool getData(PictureLayered & /*dst*/) const
    { 
        return false; 
    }

    //! Konwertuje bieżącą ramkę do zadanego formatu. Docelowa ramka
    //! musi być zaalokowana.
    //! \param dst
    bool getFrame(Picture & dst) const;
    //! Konwertuje bieżącą ramkę do zadanego formatu. Docelowa ramka
    //! musi być zaalokowana.
    //! \param dst
    bool getFrame(PictureLayered & dst) const;

    //! \return Źródło.
    const std::string& getSource() const;
    //! \return Szerokość strumienia.
    inline int getWidth() const
    {
        return width;
    }
    //! \return Wysokość strumienia.
    inline int getHeight() const
    {
        return height;
    }
    //! \return Format strumienia.
    inline VIDLIB_PixelFormat getPixelFormat() const
    {
        return format;
    }
    //! \return Długość strumienia.
    inline double getDuration() const
    {
        return duration;
    }
    //! \return Długość ramki w sekundach.
    inline double getFrameDuration() const
    {
        return frameDuration;
    }
    //! \return Liczba klatek na sekundę.
    inline double getFramerate() const
    {
        return frameRate;
    }
    //! \return Łączna, przybliżona liczba klatek.
    int getFrameCount() const;

    //! \return Format.
    inline double getAspectRatio() const
    {
        return aspectRatio;
    }
    
    //! \return Znormalizowana pozycja w strumieniu.
    double getNormalizedTime() const;

    //! \param normalizedTime Znormalizowana pozycja w źródle.
    void setNormalizedTime(double normalizedTime);

    //! Czy to koniec strumienia?
    inline bool isEndOfStream() const
    {
        return getFrameTimestamp() >= getDuration();
    }

    //! Metoda używana tylko w przypadku wyłączenia wyjątków. Jeśli jakakolwiek
    //! publiczna metoda zwróci false można sprawdzić dokładnie błąd. Jeśli wyjątki
    //! są włączone zawsze zwróci NULL.
    //! \return Ostatni błąd.
    const VideoError * getLastError() const;

    //! \return
    inline Callback * getCallback()
    { 
        return callback;
    }
    //! \return
    inline const Callback * getCallback() const
    { 
        return callback;
    }
    //! \param callback Wskaźnik na callback. Strumień NIE kontroluje odwołania, tj.
    //!                 samodzielnie nie go nie zwalnia ani nie zeruje.
    inline void setCallback(Callback * callback) 
    { 
        this->callback = callback; 
    }

protected:
    //! Metoda którą należy wywołać po inicjalizacji strumienia w klasie pochodnej.
    //! \param source Źródło.
    bool onAfterInit(const std::string& source, double frameRate, double duration, 
      VIDLIB_PixelFormat format, int width, int height, double aspectRatio);

    //!
    bool notifyError( const VideoError& error );

    //!
    void onFrameRead();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__VIDEOSTREAM_H__
