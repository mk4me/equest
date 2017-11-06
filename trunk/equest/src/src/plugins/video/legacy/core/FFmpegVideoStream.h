/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:14
	filename: FFmpegVideoStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __AVINOUT_FFMPEGVIDEOINPUT_H__
#define __AVINOUT_FFMPEGVIDEOINPUT_H__

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVStream;
struct AVPacket;
struct AVPicture;

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Strumień wejściowy video kontorlowany przez bibliotekę ffmpeg.
 */
class FFmpegVideoStream : public VideoStream
{
//------------------------------------------------------------------------------
public:
  //!
  enum LockOp {
    LockOpCreate,
    LockOpDestroy,
    LockOpRelease,
    LockOpObtain
  };

  //!
  typedef int (*LockManager)(void **mutex, LockOp lock);

  //! Klasa FFmpeg używana podczas logowania wiadomości.
  struct FFmpegClass {
      //! Nazwa klasy.
      const char* name;
      //! Wskaźnik do AVClass. Gdy chce się więcej informacji - należy zaincludować
      //! avutil/log.h oraz ręcznie rzutować.
      void* ptr;
  };

  //! Poziom ważności komunikatów.
  enum LogSeverity {
    LogSeverityQuiet,
    LogSeverityPanic,
    LogSeverityFatal,
    LogSeverityError,
    LogSeverityWarning,
    LogSeverityInfo,
    LogSeverityVerbose,
    LogSeverityDebug
  };

  //! Typ callbacka otrzymującego komunikaty z ffmpeg.
  typedef void (*LogCallback)(LogSeverity severity, const char* msg, FFmpegClass* item, FFmpegClass* parent);

//------------------------------------------------------------------------------
private:
  //! Pomocnicza klasa do inicjalizacji ffmpega.
  class Initializer
  {
  public:
    Initializer();
    ~Initializer();
  };

  //! Statystki dotyczące klatek kluczowych. Służą do optymalizacji operacji
  //! przesuwania się w strumieniu.
  struct KeyframeStats {
    //! Liczba przetworzonych klatek kluczowych.
    unsigned int count;
    //! Czas ostatniej ramki kluczowej. W jednostkach strumienia.
    int64_t lastTimestamp;
    //! Średni odstęp między ramkami kluczowymi.
    double avgSpan;
    //! Maksymalny odstęp między klatkami kluczowymi.
    int64_t maxSpan;
  };

//------------------------------------------------------------------------------
private:
  //! 
  AVFormatContext * formatContext;
  //! Kodek video.
  AVCodecContext * codecContext;
  //! Strumień.
  AVStream * videoStream;
  //! Bieżąca ramka.
  AVFrame * frame;
  //! Prawdziwy timestamp prezentacji bieżącej ramki. W jednostkach strumienia.
  int64_t frameTimestamp;
  //! Minimalny odstęp między ramkami.
  int64_t frameSpan;
  //! Timestamp następnej ramki.
  int64_t nextFrameTimestamp;

  //! Czas w źródle widziany na zewnątrz klasy. Może być >= frameTimestamp,
  //! ale tylko jeśli wciąż dotyczy tej samej klatki.
  double wantedTime;
  //! Statystyki klatek kluczowych.
  KeyframeStats keyframeStats;

  //! Liczba bajtów do zdekodowania. Używane tylko w starym API.
  int frameBytesRemaining;
  //! Dane pakietu. Tylko w starym API.
  const uint8_t * frameData;
  //! Pakiet.
  AVPacket * packet;
  //! Prawdziwy używany w momencie, gdy źródłowy nie ma wyalignowanych danych.
  AVPacket * alignedPacket;

//------------------------------------------------------------------------------
public:
  //! \param source Źródło.
  FFmpegVideoStream(const std::string& source, int wantedVideoStream = -1);
  //!
  virtual ~FFmpegVideoStream();

//------------------------------------------------------------------------------
public:
  

  //!
  virtual bool readNext();
  //! 
  virtual double getFrameTimestamp() const;
  //!
  virtual double getNextFrameTimestamp() const;
  //! \retrun Pozycja w strumieniu.
  virtual double getTime() const;
  //! \param time Pozycja w źródle.
  virtual bool setTime(double time);

  //! \param callback
  static void setLockManager(LockManager callback);

  //! 
  //! \param callback
  static void setLogCallback(LogCallback callback);

//------------------------------------------------------------------------------
protected:
  //! Jeżeli strumień jest w stanie zaprezentować ramkę w postaci zdjęcia
  //! powinien przeładować tę metodę.
  //! \param dst
  virtual bool getData(PictureLayered & dst);

//------------------------------------------------------------------------------
private:
  //! Pomija ramki aż dotrze do zadanego czasu.
  //! Jeśli po wywołaniu tej funkcji warunek frameTimestamp >= targetTimestamp nie jest spełniony
  //! strumień znajduje się w nieokreślonym stanie i trzeba ręcznie ustawić jego pozycję metodą
  //! seekToKeyframe.
  bool skipFramesToTimestamp( int64_t targetTimestamp, int maxFramesToSkip, bool stopOnKeyframe );
  //! Przechodzi do wybranego miejsca w strumieniu.
  bool seekToKeyframe(int64_t timestamp, bool pickNextFrame);
  //! Odczytuje klatkę z bieżącej pozycji w strumieniu.
  bool readFrame();
  //!
  void alignPacket(AVPacket * packet);
  //!
  int64_t getTimestamp( AVFrame * pFrame, int64_t packetDTS );
  //!
  inline double timestampToSec( int64_t timestamp ) const;
  //!
  inline int64_t secToTimestap( double time ) const;
  //! Czy bieżąca klatka jest kluczowa?
  inline bool isKeyframe() const;

  //!
  bool init(const std::string& source, int wantedVideoStream = -1);
  //! Wewnętrzny callback wywoływany w momencie kiedy uda się zdekodować ramkę.
  void onGotFrame(AVFrame * frame, int64_t timestamp);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_FFMPEGVIDEOINPUT_H__
