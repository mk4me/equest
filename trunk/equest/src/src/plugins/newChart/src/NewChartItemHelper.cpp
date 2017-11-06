#include <plugins/newChart/NewChartItemHelper.h>
#include <corelib/IVisualizerManager.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <utils/Debug.h>
#include <loglib/Exceptions.h>
#include <boost/lexical_cast.hpp>
#include <dataaccessorlib/Wrappers.h>
#include <plugins/c3d/C3DChannels.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <corelib/Variant.h>
#include "QCoreApplication"

QString processTitle(const std::string & title)
{
	QString ret(title.c_str());

	ret.replace("deg", QChar(0x00B0));
	ret.replace("micro", QChar(0x00B5));

	return ret;
}


core::VisualizerPtr NewChartItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(c3dlib::ScalarChannelReaderInterface), prototypes, true);
	core::VisualizerPtr visualizer(prototypes.front()->create());

	QWidget * visWidget = visualizer->getOrCreateWidget();
	visWidget->layout()->setContentsMargins(2, 0, 2, 2);
	INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
	if (!chart) {
		UTILS_ASSERT(false);
		throw loglib::runtime_error("Wrong visualizer type!");
	} else {
		std::string title;
		c3dlib::ScalarChannelReaderInterfaceConstPtr scalar = wrapper->get();

		auto df = scalar->feature<dataaccessor::DescriptorFeature>();

		if (df != nullptr) {

			title += df->name();
			title += " [";
			title += df->valueUnit();
			title += "]";

		} else {

		}

		chart->setTitle(processTitle(title));

	}

	return visualizer;
}


void NewChartItemHelper::createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series)
{
	auto serie = visualizer->createSerie(wrapper);
	serie->innerSerie()->setName(path.toStdString());
	series.push_back(serie);
}

std::vector<utils::TypeInfo> NewChartItemHelper::getTypeInfos() const
{
	std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(c3dlib::ScalarChannelReaderInterface));
	return ret;
}

NewChartItemHelper::NewChartItemHelper(const core::VariantConstPtr& wrapper, const c3dlib::EventsCollectionConstPtr& events) :
WrappedItemHelper(wrapper),
events(events)
{
}

core::VisualizerPtr NewVector3ItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(c3dlib::ScalarChannelReaderInterface), prototypes, true);
	core::VisualizerPtr visualizer(prototypes.front()->create());

	QWidget * visWidget = visualizer->getOrCreateWidget();
	visWidget->layout()->setContentsMargins(2, 0, 2, 2);
	INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
	if (!chart) {
		UTILS_ASSERT(false);
		throw loglib::runtime_error("Wrong visualizer type!");
	} else {
		std::string title;
		c3dlib::VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();

		auto df = vectorChannel->feature<dataaccessor::DescriptorFeature>();

		if (df != nullptr) {

			title += df->name();
			title += " [";
			title += df->valueUnit();
			title += "]";

		} else {

		}

		chart->setTitle(processTitle(title));
	}
	return visualizer;
}

void NewVector3ItemHelper::createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series)
{
	c3dlib::VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();

	auto ff = dataaccessor::FunctionFeature::feature(true);
	auto uaf = vectorChannel->feature<dataaccessor::UniformArgumentsFeature>();
	auto baf = vectorChannel->feature<dataaccessor::BoundedArgumentsFeature>();
	auto descf = vectorChannel->feature<dataaccessor::DescriptorFeature>();

	auto x = dataaccessor::StaticVector<0>::wrap(vectorChannel);
	auto y = dataaccessor::StaticVector<1>::wrap(vectorChannel);
	auto z = dataaccessor::StaticVector<2>::wrap(vectorChannel);

	x->attachFeature(ff);
	x->attachFeature(uaf);
	x->attachFeature(baf);
	y->attachFeature(ff);
	y->attachFeature(uaf);
	y->attachFeature(baf);
	z->attachFeature(ff);
	z->attachFeature(uaf);
	z->attachFeature(baf);

	static int number = 0;
	// hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
	std::string suffix = boost::lexical_cast<std::string>(number++);
	if (descf) {
		typedef dataaccessor::DescriptorFeature DF;
		x->attachFeature(utils::shared_ptr<DF>(DF::create<float, float>("X_" + suffix, descf->valueUnit(), descf->argumentUnit())));
		y->attachFeature(utils::shared_ptr<DF>(DF::create<float, float>("Y_" + suffix, descf->valueUnit(), descf->argumentUnit())));
		z->attachFeature(utils::shared_ptr<DF>(DF::create<float, float>("Z_" + suffix, descf->valueUnit(), descf->argumentUnit())));
	}

	/*
	core::VariantPtr wrapperX = core::Variant::wrap<decltype(x)>(x);
	core::VariantPtr wrapperY = core::Variant::wrap<decltype(x)>(y);
	core::VariantPtr wrapperZ = core::Variant::wrap<decltype(x)>(z);
	*/

	core::VariantPtr wrapperX = core::Variant::wrap(x);
	core::VariantPtr wrapperY = core::Variant::wrap(y);
	core::VariantPtr wrapperZ = core::Variant::wrap(z);

	
	std::string p = path.toStdString();

	wrapperX->setMetadata("core/name", "X_" + suffix);
	wrapperY->setMetadata("core/name", "Y_" + suffix);
	wrapperZ->setMetadata("core/name", "Z_" + suffix);
	wrapperX->setMetadata("core/source", p + "/X_" + suffix);
	wrapperY->setMetadata("core/source", p + "/Y_" + suffix);
	wrapperZ->setMetadata("core/source", p + "/Z_" + suffix);
	visualizer->getOrCreateWidget();
	
	auto serieX = visualizer->createSerie(wrapperX);
	serieX->innerSerie()->setName(xAxisName.isEmpty() == false ? xAxisName.toStdString() : ("X_" + suffix));
	auto serieY = visualizer->createSerie(wrapperY);
	serieY->innerSerie()->setName(yAxisName.isEmpty() == false ? yAxisName.toStdString() : ("Y_" + suffix));
	auto serieZ = visualizer->createSerie(wrapperZ);
	serieZ->innerSerie()->setName(zAxisName.isEmpty() == false ? zAxisName.toStdString() : ("Z_" + suffix));

	INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->innerSerie());
	INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->innerSerie());
	INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->innerSerie());

	chartSerieX->setColor(QColor(255, 0, 0));
	chartSerieY->setColor(QColor(0, 255, 0));
	chartSerieZ->setColor(QColor(0, 0, 255));

	if (events) {
		EventSerieBase* eventSerieX = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
		EventSerieBase* eventSerieY = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
		EventSerieBase* eventSerieZ = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
		if (eventSerieX && eventSerieY && eventSerieZ) {
			eventSerieX->setEvents(events);
			eventSerieY->setEvents(events);
			eventSerieZ->setEvents(events);
		}
	}


	series.push_back(serieX);
	series.push_back(serieY);
	series.push_back(serieZ);
}

NewVector3ItemHelper::NewVector3ItemHelper(const core::VariantConstPtr& wrapper, const c3dlib::EventsCollectionConstPtr& events,
										   const QString & xAxisName, const QString & yAxisName, const QString & zAxisName) :
										   WrappedItemHelper(wrapper), events(events),
										   xAxisName(xAxisName), yAxisName(yAxisName), zAxisName(zAxisName)
{
}

std::vector<utils::TypeInfo> NewVector3ItemHelper::getTypeInfos() const
{
	std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(c3dlib::ScalarChannelReaderInterface));
	return ret;
}
