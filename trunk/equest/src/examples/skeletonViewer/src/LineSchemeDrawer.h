#ifndef HEADER_GUARD_LINESCHEMEDRAWER_H__
#define HEADER_GUARD_LINESCHEMEDRAWER_H__

#include <boost/shared_ptr.hpp>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujacy linie szkieletu
class LineSchemeDrawer : public OsgSchemeDrawer
{
public:
    virtual void draw();
    virtual void update(double time);
    virtual void deinit();
    virtual void init(SkeletalVisualizationSchemeWeak scheme);
    virtual osg::ref_ptr<osg::Node> getNode();

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    typedef SkeletalVisualizationScheme::Connection Connection;
    //typedef SkeletalVisualizationScheme::BoneState JointState;

private:
    void addLine(const osg::Vec3& from, const osg::Vec3& to, osg::Vec4& color);

private:
    std::vector<GeodePtr> lines;
    std::vector< osg::ref_ptr<osg::Vec3Array> > buffers;
    osg::ref_ptr<osg::Group> node;
};

#endif
