#include <plugins/hmmlib/TreeItemHelper.h>
#include <QtWidgets/QHBoxLayout>
#include <corelib/IVisualizerManager.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <utils/Debug.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/hmmlib/EMGFilter.h>
#include <loglib/Exceptions.h>
#include <boost/lexical_cast.hpp>
#include <dataaccessorlib/Wrappers.h>

using namespace core;

void Multiserie3D::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
	std::vector<Visualizer::Serie*> tmpSeries;

	try{
		if (motion->hasObject(typeid(c3dlib::MarkerCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(c3dlib::MarkerCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(c3dlib::MarkerCollection), m.front());
				s->innerSerie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw loglib::runtime_error("Empty object - markers");
			}
		}
		if (motion->hasObject(typeid(SkeletonWithStates), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(SkeletonWithStates), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(SkeletonWithStates), m.front());
				s->innerSerie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw loglib::runtime_error("Empty object - joints");
			}
		}
		if (motion->hasObject(typeid(c3dlib::GRFCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(c3dlib::GRFCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(c3dlib::GRFCollection), m.front());
				s->innerSerie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw loglib::runtime_error("Empty object - grfs");
			}
		}
	}catch(...){

	}

	series.insert(series.end(), tmpSeries.begin(), tmpSeries.end());
}

VisualizerPtr Multiserie3D::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(SkeletonWithStates), prototypes, true);
	return VisualizerPtr(prototypes.front()->create());
}

std::vector<utils::TypeInfo> Multiserie3D::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
    ret.push_back(typeid(SkeletonWithStates));
	ret.push_back(typeid(c3dlib::GRFCollection));
	ret.push_back(typeid(c3dlib::MarkerCollection));
    return ret;
}

Multiserie3D::Multiserie3D(const PluginSubject::MotionConstPtr & motion ) :
    motion(motion)
{

}


void NewMultiserieHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
    int count = wrappers.size();
    for (int i = 0; i < count; ++i) {
        auto wrapper = wrappers[i].wrapper;
		std::string source;
		wrapper->getMetadata("core/source", source);
		auto serieX = visualizer->createSerie(wrapper);
		serieX->innerSerie()->setName(source);
        if (wrappers[i].events) {
			EventSerieBase * eventSerie = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
            eventSerie->setEvents(wrappers[i].events);
        }
		INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->innerSerie());

        chartSerieX->setColor(colorStrategy->getColor(chartSerieX, wrapper));
        series.push_back(serieX);
    }
}


VisualizerPtr NewMultiserieHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
    // TODO : jawnie pobrac New Chart
	manager->getVisualizerPrototypes(typeid(c3dlib::ScalarChannelReaderInterface), prototypes, true);
	VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
    if (!chart) {
        UTILS_ASSERT(false);
        throw loglib::runtime_error("Wrong visualizer type!");
    } else {
        chart->setTitle(title);
        //chart->setShowLegend(false);
    }
    return visualizer;
}

std::vector<utils::TypeInfo> NewMultiserieHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(c3dlib::ScalarChannelReaderInterface));
    return ret;
}

NewMultiserieHelper::NewMultiserieHelper(const ChartWithDescriptionCollection& charts ):
    wrappers(charts),
    title(""),
    colorStrategy(new RandomMultiserieColorStrategy())
{
}

NewMultiserieHelper::NewMultiserieHelper(const std::vector<core::VariantConstPtr>& charts ):
    title(""),
    colorStrategy(new RandomMultiserieColorStrategy())
{
    UTILS_ASSERT(false);
    for (auto it = charts.begin(); it != charts.end(); ++it) {
		wrappers.push_back(ChartWithDescription(*it, c3dlib::EventsCollectionConstPtr(), PluginSubject::MotionConstPtr()));
    }
}



EMGFilterHelper::EMGFilterHelper(const core::VariantConstPtr& wrapper, const c3dlib::EventsCollectionConstPtr& events) :
    NewChartItemHelper(wrapper, events)
{
}


void EMGFilterHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series )
{
	c3dlib::ScalarChannelReaderInterfaceConstPtr channel = wrapper->get();

	auto ff = dataaccessor::FunctionFeature::feature(true);

    auto absTest = AbsMeanValueChannel::wrap(channel);
	auto uaf = channel->feature<dataaccessor::UniformArgumentsFeature>();
	auto baf = channel->feature<dataaccessor::BoundedArgumentsFeature>();
	absTest->attachFeature(ff);
	absTest->attachFeature(uaf);
	absTest->attachFeature(baf);


    //utils::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, ScalarChannelIntegrator()));
	//utils::shared_ptr<c3dlib::ScalarModifier> integratorChannel(new c3dlib::ScalarModifier(absTest, RMSModifier()));

	auto rmsChannel = RMSChannel::create(*absTest);
	rmsChannel->attachFeature(ff);
	rmsChannel->attachFeature(uaf);
	rmsChannel->attachFeature(baf);
	auto wrapperX = core::Variant::wrap<c3dlib::ScalarChannelReaderInterface>(rmsChannel);
	
    wrapperX->copyMetadata(*wrapper);
    visualizer->getOrCreateWidget();

    std::string name("UNKNOWN");
    wrapperX->getMetadata("core/name", name);    
	auto s = visualizer->createSerie(typeid(c3dlib::ScalarChannelReaderInterface), wrapperX);

	EventSerieBase* chartSerie = dynamic_cast<EventSerieBase*>(s->innerSerie());
    if (events && chartSerie) {
        chartSerie->setEvents(events);
    }
	s->innerSerie()->setName(name);
    series.push_back(s);

}
