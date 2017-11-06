#include "NewChartPCH.h"
#include <QtGui/QPainter>
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"
#include "NewChartSeriesData.h"
#include <QtWidgets/QTreeWidgetItem>
#include <dataaccessorlib/BoundedArgumentsFeature.h>
#include <dataaccessorlib/BoundedValuesFeature.h>
#include <dataaccessorlib/Adapters.h>


const int ACTIVE_WIDTH = 2;
const int NON_ACTIVE_WIDTH = 1;

NewChartSerie::NewChartSerie(NewChartVisualizer * visualizer) :
		visualizer(visualizer), curve(nullptr), active(false), time(0.0f), _z(
				0), _zBase(0), statsEntry(nullptr) {

}

void NewChartSerie::setData(const utils::TypeInfo & requestedType,
		const core::VariantConstPtr & data) {
	this->data = data;
	this->requestedType = requestedType;
	std::string name = getName();
	if (name.empty()) {
		data->getMetadata("core/name", name);
	}
	curve = new NewChartCurve(name.c_str());
	data->tryGet(reader);
	continousReder = utils::make_shared < dataaccessor::DiscreteFunctionAccessorAdapter < c3dlib::ScalarChannelReaderInterface::value_type,
		c3dlib::ScalarChannelReaderInterface::argument_type >> (*reader);
	curve->setSamples(new NewChartSeriesData(reader));
	int r = rand() % 256;
	int g = rand() % 256;
	int b = rand() % 256;
	setColor(r, g, b);
	setWidth(active ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
	curve->setItemAttribute(QwtPlotItem::AutoScale, true);
	curve->setItemAttribute(QwtPlotItem::Legend, true);
	curve->setCurveFitter(nullptr);
	visualizer->addPlotCurve(curve, getScales());

	_zBase = curve->z();
	curve->setZ(_zBase + _z);
}
void NewChartSerie::updateData(const core::VariantConstPtr & data)
{
	//TODO : setData powinno wywolywac ta metode
	this->data = data;
	std::string name = getName();
	if (name.empty()) {
		data->getMetadata("core/name", name);
	}
	data->tryGet(reader);
	continousReder = utils::make_shared < dataaccessor::DiscreteFunctionAccessorAdapter < c3dlib::ScalarChannelReaderInterface::value_type,
		c3dlib::ScalarChannelReaderInterface::argument_type >> (*reader);

	curve->setSamples(new NewChartSeriesData(reader));
}
void NewChartSerie::setStatsEntry(QTreeWidgetItem * statsEntry)
{
	this->statsEntry = statsEntry;
}

const utils::TypeInfo & NewChartSerie::getRequestedDataType() const {
	return requestedType;
}

void NewChartSerie::update() {

}

void NewChartSerie::setTime(double time) {
	this->time = time;
	requestUpdate();
}

void NewChartSerie::setZ(double z, bool replot) {
	_z = z;
	if (replot == true && curve != nullptr) {
		curve->setZ(_zBase + _z);
	}
}

double NewChartSerie::z() const {
	return _z;
}

void NewChartSerie::setActive( bool val) {
	this->active = val;
	if (curve) {
		setWidth(val ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
	}
}

NewChartSerie::~NewChartSerie() {
	delete curve;
}

void NewChartSerie::setEvents(c3dlib::EventsCollectionConstPtr val) {
	UTILS_ASSERT(val);
	eventsHelper = EventsHelperPtr(new EventsHelper(val, getReader()));
	setColorsForEvents(eventsHelper->getLeftSegments(), getColor());
	setColorsForEvents(eventsHelper->getRightSegments(), getColor());
	visualizer->setEvents(this, val);
}

void NewChartSerie::removeItemsFromPlot() {
	curve->detach();
}

const QwtPlotCurve* NewChartSerie::getCurve() const {
	return curve;
}

QwtPlotCurve* NewChartSerie::getCurve() {
	return curve;
}

void NewChartSerie::setVisible( bool visible) {
	curve->setVisible(visible);
}

bool NewChartSerie::isVisible() const {
	return curve->isVisible();
}

void NewChartSerie::setWidth(int width) {
	UTILS_ASSERT(curve);
	QPen pen = curve->pen();
	pen.setWidth(width);
	curve->setPen(pen);
}

double NewChartSerie::getXOffset() const {
	return curve->getXOffset();
}

void NewChartSerie::setXOffset(double val) {
	curve->setXOffset(val);
}

double NewChartSerie::getYOffset() const {
	return curve->getYOffset();
}

void NewChartSerie::setYOffset(double val) {
	curve->setYOffset(val);
}

QPointF NewChartSerie::getOffset() const {
	return curve->getOffset();
}

void NewChartSerie::setOffset(const QPointF& offset) {
	curve->setOffset(offset);
}



double NewChartSerie::getXScale() const {
	return curve->getXScale();
}

void NewChartSerie::setXScale(double val) {
	curve->setXScale(val);
}

double NewChartSerie::getYScale() const {
	return curve->getYScale();
}

void NewChartSerie::setYScale(double val) {
	curve->setYScale(val);
}

Scales NewChartSerie::getScales() const {

	auto baf = reader->getOrCreateFeature<dataaccessor::BoundedArgumentsFeature>();
	auto bvf = reader->getOrCreateFeature<dataaccessor::BoundedValuesFeature>();

	return Scales(baf->minArgument(), baf->maxArgument(), bvf->minValue(),
		bvf->maxValue());
}

void NewChartSerie::setColorsForEvents(EventsHelper::SegmentsRange range,
		const QColor& color) {
	for (auto it = range.begin(); it != range.end(); ++it) {
		QPen pen = (*it)->normalizedCurve->pen();
		pen.setColor(color);
		(*it)->normalizedCurve->setPen(pen);
	}
}

double NewChartSerie::getCurrentValue() const {

	return continousReder->value(time);	
}

double NewChartSerie::getLength() const {
	auto baf = reader->getOrCreateFeature<dataaccessor::BoundedArgumentsFeature>();
	return baf->maxArgument() - baf->minArgument();
}

double NewChartSerie::getBegin() const {
	return reader->argument(0);
}

double NewChartSerie::getEnd() const {
	return reader->argument(reader->size() - 1);
}

void NewChartSerie::setColor(int r, int g, int b, int a /*= 255*/) {
	setColor(QColor(r, g, b, a));
}

void NewChartSerie::setColor(const QColor& color) {
	UTILS_ASSERT(curve);
	QPen pen = curve->pen();
	pen.setColor(color);
	curve->setPen(pen);
}

QColor NewChartSerie::getColor() const {
	UTILS_ASSERT(curve);
	return curve->pen().color();
}

void NewChartSerie::setName(const std::string & name) {
	this->name = name;
	if (curve) {
		curve->setTitle(QwtText(QString::fromStdString(name)));
	}

	if (statsEntry){
		statsEntry->setText(0, QString::fromStdString(name));
	}
}

const std::string NewChartSerie::getName() const {
	return name;
}

const core::VariantConstPtr & NewChartSerie::getData() const {
	return data;
}

double NewChartSerie::getTime() const {
	return time;
}
