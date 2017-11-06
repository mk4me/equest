#include "PCH.h"
#include <kinematiclib/KinematicModel.h>
#include "uniqueCollection.h"
#include "skeletalVisualizationScheme.h"
#include "ISchemeDrawer.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

SkeletalVisualizationScheme::SkeletalVisualizationScheme() :
    currentTime(0)
{
}

void SkeletalVisualizationScheme::setCurrentTime( double val )
{
    UTILS_ASSERT(kinematicModel);
    if (currentTime != val) {
        currentTime = val;
        if (val < 0.0 || val > 1.0) {
            LOGGER(Logger::Debug, "SkeletalVisualizationScheme : value out of <0,1>");
        }

        updateJointTransforms(val);

        schemeDrawer->update(val);
    }
}

osg::Quat rotationParentChild(JointPtr parent, JointPtr child)
{
    double mul  = osg::DegreesToRadians(1.0);
    osg::Matrix matParZ; matParZ.makeRotate(-parent->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matParY; matParY.makeRotate(-parent->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matParX; matParX.makeRotate(-parent->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matPar = matParZ * matParY * matParX;
    //osg::Quat rz; rz.makeRotate(-parent->axis[2] * mul, 0.0, 0.0, 1.0);      
   // osg::Quat ry; ry.makeRotate(-parent->axis[1] * mul, 0.0, 1.0, 0.0);  
   // osg::Quat rx; rx.makeRotate(-parent->axis[0] * mul, 1.0, 0.0, 0.0);

    //osg::Quat par = rz * ry * rx;
    //osg::Quat par = rz;


    osg::Matrix matChiZ; matChiZ.makeRotate(child->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matChiY; matChiY.makeRotate(child->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matChiX; matChiX.makeRotate(child->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matChi = matChiX * matChiY * matChiZ;
    //rz.makeRotate(child->axis[2] * mul, 0.0, 0.0, 1.0);
   // ry.makeRotate(child->axis[1] * mul, 0.0, 1.0, 0.0);
   // rx.makeRotate(child->axis[0] * mul, 1.0, 0.0, 0.0);
    osg::Matrix resM = matChi * matPar ;
    //resM.invert_4x4(resM);
   // osg::Quat chil = rx * ry * rz;
    //osg::Quat chil = rz;
    //osg::Quat res = par * chil;
    //res = res.inverse();
    Quat res; res.set(resM);
    return res;
}

osg::Vec3 vectorRotation(Vec3 v, double a, double b , double c)
{
    Quat rz; rz.makeRotate(c, 0,0,1);
    Quat ry; ry.makeRotate(b, 0,1,0);
    Quat rx; rx.makeRotate(a, 1,0,0);

    //rotationZ(Rz, c);
    //rotationY(Ry, b);
    //rotationX(Rx, a);
    
    //Matrix vector multiplication to generate the output vector v.
    /*matrix_transform_affine(Rz, v[0], v[1], v[2], v);
    matrix_transform_affine(Ry, v[0], v[1], v[2], v);
    matrix_transform_affine(Rx, v[0], v[1], v[2], v);*/

    v = rz * v;
    v = ry * v;
    v = rx * v;
    return v;
}
void SkeletalVisualizationScheme::updateJointTransforms(const std::vector<osg::Quat>& rotations, hAnimJointPtr joint, Quat parentRot, Vec3 parentPos)
{
	//rewizja - chyba i tak nie bedzie potrzebny ten viewer
	//// zapewnienie zgodnosci indeksow (miedzy tablicami connections i jointStates)
	//int i = visJoints[joint];
	//Quat ident;
	//double mul = osg::DegreesToRadians(1.0);
	//Vec3 shift = joint->getLocalShift();

	//Quat pc = joint->getChildParentRotation();
	//int index = kinematicModel->getIndex(joint->name);
	////auto it = rotations.find(joint->name);
	//Quat rotation = rotations[index] * pc  * parentRot;

	//shift = rotation * shift;

	//jointMarkersStates[i].position = (parentPos + shift);

	//BOOST_FOREACH(JointPtr child, joint->children) {
	//	counterHelper++;
	//	updateJointTransforms(rotations, boost::dynamic_pointer_cast<hAnimJoint>(child), rotation , parentPos + shift);
	//}
}

//void SkeletalVisualizationScheme::updateJointTransforms(map<string, Quat>& rotations, hAnimJointPtr joint, Quat parentRot, Vec3 parentPos)
//{
//    // zapewnienie zgodnosci indeksow (miedzy tablicami connections i jointStates)
//    int i = visJoints[joint];
//    Quat ident;
//    double mul = osg::DegreesToRadians(1.0);
//    /*Vec3 shift = joint->direction * joint->length;
//    shift = vectorRotation(shift, -joint->axis[0] * mul, -joint->axis[1] * mul, -joint->axis[2] * mul);
//    */
//    Vec3 shift = joint->getLocalShift();
//       
//    /*Quat pc;
//    JointPtr parent = joint->parent.lock();
//    if (parent) {
//       pc = rotationParentChild(parent, joint);
//    }*/
//    Quat pc = joint->getChildParentRotation();
//    Quat rotation = rotations[joint->name]* pc  * parentRot ;
//
//    //Quat rotation =  parentRot * pc;
//    shift = rotation * shift;
//    //shift = rotation  * shift;
//    //n = shift / shift.length();
//    //printN(joint->name, n[0], n[1], n[2]); 
//
//    jointMarkersStates[i].position = (parentPos + shift);
//    //jointStates[i].from = parentPos;
//    //jointStates[i].to = parentPos + shift;
//    BOOST_FOREACH(JointPtr child, joint->children) {
//        counterHelper++;
//        updateJointTransforms(rotations, boost::dynamic_pointer_cast<hAnimJoint>(child), rotation , parentPos + shift);
//    }
//}

void SkeletalVisualizationScheme::updateJointTransforms( double time )
{
    UTILS_ASSERT(kinematicModel);
    counterHelper = 0;
    auto frame = kinematicModel->getValues(time);
    hAnimSkeletonPtr skeleton = kinematicModel->getHAnimSkeleton();
    osg::Quat q; Vec3 pos;
    pos = kinematicModel->getRootPosition(time);
    updateJointTransforms(frame, skeleton->getRoot(), q, pos);
}

void SkeletalVisualizationScheme::updateWorldTransforms( osg::Vec3 worldPosition, osg::Quat worldRotation )
{
    throw kinematic::NotYetImplemented("");
}

void SkeletalVisualizationScheme::updateSkinnedTransforms()
{
    throw kinematic::NotYetImplemented("");
}

void SkeletalVisualizationScheme::setSchemeDrawer( ISchemeDrawerPtr drawer )
{
   if (this->schemeDrawer) {
       this->schemeDrawer->deinit();
   }

   this->schemeDrawer = drawer;
   this->schemeDrawer->init(weak);
}

boost::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationScheme::create()
{
    boost::shared_ptr<SkeletalVisualizationScheme> scheme(new SkeletalVisualizationScheme());
    scheme->weak = scheme;
    return scheme;
}

void SkeletalVisualizationScheme::setKinematicModel( kinematic::JointAnglesCollectionPtr val )
{
    kinematicModel = val;
    auto jointMap = kinematicModel->getJoints();
    int count = jointMap.size();
    jointMarkersStates.reserve(count + 5);
    jointMarkersStates.resize(count);
    auto it = jointMap.begin();
    for (int index = 0; it != jointMap.end(); it++) {
        if (it->second->isActive()) {
            
            visJoints[it->second] = index++;
        }
    }
    hAnimSkeletonPtr skeleton = kinematicModel->getHAnimSkeleton();
	//rewizja
    //createSkeletonConnections(skeleton->getRoot());
}

//rewizja
//void SkeletalVisualizationScheme::createSkeletonConnections(JointPtr joint)
//{
//    BOOST_FOREACH(JointPtr child, joint->children) {
//        Connection c;
//        c.index1 = visJoints[joint];
//        c.index2 = visJoints[child];
//        jointConnections.push_back(c);
//        createSkeletonConnections(child);
//    }
//}

//void PointSchemeDrawer::init( SkeletalVisualizationSchemeWeak scheme )
//{
//    UTILS_ASSERT(scheme.lock());
//    OsgSchemeDrawer::init(scheme);
//
//    node = new osg::Group;
//    auto markers = scheme.lock()->getJointMarkersStates();
//    createMarkersCrowd(markers);
//}
//
//void PointSchemeDrawer::deinit()
//{
//    // TODO -> unparent
//}
//
//void PointSchemeDrawer::update( double time )
//{
//    auto markers = getVisualiztionScheme().lock()->getJointMarkersStates();
//    for (int i = markers.size() - 1; i >= 0; --i) {
//        points[i]->setPosition(markers[i].position);
//    }
//}
//
//void PointSchemeDrawer::draw()
//{
//
//}
//
//void PointSchemeDrawer::createMarkersCrowd(const std::vector<MarkerState>& markers)
//{
//    int count = markers.size();
//    points.resize(count);
//    for (int i = 0; i < count; ++i) {
//        Vec3 position = markers[i].position;
//        points[i] = addPoint(position);
//        node->addChild(points[i]);
//    }
//}
//
//PointSchemeDrawer::GeodePtr PointSchemeDrawer::createMarker(const osg::Vec4& color, float scale) const
//{
//    PointSchemeDrawer::GeodePtr geode = new osg::Geode();
//    ref_ptr<Geometry> geometry = new osg::Geometry();
//
//    geode->addDrawable(geometry);
//
//    ref_ptr<Vec3Array> pyramidVertices = new osg::Vec3Array;
//
//    pyramidVertices->push_back(osg::Vec3( 0.0f, 0.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 1.0f, 0.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 1.0f, 1.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 0.0f, 1.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 0.5f, 0.5f, 1.0f) * scale);
//
//    geometry->setVertexArray( pyramidVertices );
//
//    osg::DrawElementsUInt* geometryBase =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
//    geometryBase->push_back(3);
//    geometryBase->push_back(2);
//    geometryBase->push_back(1);
//    geometryBase->push_back(0);
//    geometry->addPrimitiveSet(geometryBase);
//
//    osg::DrawElementsUInt* pyramidFaceOne =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceOne->push_back(0);
//    pyramidFaceOne->push_back(1);
//    pyramidFaceOne->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceOne);
//
//    osg::DrawElementsUInt* pyramidFaceTwo =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceTwo->push_back(1);
//    pyramidFaceTwo->push_back(2);
//    pyramidFaceTwo->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceTwo);
//
//    osg::DrawElementsUInt* pyramidFaceThree =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceThree->push_back(2);
//    pyramidFaceThree->push_back(3);
//    pyramidFaceThree->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceThree);
//
//    osg::DrawElementsUInt* pyramidFaceFour =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceFour->push_back(3);
//    pyramidFaceFour->push_back(0);
//    pyramidFaceFour->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceFour);
//
//    osg::Vec4Array* colors = new osg::Vec4Array;
//    colors->push_back(color);
//    colors->push_back(color);
//    colors->push_back(color);
//    colors->push_back(color);
//    colors->push_back(color);
//
//    geometry->setColorArray(colors);
//    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
//
//    return geode;
//}
//
//PointSchemeDrawer::TransformPtr PointSchemeDrawer::addPoint( const osg::Vec3& point )
//{
//    float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
//    Vec4 color(rand, 1, rand, 1);
//    GeodePtr marker = createMarker(color, 1.5f);
//    TransformPtr transform = new PositionAttitudeTransform();
//    transform->addChild(marker);
//    transform->setPosition(point);
//    return transform;
//}

//void LineSchemeDrawer::draw()
//{
//
//}
//
//void LineSchemeDrawer::update( double time )
//{
//    auto joints = getVisualiztionScheme().lock()->getJointsStates();
//    for (int i = joints.size() - 1; i >= 0; --i) {
//        ref_ptr<Vec3Array> vertices = buffers[i];
//        (*vertices)[0] = joints[i].from;
//        (*vertices)[1] = joints[i].to;
//        osg::Drawable* d = lines[i]->getDrawable(0);
//        osg::Geometry* g = dynamic_cast<osg::Geometry*>(d);
//
//        //std::cout << "from : (" << (*vertices)[0][0] << ", " << (*vertices)[0][1] << ", "  << (*vertices)[0][2] << ") ";
//        //std::cout << "to : (" << (*vertices)[1][0] << ", " << (*vertices)[1][1] << ", "  << (*vertices)[1][2] << ") " << endl;
//
//        g->setVertexArray(vertices);
//    }
//}
//
//void LineSchemeDrawer::deinit()
//{
//
//}
//
//void LineSchemeDrawer::init( SkeletalVisualizationSchemeWeak scheme )
//{
//    UTILS_ASSERT(scheme.lock());
//    OsgSchemeDrawer::init(scheme);
//
//    node = new osg::Group;
//    auto bones = scheme.lock()->getJointsStates();
//    
//    BOOST_FOREACH(JointState& state, bones) {
//        addLine(state.from, state.to);
//    }
//}
//
//osg::ref_ptr<osg::Node> LineSchemeDrawer::getNode()
//{
//    return node;
//}
//
//void LineSchemeDrawer::addLine( const osg::Vec3& from, const osg::Vec3& to )
//{
//    GeodePtr geode = new Geode();
//    lines.push_back(geode);
//    osg::ref_ptr<Geometry> linesGeom = new osg::Geometry();
//
//    ref_ptr<Vec3Array> vertices = new osg::Vec3Array(2);
//    buffers.push_back(vertices);
//    (*vertices)[0] = from;
//    (*vertices)[1] = to;
//
//    linesGeom->setVertexArray(vertices);
//
//    ref_ptr<Vec4Array> colors = new osg::Vec4Array;
//    colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
//    linesGeom->setColorArray(colors);
//    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
//
//
//    osg::Vec3Array* normals = new osg::Vec3Array;
//    normals->push_back(osg::Vec3(1.0f, -0.0f, 0.0f));
//    linesGeom->setNormalArray(normals);
//    linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
//
//    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
//
//    geode->addDrawable(linesGeom);
//    node->addChild(geode);
//}
