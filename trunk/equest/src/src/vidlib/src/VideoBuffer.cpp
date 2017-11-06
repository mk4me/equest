#include "PCH.h"
#include <iostream>
#include "VidLibPrivate.h"
#include "VideoBufferChunk.h"
#include <vidlib/VideoBuffer.h>
#include <float.h>
#include <stack>

// TODO: pixelFormat->bogatyEnum, buforowanie


template <class Iter, class Val, class Pred>
Iter lowerBound( Iter first, Iter last, const Val& val, Pred pred )
{
  VIDLIB_FUNCTION_PROLOG;
  typedef typename std::iterator_traits<Iter>::difference_type Diff;
  Diff count = std::distance(first, last);
  while ( count > 0 ) {
    // wybieramy środkowy element
    Diff count2 = (count>>1);
    Iter mid = first;
    std::advance(mid, count2);
    // równoważne *mid < val
    if ( pred(*mid, val) ) {
      first = ++mid;
      count -= count2 + 1;
    } else {
      count = count2;
    }
  }
  return first;
}

template <class Iter, class Val, class Pred>
Iter upperBound( Iter first, Iter last, const Val& val, Pred pred )
{
  VIDLIB_FUNCTION_PROLOG;
  typedef typename std::iterator_traits<Iter>::difference_type Diff;
  Diff count = std::distance(first, last);
  while ( count > 0 ) {
    // wybieramy środkowy element
    Diff count2 = (count>>1);
    Iter mid = first;
    std::advance(mid, count2);
    // równoważne *mid < val
    if ( !pred(val, *mid) ) {
      first = ++mid;
      count -= count2 + 1;
    } else {
      count = count2;
    }
  }
  return first;
}



////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

struct PredStartTimeLE
{
    bool operator()(VideoBufferChunk* chunk, double time)
    {
        return chunk->getStartTime() <= time;
    }
};

struct PredStartTimeL
{
    bool operator()(VideoBufferChunk* chunk, double time)
    {
        return chunk->getStartTime() < time;
    }
    bool operator()(double time, VideoBufferChunk* chunk)
    {
        return time < chunk->getStartTime();
    }
};

//------------------------------------------------------------------------------

//! Bufor ramek o maksymalnym zadanym rozmiarze.
class VideoBufferImpl
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
    VIDLIB_PixelFormat format;

public:
    //! 
    //! \param maxSize
    //! \param width
    //! \param height
    //! \param format
    VideoBufferImpl(int maxSize, int width, int height, VIDLIB_PixelFormat format);
    //! Destruktor, zwalnia cały bufor.
    ~VideoBufferImpl();

    //! \return
    inline VIDLIB_PixelFormat getFormat() const
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
    const Picture * getLastFrame(double * timestamp = NULL, double * finish = NULL);
    //! \param timestamp
    //! \param finish
    const Picture * getFirstFrame(double * timestamp = NULL, double * finish = NULL);

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

//------------------------------------------------------------------------------

VideoBufferImpl::VideoBufferImpl(int maxSize, int width, int height, VIDLIB_PixelFormat format)
  : maxSize(maxSize), width(width), height(height), format(format)
{
  VIDLIB_FUNCTION_PROLOG;
  maxBufferSize = maxSize / Picture::getAllocSize(width, height, format);
}

VideoBufferImpl::~VideoBufferImpl()
{
  VIDLIB_FUNCTION_PROLOG;
  for ( std::vector<Picture*>::iterator it = buffer.begin(); it != buffer.end(); ++it ) {
    (*it)->free();
    delete *it;
  }
}

Picture * VideoBufferImpl::pop()
{
  VIDLIB_FUNCTION_PROLOG;
  if ( unused.empty() ) {
    // nie ma niewykorzystanych, trzeba więc zaalokować nowe (jeśli jest miejsce)
    if ( buffer.size() == maxBufferSize ) {
      // koniec miejsca
      return NULL;
    } else {
      // miejsce jest, można alokować
      Picture * picture = new Picture(Picture::create(width, height, format));
      buffer.push_back(picture);
      return picture;
    }
  } else {
    // zdejmujemy ze stosu nieużywanych
    Picture * result = unused.top();
    unused.pop();
    return result;
  }
}

void VideoBufferImpl::push( Picture * frame )
{
  VIDLIB_FUNCTION_PROLOG;
  // dodajemy do stosu nieużywanych
  unused.push(frame);
}

bool VideoBufferImpl::freeFrame( double time )
{
  VIDLIB_FUNCTION_PROLOG;
  Chunks::iterator begin = chunks.begin(), end = chunks.end();
  Chunks::iterator it = getChunk(time, begin, end);
  if ( it != end ) {
    VideoBufferChunk * chunk = *it;
    // ok, jak usuwamy?
    VideoBufferChunk::iterator frame = chunk->findFrame(time);
    unused.push(frame->second.second);
    if ( frame == chunk->begin() ) {
      if ( frame == --chunk->end() ) {
        // chunk ma tylko tę ramkę, usuwamy go
        delete *it;
        chunks.erase(it);
      } else {
        // usuwamy z początku
        chunk->popFront();
      }
    } else if ( frame == --chunk->end() ) {
      // usuwamy z tyłu
      chunk->popBack();
    } else {
      // kopiujemy cześć do nowego bufora
      VideoBufferChunk * newChunk = new VideoBufferChunk();
      VideoBufferChunk::const_iterator startCopy = frame;
      newChunk->append<VideoBufferChunk::const_iterator>(++startCopy, chunk->end());
      // usuwamy zbędne z bufora
      chunk->erase(frame, chunk->end());
      // dodajemy do listy chunków
      chunks.insert(++it, newChunk);
    }
    return true;
  } else {
    // nie ma chunka który by to obsłużył
    return false;
  }
}


bool VideoBufferImpl::notifyUsed( Picture * frame, double startTime, double endTime )
{
  VIDLIB_FUNCTION_PROLOG;
#if _DEBUG
  // sprawdzamy, czy ramki na pewno nie ma na liście użytych i nieużytych
  //assert( std::find(unused.begin(), unused.end(), frame) == unused.end() );
  UTILS_ASSERT( getFrame(startTime) == NULL );
#endif
  UTILS_ASSERT( startTime < endTime );
  UTILS_ASSERT( startTime >= 0.0 );

  // wyszukujemy bufor, który się nadaje
  PredStartTimeL pred;
  Chunks::iterator begin = chunks.begin(), end = chunks.end();
  Chunks::iterator found = upperBound( begin, end, startTime, pred );
  Chunks::iterator next = found;
  Chunks::iterator prev = next==begin ? end : --Chunks::iterator(next);

  // sprawdzenie poprawności bufora, tj fragmenty nie mogą na siebie nachodzić
  if ( next != end ) {
    double chunkStart = (*next)->getStartTime();
    if ( chunkStart < endTime ) {
      return false;
    } else if ( chunkStart != endTime ) {
      next = end;
    }
  }
  if ( prev != end ) {
    double chunkEnd = (*prev)->getEndTime();
    if ( chunkEnd > startTime ) {
      return false;
    } else if ( chunkEnd != startTime ) {
      prev = end;
    }
  }

  if ( next != end ) {
    if ( prev != end ) {
      // dopisać i połączyć oba bufory
      (*prev)->append(frame, startTime, endTime);
      (*prev)->append((*next)->begin(), (*next)->end());
      delete *next;
      chunks.erase(next);
    } else {
      // dopisać na początek
      (*next)->append(frame, startTime, endTime);
    }
  } else if ( prev != end ) {
    // dopisać na koniec
    (*prev)->append(frame, startTime, endTime);
  } else {
    // trzeba stworzyć nowy bufor
    VideoBufferChunk * newChunk = new VideoBufferChunk();
    newChunk->append(frame, startTime, endTime);
    // wstawiamy
    chunks.insert( found, newChunk );
  }
  return true;
}

bool VideoBufferImpl::freeFirstFrame()
{
  VIDLIB_FUNCTION_PROLOG;
  if ( !chunks.empty() ) {
    // usuwamy pierwszy element
    VideoBufferChunk * chunk = *chunks.begin();
    Picture * picture = chunk->popFront();
    // czy usuwamy chunk?
    if ( chunk->empty() ) {
      delete chunk;
      chunks.pop_front();
    }
    // dodajemy do nieużywanych
    unused.push(picture);
    return true;
  } else {
    return false;
  }
}

bool VideoBufferImpl::freeLastFrame()
{
  VIDLIB_FUNCTION_PROLOG;
  if ( !chunks.empty() ) {
    // usuwamy pierwszy element
    VideoBufferChunk * chunk = *--chunks.end();
    Picture * picture = chunk->popBack();
    // czy usuwamy chunk?
    if ( chunk->empty() ) {
      delete chunk;
      chunks.pop_back();
    }
    // dodajemy do nieużywanych
    unused.push(picture);
    return true;
  } else {
    return false;
  }
}

const Picture * VideoBufferImpl::getFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
  VIDLIB_FUNCTION_PROLOG;
  Chunks::iterator it = getChunk(time, chunks.begin(), chunks.end());
  if ( it != chunks.end() ) {
    return (*it)->getFrame(time, timestamp, finish);
  } else {
    return NULL;
  }
}

const Picture * VideoBufferImpl::getNearestFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
  VIDLIB_FUNCTION_PROLOG;
  // jest co szukać?
  if ( chunks.empty() ) {
    return NULL;
  }

  // wyszukanie pierwszego chunka za zadanym czasem
  PredStartTimeLE pred;
  Chunks::iterator begin = chunks.begin();
  Chunks::iterator end = chunks.end();
  Chunks::iterator next = lowerBound(begin, end, time, pred);
  Chunks::iterator prev = next;

  // odległości od obu chunków
  double deltaRight = (next == end ? DBL_MAX : (*next)->getStartTime() - time);
  double deltaLeft = (prev == begin ? DBL_MAX : time - (*--prev)->getEndTime());

  if ( deltaLeft < 0.0 ) {
    return (*prev)->getFrame(time, timestamp, finish);
  } else if ( deltaRight < deltaLeft ) {
    return (*next)->getFirstFrame(timestamp, finish);
  } else {
    return (*prev)->getLastFrame(timestamp, finish);
  }
}


template <class Iter>
Iter VideoBufferImpl::getChunk( double time, Iter begin, Iter end ) const
{
  VIDLIB_FUNCTION_PROLOG;
  PredStartTimeL pred;
  Iter found = upperBound(begin, end, time, pred);
  if ( found != begin && (*--found)->getEndTime() > time ) {
    return found;
  } else {
    return end;
  }
}

const Picture * VideoBufferImpl::getLastFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    VIDLIB_FUNCTION_PROLOG;
    return chunks.empty() ? NULL : (*--chunks.end())->getLastFrame(timestamp, finish);
}

const Picture * VideoBufferImpl::getFirstFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    VIDLIB_FUNCTION_PROLOG;
    return chunks.empty() ? NULL : (*chunks.begin())->getFirstFrame(timestamp, finish);
}





// void VideoBufferImpl::trimBuffer( double begin, double end )
// {
//   // wyszukujemy granicy przedziału
//   UsedMap::iterator first = used.lower_bound(begin);
//   // dodajemy do listy nieużywanych
//   for (UsedMap::iterator it = used.begin(); it != first; ++it) {
//     unused.push_back(it->second.second);
//   }
//   // usuwamy z mapy
//   used.erase(used.begin(), first);
//
//   // powtarzamy...
//   UsedMap::iterator last = used.upper_bound(end);
//   for (UsedMap::iterator it = last; it != used.end(); ++it) {
//     unused.push_back(it->second.second);
//   }
//   used.erase(last, used.end());
// }
//
// void VideoBufferImpl::freeOldest()
// {
//   if ( !used.empty() ) {
//     unused.push_back(used.begin()->second.second);
//     used.erase(used.begin());
//   }
// }
//
// void VideoBufferImpl::freeLatest()
// {
//   if ( !used.empty() ) {
//     UsedMap::iterator it = --used.end();
//     unused.push_back(it->second.second);
//     used.erase(it);
//   }
// }

VideoBuffer::VideoBuffer( int maxSize, int width, int height, VIDLIB_PixelFormat format )
: impl(new VideoBufferImpl(maxSize, width, height, format))
{

}

VideoBuffer::~VideoBuffer()
{
    utils::deletePtr(impl);
}

VIDLIB_PixelFormat VideoBuffer::getFormat() const
{
    return impl->getFormat();
}

Picture * VideoBuffer::pop()
{
    return impl->pop();
}

void VideoBuffer::push( Picture * frame )
{
    impl->push(frame);
}

bool VideoBuffer::notifyUsed( Picture * frame, double startTime, double endTime )
{
    return impl->notifyUsed(frame, startTime, endTime);
}

bool VideoBuffer::freeFrame( double time )
{
    return impl->freeFrame(time);
}

bool VideoBuffer::freeFirstFrame()
{
    return impl->freeFirstFrame();
}

bool VideoBuffer::freeLastFrame()
{
    return impl->freeLastFrame();
}

const Picture * VideoBuffer::getLastFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getLastFrame(timestamp, finish);
}

const Picture * VideoBuffer::getFirstFrame( double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getFirstFrame(timestamp, finish);
}

const Picture * VideoBuffer::getFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getFrame(time, timestamp, finish);
}

const Picture * VideoBuffer::getNearestFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
    return impl->getNearestFrame(time, timestamp, finish);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
