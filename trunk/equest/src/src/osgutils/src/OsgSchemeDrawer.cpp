#include "PCH.h"
#include <osgutils/OsgSchemeDrawer.h>
#include <osgutils/CustomPrimitivesFactory.h>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>

namespace osgutils
{

osg::Vec3Array * convert(const std::vector<osg::Vec3> & input)
{
	auto ret = new osg::Vec3Array(input.begin(), input.end());
	ret->setDataVariance(osg::Object::STATIC);
	return ret;
}

class PointInstance
{
public:
	osgutils::CustomSphereDescription sphere;
	osg::ref_ptr<osg::Geode> geode;
	osg::ref_ptr<osg::PositionAttitudeTransform> posAtt;

	static const std::vector<utils::shared_ptr<PointInstance>> createPointsScheme(const unsigned int pointsNum,
		const unsigned int complexity);

	static void update(std::vector<utils::shared_ptr<PointInstance>> & points,
		const std::vector<osg::Vec3> & positions);

	static void setSize(std::vector<utils::shared_ptr<PointInstance>> & points,
		const double size);

	static void setSize(osgutils::CustomSphereDescription & sd, const float size);

	static void setColor(std::vector<utils::shared_ptr<PointInstance>> & points,
		const osg::Vec4 & color);

	static void setColor(osgutils::CustomSphereDescription & sd, const osg::Vec4 & color);
};

const std::vector<utils::shared_ptr<PointInstance>> PointInstance::createPointsScheme(const unsigned int pointsNum,
	const unsigned int complexity)
{
	std::vector<utils::shared_ptr<PointInstance>> ret(pointsNum);	

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setDataVariance(osg::Object::STATIC);
    
	auto geode = new osg::Geode;
	geode->setStateSet(stateset);
	auto sphere = osgutils::CustomPrimitivesFactory::createSphere(complexity, 1.0, osg::Vec4(0.5, 0.5, 0.5, 0.5));
	geode->addDrawable(sphere.geom);
	auto pat = new osg::PositionAttitudeTransform;
	pat->addChild(geode);

	for(unsigned int i = 0; i < pointsNum; ++i){
		utils::shared_ptr<PointInstance> lpi(new PointInstance);
		lpi->sphere = sphere;
		lpi->geode = geode;
		lpi->posAtt = dynamic_cast<osg::PositionAttitudeTransform*>(pat->clone(osg::CopyOp::SHALLOW_COPY));
		ret[i] = lpi;
	}

	return ret;
};

void PointInstance::update(std::vector<utils::shared_ptr<PointInstance>> & points,
	const std::vector<osg::Vec3> & positions)
{
	auto s = std::min(positions.size(), points.size());
	for(unsigned int i = 0; i < s; ++i){
		points[i]->posAtt->setPosition(positions[i]);
	}
}

void PointInstance::setSize(std::vector<utils::shared_ptr<PointInstance>> & points,
	const double size)
{
	auto v = osgutils::CustomPrimitivesHelper::recreateVerticesForRadius(points[0]->sphere, size);

	for(auto it = points.begin(); it != points.end(); ++it){
		osgutils::CustomPrimitivesHelper::updatePrimitiveShape((*it)->sphere, v);
		(*it)->sphere.radius = size;
	}
}

void PointInstance::setSize(osgutils::CustomSphereDescription & sd, const float size)
{
	//! TODO
	//! mapować promień do wierzchołków aby nie powielać danych w pamięci
	auto v = osgutils::CustomPrimitivesHelper::recreateVerticesForRadius(sd, size);
	osgutils::CustomPrimitivesHelper::updatePrimitiveShape(sd, v);
}

void PointInstance::setColor(std::vector<utils::shared_ptr<PointInstance>> & points,
	const osg::Vec4 & color)
{
	osg::Vec4Array * newColorArray = new osg::Vec4Array;
	newColorArray->push_back(color);

	for(auto it = points.begin(); it != points.end(); ++it){
		osgutils::CustomPrimitivesHelper::setColor((*it)->sphere, newColorArray);
	}
}

void PointInstance::setColor(osgutils::CustomSphereDescription & sd,
	const osg::Vec4 & color)
{
	osg::Vec4Array * newColorArray = new osg::Vec4Array;
	newColorArray->push_back(color);
	osgutils::CustomPrimitivesHelper::setColor(sd, newColorArray);
}

PointsDrawer::PointsDrawer(const unsigned int complexity) : complexity(complexity)
{

}

void PointsDrawer::init(const unsigned int pointsNum)
{
	osg::ref_ptr<osg::Switch> tmpNode = new osg::Switch;

	std::vector<utils::shared_ptr<PointInstance>> locPointsInstances(PointInstance::createPointsScheme(pointsNum, complexity));

	for(auto it = locPointsInstances.begin(); it != locPointsInstances.end(); ++it){
		tmpNode->addChild((*it)->posAtt, true);
	}

	std::swap(node, tmpNode);
	std::swap(pointsInstances, locPointsInstances);
}

osg::ref_ptr<osg::Node> PointsDrawer::getNode()
{
	return node;
}

void PointsDrawer::update(const std::vector<osg::Vec3> & positions)
{
	PointInstance::update(pointsInstances, positions);	
}

void PointsDrawer::setSize(const float size)
{
	PointInstance::setSize(pointsInstances, size);
}

void PointsDrawer::setColor(const osg::Vec4 & color)
{
	PointInstance::setColor(pointsInstances, color);
}

void PointsDrawer::setVisible(const bool visible)
{
	if(visible == true){
		node->setAllChildrenOn();
	}else{
		node->setAllChildrenOff();
	}
}

void PointsDrawer::setSize(const unsigned int idx, const float size)
{
	//! TODO
	//! mapować promień do wierzchołków aby nie powielać danych w pamięci
	PointInstance::setSize(pointsInstances[idx]->sphere, size);	
}

void PointsDrawer::setColor(const unsigned int idx, const osg::Vec4 & color)
{	
	PointInstance::setColor(pointsInstances[idx]->sphere, color);
}

void PointsDrawer::setVisible(const unsigned int idx, const bool visible)
{
	node->setValue(idx, visible);
}

const osg::Vec4 & PointsDrawer::color(const unsigned int idx) const
{
	return pointsInstances[idx]->sphere.colors->at(0);
}

const bool PointsDrawer::visible(const unsigned int idx) const
{
	return node->getValue(idx);
}

const float PointsDrawer::size(const unsigned int idx) const
{
	return pointsInstances[idx]->sphere.radius;
}

class ConnectionSphereInstance
{
public:
	osgutils::CustomSphereDescription sphere;
	osg::ref_ptr<osg::Geode> geode;
	osg::ref_ptr<osg::PositionAttitudeTransform> posAtt;
	SegmentRange connectionIndices;

	static void updatePositionOrientation(const osg::Vec3 & start, const osg::Vec3 & end,
		osg::PositionAttitudeTransform * pat);

	static const std::vector<utils::shared_ptr<ConnectionSphereInstance>> createConnectionsScheme(const SegmentsDescriptors & connections,
		const unsigned int complexity);

	static void setSize(std::vector<utils::shared_ptr<ConnectionSphereInstance>> & connections,
		const double size);

	static void setSize(osgutils::CustomSphereDescription & cd, const float size);

	static void setColor(std::vector<utils::shared_ptr<ConnectionSphereInstance>> & connections,
		const osg::Vec4 & color);

	static void setColor(osgutils::CustomSphereDescription & cd, const osg::Vec4 & color);
};

void ConnectionSphereInstance::updatePositionOrientation(const osg::Vec3 & start,
	const osg::Vec3 & end, osg::PositionAttitudeTransform * pat)
{
	if (start != end) {
		static const osg::Vec3 originalDir = osg::Vec3(0.0, 0.0, 1.0);
		auto diff = end - start;
		auto nDiff(diff);
		nDiff.normalize();

		osg::Quat q;
		q.makeRotate(originalDir, nDiff);

		pat->setPosition(start + diff / 2.0);
		pat->setAttitude(q);
	}
}

const std::vector<utils::shared_ptr<ConnectionSphereInstance>> ConnectionSphereInstance::createConnectionsScheme(
	const SegmentsDescriptors & connections,
	const unsigned int complexity)
{
	if (connections.empty()) {
		UTILS_ASSERT(false);
		throw std::runtime_error("No connections");
	}
	std::vector<utils::shared_ptr<ConnectionSphereInstance>> ret(connections.size());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setDataVariance(osg::Object::STATIC);

	auto sphere = osgutils::CustomPrimitivesFactory::createSphere(complexity, 1, osg::Vec4(0.5, 0.5, 0.5, 0.5));
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setStateSet(stateset);
	geode->addDrawable(sphere.geom);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->addChild(geode);

	for(unsigned int i = 0; i < connections.size(); ++i){
		utils::shared_ptr<ConnectionSphereInstance> lci(new ConnectionSphereInstance);
		lci->connectionIndices = connections[i].range;
		float r = connections[i].length / 2;

		r = std::max(r, 0.001f);

		/*auto vertices = dynamic_cast<osg::Vec3Array*>(sphere.geomBase.verticies->clone(osg::CopyOp::DEEP_COPY_ALL));

		for (auto & v : *vertices)
		{
			v *= r;
		}*/		

		lci->sphere = sphere;
		//lci->sphere.geomBase.verticies = vertices;
		//lci->sphere.geom = dynamic_cast<osg::Geometry*>(sphere.geom->clone(osg::CopyOp::SHALLOW_COPY));
		//lci->sphere.geom->setVertexArray(vertices);
		lci->geode = geode;
		lci->posAtt = dynamic_cast<osg::PositionAttitudeTransform*>(pat->clone(osg::CopyOp::SHALLOW_COPY));
		lci->posAtt->setScale(osg::Vec3d(0.3, 0.3, 1.0) * r);		
		ret[i] = lci;
	}

	return ret;
}

void ConnectionSphereInstance::setSize(std::vector<utils::shared_ptr<ConnectionSphereInstance>> & connections,
	const double size)
{
	/*for(auto it = connections.begin(); it != connections.end(); ++it){
		osgutils::CustomPrimitivesHelper::scaleSphereRadius((*it)->sphere.geomBase.verticies, size / (*it)->sphere.radius);
		(*it)->sphere.radius = size;
		(*it)->sphere.geom->dirtyDisplayList();
	}*/
}

void ConnectionSphereInstance::setSize(osgutils::CustomSphereDescription & cd,
	const float size)
{
	//osgutils::CustomPrimitivesHelper::scaleCylinderRadius(cd.geomBase.verticies, size / cd.radius);
	osgutils::CustomPrimitivesHelper::scaleSphereRadius(cd.geomBase.verticies, size / cd.radius);
	cd.radius = size;
	cd.geom->dirtyDisplayList();
}

void ConnectionSphereInstance::setColor(std::vector<utils::shared_ptr<ConnectionSphereInstance>> & connections,
	const osg::Vec4 & color)
{
	osg::Vec4Array * newColorArray = new osg::Vec4Array;
	newColorArray->push_back(color);

	for(auto it = connections.begin(); it != connections.end(); ++it){
		osgutils::CustomPrimitivesHelper::setColor((*it)->sphere, newColorArray);
	}
}

void ConnectionSphereInstance::setColor(osgutils::CustomSphereDescription & cd,
	const osg::Vec4 & color)
{
	osg::Vec4Array * newColorArray = new osg::Vec4Array;
	newColorArray->push_back(color);
	osgutils::CustomPrimitivesHelper::setColor(cd, newColorArray);
}

ConnectionsSphereDrawer::ConnectionsSphereDrawer(const unsigned int complexity)
	: complexity(complexity)
{
	
}

void ConnectionsSphereDrawer::init(const SegmentsDescriptors & connections)
{
	if (connections.empty()) {
		UTILS_ASSERT(false);
		throw std::runtime_error("No connections");
	}

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setDataVariance(osg::Object::STATIC);

	osg::ref_ptr<osg::Switch> tmpNode(new osg::Switch);	
	std::vector<utils::shared_ptr<ConnectionSphereInstance>> tmpConnections;

	auto sphere = osgutils::CustomPrimitivesFactory::createSphere(3, 1, osg::Vec4(0.5, 0.5, 0.5, 0.5));
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setStateSet(stateset);
	geode->addDrawable(sphere.geom);
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
	pat->addChild(geode);

	for (unsigned int i = 0; i < connections.size(); ++i){
		utils::shared_ptr<ConnectionSphereInstance> lci(new ConnectionSphereInstance);
		lci->connectionIndices = connections[i].range;
		float r = connections[i].length / 2;
		r = std::max(r, 0.001f);

		/*auto vertices = dynamic_cast<osg::Vec3Array*>(sphere.geomBase.verticies->clone(osg::CopyOp::DEEP_COPY_ALL));

		for (auto & v : *vertices)
		{
			v *= r;
		}*/

		lci->sphere = sphere;
		//lci->sphere.geomBase.verticies = vertices;
		//lci->sphere.geom = dynamic_cast<osg::Geometry*>(sphere.geom->clone(osg::CopyOp::SHALLOW_COPY));
		//lci->sphere.geom->setVertexArray(vertices);
		lci->geode = geode;
		lci->posAtt = dynamic_cast<osg::PositionAttitudeTransform*>(pat->clone(osg::CopyOp::SHALLOW_COPY));
		lci->posAtt->setScale(osg::Vec3d(0.3, 0.3, 1.0) * r);		
		tmpConnections.push_back(lci);
		tmpNode->addChild(lci->posAtt, true);
	}

	/*for(unsigned int i = 0; i < locConnsInst.size(); ++i){		
		tmpNode->addChild(locConnsInst[i]->posAtt, true);
	}*/
	std::swap(connectionsInstances, tmpConnections);
	std::swap(node, tmpNode);
}

osg::ref_ptr<osg::Node> ConnectionsSphereDrawer::getNode()
{
	return node;
}

void ConnectionsSphereDrawer::update(const std::vector<osg::Vec3> & positions)
{
	for(unsigned int i = 0; i < connectionsInstances.size(); ++i){
		if(node->getValue(i) == true){
			connectionsInstances[i]->updatePositionOrientation(positions[connectionsInstances[i]->connectionIndices.first],
				positions[connectionsInstances[i]->connectionIndices.second], connectionsInstances[i]->posAtt);
		}
	}
}

void ConnectionsSphereDrawer::setSize(const float size)
{
	//ConnectionSphereInstance::setSize(connectionsInstances, size);
}

void ConnectionsSphereDrawer::setColor(const osg::Vec4 & color)
{
	//ConnectionSphereInstance::setColor(connectionsInstances, color);
}

void ConnectionsSphereDrawer::setVisible(const bool visible)
{
	if(visible == true){
		/*for(auto it = updateCache.begin(); it != updateCache.end(); ++it){
			connectionsInstances[it->first]->updatePositionOrientation(it->second.first,
				it->second.second, connectionsInstances[it->first]->posAtt);
		}

		std::map<unsigned int, std::pair<osg::Vec3, osg::Vec3>>().swap(updateCache);*/

		node->setAllChildrenOn();
	}else{
		node->setAllChildrenOff();
	}
}

void ConnectionsSphereDrawer::setSize(const unsigned int idx, const float size)
{
	//ConnectionSphereInstance::setSize(connectionsInstances[idx]->sphere, size);
}

void ConnectionsSphereDrawer::setColor(const unsigned int idx, const osg::Vec4 & color)
{
	//ConnectionSphereInstance::setColor(connectionsInstances[idx]->sphere, color);
}

void ConnectionsSphereDrawer::setVisible(const unsigned int idx, const bool visible)
{
	if(node->getValue(idx) != visible){

		/*if(visible == true){		
			auto it = updateCache.find(idx);
			if(it != updateCache.end()){
				connectionsInstances[idx]->updatePositionOrientation(it->second.first,
					it->second.second, connectionsInstances[idx]->posAtt);

				updateCache.erase(it);
			}
		}*/

		node->setValue(idx, visible);
	}
}

const osg::Vec4 & ConnectionsSphereDrawer::color(const unsigned int idx) const
{
	return connectionsInstances[idx]->sphere.colors->at(0);
	//return osg::Vec4();
}

const bool ConnectionsSphereDrawer::visible(const unsigned int idx) const
{
	return node->getValue(idx);
	//return true;
}

const float ConnectionsSphereDrawer::size(const unsigned int idx) const
{
	return connectionsInstances[idx]->sphere.radius;
	//return 1.0f;
}

class TrajectoryInstance
{
public:
	osg::ref_ptr<osg::Vec4Array> colors;
	osg::ref_ptr<osg::LineWidth> lineWidth;
	osg::ref_ptr<osg::Geometry> geom;
	osg::ref_ptr<osg::DrawArrays> da;
	osg::ref_ptr<osg::Geode> geode;
};

void TrajectorySchemeDrawer::init(const std::vector<osg::Vec3> & points)
{
    pointCount = points.size();
	utils::shared_ptr<TrajectoryInstance> tmpTrajectory(new TrajectoryInstance);
	tmpTrajectory->colors = new osg::Vec4Array;
	tmpTrajectory->colors->push_back(osg::Vec4());

	tmpTrajectory->lineWidth = new osg::LineWidth(2.0);
	tmpTrajectory->da = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, points.size());
	tmpTrajectory->geom = new osg::Geometry;
	//tmpTrajectory->geom->setUseDisplayList(false);
	//tmpTrajectory->geom->setUseVertexBufferObjects(true);
	tmpTrajectory->geom->setVertexArray(convert(points));
	tmpTrajectory->geom->setColorArray(tmpTrajectory->colors);
	tmpTrajectory->geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	tmpTrajectory->geom->addPrimitiveSet(tmpTrajectory->da);
	tmpTrajectory->geom->setDataVariance(osg::Object::DYNAMIC);

	osg::StateSet * ss;

	if(trajectory != nullptr){
		ss = dynamic_cast<osg::StateSet*>(trajectory->geom->getStateSet()->clone(osg::CopyOp::SHALLOW_COPY));
	}else{
		ss = tmpTrajectory->geom->getOrCreateStateSet();
	}

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	ss->setDataVariance(osg::Object::STATIC);

	ss->setAttribute(tmpTrajectory->lineWidth);

	tmpTrajectory->geode = new osg::Geode;
	tmpTrajectory->geode->addDrawable(tmpTrajectory->geom);
	tmpTrajectory->geode->setStateSet(ss);
	
	std::swap(trajectory, tmpTrajectory);
}

osg::ref_ptr<osg::Node> TrajectorySchemeDrawer::getNode()
{
	return trajectory->geode;
}

void TrajectorySchemeDrawer::setColor(const osg::Vec4 & color)
{
	(*(trajectory->colors))[0] = color;
    trajectory->geom->setColorArray(trajectory->colors);
}

void TrajectorySchemeDrawer::setWidth(const float width)
{
	trajectory->lineWidth->setWidth(width);
}

void TrajectorySchemeDrawer::setRange(const Range & range)
{
	trajectory->da->setFirst(range.first);
	trajectory->da->setCount(range.second - range.first);
    trajectory->geom->setPrimitiveSet(0, trajectory->da);
}

const osg::Vec4 & TrajectorySchemeDrawer::color() const
{
	return (*(trajectory->colors))[0];
}

const float TrajectorySchemeDrawer::width() const
{
	return trajectory->lineWidth->getWidth();
}

IRangeDrawer::Range TrajectorySchemeDrawer::range() const
{
    int first = trajectory->da->getFirst();
    int count = trajectory->da->getCount();
    return std::make_pair(first, first + count);
}

int TrajectorySchemeDrawer::count() const
{
    return pointCount;
}

TrajectorySchemeDrawer::TrajectorySchemeDrawer() :
    pointCount(-1)
{

}

class GhostPointsSchemeDrawer : public IBaseDrawerScheme
{
public:
	GhostPointsSchemeDrawer(GhostInstance * gi);
	virtual ~GhostPointsSchemeDrawer();

	virtual void setColor(const osg::Vec4 & color);

	virtual void setColor(const unsigned int idx, const osg::Vec4 & color);

	virtual void setVisible(const bool visible);

	virtual void setVisible(const unsigned int idx, const bool visible);

	virtual void setSize(const float size);

	virtual void setSize(const unsigned int idx, const float size);

	virtual const osg::Vec4 & color(const unsigned int idx) const;
	virtual const bool visible(const unsigned int idx) const;
	virtual const float size(const unsigned int idx) const;

private:
	GhostInstance * gi;
};

class GhostConnectionsSchemeDrawer : public IBaseDrawerScheme
{
public:
	GhostConnectionsSchemeDrawer(GhostInstance * gi);
	virtual ~GhostConnectionsSchemeDrawer();

	virtual void setColor(const osg::Vec4 & color);

	virtual void setColor(const unsigned int idx, const osg::Vec4 & color);

	virtual void setVisible(const bool visible);

	virtual void setVisible(const unsigned int idx, const bool visible);

	virtual void setSize(const float size);

	virtual void setSize(const unsigned int idx, const float size);

	virtual const osg::Vec4 & color(const unsigned int idx) const;
	virtual const bool visible(const unsigned int idx) const;
	virtual const float size(const unsigned int idx) const;

private:
	GhostInstance * gi;
};

class GhostInstance
{
public:
	//! Ramka punktów lub połaczeń
	typedef std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> Frame;
	//! Zbiór ramek punktów i połączeń
	typedef std::vector<std::pair<Frame, Frame>> Frames;

public:
	osg::ref_ptr<osg::Switch> node;
	Frames frames;	
	IRangeDrawer::Range range;

	unsigned int frameSize;

	std::vector<utils::shared_ptr<PointInstance>> pointsInstances;
	std::vector<utils::shared_ptr<ConnectionSphereInstance>> connectionsInstances;	

	utils::shared_ptr<GhostPointsSchemeDrawer> pointsDrawer;
	utils::shared_ptr<GhostConnectionsSchemeDrawer> connectionsDrawer;

	void setRange(const unsigned int start, const unsigned int end);

	void addFrame(const Frame& f);
	void setBufferSize(int size);
};

GhostPointsSchemeDrawer::GhostPointsSchemeDrawer(GhostInstance * gi) : gi(gi)
{

}
	
GhostPointsSchemeDrawer::~GhostPointsSchemeDrawer() {}

void GhostPointsSchemeDrawer::setColor(const osg::Vec4 & color)
{
	PointInstance::setColor(gi->pointsInstances, color);
}

void GhostPointsSchemeDrawer::setColor(const unsigned int idx, const osg::Vec4 & color)
{
	PointInstance::setColor(gi->pointsInstances[idx]->sphere, color);
}

void GhostPointsSchemeDrawer::setVisible(const bool visible)
{
	unsigned int f = 0;

	for(unsigned int i = 0; i < gi->frames.size(); ++i, f += gi->frameSize){

		for(unsigned int j = 0; j < gi->pointsInstances.size(); ++j){
			gi->node->setValue(f+j, visible);
		}

	}
}

void GhostPointsSchemeDrawer::setVisible(const unsigned int idx, const bool visible)
{
	unsigned int f = idx;

	for(unsigned int i = 0; i < gi->frames.size(); ++i, f += gi->frameSize){

		gi->node->setValue(f, visible);			

	}
}

void GhostPointsSchemeDrawer::setSize(const float size)
{
	PointInstance::setSize(gi->pointsInstances, size);
}

void GhostPointsSchemeDrawer::setSize(const unsigned int idx, const float size)
{
	PointInstance::setSize(gi->pointsInstances[idx]->sphere, size);
}

const osg::Vec4 & GhostPointsSchemeDrawer::color(const unsigned int idx) const
{
	return gi->pointsInstances[idx]->sphere.colors->at(0);
}

const bool GhostPointsSchemeDrawer::visible(const unsigned int idx) const
{
	return gi->node->getValue(idx);
}

const float GhostPointsSchemeDrawer::size(const unsigned int idx) const
{
	return gi->pointsInstances[idx]->sphere.radius;
}

GhostConnectionsSchemeDrawer::GhostConnectionsSchemeDrawer(GhostInstance * gi)
	: gi(gi)
{

}

GhostConnectionsSchemeDrawer::~GhostConnectionsSchemeDrawer()
{

}

void GhostConnectionsSchemeDrawer::setColor(const osg::Vec4 & color)
{
	ConnectionSphereInstance::setColor(gi->connectionsInstances, color);
}

void GhostConnectionsSchemeDrawer::setColor(const unsigned int idx, const osg::Vec4 & color)
{
	ConnectionSphereInstance::setColor(gi->connectionsInstances[idx]->sphere, color);
}

void GhostConnectionsSchemeDrawer::setVisible(const bool visible)
{
	unsigned int f = gi->pointsInstances.size();

	for(unsigned int i = 0; i < gi->frames.size(); ++i, f += gi->frameSize){

		for(unsigned int j = 0; j < gi->connectionsInstances.size(); ++j){
			gi->node->setValue(f+j, visible);
		}

	}
}

void GhostConnectionsSchemeDrawer::setVisible(const unsigned int idx, const bool visible)
{
	unsigned int f = gi->pointsInstances.size() + idx;

	for(unsigned int i = 0; i < gi->frames.size(); ++i, f += gi->frameSize){

		gi->node->setValue(f, visible);			

	}
}

void GhostConnectionsSchemeDrawer::setSize(const float size)
{
	ConnectionSphereInstance::setSize(gi->connectionsInstances, size);
}

void GhostConnectionsSchemeDrawer::setSize(const unsigned int idx, const float size)
{
	ConnectionSphereInstance::setSize(gi->connectionsInstances[idx]->sphere, size);
}

const osg::Vec4 & GhostConnectionsSchemeDrawer::color(const unsigned int idx) const
{
	return gi->connectionsInstances[idx]->sphere.colors->at(0);
}

const bool GhostConnectionsSchemeDrawer::visible(const unsigned int idx) const
{
	return gi->node->getValue(gi->pointsInstances.size() + idx);
}

const float GhostConnectionsSchemeDrawer::size(const unsigned int idx) const
{
	return gi->connectionsInstances[idx]->sphere.radius;
}

GhostSchemeDrawer::GhostSchemeDrawer(const unsigned int pointComplexity,
	const unsigned int connectionComplexity) : pointComplexity(pointComplexity),
	connectionComplexity(connectionComplexity)
{

}

void GhostSchemeDrawer::init(const std::vector<std::vector<osg::Vec3>> & points,
	const SegmentsDescriptors & connections)
{
    if (connections.empty()) {
        return;
    }
	utils::shared_ptr<GhostInstance> tmpGhost(new GhostInstance);
	tmpGhost->pointsDrawer.reset(new GhostPointsSchemeDrawer(tmpGhost.get()));
	tmpGhost->connectionsDrawer.reset(new GhostConnectionsSchemeDrawer(tmpGhost.get()));
	tmpGhost->range.first = 0;
	tmpGhost->range.second = points.size()-1;
	tmpGhost->node = new osg::Switch;
	tmpGhost->pointsInstances = PointInstance::createPointsScheme(points[0].size(), pointComplexity);
	tmpGhost->connectionsInstances = ConnectionSphereInstance::createConnectionsScheme(connections, connectionComplexity);
	tmpGhost->frameSize = tmpGhost->pointsInstances.size() + tmpGhost->connectionsInstances.size();
	tmpGhost->frames.resize(points.size());

	//!! Kolejność ma znaczenie!! Używam indeksów przy setRange do szybkiego ustawienia widoczności ramek

	//teraz powielam węzły i połaczenia - ostatnią ramke zostawiam - moje prototypy nią będą
	for(unsigned int i = 0; i < points.size() - 1; ++i){

		for(unsigned int j = 0; j < tmpGhost->pointsInstances.size(); ++j){		

			auto pat = dynamic_cast<osg::PositionAttitudeTransform*>(tmpGhost->pointsInstances[0]->posAtt->clone(osg::CopyOp::SHALLOW_COPY));
			pat->setPosition(points[i][j]);
			tmpGhost->frames[i].first.push_back(pat);
			tmpGhost->node->addChild(pat);
		}

		for(unsigned int j = 0; j < tmpGhost->connectionsInstances.size(); ++j){

			auto pat = dynamic_cast<osg::PositionAttitudeTransform*>(tmpGhost->connectionsInstances[j]->posAtt->clone(osg::CopyOp::SHALLOW_COPY));
			ConnectionSphereInstance::updatePositionOrientation(points[i][tmpGhost->connectionsInstances[j]->connectionIndices.first],
				points[i][tmpGhost->connectionsInstances[j]->connectionIndices.second], pat);
			tmpGhost->frames[i].second.push_back(pat);
			tmpGhost->node->addChild(pat);
		}
	}

	GhostInstance::Frame cF;

	for(unsigned int i = 0; i < tmpGhost->pointsInstances.size(); ++i){
		cF.push_back(tmpGhost->pointsInstances[i]->posAtt);
		tmpGhost->node->addChild(tmpGhost->pointsInstances[i]->posAtt);
	}

	tmpGhost->frames.back().first = cF;

	PointInstance::update(tmpGhost->pointsInstances, points[points.size()-1]);	

	for(unsigned int i = 0; i < tmpGhost->connectionsInstances.size(); ++i){
		ConnectionSphereInstance::updatePositionOrientation(points.back()[tmpGhost->connectionsInstances[i]->connectionIndices.first],
			points.back()[tmpGhost->connectionsInstances[i]->connectionIndices.second],
			tmpGhost->connectionsInstances[i]->posAtt);
		cF.push_back(tmpGhost->connectionsInstances[i]->posAtt);
		tmpGhost->node->addChild(tmpGhost->connectionsInstances[i]->posAtt);
	}

	tmpGhost->frames.back().second = cF;
	tmpGhost->node->setDataVariance(osg::Object::STATIC);
	std::swap(ghostInstance, tmpGhost);
}

osg::ref_ptr<osg::Node> GhostSchemeDrawer::getNode()
{	
	return ghostInstance->node;
}

void GhostSchemeDrawer::setRange(const Range & range)
{
	// zakresy rozdzielne
	if(range.first > ghostInstance->range.second ||
		range.second < ghostInstance->range.first){		

		for(unsigned int i = ghostInstance->range.first * ghostInstance->frameSize;
			i < (ghostInstance->range.second * ghostInstance->frameSize); ++i){

			ghostInstance->node->setValue(i, false);
		
		}

		for(unsigned int i = range.first * ghostInstance->frameSize;
			i < (range.second * ghostInstance->frameSize); ++i){

			ghostInstance->node->setValue(i, true);

		}

	// zakresy zazębiaja się
	}else{

		// początek
		if(ghostInstance->range.first < range.first){
			for(unsigned int i = ghostInstance->range.first * ghostInstance->frameSize;
				i < (range.first * ghostInstance->frameSize); ++i){
					ghostInstance->node->setValue(i, false);
			}
		}else if(range.first < ghostInstance->range.first){
			for(unsigned int i = range.first * ghostInstance->frameSize;
				i < (ghostInstance->range.first * ghostInstance->frameSize); ++i){
				ghostInstance->node->setValue(i, true);
			}
		}

		//koniec

		if(ghostInstance->range.second < range.second){
			for(unsigned int i = ((ghostInstance->range.second + 1) * ghostInstance->frameSize);
				i < ((range.second + 1) * ghostInstance->frameSize); ++i){
				ghostInstance->node->setValue(i, true);
			}
		}else if(range.second < ghostInstance->range.second){
			for(unsigned int i = ((range.second + 1) * ghostInstance->frameSize);
				i < ((ghostInstance->range.second + 1) * ghostInstance->frameSize); ++i){
				ghostInstance->node->setValue(i, false);
			}
		}

	}

	ghostInstance->range = range;
}

IBaseDrawerScheme * GhostSchemeDrawer::connectionsDrawer()
{
	return ghostInstance->connectionsDrawer.get();
}

IBaseDrawerScheme * GhostSchemeDrawer::pointsDrawer()
{
	return ghostInstance->pointsDrawer.get();
}

class ConnectionInstance
{
public:
	osgutils::CustomCylinderDescription cylinder;
	osg::ref_ptr<osg::Geode> geode;
	osg::ref_ptr<osg::PositionAttitudeTransform> posAtt;
	SegmentRange connectionIndices;

	static void updatePositionOrientation(const osg::Vec3 & start, const osg::Vec3 & end,
		osg::PositionAttitudeTransform * pat);

	static const std::vector<utils::shared_ptr<ConnectionInstance>> createConnectionsScheme(const SegmentsDescriptors & connections,
		const unsigned int complexity);

	static void setSize(std::vector<utils::shared_ptr<ConnectionInstance>> & connections,
		const double size);

	static void setSize(osgutils::CustomCylinderDescription & cd, const float size);

	static void setColor(std::vector<utils::shared_ptr<ConnectionInstance>> & connections,
		const osg::Vec4 & color);

	static void setColor(osgutils::CustomCylinderDescription & cd, const osg::Vec4 & color);
};

void ConnectionInstance::updatePositionOrientation(const osg::Vec3 & start,
	const osg::Vec3 & end, osg::PositionAttitudeTransform * pat)
{
	static const osg::Vec3 originalDir = osg::Vec3(0.0, 0.0, 1.0);
	auto diff = end - start;
	auto nDiff(diff);
	nDiff.normalize();

	osg::Quat q;
	q.makeRotate(originalDir, nDiff);

	pat->setPosition(start + diff / 2.0);
	pat->setAttitude(q);
}

const std::vector<utils::shared_ptr<ConnectionInstance>> ConnectionInstance::createConnectionsScheme(
	const SegmentsDescriptors & connections,
	const unsigned int complexity)
{
	std::vector<utils::shared_ptr<ConnectionInstance>> ret(connections.size());

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);


	auto cylinder = osgutils::CustomPrimitivesFactory::createCylinder(complexity, 1.0, 5.0, osg::Vec4(0.5, 0.5, 0.5, 0.5));
	for (unsigned int i = 0; i < connections.size(); ++i){
		utils::shared_ptr<ConnectionInstance> lci(new ConnectionInstance);
		lci->connectionIndices = connections[i].range;
		lci->cylinder = osgutils::CustomPrimitivesHelper::cloneForHeightChange(cylinder, connections[i].length);
		lci->geode = new osg::Geode;
		lci->geode->setStateSet(stateset);
		lci->posAtt = new osg::PositionAttitudeTransform;
		lci->geode->addDrawable(lci->cylinder.geom);
		lci->posAtt->addChild(lci->geode);
		ret[i] = lci;
	}

	return ret;
}

void ConnectionInstance::setSize(std::vector<utils::shared_ptr<ConnectionInstance>> & connections,
	const double size)
{
	for (auto it = connections.begin(); it != connections.end(); ++it){
		osgutils::CustomPrimitivesHelper::scaleCylinderRadius((*it)->cylinder.geomBase.verticies, size / (*it)->cylinder.radius);
		(*it)->cylinder.radius = size;
		(*it)->cylinder.geom->dirtyDisplayList();
	}
}

void ConnectionInstance::setSize(osgutils::CustomCylinderDescription & cd,
	const float size)
{
	osgutils::CustomPrimitivesHelper::scaleCylinderRadius(cd.geomBase.verticies, size / cd.radius);
	cd.radius = size;
	cd.geom->dirtyDisplayList();
}

void ConnectionInstance::setColor(std::vector<utils::shared_ptr<ConnectionInstance>> & connections,
	const osg::Vec4 & color)
{
	osg::Vec4Array * newColorArray = new osg::Vec4Array;
	newColorArray->push_back(color);

	for (auto it = connections.begin(); it != connections.end(); ++it){
		osgutils::CustomPrimitivesHelper::setColor((*it)->cylinder, newColorArray);
	}
}

void ConnectionInstance::setColor(osgutils::CustomCylinderDescription & cd,
	const osg::Vec4 & color)
{
	osg::Vec4Array * newColorArray = new osg::Vec4Array;
	newColorArray->push_back(color);
	osgutils::CustomPrimitivesHelper::setColor(cd, newColorArray);
}

ConnectionsDrawer::ConnectionsDrawer(const unsigned int complexity)
: complexity(complexity)
{

}

void ConnectionsDrawer::init(const SegmentsDescriptors & connections)
{
	std::vector<utils::shared_ptr<ConnectionInstance>> locConnsInst(ConnectionInstance::createConnectionsScheme(connections, complexity));
	osg::ref_ptr<osg::Switch> tmpNode(new osg::Switch);

	for (auto& instance : locConnsInst) {
		tmpNode->addChild(instance->posAtt, true);
	}

	std::swap(node, tmpNode);
	std::swap(connectionsInstances, locConnsInst);
}

osg::ref_ptr<osg::Node> ConnectionsDrawer::getNode()
{
	return node;
}

void ConnectionsDrawer::update(const std::vector<osg::Vec3> & positions)
{
	for (unsigned int i = 0; i < connectionsInstances.size(); ++i){
		if (node->getValue(i) == true){
			connectionsInstances[i]->updatePositionOrientation(positions[connectionsInstances[i]->connectionIndices.first],
				positions[connectionsInstances[i]->connectionIndices.second], connectionsInstances[i]->posAtt);
		}
		else{
			updateCache[i] = std::make_pair(positions[connectionsInstances[i]->connectionIndices.first],
				positions[connectionsInstances[i]->connectionIndices.second]);
		}
	}
}

void ConnectionsDrawer::setSize(const float size)
{
	ConnectionInstance::setSize(connectionsInstances, size);
}

void ConnectionsDrawer::setColor(const osg::Vec4 & color)
{
	ConnectionInstance::setColor(connectionsInstances, color);
}

void ConnectionsDrawer::setVisible(const bool visible)
{
	if (visible == true){
		for (auto it = updateCache.begin(); it != updateCache.end(); ++it){
			connectionsInstances[it->first]->updatePositionOrientation(it->second.first,
				it->second.second, connectionsInstances[it->first]->posAtt);
		}

		std::map<unsigned int, std::pair<osg::Vec3, osg::Vec3>>().swap(updateCache);

		node->setAllChildrenOn();
	}
	else{
		node->setAllChildrenOff();
	}
}

void ConnectionsDrawer::setSize(const unsigned int idx, const float size)
{
	ConnectionInstance::setSize(connectionsInstances[idx]->cylinder, size);
}

void ConnectionsDrawer::setColor(const unsigned int idx, const osg::Vec4 & color)
{
	ConnectionInstance::setColor(connectionsInstances[idx]->cylinder, color);
}

void ConnectionsDrawer::setVisible(const unsigned int idx, const bool visible)
{
	if (node->getValue(idx) != visible){

		if (visible == true){
			auto it = updateCache.find(idx);
			if (it != updateCache.end()){
				connectionsInstances[idx]->updatePositionOrientation(it->second.first,
					it->second.second, connectionsInstances[idx]->posAtt);

				updateCache.erase(it);
			}
		}

		node->setValue(idx, visible);
	}
}

const osg::Vec4 & ConnectionsDrawer::color(const unsigned int idx) const
{
	return connectionsInstances[idx]->cylinder.colors->at(0);
}

const bool ConnectionsDrawer::visible(const unsigned int idx) const
{
	return node->getValue(idx);
}

const float ConnectionsDrawer::size(const unsigned int idx) const
{
	return connectionsInstances[idx]->cylinder.radius;
}

}
