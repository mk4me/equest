#include "PCH.h"
#include "PointSchemeDrawer.h"

using namespace osg;
using namespace boost;
using namespace std;


void PointSchemeDrawer::init( SkeletalVisualizationSchemeWeak scheme )
{
    UTILS_ASSERT(scheme.lock());
    OsgSchemeDrawer::init(scheme);

    node = new osg::Group;
    auto markers = scheme.lock()->getJointStates();
    createMarkersCrowd(markers);
}

void PointSchemeDrawer::deinit()
{
    // TODO -> unparent
}

void PointSchemeDrawer::update( double time )
{
    auto markers = getVisualiztionScheme().lock()->getJointStates();
    for (int i = markers.size() - 1; i >= 0; --i) {
        points[i]->setPosition(markers[i].position);
    }
}

void PointSchemeDrawer::draw()
{

}

void PointSchemeDrawer::createMarkersCrowd(const std::vector<MarkerState>& markers)
{
    int count = markers.size();
    points.resize(count);
    for (int i = 0; i < count; ++i) {
        Vec3 position = markers[i].position;
        points[i] = addPoint(position);
        node->addChild(points[i]);
    }
}

PointSchemeDrawer::GeodePtr PointSchemeDrawer::createMarker(const osg::Vec4& color, float scale) const
{
    PointSchemeDrawer::GeodePtr geode = new osg::Geode();
    ref_ptr<Geometry> geometry = new osg::Geometry();

    geode->addDrawable(geometry);

    ref_ptr<Vec3Array> pyramidVertices = new osg::Vec3Array;

    pyramidVertices->push_back(osg::Vec3( 0.0f, 0.0f, 0.0f) * scale);
    pyramidVertices->push_back(osg::Vec3( 1.0f, 0.0f, 0.0f) * scale);
    pyramidVertices->push_back(osg::Vec3( 1.0f, 1.0f, 0.0f) * scale);
    pyramidVertices->push_back(osg::Vec3( 0.0f, 1.0f, 0.0f) * scale);
    pyramidVertices->push_back(osg::Vec3( 0.5f, 0.5f, 1.0f) * scale);

    geometry->setVertexArray( pyramidVertices );

    osg::DrawElementsUInt* geometryBase =
        new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    geometryBase->push_back(3);
    geometryBase->push_back(2);
    geometryBase->push_back(1);
    geometryBase->push_back(0);
    geometry->addPrimitiveSet(geometryBase);

    osg::DrawElementsUInt* pyramidFaceOne =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceOne->push_back(0);
    pyramidFaceOne->push_back(1);
    pyramidFaceOne->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceOne);

    osg::DrawElementsUInt* pyramidFaceTwo =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceTwo->push_back(1);
    pyramidFaceTwo->push_back(2);
    pyramidFaceTwo->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceTwo);

    osg::DrawElementsUInt* pyramidFaceThree =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceThree->push_back(2);
    pyramidFaceThree->push_back(3);
    pyramidFaceThree->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceThree);

    osg::DrawElementsUInt* pyramidFaceFour =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceFour->push_back(3);
    pyramidFaceFour->push_back(0);
    pyramidFaceFour->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceFour);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);
    colors->push_back(color);
    colors->push_back(color);
    colors->push_back(color);
    colors->push_back(color);

    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    return geode;
}

PointSchemeDrawer::TransformPtr PointSchemeDrawer::addPoint( const osg::Vec3& point )
{
    float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    Vec4 color(rand, 1, rand, 1);
    GeodePtr marker = createMarker(color, 1.5f);
    TransformPtr transform = new PositionAttitudeTransform();
    transform->addChild(marker);
    transform->setPosition(point);
    return transform;
}
