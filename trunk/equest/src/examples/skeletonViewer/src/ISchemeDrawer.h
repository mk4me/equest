#ifndef HEADER_GUARD_ISCHEMEDRAWER_H__
#define HEADER_GUARD_ISCHEMEDRAWER_H__

#include <boost/utility.hpp>
#include "skeletalVisualizationScheme.h"

//! Interfejs realizujacy 'strategie' dla schematu wizualizacji
class ISchemeDrawer : boost::noncopyable
{
public:
    virtual void draw() = 0;
    virtual void update(double time) = 0;
    virtual void deinit() = 0;
    virtual void init(SkeletalVisualizationSchemeWeak scheme) = 0;
};
#endif //HEADER_GUARD_ISCHEMEDRAWER_H__
