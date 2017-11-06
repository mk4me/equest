#include "PCH.h"
#include "VideoBufferChunk.h"
#include "VidLibPrivate.h"
#include <vidlib/VideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

VideoBufferChunk::VideoBufferChunk()
  : start(INVALID_TIMESTAMP), stop(INVALID_TIMESTAMP)
{
  VIDLIB_FUNCTION_PROLOG;
}


VideoBufferChunk::const_iterator VideoBufferChunk::findFrame( double time ) const
{
  VIDLIB_FUNCTION_PROLOG;
  // zatrzymujemy się na pierszym wpisie mającym czas większy od zadanego
  const_iterator found = framesMap.upper_bound(time);
  if ( found == framesMap.begin() ) {
    // nie ma żadnego, zbyt mały timestamp
    return framesMap.end();
  } else {
    // tak naprawdę potrzebujemy poprzedniej ramki
    const_iterator result = --found;
    // sprawdzamy, czy to na pewno dobra ramka
    if ( result->second.first > time ) {
      // mieści się w czasie przed zakończeniem znalezionej ramki
      return result;
    } else {
      // po prostu dostaliśmy ostatni element
      return framesMap.end();
    }
  }
}

VideoBufferChunk::iterator VideoBufferChunk::findFrame( double time )
{
  VIDLIB_FUNCTION_PROLOG;
  // zatrzymujemy się na pierszym wpisie mającym czas większy od zadanego
  iterator found = framesMap.upper_bound(time);
  if ( found == framesMap.begin() ) {
    // nie ma żadnego, zbyt mały timestamp
    return framesMap.end();
  } else {
    // tak naprawdę potrzebujemy poprzedniej ramki
    iterator result = --found;
    // sprawdzamy, czy to na pewno dobra ramka
    if ( result->second.first > time ) {
      // mieści się w czasie przed zakończeniem znalezionej ramki
      return result;
    } else {
      // po prostu dostaliśmy ostatni element
      return framesMap.end();
    }
  }
}

const Picture * VideoBufferChunk::getHelper( const_iterator found, double * startTime /*= NULL*/, double * endTime /*= NULL */ ) const
{
  VIDLIB_FUNCTION_PROLOG;
  if ( found != framesMap.end() ) {
    // znaleziono!
    if ( startTime ) {
      *startTime = found->first;
    }
    if ( endTime ) {
      *endTime = found->second.first;
    }
    return found->second.second;
  } else {
    // nie znaleziono
    return NULL;
  }
}

Picture* VideoBufferChunk::popBack()
{
  VIDLIB_FUNCTION_PROLOG;
  Picture* result = NULL;
  if ( !framesMap.empty() ) {
    iterator it = --framesMap.end();
    stop = it->first;
    result = it->second.second;
    framesMap.erase(it);
  } else {
    start = stop = INVALID_TIMESTAMP;
  }
  return result;
}

void VideoBufferChunk::erase( iterator from, iterator to )
{
  VIDLIB_FUNCTION_PROLOG;
  if ( from != begin() && to != end() ) {
    throw std::runtime_error("Can remove only from bundaries.");
  }
  // usuwamy
  framesMap.erase(from, to);
  //
  if ( framesMap.empty() ) {
    start = stop = INVALID_TIMESTAMP;
  } else {
    start = framesMap.begin()->first;
    stop = framesMap.rbegin()->second.first;
  }
}

Picture* VideoBufferChunk::popFront()
{
  VIDLIB_FUNCTION_PROLOG;
  Picture* result = NULL;
  if ( !framesMap.empty() ) {
    iterator it = framesMap.begin();
    start = it->second.first;
    result = it->second.second;
    framesMap.erase(it);
  } else {
    start = stop = INVALID_TIMESTAMP;
  }
  return result;
}

void VideoBufferChunk::append( Picture * picture, double startTime, double endTime )
{
  VIDLIB_FUNCTION_PROLOG;
  UTILS_ASSERT(startTime < endTime);
  if ( !framesMap.empty() ) {
    // czy jest sens wrzucać?
    if (startTime == getEndTime()) {
      stop = endTime;
    } else if ( endTime == getStartTime() ) {
      start = startTime;
    } else {
      throw new std::runtime_error("Could not append.");
    }
  } else {
    // pierwsza ramka, ustalamy ram
    start = startTime;
    stop = endTime;
  }
  // czy już było?
  if (!framesMap.insert({ startTime, std::make_pair(endTime, picture) }).second) {
    throw new std::runtime_error("Frame already in buffer.");
  }
}


////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

