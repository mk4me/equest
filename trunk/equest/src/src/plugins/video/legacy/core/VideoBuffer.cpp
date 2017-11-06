#include "../VideoPCH.h"
#include <iostream>
#include "VMPrivate.h"
#include "VideoBufferChunk.h"
#include "VideoBuffer.h"
#include <float.h>

// TODO: pixelFormat->bogatyEnum, buforowanie


template <class Iter, class Val, class Pred>
Iter lowerBound( Iter first, Iter last, const Val& val, Pred pred )
{
  VM_FUNCTION_PROLOG;
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
  VM_FUNCTION_PROLOG;
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
namespace video {
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

VideoBuffer::VideoBuffer(int maxSize, int width, int height, video::PixelFormat format)
  : maxSize(maxSize), width(width), height(height), format(format)
{
  VM_FUNCTION_PROLOG;
  maxBufferSize = maxSize / Picture::getAllocSize(width, height, format);
}

VideoBuffer::~VideoBuffer()
{
  VM_FUNCTION_PROLOG;
  for ( std::vector<Picture*>::iterator it = buffer.begin(); it != buffer.end(); ++it ) {
    (*it)->free();
    delete *it;
  }
}

Picture * VideoBuffer::pop()
{
  VM_FUNCTION_PROLOG;
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
    video::Picture * result = unused.top();
    unused.pop();
    return result;
  }
}

void VideoBuffer::push( video::Picture * frame )
{
  VM_FUNCTION_PROLOG;
  // dodajemy do stosu nieużywanych
  unused.push(frame);
}

bool VideoBuffer::freeFrame( double time )
{
  VM_FUNCTION_PROLOG;
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


bool VideoBuffer::notifyUsed( Picture * frame, double startTime, double endTime )
{
  VM_FUNCTION_PROLOG;
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

bool VideoBuffer::freeFirstFrame()
{
  VM_FUNCTION_PROLOG;
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

bool VideoBuffer::freeLastFrame()
{
  VM_FUNCTION_PROLOG;
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

const Picture * VideoBuffer::getFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
  VM_FUNCTION_PROLOG;
  Chunks::iterator it = getChunk(time, chunks.begin(), chunks.end());
  if ( it != chunks.end() ) {
    return (*it)->getFrame(time, timestamp, finish);
  } else {
    return NULL;
  }
}

const Picture * VideoBuffer::getNearestFrame( double time, double * timestamp /*= NULL*/, double * finish /*= NULL*/ )
{
  VM_FUNCTION_PROLOG;
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
Iter VideoBuffer::getChunk( double time, Iter begin, Iter end ) const
{
  VM_FUNCTION_PROLOG;
  PredStartTimeL pred;
  Iter found = upperBound(begin, end, time, pred);
  if ( found != begin && (*--found)->getEndTime() > time ) {
    return found;
  } else {
    return end;
  }
}





// void VideoBuffer::trimBuffer( double begin, double end )
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
// void VideoBuffer::freeOldest()
// {
//   if ( !used.empty() ) {
//     unused.push_back(used.begin()->second.second);
//     used.erase(used.begin());
//   }
// }
//
// void VideoBuffer::freeLatest()
// {
//   if ( !used.empty() ) {
//     UsedMap::iterator it = --used.end();
//     unused.push_back(it->second.second);
//     used.erase(it);
//   }
// }

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////
