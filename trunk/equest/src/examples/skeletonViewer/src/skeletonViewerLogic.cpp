#include "PCH.h"
#include <iostream>
#include <iomanip>
#include <boost/shared_ptr.hpp>
#include "uniqueCollection.h"
#include "skeletalVisualizationScheme.h"
#include "skeletonViewerLogic.h"
#include "mainwindow.h"
#include "ISchemeDrawer.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"

using namespace kinematic;
using namespace osg;
using namespace std;
using namespace boost;

SkeletonViewerLogic::SkeletonViewerLogic() :
    mainWindow(nullptr),
    time(0.0),
    loaded(false)
{
}

void SkeletonViewerLogic::loadBVH( const std::string& bvhFileName )
{
    SkeletalModelPtr model(new SkeletalModel);
    BvhParser parser;
    parser.parse(model, bvhFileName);
    setModel(model);
}

void SkeletonViewerLogic::loadAsfAmc( const std::string& asfFile, const std::string& amcFile )
{
    SkeletalModelPtr model(new SkeletalModel);
    AsfParser asf;
    AmcParser amc;
    asf.parse(model, asfFile);
    amc.parse(model, amcFile);
    setModel(model);
}

void SkeletonViewerLogic::createTree( hAnimJointPtr parentJoint, QTreeWidgetItem* parentItem ) {

	int count = parentJoint->GetChildrenBones().size();
	for (int i = 0; i < count; i++) {
		QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
		hAnimBonePtr bone = parentJoint->GetChildrenBones()[i];
		QString name(bone->name.c_str());
		item->setText(0, name);

		for (int j = 0; j < bone->childrenJoints.size(); j++) {
			QTreeWidgetItem* item2 = new QTreeWidgetItem(item);
			hAnimJointPtr child = bone->childrenJoints[j];
			QString name(child->getName().c_str());
			item2->setText(0, name);
			createTree(child, item2);
		}
		
	}
    /*int count = parentJoint->children.size();
    for (int i = 0; i < count; i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
        JointPtr child = parentJoint->children[i];
        QString name(child->name.c_str());
        item->setText(0, name);
        createTree(child, item);
    }*/
}

void SkeletonViewerLogic::createMarkersCrowd( kinematic::JointPtr joint, const Vec3& position)
{
    LOGGER(Logger::Info, joint->name + " adds point at : " +
        lexical_cast<string>(position[0]) + ", " +
        lexical_cast<string>(position[1]) + ", " +
        lexical_cast<string>(position[2]));
    addPoint(position);
    Vec3 dir = joint->direction * joint->length;
    Vec3 position2 = position + dir;
    addLine(position, position2);
    int count = joint->children.size();
    if (count > 0) {
        for (int i = 0; i < count; ++i) {
            createMarkersCrowd(joint->children[i], position2);
        }
    } else {
        addPoint(position2);
        LOGGER(Logger::Info, joint->name + " adds point at : " +
            lexical_cast<string>(position[0]) + ", " +
            lexical_cast<string>(position[1]) + ", " +
            lexical_cast<string>(position[2]));
    }
}

GeodePtr SkeletonViewerLogic::createMarker(const osg::Vec4& color, float scale) const
{
    GeodePtr geode = new osg::Geode();
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

void SkeletonViewerLogic::addPoint( const osg::Vec3& point )
{
    UTILS_ASSERT(mainWindow && mainWindow->getRootNode().valid());
    float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    Vec4 color(rand, 1, rand, 1);
    GeodePtr marker = createMarker(color, 1.5f);

    ref_ptr<MatrixTransform> transform = new MatrixTransform();

    mainWindow->getRootNode()->addChild(transform);
    transform->addChild(marker);

    osg::Matrix mat;
    mat.identity();
    mat.setTrans(point);
    transform->setMatrix(mat);
    mainWindow->getRootNode()->addChild(transform.get());
}

void SkeletonViewerLogic::addLine( const osg::Vec3& from, const osg::Vec3& to )
{
    GeodePtr geode = new Geode();
    osg::ref_ptr<Geometry> linesGeom = new osg::Geometry();

    ref_ptr<Vec3Array> vertices = new osg::Vec3Array(2);
    (*vertices)[0] = from;
    (*vertices)[1] = to;

    linesGeom->setVertexArray(vertices);

    ref_ptr<Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);


    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
    linesGeom->setNormalArray(normals);
    linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

    geode->addDrawable(linesGeom);
    mainWindow->getRootNode()->addChild(geode);
}

void SkeletonViewerLogic::setModel( kinematic::SkeletalModelPtr skeletalModel )
{
    UTILS_ASSERT(mainWindow);
    JointPtr root = skeletalModel->getSkeleton().getRoot();

    JointAnglesCollectionPtr kinematic(new JointAnglesCollection);
    kinematic->setSkeletalData(skeletalModel);

    visualization->setKinematicModel(kinematic);
    visualization->setCurrentTime(0.0);
    ISchemeDrawerPtr drawer(new LineSchemeDrawer);
    visualization->setSchemeDrawer(drawer);

    QString rootName(skeletalModel->getSkeleton().getRootName().c_str());
    mainWindow->getTreeRootItem()->setText(0, rootName);

    createTree(boost::dynamic_pointer_cast<hAnimJoint>(kinematic->getHAnimSkeleton()->getRoot()), mainWindow->getTreeRootItem());
    //createMarkersCrowd(root);

    boost::shared_ptr<OsgSchemeDrawer> osgDrawer = boost::dynamic_pointer_cast<OsgSchemeDrawer>(drawer);
    if (osgDrawer) {
        mainWindow->getRootNode()->addChild(osgDrawer->getNode());
    }
    loaded = true;
    setTime(0);
    update();
}

void SkeletonViewerLogic::setTime( double time )
{
    this->time = time;
    this->visualization->setCurrentTime(time);
}

void SkeletonViewerLogic::start()
{
    if (loaded) {
        QTimer* timer = mainWindow->getTimer();
        // todo
        timer->start(10);
    }
}

void SkeletonViewerLogic::update(bool updateSlider)
{
    if (!loaded) {
        return;
    }
    double duration = visualization->getDuration();
    double time = duration * getTime() + visualization->getFrameTime();
    if (time >= duration) {
        setTime(1.0);
    } else {
        setTime(time / duration);
    }
    
    std::stringstream ss;
    ss << setiosflags(ios::fixed);
    ss.precision(2);
    ss <<  "Time : " << time << "/" << duration;
    std::string timeLabel;
    std::getline(ss, timeLabel);
    mainWindow->getTimeLabel()->setText(timeLabel.c_str());

    int numFrames = visualization->getNumFrames();
    int frame = static_cast<int>((time / duration) * numFrames);
    ss.clear();
    ss << "Frame : " << frame << "/" << numFrames;
    string frameLabel;
    getline(ss, frameLabel);
    mainWindow->getFrameLabel()->setText(frameLabel.c_str());

    if (updateSlider) {
        int pos = static_cast<int>((time / duration) * 99) ;
        mainWindow->getSlider()->setSliderPosition(pos);
        mainWindow->getSlider()->setValue(pos);
    }
}

void SkeletonViewerLogic::stop()
{
    if (loaded) {
        setTime(0.0);
        QTimer* timer = mainWindow->getTimer();
        mainWindow->getSlider()->setSliderPosition(0);
        timer->stop();
    }
}

void SkeletonViewerLogic::pause()
{
    if (loaded) {
        QTimer* timer = mainWindow->getTimer();
        timer->stop();
    }
}

void SkeletonViewerLogic::setMainWindow( MainWindow* val )
{
    UTILS_ASSERT(visualization);
    mainWindow = val;
}
