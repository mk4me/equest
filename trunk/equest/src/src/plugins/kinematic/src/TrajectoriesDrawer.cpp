#include "PCH.h"
#include "TrajectoriesDrawer.h"

TrajectoryDrawerManager::TrajectoryDrawerManager()
{

}

TrajectoryDrawerManager::~TrajectoryDrawerManager()
{

}

void TrajectoryDrawerManager::setRange(const Range & range)
{
	for(auto it = featuresDescriptors.begin(); it != featuresDescriptors.end(); ++it){
		(*it)->setRange(range);
	}
}

void TrajectoryDrawerManager::setRange( const unsigned int idx, const Range & range )
{
    featuresDescriptors[idx]->setRange(range);
}

TrajectoryDrawerManager::Range TrajectoryDrawerManager::range( const unsigned int idx ) const
{
    return featuresDescriptors[idx]->range();
}

void TrajectoryDrawerManager::initialize(const std::vector<std::vector<osg::Vec3>> & pointsPositions)
{
	FeaturesDescriptors locFDs;
	locFDs.reserve(pointsPositions.size());

	osg::ref_ptr<osg::Switch> tmpNode(new osg::Switch);

	for(auto& positions : pointsPositions){
		utils::shared_ptr<osgutils::TrajectorySchemeDrawer> td(new osgutils::TrajectorySchemeDrawer());
		td->init(positions);
		tmpNode->addChild(td->getNode(), false);
		locFDs.push_back(td);
	}

    
	std::swap(locFDs, featuresDescriptors);
	std::swap(node, tmpNode);	
}

osg::ref_ptr<osg::Node> TrajectoryDrawerManager::getNode()
{
	return node;
}

const unsigned int TrajectoryDrawerManager::count() const
{
	return featuresDescriptors.size();
}

const bool TrajectoryDrawerManager::visible(const unsigned int idx) const
{
	return node->getValue(idx);
}

void TrajectoryDrawerManager::setColor(const osg::Vec4 & color)
{
	for(unsigned int i = 0; i < featuresDescriptors.size(); ++i){
		setColor(i, color);
	}	
}

void TrajectoryDrawerManager::setColor(const unsigned int idx, const osg::Vec4 & color)
{	
	featuresDescriptors[idx]->setColor(color);
}

void TrajectoryDrawerManager::setVisible(const bool visible)
{
	if(visible == true){
		node->setAllChildrenOn();
	}else{
		node->setAllChildrenOff();
	}
}

void TrajectoryDrawerManager::setVisible(const unsigned int idx, const bool visible)
{
	node->setValue(idx, visible);
}

void TrajectoryDrawerManager::setSize(const float size)
{
	for(auto it = featuresDescriptors.begin(); it != featuresDescriptors.end(); ++it){
		(*it)->setWidth(size);
	}
}

void TrajectoryDrawerManager::setSize(const unsigned int idx, const float size)
{
	featuresDescriptors[idx]->setWidth(size);
}

const osg::Vec4 & TrajectoryDrawerManager::color(const unsigned int idx) const
{
	return featuresDescriptors[idx]->color();
}

const float TrajectoryDrawerManager::size(const unsigned int idx) const
{
	return featuresDescriptors[idx]->width();
}

int TrajectoryDrawerManager::maxRange( const unsigned int idx ) const
{
    return featuresDescriptors[idx]->count();
}
