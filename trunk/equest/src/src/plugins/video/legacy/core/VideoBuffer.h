/********************************************************************
	created:  2010/07/28
	created:  28:7:2010   14:40
	filename: VideoBuffer.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__FRAMEBUFFER_H__
#define __HEADER_GUARD__FRAMEBUFFER_H__

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

class VideoBufferChunk;

//! Bufor ramek o maksymalnym zadanym rozmiarze.
class VideoBuffer
{
private:
  //! Typ listy chunków.
  typedef std::list<VideoBufferChunk*> Chunks;

private:
  //! Chunki bufora.
  Chunks chunks;
  //! Wszystkie ramki.
  std::vector<Picture*> buffer;
  //! Nieużywne, a zaalokowane ramki
  std::stack<Picture*> unused;
  //! Maksymalny rozmiar buforu.
  const int maxSize;
  //! Szerokość ramki.
  int width;
  //! Wysokość ramki.
  int height;
  //! Ile ramek maksymalnie można zbuforować?
  size_t maxBufferSize;
  //! Format ramki
  PixelFormat format;

public:
  //! 
  //! \param maxSize
  //! \param width
  //! \param height
  //! \param format
  VideoBuffer(int maxSize, int width, int height, PixelFormat format);
  //! Destruktor, zwalnia cały bufor.
  ~VideoBuffer();

  //! \return
  inline PixelFormat getFormat() const
  { 
    return format;
  }

  //! \return Wskaźnik na bufor ramki albo NULL, jeżeli wyczerpało się miejsce w buforze.
  Picture * pop();
  //! \param frame Ramka, która ma być zwrócona do bufora. Wywoływać TYLKO, jeśli po użyciu
  //!              metody pop ramka nie została użyta.
  void push(Picture * frame);

  //! Zaznacza, że ramka jest użyta.
  //! \param frame
  //! \param startTime Początek przedziału czasu.
  //! \param endTime Koniec przedziału czasu.
  bool notifyUsed(Picture * frame, double startTime, double endTime);
  //! Zwalania ramkę w zadanym punkcie czasowym.
  bool freeFrame(double time);
  //!
  bool freeFirstFrame();
  //!
  bool freeLastFrame();

  //! \param timestamp
  //! \param finish
  inline const Picture * getLastFrame(double * timestamp = NULL, double * finish = NULL)
  {
    return chunks.empty() ? NULL : (*--chunks.end())->getLastFrame(timestamp, finish);
  }
  //! \param timestamp
  //! \param finish
  inline const Picture * getFirstFrame(double * timestamp = NULL, double * finish = NULL)
  {
    return chunks.empty() ? NULL : (*chunks.begin())->getFirstFrame(timestamp, finish);
  }

  //! \param time
  //! \param timestamp
  //! \param finish
  //! \return Zbuforowana ramka dla zdanego czasu bądź NULL, jeżeli nie jest zbuforowana.
  const Picture * getFrame(double time, double * timestamp = NULL, double * finish = NULL);
  //! \param time
  //! \param timestamp
  //! \param finish
  //! \return Zbuforowana ramka dla zdanego czasu bądź NULL, jeżeli nie jest zbuforowana.
  const Picture * getNearestFrame(double time, double * timestamp = NULL, double * finish = NULL);



private:
 
  Chunks::iterator getChunk( double time );
  Chunks::const_iterator getChunk( double time ) const;

  template <class Iter>
  Iter getChunk( double time, Iter begin, Iter end ) const;

};


////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__FRAMEBUFFER_H__
