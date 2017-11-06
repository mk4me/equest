/********************************************************************
	created:  2010/08/04
	created:  4:8:2010   14:44
	filename: VideoBufferChunk.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOBUFFERCHUNK_H__
#define HEADER_GUARD_VIDLIB__VIDEOBUFFERCHUNK_H__

#include <map>
#include <vidlib/PixelFormat.h>
#include <vidlib/Picture.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! Ciągły fragment bufora.
class VideoBufferChunk
{
public:
  // TODO: zamienić na deque
  //! Mapa użycia bufora.
  typedef std::map<double, std::pair<double, Picture*> > UsedMap;
  //! Iterator.
  typedef UsedMap::iterator iterator;
  //! Stały iterator.
  typedef UsedMap::const_iterator const_iterator;

private:
  //! Czas rozpoczęcia.
  double start;
  //! Czas zakończenia.
  double stop;
  //! Mapa użycia bufora.
  UsedMap framesMap;

public:

  //! Konstruktor zerujący.
  VideoBufferChunk();

  //! \return Początek obejmowanego przedziału (domknięty).
  inline double getStartTime() const
  {
    return start;
  }
  //! \return Koniec obejmowanego przedziału (otwarty)
  inline double getEndTime() const
  {
    return stop;
  }

  //! \return Początek.
  inline iterator begin()
  {
    return framesMap.begin();
  }
  //! \return Koniec.
  inline iterator end()
  {
    return framesMap.end();
  }
  //! \return Początek.
  inline const_iterator begin() const
  {
    return framesMap.begin();
  }
  //! \return Koniec.
  inline const_iterator end() const
  {
    return framesMap.end();
  }
  //! Czy coś jest?
  inline bool empty() const
  {
    return framesMap.empty();
  }
  //! \param time Czas do sprawdzenia.
  //! \return Czy bufor zawiera dany czas?
  inline bool contains(double time) const
  {
    return time >= getStartTime() && time < getEndTime();
  }
  //! \param startTime
  //! \param endTime
  //! \return Czy można uzupełnić bufor?
  inline bool canAppend(double startTime, double endTime) const
  {
    return startTime == getEndTime() || endTime == getStartTime();
  }
  //!
  //! \param first
  //! \param last
  //! \throws std::runtime_error
  template <class Iter>
  void append(Iter first, Iter last)
  {
    for ( Iter it = first; it != last; ++it ) {
      append( it->second.second, it->first, it->second.first );
    }
  }
  //!
  //! \param picture
  //! \param startTime
  //! \param endTime
  //! \throws std::runtime_error
  void append(Picture * picture, double startTime, double endTime);

  //! \return Usunięty element z końca bufora.
  Picture* popBack();

  //!
  void erase(iterator from, iterator to);

  //! \return Usunięty element z początku bufora.
  Picture* popFront();

  //! \param time
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu bądź NULL, jeżeli nie jest zbuforowana.
  inline const Picture * getFrame(double time, double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( findFrame(time), startTime, endTime );
  }
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu bądź NULL, jeżeli nie jest zbuforowana.
  inline const Picture * getFirstFrame(double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( framesMap.begin(), startTime, endTime );
  }
  //! \param startTime
  //! \param endTime
  //! \return Zbuforowana ramka dla zdanego czasu bądź NULL, jeżeli nie jest zbuforowana.
  inline const Picture * getLastFrame(double * startTime = NULL, double * endTime = NULL) const
  {
    return getHelper( framesMap.empty() ? framesMap.end() : --framesMap.end(), startTime, endTime );
  }

  //!
  //! \param time
  //! \return Zbuforowana ramka dla zdanego czasu bądź end(), jeżeli nie jest zbuforowana.
  const_iterator findFrame(double time) const;
    //!
  //! \param time
  //! \return Zbuforowana ramka dla zdanego czasu bądź end(), jeżeli nie jest zbuforowana.
  iterator findFrame(double time);

private:

  //!
  //! \param found
  //! \param startTime
  //! \param endTime
  const Picture * getHelper( UsedMap::const_iterator found, double * startTime = NULL, double * endTime = NULL ) const;

};


////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////



#endif  // HEADER_GUARD_VIDLIB__VIDEOBUFFERCHUNK_H__
