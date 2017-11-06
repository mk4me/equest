#include "CoreLibPCH.h"
#include <corelib/HierarchyHelper.h>
#include <corelib/IVisualizerManager.h>

using namespace core;

core::HierarchyHelper::HierarchyHelper()
{
    text = QObject::tr("Create Visualizer");
}


WrappedItemHelper::WrappedItemHelper(const VariantConstPtr & wrapper) :
    wrapper(wrapper)
{
}

VisualizerPtr WrappedItemHelper::createVisualizer(IVisualizerManager* manager)
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    IVisualizerManager::VisualizerPrototypes prototypes;

    manager->getVisualizerPrototypes(wrapper->data()->getTypeInfo(), prototypes, false);
    return VisualizerPtr(prototypes.front()->create());
}

void WrappedItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
	//wrapper->getRawPtr();
	
	auto serie = visualizer->createSerie(wrapper);
	serie->innerSerie()->setName(path.toStdString());
    series.push_back(serie);
}

std::vector<utils::TypeInfo> WrappedItemHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
	ret.push_back(wrapper->data()->getTypeInfo());
    return ret;
}

void HierarchyHelper::getSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
    createSeries(visualizer, path, series);

	//filtrujemy serie -> przechodza tylko te różne od nullptr
    series.erase(std::remove(series.begin(), series.end(), nullptr), series.end());

	if(series.empty() == false && visualizer->getActiveSerie() == nullptr){
		visualizer->setActiveSerie(series.front());
	}
}

MetaTitlePatternWrappedItemHelper::MetaTitlePatternWrappedItemHelper(const VariantConstPtr & wrapper,
	const QString & pattern, const std::list<std::string> & metadata)
	: WrappedItemHelper(wrapper), pattern(pattern), metadata(metadata)
{
	UTILS_ASSERT(!metadata.empty(), "Metadata should not be empty");
}

MetaTitlePatternWrappedItemHelper::~MetaTitlePatternWrappedItemHelper()
{

}

void MetaTitlePatternWrappedItemHelper::createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series)
{
	QString serieName(pattern);

	for (const auto & meta : metadata)
	{
		std::string value;
		if (wrapper->getMetadata(meta, value) == false){
			value = "[meta:|" + meta + "|]";
		}

		serieName = serieName.arg(value.c_str());
	}

	WrappedItemHelper::createSeries(visualizer, serieName, series);
}
