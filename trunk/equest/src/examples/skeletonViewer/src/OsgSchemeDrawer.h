#ifndef HEADER_GUARD_OSGSCHEMEDRAWER_H__
#define HEADER_GUARD_OSGSCHEMEDRAWER_H__

#include "ISchemeDrawer.h"

//! Klasa bazowa dla schematow osg specyficznych
class OsgSchemeDrawer : public ISchemeDrawer
{
public:
    //! metoda powinna zwracac wezel 
    virtual osg::ref_ptr<osg::Node> getNode() = 0;
    virtual void init(SkeletalVisualizationSchemeWeak scheme) { visualizationScheme = scheme; }

protected:
    SkeletalVisualizationSchemeWeak getVisualiztionScheme() const { return visualizationScheme; }

private:
    SkeletalVisualizationSchemeWeak visualizationScheme;
};

#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__
