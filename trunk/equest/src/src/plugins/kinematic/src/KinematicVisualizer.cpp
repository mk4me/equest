#include "PCH.h"
#include "KinematicVisualizer.h"
#include "GRFSerie.h"
#include "SkeletonSerie.h"
#include "SkeletonStreamSerie.h"
#include "SkeletonStateStreamSerie.h"
#include "MarkerSerie.h"
#include "PickHandler.h"
#include "Manipulators.h"
#include <coreui/CoreAction.h>
#include <coreui/CoreWidgetAction.h>
#include <coreui/CoreLabeledWidget.h>
#include <kinematiclib/Skeleton.h>
#include <plugins/kinematic/Wrappers.h>
#include <QtWidgets/QToolButton>
#include <kinematicUtils/RotationConverter.h>
#include <osgGA/StateSetManipulator>
#include "OsgSerie.h"
#include "BvhSerie.h"

using namespace coreUI;

KinematicVisualizer::KinematicVisualizer() :
	actionGhost(nullptr), actionSwitchAxes(nullptr), lastTime(-1.0f),
	currentSerie(-1), trajectoriesDialog(nullptr), schemeDialog(nullptr),
	innerUpdate(false)
{
	
}

plugin::IVisualizer* KinematicVisualizer::create() const
{
	auto kv = new KinematicVisualizer();
	kv->setAxisXYZ(getAxisXYZ());
	return kv;
}

void KinematicVisualizer::getSupportedTypes(utils::TypeInfoList & supportedTypes) const
{
	supportedTypes.push_back(typeid(c3dlib::MarkerCollection));
	supportedTypes.push_back(typeid(c3dlib::GRFCollection));
	supportedTypes.push_back(typeid(SkeletonDataStream));
	supportedTypes.push_back(typeid(osg::PositionAttitudeTransform));
	supportedTypes.push_back(typeid(SkeletonWithStates));
	supportedTypes.push_back(typeid(SkeletonWithStreamData));
}

int KinematicVisualizer::getMaxDataSeries() const
{
    return -1;
}

plugin::IVisualizer::ISerie *KinematicVisualizer::createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data)
{
	KinematicSerieBase * ret = nullptr;
	if (requestedType == typeid(c3dlib::GRFCollection)) {
		ret = new GRFSerie(this, requestedType, data);		
	}
	else if (requestedType == typeid(c3dlib::MarkerCollection)) {
		auto ms = new MarkerSerie(this, requestedType, data);		
        trajectoriesDialog->setDrawer(ms->getTrajectoriesManager(), getRootName(data, tr("Markers")), getMarkersNames(data->get()));
        schemeDialog->setDrawer(ms->getPointsDrawer(), getRootName(data, tr("Markers")), getMarkersNames(data->get()), ms->getConnectionsDrawer());
        //schemeDialog->setDrawer(ms->getConnectionsDrawer(), getRootName(data, tr("Markers")));
        ret = ms;
	}
	else if (requestedType == typeid(SkeletonWithStates)) {
		auto ss = new SkeletonSerie(this, requestedType, data);
		QStringList allNames;
		QStringList names;

		NonDummyJointFilter ndjf;

		kinematic::LinearizedSkeleton::Visitor::visit(*(ss->getSkeleton()), [&allNames, &names, &ndjf](kinematic::Skeleton::JointConstPtr joint)
		{
			allNames.push_back(QString::fromStdString(joint->value().name()));
			if (joint->isRoot() == true || ndjf(joint) == true){
				names.push_back(QString::fromStdString(joint->value().name()));
			}
		});
		
		trajectoriesDialog->setDrawer(ss->getTrajectoriesManager(), getRootName(data, tr("Skeleton")), allNames); //getSkeletonNames(data->get()));
		//schemeDialog->setDrawer(ss->getConnectionsDrawer().first, getRootName(data, tr("Connections")), names, ss->getConnectionsDrawer());
		schemeDialog->setDrawer(ss->getPointsDrawer(), getRootName(data, tr("Joints")), names, ss->getConnectionsDrawer());

		ret = ss;
	}
	else if (requestedType == typeid(SkeletonWithStreamData)) {
			auto ss = new SkeletonStateStreamSerie(this, requestedType, data);

			//QStringList allNames;
			QStringList names;

			NonDummyJointFilter ndjf;

			kinematic::LinearizedSkeleton::Visitor::visit(*(ss->getSkeleton()), [&](kinematic::Skeleton::JointConstPtr joint)
			{
				//allNames.push_back(QString::fromStdString(joint->value().name()));
				if (joint->isRoot() == true || ndjf(joint) == true){
					names.push_back(QString::fromStdString(joint->value().name()));
				}
			});

			//trajectoriesDialog->setDrawer(ss->getTrajectoriesManager(), getRootName(data, tr("Skeleton")), allNames); //getSkeletonNames(data->get()));
			schemeDialog->setDrawer(ss->getPointsDrawer(), getRootName(data, tr("Joints")), names, ss->getConnectionsDrawer(), ss->pointsAxisDrawer());

			ret = ss;
	}
	else if (requestedType == typeid(SkeletonDataStream)) {
		ret = new SkeletonStreamSerie(this, requestedType, data);
	} /*else if (requestedType == typeid (biovision::BVHData)) {
		ret = new BvhSerie(this, requestedType, data);
	} */else if (requestedType == typeid (osg::PositionAttitudeTransform)) {
		ret = new OsgSerie(this, requestedType, data);
	}else {
		UTILS_ASSERT(false);
	}

    if (ret) {
        transformNode->addChild(ret->getMatrixTransformNode());
        series.push_back(ret);
    }

    refreshSpinboxes();
	//widget->requestRedraw();
    return dynamic_cast<plugin::IVisualizer::ISerie*>(ret);
}

plugin::IVisualizer::ISerie * KinematicVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie)
{
    return nullptr;
}

plugin::IVisualizer::ISerie * KinematicVisualizer::createSerie(const plugin::IVisualizer::ISerie *,const utils::TypeInfo &,const core::VariantConstPtr &)
{
	return nullptr;
}

void KinematicVisualizer::removeSerie(plugin::IVisualizer::ISerie *serie)
{
    KinematicSerieBase* kSerie = dynamic_cast<KinematicSerieBase*>(serie);

    auto it = std::find(series.begin(), series.end(), kSerie);

    transformNode->removeChild((*it)->getMatrixTransformNode());
    series.erase(it);
	//widget->requestRedraw();
}

void KinematicVisualizer::requestUpdate()
{
	innerUpdate = true;
}

bool KinematicVisualizer::innerUpdateRequired()
{
	return innerUpdate.exchange(false);
}

QWidget* KinematicVisualizer::createWidget()
{
	osg::DisplaySettings::instance()->setNumMultiSamples( 8 );

    widget = new osgui::QOsgDefaultWidget();
	widget->setKeyEventSetsDone(0);

    trajectoriesDialog = new TrajectoriesDialog(widget, this);
    schemeDialog = new SchemeDialog(widget, this);    

    widget->addEventHandler(new osgGA::StateSetManipulator(
        widget->getCamera()->getOrCreateStateSet()
        ));

    cameraManipulator = new osgGA::OrbitManipulator();
    widget->setCameraManipulator(cameraManipulator);
	setLeft();
    // stworzenie i dodanie światła przyczepionego do kamery
    osg::ref_ptr<osg::Light> light = new osg::Light;
    light->setLightNum(1);
    light->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
    light->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,0.0f));
    light->setConstantAttenuation(1.0f);
	light->setDataVariance(osg::Object::STATIC);
    widget->setLight(light);
    // tworzenie kamery
    widget->getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.1f, 1));


	actionTrajectories = new CoreAction(tr("Properties"), QIcon(QString::fromUtf8(":/kinematic/icons/tracea.png")), tr("Trajectories"), widget, CoreTitleBar::Left);
    actionTrajectories->setCheckable(true);
    connect(actionTrajectories, SIGNAL(triggered()), this, SLOT(showTrajectoriesDialog()));
	widget->addAction(actionTrajectories);


    actionScheme = new CoreAction(tr("Properties"), QIcon(QString::fromUtf8(":/kinematic/icons/trace-blue.png")), tr("Scheme Drawers"), widget, CoreTitleBar::Left);
    actionScheme->setCheckable(true);
    connect(actionScheme, SIGNAL(triggered()), this, SLOT(showSchemeDialog()));
	widget->addAction(actionScheme);
	

    actionGhost = new CoreAction(tr("Properties"), QIcon(QString::fromUtf8(":/kinematic/icons/skeletal_tracea.png")), tr("Ghost"), widget, CoreTitleBar::Left);
    actionGhost->setCheckable(true);
    actionGhost->setVisible(true);
	connect(actionGhost, SIGNAL(triggered(bool)), this, SLOT(showGhost(bool)));
	widget->addAction(actionGhost);
	
	QToolButton * viewMenuButton = new QToolButton;
	viewMenuButton->setIcon(QIcon(QString::fromUtf8(":/kinematic/icons/viewa.png")));
	viewMenuButton->setText(tr("View"));
	viewMenuButton->setPopupMode(QToolButton::MenuButtonPopup);	
	QMenu* viewMenu = new QMenu(tr("View"), viewMenuButton);    
	QAction* lft_action = viewMenu->addAction(tr("Left")); 
	QAction* rht_action = viewMenu->addAction(tr("Right")); 
	QAction* frn_action = viewMenu->addAction(tr("Front")); 
	QAction* bck_action = viewMenu->addAction(tr("Back")); 
	QAction* top_action = viewMenu->addAction(tr("Top")); 
	QAction* btm_action = viewMenu->addAction(tr("Bottom"));
	
	connect(lft_action, SIGNAL(triggered()), this, SLOT(setLeft()));
	connect(rht_action, SIGNAL(triggered()), this, SLOT(setRight())); 
	connect(frn_action, SIGNAL(triggered()), this, SLOT(setFront())); 
	connect(bck_action, SIGNAL(triggered()), this, SLOT(setBehind()));
	connect(top_action, SIGNAL(triggered()), this, SLOT(setTop()));
	connect(btm_action, SIGNAL(triggered()), this, SLOT(setBottom())); 

	viewMenuButton->setMenu(viewMenu);

	CoreWidgetAction * viewAction = new CoreWidgetAction(widget, tr("View"), CoreTitleBar::Right);
	viewAction->setDefaultWidget(viewMenuButton);
	widget->addAction(viewAction);


    translateSpinWidgetX = createSpinWidget(tr("TX: "), 0.3);
    translateSpinWidgetY = createSpinWidget(tr("TY: "), 0.3);
    translateSpinWidgetZ = createSpinWidget(tr("TZ: "), 0.3);

    connect(translateSpinWidgetX.second, SIGNAL(valueChanged(double)), this, SLOT(shiftX(double)));
    connect(translateSpinWidgetY.second, SIGNAL(valueChanged(double)), this, SLOT(shiftY(double)));
    connect(translateSpinWidgetZ.second, SIGNAL(valueChanged(double)), this, SLOT(shiftZ(double)));

	widget->addAction(createWidgetAction(translateSpinWidgetX.first, widget, tr("Manipulators properties"), CoreTitleBar::Left));
	widget->addAction(createWidgetAction(translateSpinWidgetY.first, widget, tr("Manipulators properties"), CoreTitleBar::Left));
	widget->addAction(createWidgetAction(translateSpinWidgetZ.first, widget, tr("Manipulators properties"), CoreTitleBar::Left));


    rotateSpinWidgetX = createSpinWidget(tr("RX: "), 5.0);
    rotateSpinWidgetY = createSpinWidget(tr("RY: "), 5.0);
    rotateSpinWidgetZ = createSpinWidget(tr("RZ: "), 5.0);
    
    connect(rotateSpinWidgetX.second, SIGNAL(valueChanged(double)), this, SLOT(rotateX(double)));
    connect(rotateSpinWidgetY.second, SIGNAL(valueChanged(double)), this, SLOT(rotateY(double)));
    connect(rotateSpinWidgetZ.second, SIGNAL(valueChanged(double)), this, SLOT(rotateZ(double)));

	widget->addAction(createWidgetAction(rotateSpinWidgetX.first, widget, tr("Manipulators properties"), CoreTitleBar::Right));
	widget->addAction(createWidgetAction(rotateSpinWidgetY.first, widget, tr("Manipulators properties"), CoreTitleBar::Right));
	widget->addAction(createWidgetAction(rotateSpinWidgetZ.first, widget, tr("Manipulators properties"), CoreTitleBar::Right));

    scaleSpinWidgetX = createSpinWidget("SX: ", 0.1);
    scaleSpinWidgetY = createSpinWidget("SY: ", 0.1);
    scaleSpinWidgetZ = createSpinWidget("SZ: ", 0.1);

    connect(scaleSpinWidgetX.second, SIGNAL(valueChanged(double)), this, SLOT(scaleX(double)));
    connect(scaleSpinWidgetY.second, SIGNAL(valueChanged(double)), this, SLOT(scaleY(double)));
    connect(scaleSpinWidgetZ.second, SIGNAL(valueChanged(double)), this, SLOT(scaleZ(double)));

	widget->addAction(createWidgetAction(scaleSpinWidgetX.first, widget, tr("Manipulators properties"), CoreTitleBar::Left));
	widget->addAction(createWidgetAction(scaleSpinWidgetY.first, widget, tr("Manipulators properties"), CoreTitleBar::Left));
	widget->addAction(createWidgetAction(scaleSpinWidgetZ.first, widget, tr("Manipulators properties"), CoreTitleBar::Left));

    
    pickerAction = new CoreAction(tr("Manipulators"), QIcon(":/kinematic/icons/picker.png"), tr("Picker"), widget, CoreTitleBar::Left);
	connect(pickerAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(pickerAction);


	translateAction = new CoreAction(tr("Manipulators"), QIcon(":/kinematic/icons/manipulator-translate.png"), tr("Translate"), widget, CoreTitleBar::Left);
	connect(translateAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(translateAction);


	rotateAction = new CoreAction(tr("Manipulators"), QIcon(":/kinematic/icons/manipulator-rotate.png"), tr("Rotate"), widget, CoreTitleBar::Left);
	connect(rotateAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(rotateAction);


	scaleAction = new CoreAction(tr("Manipulators"), QIcon(":/kinematic/icons/manipulator-scale.png"), tr("Scale"), widget, CoreTitleBar::Left);    
    connect(scaleAction, SIGNAL(triggered()), this, SLOT(draggerTriggered()));
	widget->addAction(scaleAction);
	
	CoreAction * resetTransformAction = new CoreAction(tr("Manipulators"), QIcon(":/kinematic/icons/left-b.png"), tr("Reset transform"), widget, CoreTitleBar::Left);    
    connect(resetTransformAction, SIGNAL(triggered()), this, SLOT(resetTransform()));
	widget->addAction(resetTransformAction);

	

	//! akcja wyświetla orientacje jointów
	coreUI::CoreAction* showJointsOrientationsAction = new CoreAction(tr("Manipulators"), QIcon(":/kinematic/icons/manipulator-translate.png"), tr("Points orientations"), widget, CoreTitleBar::Left);
	connect(showJointsOrientationsAction, SIGNAL(triggered()), this, SLOT(showJointsOrientations()));
	widget->addAction(showJointsOrientationsAction);

	coreUI::CoreAction* showBodyPlanesAction = new CoreAction(tr("Manipulators"), QIcon(":/kinematic/icons/manipulator-translate.png"), tr("Body planes"), widget, CoreTitleBar::Left);
	connect(showBodyPlanesAction, SIGNAL(triggered()), this, SLOT(showBodyPlanes()));
	widget->addAction(showBodyPlanesAction);

    osg::Vec3 pos (0.0f, 9.0f, 3.0f);
    osg::Vec3 up(0,0,1);

    osg::Vec3 at;
    widget->getCamera()->setViewMatrixAsLookAt(pos, at, up);
    rootNode = new osg::Group;
    rootNode->addChild(createFloor());
    widget->setSceneData(rootNode);
    widget->setMinimumSize(50, 50);

    indicatorNode = createIndicator();

//#ifdef _DEBUG

	//dodajemy tez event handler ze statystykami
	widget->addEventHandler( new osgViewer::StatsHandler() );
	//widget->setTimerActive(true);

//#endif

	widget->setFocusPolicy(Qt::StrongFocus);

    auto info = [&](const PickHandler::PickerList& list) {
        auto it = list.begin();
        if (it != list.end()) {
            GeodePtr geode = it->get<0>();
            KinematicSerieBase* s = getParentSerie(geode);
            if (s) {
                setActiveSerie(dynamic_cast<plugin::IVisualizer::ISerie*>(s));
            }
        }
    };
    widget->addEventHandler(new PickHandler(info));

    currentDragger = nullptr;
    translateDragger = Manipulators::creatreDraggerContainer(Manipulators::TranslateAxis);
    rotationDragger = Manipulators::creatreDraggerContainer(Manipulators::Trackball);
    scaleDragger = Manipulators::creatreDraggerContainer(Manipulators::TabBox);

    translateDragger->getDragger()->addDraggerCallback(new KinematicDraggerCallback(this));
    rotationDragger->getDragger()->addDraggerCallback(new KinematicDraggerCallback(this));
    scaleDragger->getDragger()->addDraggerCallback(new KinematicDraggerCallback(this));


	resetScene();
	//widget->requestRedraw();
	widget->show();
    return widget;
}

coreUI::CoreWidgetAction * KinematicVisualizer::createWidgetAction(QWidget * widget, QObject * parent, const QString & sectionName, coreUI::CoreTitleBar::SideType side)
{
	coreUI::CoreWidgetAction * action = new coreUI::CoreWidgetAction(parent, sectionName, side);
	action->setDefaultWidget(widget);
	return action;
}

void KinematicVisualizer::update( double deltaTime )
{
	if (deltaTime > 0.0){
		auto s = tryGetCurrentISerie();

		if (s != nullptr) {
			auto serie = dynamic_cast<KinematicSerieBase*>(s);
			if (currentDragger && serie) {

				//auto bs = serie->getMatrixTransformNode()->getBound();

				//bs.center().z() + bs.radius() * 1.05

				currentDragger->setDraggerPivot(osg::Vec3(
					translateSpinWidgetX.second->value(),
					translateSpinWidgetY.second->value(),
					translateSpinWidgetZ.second->value()));
			}

			updateIndicator();

			refreshSpinboxes();
			widget->requestRedraw();
		}
	}
	else{
		widget->requestRedraw();
	}
}

osg::Node* KinematicVisualizer::createFloor()
{	
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );	
	stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
	stateset->setMode( GL_LINE_SMOOTH, osg::StateAttribute::ON );
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	stateset->setDataVariance(osg::Object::STATIC);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.05f, 0.05f, 0.3f, 1.0f));
	colors->setDataVariance(osg::Object::STATIC);

    osg::Geode* geode = new osg::Geode();
    // todo sparametryzować
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();
	//linesGeom->setUseDisplayList(false);
	//linesGeom->setUseVertexBufferObjects(true);

	float length = 3.5f;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(44);
    osg::Vec3 v;
    for (int i = 0; i < 11; ++i) {
        v.set(-length + 2 * (i / 10.0f) * length, -length, 0);
        (*vertices)[2 * i] = v;      
        v.set(-length + 2 * (i / 10.0f) * length, length, 0);
        (*vertices)[2 * i + 1] = v;   
        v.set(-length, -length + 2 * (i / 10.0f) * length, 0);
        (*vertices)[22 + 2 * i] = v;   
        v.set(length, -length + 2 * (i / 10.0f) * length, 0);
        (*vertices)[22 + 2 * i + 1] = v;
    }

    linesGeom->setVertexArray(vertices);
    linesGeom->setDataVariance(osg::Object::STATIC);
    geode->setStateSet(stateset);
	geode->setDataVariance(osg::Object::STATIC);
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 44));

    geode->addDrawable(linesGeom);	
	
	stateset->setDataVariance(osg::Object::STATIC);
	geode->setDataVariance(osg::Object::STATIC);
	linesGeom->setDataVariance(osg::Object::STATIC);
	vertices->setDataVariance(osg::Object::STATIC);	
    return geode;
}

QIcon* KinematicVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/kinematic/icons/3D.png"));
}


void KinematicVisualizer::resetScene()
{
    if (rootNode) {
		rootNode->removeChildren(0, rootNode->getNumChildren());
		transformNode = new osg::PositionAttitudeTransform();
		rootNode->addChild(createFloor());
		rootNode->addChild(transformNode);
    }
}

void KinematicVisualizer::setRight()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(-90.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
	requestUpdate();
}

void KinematicVisualizer::setLeft()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(90.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
	requestUpdate();
}

void KinematicVisualizer::setFront()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
	requestUpdate();
}

void KinematicVisualizer::setBehind()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(180.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
	requestUpdate();
}

void KinematicVisualizer::setBottom()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(osg::DegreesToRadians(-90.0));
	this->cameraManipulator->setDistance(8);
	requestUpdate();
}

void KinematicVisualizer::setTop()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(osg::DegreesToRadians(90.0));
	this->cameraManipulator->setDistance(8);
	requestUpdate();
}

void KinematicVisualizer::showTrajectoriesDialog()
{
    trajectoriesDialog->show();
}

void KinematicVisualizer::setActiveSerie( int idx )
{
	if (currentSerie >= 0) {
		series[currentSerie]->getMatrixTransformNode()->removeChild(indicatorNode);
		if (currentDragger) {
			Manipulators::disconnect(transformNode, series[currentSerie]->getMatrixTransformNode(), currentDragger);
		}
	}

    if (idx >= 0 && idx < static_cast<int>(series.size())) {
        currentSerie = idx;
        refreshSpinboxes();
        series[currentSerie]->getMatrixTransformNode()->addChild(indicatorNode);
    }
}

void KinematicVisualizer::updateIndicator()
{
	auto pos = series[currentSerie]->pivotPoint();
	pos.z() = indicatorNode->getPosition().z();
	indicatorNode->setPosition(pos);

	auto s = series[currentSerie]->getMatrixTransformNode()->getNumChildren();

	if(s > 1)
	{	
		auto bs = series[currentSerie]->getMatrixTransformNode()->getChild(0)->getBound();

		for(unsigned int i = 1; i < s - 2; ++i){
			bs.expandBy(series[currentSerie]->getMatrixTransformNode()->getChild(i)->getBound());
		}

		auto pos = series[currentSerie]->pivotPoint();
		pos.z() += bs.radius() * 2;

		indicatorNode->setPosition(pos);
	}else{
		indicatorNode->setPosition(osg::Vec3(0.0, 0.0, 0.0));
	}
}

void KinematicVisualizer::setActiveSerie( plugin::IVisualizer::ISerie *serie )
{
	auto ks = dynamic_cast<KinematicSerieBase*>(serie);
	auto it = std::find(series.begin(), series.end(), ks);
    int idx = it != series.end() ? std::distance(series.begin(), it) : -1;
	setActiveSerie(idx);
}

const plugin::IVisualizer::ISerie * KinematicVisualizer::getActiveSerie() const
{
	if(currentSerie == -1){
		return nullptr;
	}

	return dynamic_cast<const plugin::IVisualizer::ISerie *>(series[currentSerie]);
}

plugin::IVisualizer::ISerie * KinematicVisualizer::getActiveSerie()
{
	if(currentSerie == -1){
		return nullptr;
	}

	return dynamic_cast<plugin::IVisualizer::ISerie *>(series[currentSerie]);
}

KinematicSerieBase* KinematicVisualizer::tryGetCurrentSerie()
{
    int count = static_cast<int>(series.size());
    if (currentSerie >= 0 && currentSerie < count) {
        return series[currentSerie];
    }
    return nullptr;
}

plugin::IVisualizer::ISerie* KinematicVisualizer::tryGetCurrentISerie()
{
	int count = static_cast<int>(series.size());
	if (currentSerie >= 0 && currentSerie < count) {
		return dynamic_cast<plugin::IVisualizer::ISerie*>(series[currentSerie]);
	}

	return nullptr;
}

osg::ref_ptr<osg::PositionAttitudeTransform> KinematicVisualizer::createIndicator() const
{
    using namespace osg;
    GeodePtr boxGeode = new osg::Geode();
    GeodePtr coneGeode = new osg::Geode();

    osg::ref_ptr<osg::Box> unitBox = new osg::Box(Vec3(0, 0, 0), 0.5f, 0.5f, 0.9f);
    osg::ref_ptr<osg::Cone> unitCone = new osg::Cone(Vec3(0, 0, 0), 1.0f, 0.1f);
    osg::ref_ptr<osg::ShapeDrawable> boxShape = new osg::ShapeDrawable(unitBox);
    osg::ref_ptr<osg::ShapeDrawable> coneShape = new osg::ShapeDrawable(unitCone);

    boxGeode->addDrawable(boxShape);
    coneGeode->addDrawable(coneShape);

    TransformPtr transformBox = new osg::PositionAttitudeTransform();
    TransformPtr transformCone = new osg::PositionAttitudeTransform();
    TransformPtr transform = new osg::PositionAttitudeTransform();

    transformBox->addChild(boxGeode);
    transformBox->setName("BOX");
    transformBox->setPosition(Vec3(0, 0, 0.45f));
    transformCone->addChild(coneGeode);
    transformCone->setName("CONE");
    transformCone->setPosition(Vec3(0, 0, 0.45f));

    transform->setScale(Vec3(0.03f, 0.03f, 0.5f));

    transform->addChild(transformBox);
    transformBox->addChild(transformCone);
    osg::Matrix mat; 
    mat.makeRotate(osg::DegreesToRadians(180.0f), 1.0f, 0.0f, 0.0f);
    osg::Quat orientation;
    orientation.set(mat);
    transform->setAttitude(orientation);
    transform->setPosition(osg::Vec3(0.0f, 0.0f, 1.5f));

	boxGeode->setDataVariance(osg::Object::STATIC);
	coneGeode->setDataVariance(osg::Object::STATIC);
	unitBox->setDataVariance(osg::Object::STATIC);
	unitCone->setDataVariance(osg::Object::STATIC);
	boxShape->setDataVariance(osg::Object::STATIC);
	coneShape->setDataVariance(osg::Object::STATIC);
	transformBox->setDataVariance(osg::Object::STATIC);
	transformCone->setDataVariance(osg::Object::STATIC);

    return transform;
}

QPixmap KinematicVisualizer::takeScreenshot() const
{
	return QPixmap::fromImage(widget->grabFrameBuffer(true));
}

void KinematicVisualizer::shiftX( double d )
{
    setTranslation(tryGetCurrentSerie(), 0, d);
}

void KinematicVisualizer::shiftY( double d )
{
    setTranslation(tryGetCurrentSerie(), 1, d);
}

void KinematicVisualizer::shiftZ( double d )
{
    setTranslation(tryGetCurrentSerie(), 2, d);
}

void KinematicVisualizer::showSchemeDialog()
{
    schemeDialog->show();
}

void KinematicVisualizer::refreshSpinboxes()
{
    refreshTranslateSpinboxes();
    refreshRotateSpinboxes();
    refreshScaleSpinboxes();
}

void KinematicVisualizer::draggerTriggered()
{
    auto serie = tryGetCurrentSerie();
    if (serie) {
        QAction* a = qobject_cast<QAction*>(sender());
        if (currentDragger) {
            Manipulators::disconnect(transformNode, serie->getMatrixTransformNode(), currentDragger);
        }

        if (a == translateAction) {
            currentDragger = translateDragger;
            translateSpinWidgetX.first->setVisible(true);
            translateSpinWidgetY.first->setVisible(true);
            translateSpinWidgetZ.first->setVisible(true);
        } else if (a == rotateAction) {
            currentDragger = rotationDragger;
			rotateSpinWidgetX.first->setVisible(true);
			rotateSpinWidgetY.first->setVisible(true);
			rotateSpinWidgetZ.first->setVisible(true);
        } else if (a == scaleAction) {
            currentDragger = scaleDragger;
			scaleSpinWidgetX.first->setVisible(true);
			scaleSpinWidgetY.first->setVisible(true);
			scaleSpinWidgetZ.first->setVisible(true);
        } else if (a == pickerAction) {
            currentDragger = nullptr;
        } else {
            UTILS_ASSERT(false);
        }
    
        if (currentDragger) {
            Manipulators::connect(transformNode, serie->getMatrixTransformNode(), currentDragger, serie->pivotPoint()); 
        }
    }
}

bool isChildRecursive(osg::Group* group, osg::Geode* geode)
{
    int count = group->getNumChildren();
    for (int i = 0; i < count; ++i) {
        osg::Node* child = group->getChild(i);
        if (geode == child) {
            return true;
        }
        osg::Group* childGroup = dynamic_cast<osg::Group*>(child);
        if (childGroup && isChildRecursive(childGroup, geode)) {
            return true;
        }
    }

    return false;
}

KinematicSerieBase* KinematicVisualizer::getParentSerie( GeodePtr geode )
{
    for (auto it = series.begin(); it != series.end(); ++it) {
        osg::MatrixTransform* node = (*it)->getMatrixTransformNode();
        if (isChildRecursive(node, geode)) {
            return *it;
        }
    }

    return nullptr;
}

void KinematicVisualizer::refreshTranslateSpinboxes()
{
    translateSpinWidgetX.second->blockSignals(true);
    translateSpinWidgetY.second->blockSignals(true);
    translateSpinWidgetZ.second->blockSignals(true);

    auto serie = tryGetCurrentSerie();
    if (serie) {
        auto v = serie->pivotPoint() + serie->getMatrixTransformNode()->getMatrix().getTrans();
        translateSpinWidgetX.second->setValue(v.x());
        translateSpinWidgetY.second->setValue(v.y());
        translateSpinWidgetZ.second->setValue(v.z());
    } else {
        translateSpinWidgetX.second->setValue(0.0);
        translateSpinWidgetY.second->setValue(0.0);
        translateSpinWidgetZ.second->setValue(0.0);
    }
    translateSpinWidgetX.second->blockSignals(false);
    translateSpinWidgetY.second->blockSignals(false);
    translateSpinWidgetZ.second->blockSignals(false);
}

KinematicVisualizer::LabeledDoubleSpinBox KinematicVisualizer::createSpinWidget(const QString & name, double step)
{
	QDoubleSpinBox * dblSpinBox = new QDoubleSpinBox;
	dblSpinBox->setMaximum(1000.0);
	dblSpinBox->setMinimum(-1000.0);
	dblSpinBox->setSingleStep(step);

	LabeledDoubleSpinBox spinWidget;
	spinWidget.first = coreUI::CoreLabeledWidget::create(name, dblSpinBox);
	spinWidget.second = dblSpinBox;
    return spinWidget;
}

void KinematicVisualizer::rotateX( double d )
{
    setRotation(tryGetCurrentSerie(), 0, d);
}

void KinematicVisualizer::rotateY( double d )
{
    setRotation(tryGetCurrentSerie(), 1, d);
}

void KinematicVisualizer::rotateZ( double d )
{
    setRotation(tryGetCurrentSerie(), 2, d);
}

void KinematicVisualizer::refreshRotateSpinboxes()
{
    rotateSpinWidgetX.second->blockSignals(true);
    rotateSpinWidgetY.second->blockSignals(true);
    rotateSpinWidgetZ.second->blockSignals(true);

    auto serie = tryGetCurrentSerie();
    if (serie) {
        auto q = serie->getMatrixTransformNode()->getMatrix().getRotate();
        osg::Vec3 v = kinematicUtils::convertXYZ(q);
        rotateSpinWidgetX.second->setValue(osg::RadiansToDegrees(v.x()));
        rotateSpinWidgetY.second->setValue(osg::RadiansToDegrees(v.y()));
        rotateSpinWidgetZ.second->setValue(osg::RadiansToDegrees(v.z()));
    } else {
        rotateSpinWidgetX.second->setValue(0.0);
        rotateSpinWidgetY.second->setValue(0.0);
        rotateSpinWidgetZ.second->setValue(0.0);
    }
    rotateSpinWidgetX.second->blockSignals(false);
    rotateSpinWidgetY.second->blockSignals(false);
    rotateSpinWidgetZ.second->blockSignals(false);
}

void KinematicVisualizer::refreshScaleSpinboxes()
{
    scaleSpinWidgetX.second->blockSignals(true);
    scaleSpinWidgetY.second->blockSignals(true);
    scaleSpinWidgetZ.second->blockSignals(true);

    auto serie = tryGetCurrentSerie();
    if (serie) {
        auto t = serie->getMatrixTransformNode();
        auto s = t->getMatrix().getScale();
        scaleSpinWidgetX.second->setValue(s.x());
        scaleSpinWidgetY.second->setValue(s.y());
        scaleSpinWidgetZ.second->setValue(s.z());
    } else {
        scaleSpinWidgetX.second->setValue(1.0);
        scaleSpinWidgetY.second->setValue(1.0);
        scaleSpinWidgetZ.second->setValue(1.0);
    }
    scaleSpinWidgetX.second->blockSignals(false);
    scaleSpinWidgetY.second->blockSignals(false);
    scaleSpinWidgetZ.second->blockSignals(false);
}

void KinematicVisualizer::setTranslation(KinematicSerieBase* serie, int index, double d )
{
    if (serie) {

		auto m = serie->getMatrixTransformNode()->getMatrix();

		auto t = m.getTrans() + serie->pivotPoint();
		auto oldT = t;
		t[index] = d;
		t -= oldT;

		
        osg::Vec3 pos = m.getTrans() + t;        
		m.setTrans(pos);
		serie->getMatrixTransformNode()->setMatrix(m);
        translateDragger->setDraggerPivot(pos);
    }
}

void KinematicVisualizer::setTranslation( KinematicSerieBase* serie, const osg::Vec3& t )
{
	if (serie) {

		auto m = serie->getMatrixTransformNode()->getMatrix();

		auto tt = t - (serie->pivotPoint() + m.getTrans());

		osg::Vec3 pos = m.getTrans() + tt;        
		m.setTrans(pos);
		
		serie->getMatrixTransformNode()->setMatrix(m);
		translateDragger->setDraggerPivot(pos);
	}
}

void KinematicVisualizer::setRotation( KinematicSerieBase* serie, int index, double d )
{
    if (serie) {

		auto m = serie->getMatrixTransformNode()->getMatrix();
        osg::Quat q = m.getRotate();
        osg::Vec3 euler = kinematicUtils::convertXYZ(q);
        euler[index] = osg::DegreesToRadians(d);
		q = kinematicUtils::convertXYZ(euler);

        m.setRotate(q);
		serie->getMatrixTransformNode()->setMatrix(m);
        rotationDragger->setDraggerPivot(serie->pivotPoint());
    }
}

void KinematicVisualizer::setRotation( KinematicSerieBase* serie, const osg::Vec3& r )
{
	if (serie) {
		auto m = serie->getMatrixTransformNode()->getMatrix();
		auto q = kinematicUtils::convertXYZ(osg::Vec3(osg::DegreesToRadians(r[0]),
							osg::DegreesToRadians(r[1]),
							osg::DegreesToRadians(r[2])));

		m.setRotate(q);
		serie->getMatrixTransformNode()->setMatrix(m);
		rotationDragger->setDraggerPivot(serie->pivotPoint());
	}
}

void KinematicVisualizer::setRotation( KinematicSerieBase* serie, const osg::Quat& q )
{
    if (serie) {
		auto m = serie->getMatrixTransformNode()->getMatrix();
		m.setRotate(q);
		serie->getMatrixTransformNode()->setMatrix(m);
		rotationDragger->setDraggerPivot(serie->pivotPoint());
    }
}


void KinematicVisualizer::setScale( KinematicSerieBase* serie, int index, double d )
{
    if (serie && d != 0.0) {
        KinematicTimeSerie::MatrixTransformPtr transform = serie->getMatrixTransformNode();
        osg::Matrix matrix = transform->getMatrix();
        osg::Vec3 scale(matrix(0,index), matrix(1,index), matrix(2,index));
        scale.normalize();
        scale *= d;
        matrix(0,index) = scale[0];
        matrix(1,index) = scale[1];
        matrix(2,index) = scale[2];
        transform->setMatrix(matrix);
    }
}

void KinematicVisualizer::setScale( KinematicSerieBase* serie, const osg::Vec3& s )
{
    setScale(serie, 0, s[0]);
    setScale(serie, 1, s[1]);
    setScale(serie, 2, s[2]);
}

void KinematicVisualizer::resetTransform()
{
    auto serie = tryGetCurrentSerie();
    if (serie) {
        serie->resetTransform();
		refreshSpinboxes();
    } else {
        setTranslation(serie,osg::Vec3());
        setRotation(serie, osg::Quat());
        setScale(serie, osg::Vec3(1.0f, 1.0f, 1.0f));
    }
}

void KinematicVisualizer::scaleX( double d )
{
    setScale(tryGetCurrentSerie(), 0, d);
}

void KinematicVisualizer::scaleY( double d )
{
    setScale(tryGetCurrentSerie(), 1, d);
}

void KinematicVisualizer::scaleZ( double d )
{
    setScale(tryGetCurrentSerie(), 2, d);
}

QString KinematicVisualizer::getRootName( const core::VariantConstPtr & data, const QString& suffix )
{    
	std::string name;
    if (data->getMetadata("core/name", name) == true) {
        return QString::fromStdString(name) + "_" + suffix;
    }
    return suffix;
}

QStringList KinematicVisualizer::getMarkersNames(const c3dlib::MarkerCollectionConstPtr& ms) const
{
    QStringList names;
    int count = ms->getNumAccessors();
    for (int i = 0; i < count; ++i) {
        names.push_back(QString::fromStdString(ms->getMarkerName(i)));
    }
    return names;
}

//QStringList KinematicVisualizer::getSkeletonNames( const kinematic::JointAnglesCollectionConstPtr& ss ) const
//{
//    QStringList names;
//    UTILS_ASSERT(false);
//    int count = ss->getNumChannels();
//    for (int i = 0; i < count; ++i) {
//        names.push_back(QString::fromStdString(ss->getChannel(i)->getName()));
//    }
//    return names;
//}

void KinematicVisualizer::showGhost(bool val)
{
    IGhostSerie* serie = dynamic_cast<IGhostSerie*>(tryGetCurrentSerie());
    if (serie) {
        serie->setGhostVisible(val);
    }
}

void KinematicVisualizer::showJointsOrientations()
{
	AbstractSkeletonSerie* serie = dynamic_cast<AbstractSkeletonSerie*>(tryGetCurrentSerie());
	if (serie) {
		serie->setJointsOrientationsVisible();
	}
}

void KinematicVisualizer::showBodyPlanes()
{
	AbstractSkeletonSerie* serie = dynamic_cast<AbstractSkeletonSerie*>(tryGetCurrentSerie());
	if (serie) {
		serie->setBodyPlanesVisible();
	}
}

bool KinematicDraggerCallback::receive( const osgManipulator::MotionCommand& mc)
{
    if (mc.getStage() == osgManipulator::MotionCommand::MOVE) {
        visualizer->refreshSpinboxes();
        return true;
    } 
    return osgManipulator::DraggerCallback::receive(mc);
}

