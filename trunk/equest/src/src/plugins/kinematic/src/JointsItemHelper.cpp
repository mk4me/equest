#include <plugins/kinematic/JointsItemHelper.h>
#include <corelib/IVisualizerManager.h>
#include <utils/Debug.h>
#include <plugins/kinematic/Wrappers.h>
#include <loglib/Exceptions.h>
#include <boost/lexical_cast.hpp>
#include <dataaccessorlib/Wrappers.h>

using namespace core;
//
VisualizerPtr JointsItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(SkeletonWithStates), prototypes, true);
	return VisualizerPtr(prototypes.front()->create());
}

void JointsItemHelper::createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series)
{
	if (skeletonWithStates && skeletonWithStates->getRawPtr() != nullptr) {
		auto s = visualizer->createSerie(typeid(SkeletonWithStates), skeletonWithStates);
		s->innerSerie()->setName(path.toStdString());
		series.push_back(s);
	} else {
		throw loglib::runtime_error("Empty object - joints");
	}
}

std::vector<utils::TypeInfo> JointsItemHelper::getTypeInfos() const
{
	std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(SkeletonWithStates));
	return ret;
}

JointsItemHelper::JointsItemHelper(core::VariantConstPtr skeletonWithStates) :
skeletonWithStates(skeletonWithStates)
{
}
