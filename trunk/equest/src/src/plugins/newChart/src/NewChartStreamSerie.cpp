#include "NewChartPCH.h"
#include <QtGui/QPainter>
#include "NewChartStreamSerie.h"
#include "NewChartVisualizer.h"
#include "NewChartSeriesData.h"

const int ACTIVE_WIDTH = 2;
const int NON_ACTIVE_WIDTH = 1;

CurveData::CurveData():
		d_count(0)
{
}
void CurveData::append(double *x, double *y, int count)
{
	int newSize = ( (d_count + count) / 1000 + 1 ) * 1000;
	if ( newSize > size() )
	{
		d_x.resize(newSize);
		d_y.resize(newSize);
	}
	for ( register int i = 0; i < count; i++ )
	{
		d_x[d_count + i] = x[i];
		d_y[d_count + i] = y[i];
	}
	d_count += count;
}
int CurveData::count() const
{
	return d_count;
}
int CurveData::size() const
{
	return d_x.size();
}
const double *CurveData::x() const
{
	return d_x.data();
}
const double *CurveData::y() const
{
	return d_y.data();
}

void CurveData::clear()
{
	QVector<double>().swap(d_x);
	QVector<double>().swap(d_y);
	d_count = 0;
}

NewChartStreamSerie::NewChartStreamSerie( NewChartVisualizer * visualizer ) :
visualizer(visualizer),
    curve(nullptr),
    active(false),
    _z(0),
    _zBase(0),
	a(0.0)
{
	updater.reset(new plugin::StreamDataSerieUpdater(this));
}

void NewChartStreamSerie::setData(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data )
{
	if (scalarStream != nullptr){
		utils::const_pointer_cast<ScalarStream>(scalarStream)->detach(updater);
	}

    this->data = data;
	this->requestedType = requestedType;
	UTILS_ASSERT(requestedType == typeid(ScalarStream));
	scalarStream = data->get();
	std::string name;
	data->getMetadata("core/name", name);

    curve = new NewChartCurve(name.c_str());
	curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    visualizer->addPlotCurve(curve, getScales());
    utils::const_pointer_cast<ScalarStream>(scalarStream)->attach(updater);
}

const utils::TypeInfo & NewChartStreamSerie::getRequestedDataType() const
{
	return requestedType;
}

void NewChartStreamSerie::update()
{	
	std::pair<float, float> point;
	scalarStream->data(point);
	double aa = point.first;
	double b = point.second;

	aa = std::fmod(aa / 1000, 5.0);

	if (aa < a){
		if (curveData.size() > 0){
			std::pair<double, double> spoint;

			spoint.first = 0.0;
			spoint.second = curveData.y()[curveData.count() - 1] + (point.second - curveData.y()[curveData.count() - 1]) * (5.0 - a) / (5.0 + aa - a);

			curveData.clear();
			curveData.append(&spoint.first, &spoint.second, 1);
		}
		else{
			curveData.clear();
		}
	}

	a = aa;

	curveData.append(&a, &b, 1);
	curve->setRawSamples(curveData.x(), curveData.y(), curveData.count());
	curve->show();
}


void NewChartStreamSerie::setZ(double z, bool replot)
{
    _z = z;
    if(replot == true && curve != nullptr){
        curve->setZ(_zBase + _z);
    }
}

double NewChartStreamSerie::z() const
{
    return _z;
}

void NewChartStreamSerie::setActive( bool val )
{
    this->active = val;
    if (curve) {
        setWidth(val ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    }
}

NewChartStreamSerie::~NewChartStreamSerie()
{
    //delete[] xvals;
    //delete[] yvals;
	curve = nullptr;

	if (scalarStream != nullptr && updater != nullptr){
		utils::const_pointer_cast<ScalarStream>(scalarStream)->detach(updater);
	}
}

void NewChartStreamSerie::removeItemsFromPlot()
{
    curve->detach();
}

const QwtPlotCurve* NewChartStreamSerie::getCurve() const
{
    return curve;
}

QwtPlotCurve* NewChartStreamSerie::getCurve()
{
    return curve;
}


void NewChartStreamSerie::setVisible( bool visible )
{
    curve->setVisible(visible);
}

bool NewChartStreamSerie::isVisible() const
{
    return curve->isVisible();
}


void NewChartStreamSerie::setWidth( int width )
{
    UTILS_ASSERT(curve);
    QPen pen = curve->pen();
    pen.setWidth(width);
    curve->setPen(pen);
}

double NewChartStreamSerie::getXOffset() const
{
    return curve->getXOffset();
}

void NewChartStreamSerie::setXOffset( double val )
{
    curve->setXOffset(val);
}

double NewChartStreamSerie::getYOffset() const
{
    return curve->getYOffset();
}

void NewChartStreamSerie::setYOffset( double val )
{
    curve->setYOffset(val);
}

QPointF NewChartStreamSerie::getOffset() const
{
    return curve->getOffset();
}

void NewChartStreamSerie::setOffset( const QPointF& offset )
{
    curve->setOffset(offset);
}
double NewChartStreamSerie::getXScale() const
{
    return curve->getXScale();
}

void NewChartStreamSerie::setXScale( double val )
{
    curve->setXScale(val);
}

double NewChartStreamSerie::getYScale() const
{
    return curve->getYScale();
}

void NewChartStreamSerie::setYScale( double val )
{
    curve->setYScale(val);
}

Scales NewChartStreamSerie::getScales() const
{ 
    return Scales(0.0f, 10.0, 0.0, 10.0);
}


double NewChartStreamSerie::getCurrentValue() const
{ 
    return 0.0;
} 


void NewChartStreamSerie::setColor( int r, int g, int b, int a /*= 255*/ )
{
    setColor(QColor(r, g, b, a));
}

void NewChartStreamSerie::setColor( const QColor& color )
{
    UTILS_ASSERT(curve);
    QPen pen = curve->pen();
    pen.setColor(color);
    curve->setPen(pen);
}

QColor NewChartStreamSerie::getColor() const
{
    UTILS_ASSERT(curve);
    return curve->pen().color();
}

void NewChartStreamSerie::setName( const std::string & name )
{
    this->name = name;
}

const std::string NewChartStreamSerie::getName() const
{
    return name;
}

const core::VariantConstPtr & NewChartStreamSerie::getData() const
{
    return data;
}

void NewChartStreamSerie::updateData(const core::VariantConstPtr & data)
{
	throw std::logic_error("The method or operation is not implemented.");
}

