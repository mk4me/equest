#ifndef HEADER_GUARD__UNIQUECOLLECTION_H__
#define HEADER_GUARD__UNIQUECOLLECTION_H__

#include <utils/Utils.h>
#include <utils/Enum.h>
#include <utils/Debug.h>

template<typename T>
void pushBackUnique( std::vector<T>& vec, T& val )
{
    auto it = std::find(vec.begin(), vec.end(), val);
    UTILS_ASSERT(it == vec.end());
    vec.push_back(val);
}

template<typename T>
void removeFromVector( std::vector<T>& vec, T& val )
{
    auto it = std::find(vec.begin(), vec.end(), val);
    UTILS_ASSERT(it != vec.end());
    vec.erase(it);
}



#endif
