#include "NewChartPCH.h"
#include <QtWidgets/QAction>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtGui/QPainter>
#include <QtWidgets/QMenu>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include "NewChartVisualizer.h"
#include "StatsTable.h"
#include "NewChartMarker.h"
#include "NewChartValueMarker.h"
#include "NewChartVerticals.h"
#include "NewChartHelpers.h"
#include "NewChartScaleDrawer.h"
#include "NewChartLegend.h"
#include "NewChartLegendItem.h"
#include <limits>
#include <boost/lexical_cast.hpp>
#include <coreui/CoreAction.h>
#include <coreui/CoreWidgetAction.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyDataItem.h>
#include "plugins/newChart/NewChartItemHelper.h"

#include "NewChartStreamSerie.h"
#include "dataaccessorlib/Wrappers.h"
#include "dataaccessorlib/DescriptorFeature.h"
#include "c3dlib/C3DTypes.h"

#include <plugins/c3d/C3DChannels.h>
#include "MovingAverageGenerator.h"
#include "SliceGenerator.h"

std::vector<INewChartCurveGeneratorConstPtr> INewChartVisualizer::generators;
// TODO : jak sama nazwa wskazuje - jest to tymczasowy obiekt
// trzeba przeniesc gdzies dodawanie domyslnych generatorow
struct TemporaryNewChartInitializer
{

	TemporaryNewChartInitializer() {
		INewChartVisualizer::addGenerator(utils::make_shared<MovingAverageGenerator>());
		//INewChartVisualizer::addGenerator(utils::make_shared<SliceGenerator>());
	}
};

TemporaryNewChartInitializer initer;

NewChartVisualizer::NewChartVisualizer() :
	qwtPlot(nullptr),
    showLegend(true), 
    currentSerie(-1),
    plotPanner(nullptr),
    plotMagnifier(nullptr),
    statsTable(nullptr),
	scaleToActive(false),
	percentDraw(nullptr),
	context(c3dlib::C3DEventsCollection::IEvent::General),
    eventsMenu(nullptr),
    shiftSpinX(nullptr),
    shiftSpinY(nullptr),
    scaleSpinX(nullptr),
    scaleSpinY(nullptr),
	//movingAvgSpin(nullptr),
	legend(nullptr),
	qwtMarker(nullptr),
	pickerAction(nullptr),
	valueMarkerAction(nullptr),
	vMarkerAction(nullptr),
	hMarkerAction(nullptr),
	scaleAction(nullptr),
    currentSerieTime(-1.0f),
    currentSerieValue(-1.0f),
	customScale(false),
	generatorHelper(this)
{

}


NewChartVisualizer::~NewChartVisualizer()
{
}


QWidget* NewChartVisualizer::createWidget()
{
    QWidget* widget = new QWidget();
    widget->setObjectName(QString::fromUtf8("newChartVisualizerWidget"));
    QwtText txt(name().c_str());
    qwtPlot = new QwtPlot(txt, nullptr);
    qwtPlot->setObjectName(QString::fromUtf8("plot"));
    percentDraw = new PercentScaleDraw(0.0, 5.0);
    qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, percentDraw);
    
    //aby legenda nie była usuwana podczas chowania i pokazywania trzeba ustawić parenta innego niż QwtPlot
    legend = new NewChartLegend(widget);
    legend->setDefaultItemMode(QwtLegendData::Checkable);
  
    qwtPlot->setAutoReplot(true);

    if (isShowLegend()) {    
        qwtPlot->insertLegend( legend, QwtPlot::BottomLegend );
    }

    qobject_cast<QwtPlotCanvas *>(qwtPlot->canvas())->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
    qwtPlot->canvas()->installEventFilter(this);


    eventsMenu = new QComboBox();
    eventsMenu->setToolTip(tr("Events context"));

    QIcon eventsIcon(":/newChart/icons/normalizacja1.png");
	eventsMenu->addItem(eventsIcon, tr("None events"), QVariant(c3dlib::C3DEventsCollection::IEvent::General));
	eventsMenu->addItem(eventsIcon, tr("Left events"), QVariant(c3dlib::C3DEventsCollection::IEvent::Left));
	eventsMenu->addItem(eventsIcon, tr("Right events"), QVariant(c3dlib::C3DEventsCollection::IEvent::Right));
	connect(eventsMenu, SIGNAL(activated(int)), this, SLOT(onEventContext(int)));

	coreUI::CoreWidgetAction * eventsAction = new coreUI::CoreWidgetAction(widget, tr("Events"), coreUI::CoreTitleBar::Right);
	eventsAction->setDefaultWidget(eventsMenu);
	widget->addAction(eventsAction);


	picker.reset(new NewChartPicker(this));
    connect(picker.get(), SIGNAL(serieSelected(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));

    coreUI::CoreAction * pickerAction = new coreUI::CoreAction(tr("Operations"), QIcon(":/newChart/icons/picker.png"), tr("Picker"), widget, coreUI::CoreTitleBar::Left);
    statesMap[pickerAction] = picker;
    connect(pickerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(pickerAction);
    

    coreUI::CoreAction * valueMarkerAction = new coreUI::CoreAction(tr("Tags"), QIcon(":/newChart/icons/value_tag.png"), tr("Value Marker"), widget, coreUI::CoreTitleBar::Left);
    statesMap[valueMarkerAction] =  NewChartStatePtr(new NewChartValueMarker(this));
    connect(valueMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(valueMarkerAction);


	coreUI::CoreAction * hMarkerAction = new coreUI::CoreAction(tr("Tags"), QIcon(":/newChart/icons/vertical_tag.png"), tr("Horizontal Marker"), widget, coreUI::CoreTitleBar::Left);
	statesMap[hMarkerAction] = NewChartStatePtr(new NewChartVerticals(this, NewChartVerticalsConnection::Horizontal));
    connect(hMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(hMarkerAction);


	coreUI::CoreAction * vMarkerAction = new coreUI::CoreAction(tr("Tags"), QIcon(":/newChart/icons/horizontal_tag.png"), tr("Vertical Marker"), widget, coreUI::CoreTitleBar::Left);
	statesMap[vMarkerAction] = NewChartStatePtr(new NewChartVerticals(this, NewChartVerticalsConnection::Vertical));
    connect(vMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(vMarkerAction);


	coreUI::CoreAction * scaleAction = new coreUI::CoreAction(tr("Operations"), QIcon(":/newChart/icons/pomiary.png"), tr("Scale to active"), widget, coreUI::CoreTitleBar::Left);
    scaleAction->setCheckable(true);
    scaleAction->setChecked(scaleToActive);
    connect(scaleAction, SIGNAL(triggered(bool)), this, SLOT(scaleToActiveSerie(bool)));
	widget->addAction(scaleAction);


	QIcon iconStats;
	iconStats.addFile(QString::fromUtf8(":/newChart/icons/stat-a.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconStats.addFile(QString::fromUtf8(":/newChart/icons/stat-b.png"), QSize(), QIcon::Normal, QIcon::On);
	coreUI::CoreAction * showStats = new coreUI::CoreAction(tr("Operations"), iconStats, tr("Statistics"), widget, coreUI::CoreTitleBar::Left);
    showStats->setCheckable(true);
    showStats->setChecked(false);
    connect(showStats, SIGNAL(triggered(bool)), this, SLOT(showStatistics(bool)));
	widget->addAction(showStats);


    for ( int i = 0; i < QwtPlot::axisCnt; ++i )    {
        QwtScaleWidget *scaleWidget = qwtPlot->axisWidget( i );
        if ( scaleWidget ) {
            scaleWidget->setMargin( 0 );
        }
    }
    qwtPlot->plotLayout()->setAlignCanvasToScales( true );

    plotPanner =  new QwtPlotPanner( qwtPlot->canvas() );
    plotMagnifier =  new QwtPlotMagnifier( qwtPlot->canvas() );
    

    connect(legend, SIGNAL(checked( const QVariant&, bool, int)), this, SLOT(onSerieSelected(const QVariant&, bool, int)));
    connect(legend, SIGNAL(checkboxChanged(const QVariant&, bool)), this, SLOT(onSerieVisible(const QVariant&, bool)));
    qwtMarker = new NewChartMarker();
    qwtMarker->setXValue(0);
    qwtMarker->setYValue(0);
    qwtMarker->attach(qwtPlot);

    grid.reset(new QwtPlotGrid);
    grid->enableXMin(false);
    grid->enableYMin(false);

    grid->setMajorPen(QPen(Qt::gray, 0, Qt::DashLine));
    grid->attach(qwtPlot);

    this->currentState = picker;
    this->currentState->stateBegin();

    qwtPlot->replot();

    statsTable = new StatsTable();
    statsTable->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    qwtPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QVBoxLayout* layout = new QVBoxLayout();
	QSplitter* splitter = new QSplitter(Qt::Vertical);
	splitter->addWidget(qwtPlot);
	splitter->addWidget(statsTable);
	layout->addWidget(splitter);
    statsTable->setVisible(false);
    widget->setLayout(layout);

    auto shiftX = LabeledSpinbox::create(tr("X:"), 0.03, -(std::numeric_limits<double>::max)(), (std::numeric_limits<double>::max)());
    auto shiftY = LabeledSpinbox::create(tr("Y:"), 0.03, -(std::numeric_limits<double>::max)(), (std::numeric_limits<double>::max)());
    connect(shiftX.second, SIGNAL(valueChanged(double)), this, SLOT(onShiftX(double)));
    connect(shiftY.second, SIGNAL(valueChanged(double)), this, SLOT(onShiftY(double)));
    shiftSpinX = shiftX.second;
    shiftSpinY = shiftY.second;

	coreUI::CoreWidgetAction * shiftSpinXAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Left);
	shiftSpinXAction->setDefaultWidget(shiftX.first);
	widget->addAction(shiftSpinXAction);

	coreUI::CoreWidgetAction * shiftSpinYAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Left);
	shiftSpinYAction->setDefaultWidget(shiftY.first);
	widget->addAction(shiftSpinYAction);


    auto scaleX = LabeledSpinbox::create(tr("SX:"), 0.01, -5.0, 5.0);
    auto scaleY = LabeledSpinbox::create(tr("SY:"), 0.01, -5.0, 5.0);
    connect(scaleX.second, SIGNAL(valueChanged(double)), this, SLOT(onScaleX(double)));
    connect(scaleY.second, SIGNAL(valueChanged(double)), this, SLOT(onScaleY(double)));
    scaleSpinX = scaleX.second;
    scaleSpinY = scaleY.second;

	coreUI::CoreWidgetAction * scaleSpinXAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Right);
	scaleSpinXAction->setDefaultWidget(scaleX.first);
	widget->addAction(scaleSpinXAction);

	coreUI::CoreWidgetAction * scaleSpinYAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Right);
	scaleSpinYAction->setDefaultWidget(scaleY.first);
	widget->addAction(scaleSpinYAction);
    
	auto actions = generatorHelper.getActions();
	widget->addActions(actions);

	
	/*coreUI::CoreAction * movingAvgAction = new coreUI::CoreAction(tr("Moving Average"), QIcon(":/newChart/icons/charts.png"), tr("Moving Average"), widget, coreUI::CoreTitleBar::Left);
	movingAvgAction->setCheckable(true);
	movingAvgAction->setChecked(false);
	connect(movingAvgAction, SIGNAL(triggered(bool)), this, SLOT(showMovingAverageCurve(bool)));
	widget->addAction(movingAvgAction);
	auto movingAvgBox = LabeledSpinbox::create(tr("w:"), 1, 1, 500);
	connect(movingAvgBox.second, SIGNAL(valueChanged(int)), this, SLOT(setMovingAverageTimeWindow(int)));
	movingAvgSpin = movingAvgBox.second;

	coreUI::CoreWidgetAction * movingAvgSpinAction = new coreUI::CoreWidgetAction(widget, tr("Moving Average"), coreUI::CoreTitleBar::Right);
	movingAvgSpinAction->setDefaultWidget(movingAvgBox.first);
	widget->addAction(movingAvgSpinAction);*/

	coreUI::CoreAction * removeSerieAction = new coreUI::CoreAction(tr("Operations"), QIcon(":/newChart/icons/charts.png"), tr("Remove serie"), widget, coreUI::CoreTitleBar::Left);
	removeSerieAction->setCheckable(false);
	connect(removeSerieAction, SIGNAL(triggered()), this, SLOT(removeCurrentSerie()));
	widget->addAction(removeSerieAction);
   
	coreUI::CoreAction * addToHierarchy = new coreUI::CoreAction(tr("Operations"), QIcon(":/newChart/icons/charts.png"), tr("Add to hierarchy"), widget, coreUI::CoreTitleBar::Left);
	addToHierarchy->setCheckable(false);
	connect(addToHierarchy, SIGNAL(triggered()), this, SLOT(addToHierarchy()));
	widget->addAction(addToHierarchy);

    return widget; 
}

void NewChartVisualizer::getSupportedTypes(utils::TypeInfoList & supportedTypes) const
{
	supportedTypes.push_back(typeid(c3dlib::ScalarChannelReaderInterface));
	supportedTypes.push_back(typeid(ScalarStream));
}

plugin::IVisualizer::ISerie * NewChartVisualizer::createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr& data)
{
	plugin::IVisualizer::ISerie* ret = nullptr;
	if (requestedType == typeid(ScalarStream)) {
		auto streamSerie = new NewChartStreamSerie(this);
		streamSerie->setData(requestedType, data);
		series.push_back(streamSerie);
		ret = streamSerie;
	}
	else if (requestedType == typeid(c3dlib::ScalarChannelReaderInterface)) {
		auto chartSerie = new NewChartSerie(this);

		auto name = "Serie " + boost::lexical_cast<std::string>(series.size());
		chartSerie->setName( name);
		chartSerie->setData(requestedType, data);
		series.push_back(chartSerie);
		auto statsEntry = statsTable->addEntry(QString("Whole chart"), QString(name.c_str()), chartSerie->getReader());

		chartSerie->setStatsEntry(statsEntry);
		
		//NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(ret->getCurve()));
		NewChartLegendItem * legendLabel = getLegendLabel(chartSerie->getCurve());
		if(legendLabel != nullptr){
			std::string source;
			data->getMetadata("core/source", source);
			legendLabel->setToolTip(source.c_str());
			if(series.size() == 1){
				if(legendLabel->isItemActive() == false){
					legend->blockSignals(true);
					legendLabel->setItemActive(true);
					legendLabel->setItemVisibleEnabled(false);
					legend->blockSignals(false);
				}
			}
		}

		adjustOffsetStep(shiftSpinX, QwtPlot::xBottom);
		adjustOffsetStep(shiftSpinY, QwtPlot::yLeft);
		ret = chartSerie;
	}
	UTILS_ASSERT(ret);
	return ret;
}

plugin::IVisualizer::ISerie * NewChartVisualizer::createSerie( const plugin::IVisualizer::ISerie * serie )
{
    return nullptr;
}

plugin::IVisualizer::ISerie *NewChartVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie,
	const utils::TypeInfo & requestedType, const core::VariantConstPtr& data)
{
	return nullptr;
}

void NewChartVisualizer::addPlotCurve( QwtPlotCurve* curve, const Scales& scales)
{
    plotScales.merge(scales);
    if (!customScale) {
    	qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
    	qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
    }
    curve->attach(qwtPlot);
}

void NewChartVisualizer::removeSerie( plugin::IVisualizer::ISerie *serie )
{
    INewChartSeriePrivate* chSerie = dynamic_cast<INewChartSeriePrivate*>(serie);

    if(chSerie == nullptr){
        throw loglib::runtime_error("Serie type not generated by this Visualizer!");
    }

    auto it = std::find(series.begin(), series.end(), chSerie);
    if(it == series.end()){
        throw loglib::runtime_error("Given serie dos not belong to this visualizer!");
    }

	(*it)->removeItemsFromPlot();
	qwtPlot->replot();

	auto mapIt = additionalCurve2Origin.find(*it);
	if (mapIt != additionalCurve2Origin.end()) {
		additionalCurve2Origin.erase(mapIt);
	}

    series.erase(it);
	setActiveSerie(-1);
}

void NewChartVisualizer::setActiveSerie(plugin::IVisualizer::ISerie * serie)
{
	if(serie == nullptr){
		setActiveSerie(-1);
	}else{
		for(int i = 0; i < series.size(); ++i){
			if(series[i]->asISerie() == serie){
				setActiveSerie(i);
			}
		}
	}
}

const plugin::IVisualizer::ISerie * NewChartVisualizer::getActiveSerie() const
{
	for(int i = 0; i < series.size(); ++i){
		if(series[i]->isActive() == true){
			return series[i]->asISerie();
		}
	}

	return nullptr;
}

plugin::IVisualizer::ISerie * NewChartVisualizer::getActiveSerie()
{
	for(int i = 0; i < series.size(); ++i){
		if(series[i]->isActive() == true){
			return series[i]->asISerie();
		}
	}

	return nullptr;
}

void NewChartVisualizer::setActiveSerie( int idx )
{
    bool layersRefreshRequired = false;

    //ostatnia zmieniamy na nieaktywna
    if(currentSerie > -1 && currentSerie < series.size()){
        INewChartSeriePrivate* serie = series[currentSerie];
        serie->setActive(false);

        auto helper = serie->getEventsHelper();
        if (helper && helper->getEventsItem()) {
            //helper->getEventsItem()->detach();
             helper->getEventsItem()->setVisible(false);
        }

        //odznacz w legendzie
        NewChartLegendItem * legendLabel = getLegendLabel(serie->getCurve());
        if(legendLabel != nullptr && legendLabel->isItemActive() == true){
            legendLabel->blockSignals(true);
            legendLabel->setItemActive(false);
            legendLabel->blockSignals(false);
        }

        currentSerie = -1;
        layersRefreshRequired = true;
        picker->setCurrentCurve(nullptr);
    }

    if(idx > -1){
        currentSerie = idx;
        INewChartSeriePrivate* serie = series[currentSerie];
        serie->setActive(true);
        picker->setCurrentCurve(serie->getCurve());

        refreshSpinBoxes();

        auto helper = serie->getEventsHelper();
        if (helper && helper->getEventsItem()) {
			helper->getEventsItem()->setVisible(context != c3dlib::C3DEventsCollection::IEvent::General);
        }

        //zaznacz w legendzie
        NewChartLegendItem * legendLabel = getLegendLabel(serie->getCurve());
        if(legendLabel != nullptr && legendLabel->isItemActive() == false){
            legendLabel->blockSignals(true);
            legendLabel->setItemActive(true);
            legendLabel->blockSignals(false);
        }
        
        layersRefreshRequired = true;
    }

    if(layersRefreshRequired == true){
        refreshSerieLayers();
    }
}

void NewChartVisualizer::refreshSerieLayers()
{
    qwtPlot->setAutoReplot(false);
    qwtPlot->blockSignals(true);
    int size = series.size();
    for(int i = 0; i < size; ++i){
        series[i]->setZ(i == currentSerie ? 1.0 : (double)i / (double)size , true);
    }

    qwtPlot->updateLayout();
    qwtPlot->blockSignals(false);
    qwtPlot->setAutoReplot(true);
    qwtPlot->replot();
}


void NewChartVisualizer::onSerieSelected(const QVariant& item, bool on, int idx)
{
    legend->blockSignals(true);
    QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(qwtPlot->infoToItem(item));
    if (!isCurveFromSerie(curve)) {
        QPen pen = curve->pen();
        QColor c = pen.color();
        c.setRed(255 - c.red());
        pen.setColor(c);
        curve->setPen(pen);
    } else {
        idx = -1; // idx nie chcemy wykorzystywać!
        if (on == true) {
        
            for (unsigned int i = 0; i < series.size(); ++i) {

                NewChartLegendItem * legendLabel = getLegendLabel(series[i]->getCurve());
                if (series[i]->getCurve() == curve) {
                    // powinno wywołać sygnał, który ustawi aktywną serię
                    setActiveSerie(i);
                    legendLabel->setItemVisible(true);
                    series[i]->setVisible(true);
                    legendLabel->setItemVisibleEnabled(false);
                }else{                
                    if(legendLabel != nullptr && legendLabel->isItemActive() == true){
                        legendLabel->setItemActive(false);
                        legendLabel->setItemVisibleEnabled(true);
                    }
                }
            }
        }else{
            //ignorujemy to - zawsze musi być jedna seria aktywna			
            NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget((const QwtPlotCurve*)curve));
            if(legendLabel != nullptr && legendLabel->isItemActive() == false){
                legendLabel->setItemActive(true);
                legendLabel->blockSignals(false);
            }
        }
    }
    legend->blockSignals(false);
}

void NewChartVisualizer::onSerieSelected(QwtPlotItem* item)
{
    QVariant info = qwtPlot->itemToInfo(item);
    onSerieSelected(info, true, -1);
}

void NewChartVisualizer::onStateAction()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (currentState) {
        currentState->stateEnd();
    }
    currentState = statesMap[action];
    currentState->stateBegin();
}

bool NewChartVisualizer::eventFilter( QObject *object, QEvent *event )
{
    if (currentState && !currentState->stateEventFilter(object, event)) {
        return QObject::eventFilter(object, event);
    }

    return true;
}

QwtPlot* NewChartVisualizer::getPlot()
{
    UTILS_ASSERT(qwtPlot);
    return qwtPlot;
}

void NewChartVisualizer::setManipulation( bool val )
{
    UTILS_ASSERT(plotPanner && plotMagnifier);
    plotPanner->setEnabled(val);
    plotMagnifier->setEnabled(val);
}
const INewChartSeriePrivate* NewChartVisualizer::tryGetCurrentSerie() const
{
    if (currentSerie >= 0 && currentSerie < static_cast<int>(series.size())) {
        return series[currentSerie];
    }
    return nullptr;
}

INewChartSeriePrivate* NewChartVisualizer::tryGetCurrentSerie()
{
    if (currentSerie >= 0 && currentSerie < static_cast<int>(series.size())) {
        return series[currentSerie];
    }
    return nullptr;
}

void NewChartVisualizer::update( double deltaTime )
{
    if (currentSerie >= 0 && currentSerie < static_cast<int>(series.size())) {
        
        qwtMarker->setVisible(true);
        NewChartSerie* serie = dynamic_cast<NewChartSerie*>(series[currentSerie]);

		// Dodawna seria nie sa podpieta pod timeline, dlatego przekazujemy jej czas z serii, na podstawie ktorej zostala stworzona
		auto it = additionalCurve2Origin.find(serie);
		if (it != additionalCurve2Origin.end()) {
			NewChartSerie* origin = dynamic_cast<NewChartSerie*>(it->second);
			if (origin) {
				serie->setTime(origin->getTime());
			}
		}
		if (serie) {
			if (currentSerieTime != serie->getTime() || currentSerieValue != serie->getCurrentValue()) {
				currentSerieTime = serie->getTime();
				currentSerieValue = serie->getCurrentValue();
				qwtMarker->setXValue(currentSerieTime * serie->getXScale() + serie->getXOffset());
				qwtMarker->setYValue(currentSerieValue * serie->getYScale() + serie->getYOffset());
			}
			auto helper = serie->getEventsHelper();
			if (isEventMode() && helper) {
				EventsHelper::SegmentConstPtr segment = helper->getSegment(currentSerieTime, this->context);
				if (segment != oldSegment) {
					recreateStats();
					setScale(this->scaleToActive, segment ? true : false);
					oldSegment = segment;
				}
        
			}
		}
    } else {
        qwtMarker->setVisible(false);
    }

    
}

void NewChartVisualizer::setShowLegend( bool val )
{
    if (qwtPlot) {
        // todo - sprawdzic wyciek...
        qwtPlot->insertLegend( val ? legend : nullptr, QwtPlot::RightLegend );
    }
    showLegend = val;
}

plugin::IVisualizer* NewChartVisualizer::create() const
{
    return new NewChartVisualizer();
}

QIcon* NewChartVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/newChart/icons/2D.png"));
}

int NewChartVisualizer::getMaxDataSeries( void ) const
{
    return -1;
}

void NewChartVisualizer::setEvents(NewChartSerie* serie, c3dlib::EventsCollectionConstPtr val)
{
	c3dlib::C3DEventsCollection::Context c = static_cast<c3dlib::C3DEventsCollection::Context>(eventsMenu->itemData(eventsMenu->currentIndex()).toInt());
	bool eventMode = (c != c3dlib::C3DEventsCollection::IEvent::General);
    EventsHelperPtr helper = serie->getEventsHelper();
    helper->getEventsItem()->attach(qwtPlot);
    helper->getEventsItem()->setVisible(serie == tryGetCurrentSerie() && eventMode);
    int no = 0;
    for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); ++segment) {
        statsTable->addEntry(tr("Left"), tr("%1: Left step %2").arg(serie->getName().c_str()).arg(++no), (*segment)->scalar, QColor(255, 200, 200));
    }
    no = 0;
    for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); ++segment) {
		statsTable->addEntry(tr("Right"), tr("%1: Right step %2").arg(serie->getName().c_str()).arg(++no), (*segment)->scalar, QColor(200, 255, 200));
    }
}

void NewChartVisualizer::onEventContext(int index)
{
	c3dlib::C3DEventsCollection::Context c = static_cast<c3dlib::C3DEventsCollection::Context>(eventsMenu->itemData(index).toInt());
	bool eventMode = (c != c3dlib::C3DEventsCollection::IEvent::General);
	NewChartSerie* serie = dynamic_cast<NewChartSerie*>(tryGetCurrentSerie());
    if (serie) {
        auto helper = serie->getEventsHelper();
        if (helper && helper->getEventsItem()) {
            helper->getEventsItem()->setVisible(eventMode);
            context = c;
            //setScale(scaleToActive, eventMode);
        }
    }
    setScale();
}

void NewChartVisualizer::recreateStats()
{
    statsTable->clear();
    for (auto it = series.begin(); it != series.end(); ++it) {
    	NewChartSerie* serie = dynamic_cast<NewChartSerie*>(*it);
    	if (serie) {
    		statsTable->addEntry(tr("Whole chart"), serie->getName().c_str(), serie->getReader() );
    	}
    }

	//TODO
    //if (stats) {
		//QString group = context == c3dlib::C3DEventsCollection::IEvent::Left ? tr("Left") : tr("Right");
		//QColor color = context == c3dlib::C3DEventsCollection::IEvent::Left ? QColor(255, 200, 200) : QColor(200, 255, 200);
        //statsTable->addEntry(group, stats->getChannel()->getName().c_str(), stats, color);
    //} else {
        for (auto it = series.begin(); it != series.end(); ++it) {

        	NewChartSerie* serie = dynamic_cast<NewChartSerie*>(*it);
        	if (serie) {
				int no = 0;
				EventsHelperPtr helper = serie->getEventsHelper();
				for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); ++segment) {
					statsTable->addEntry(tr("Left"), tr("%1: Left step %2").arg(serie->getName().c_str()).arg(++no), serie->getReader(), QColor(255, 200, 200));
				}
				no = 0;
				for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); ++segment) {
					statsTable->addEntry(tr("Right"), tr("%1: Right step %2").arg(serie->getName().c_str()).arg(++no), serie->getReader(), QColor(200, 255, 200));
				}
        	}
        }
    //}
}

void NewChartVisualizer::showStatistics( bool visible )
{
    if (visible) {
        statsTable->recalculateHeight();
    }
    statsTable->setVisible(visible);
}

void NewChartVisualizer::onSerieVisible(const QVariant& info, bool visible )
{
    legend->blockSignals(true);
    const QwtPlotCurve* curve = dynamic_cast<const QwtPlotCurve*>(qwtPlot->infoToItem(info));
    if (!isCurveFromSerie(curve)) {
        QwtPlotCurve* c = const_cast<QwtPlotCurve*>(curve);
        c->setVisible(visible);
    } else {
        for (unsigned int i = 0; i < series.size(); ++i) {
            if (series[i]->getCurve() == curve) {
                if (!series[i]->isActive()) {
                    series[i]->setVisible(visible);
                    auto chartSerie = dynamic_cast<NewChartSerie*>(series[i]);
                    if (chartSerie) {
						auto list = statsTable->getEntries(chartSerie->getReader());
						for(auto item : list) {
							item.second->setHidden(!visible);
						}
                    }
                    for (auto it = statesMap.begin(); it != statesMap.end(); ++it) {
                        NewChartLabelStatePtr labelState = utils::dynamic_pointer_cast<NewChartLabelState>(it->second);
                        if (labelState) {
                            labelState->setVisible(series[i], visible);
                        }
                    }
                } else {
                    // aktywna seria nie może być niewidoczna, trzeba zapobiegac tej sytuacji wyłączając checkBoxa
                    UTILS_ASSERT(false, "Active serie has to be visible");
                }

                break;
            }
        }
    }

    recreateScales();
	if (context == c3dlib::C3DEventsCollection::IEvent::General || !timeInsideEvent()) {
        setScale();
    }
    legend->blockSignals(false);
}

void NewChartVisualizer::recreateScales()
{
	if (customScale) {
		return;
	}
    plotScales.clear();
    for (auto it = series.begin(); it != series.end(); ++it) {
        if ((*it)->isVisible()) {
            plotScales.merge((*it)->getScales());
        }
    }
    UTILS_ASSERT(plotScales.isInitialized());

}

bool NewChartVisualizer::timeInsideEvent()
{
    NewChartSerie* serie = dynamic_cast<NewChartSerie*>(series[currentSerie]);

    auto helper = serie->getEventsHelper();
    if (helper) {
        if (helper->getSegment(serie->getTime(), this->context)) {
            return true;
        }
    }

    return false;
}

void NewChartVisualizer::scaleToActiveSerie( bool scaleToActive)
{
	if (customScale) {
			return;
		}
    this->scaleToActive = scaleToActive;
	setScale(scaleToActive, context != c3dlib::C3DEventsCollection::IEvent::General);
}

void NewChartVisualizer::setScale( bool scaleToActive, bool eventMode )
{
	if (customScale) {
			return;
		}
    UTILS_ASSERT(percentDraw);
   
    if (eventMode) {
        NewChartSerie* serie = dynamic_cast<NewChartSerie*>(series[currentSerie]);
		if (serie) { 
			float x = serie->getTime();
			EventsHelperPtr h = serie->getEventsHelper();
			if (h) {
				EventsHelper::SegmentConstPtr segment = h->getSegment(x, this->context);
				if (segment) {
					if (scaleToActive) {
						percentDraw->setPercentMode(true);
						percentDraw->setLeftRightValues(segment->begin, segment->end);

						auto bvf = segment->scalar->getOrCreateFeature<dataaccessor::BoundedValuesFeature>();

						qwtPlot->setAxisScale(QwtPlot::yLeft, bvf->minValue(), bvf->maxValue());
						qwtPlot->setAxisScaleDiv(QwtPlot::xBottom, percentDraw->getScaleDiv());
					} else {
						auto bvf = segment->scalar->getOrCreateFeature<dataaccessor::BoundedValuesFeature>();
						float minY = bvf->minValue();
						float maxY = bvf->maxValue();
						for (auto it = series.begin(); it != series.end(); ++it) {
							if ((*it)->isVisible()) {
								auto h = (*it)->getEventsHelper();
								if (h) {
									EventsHelper::SegmentConstPtr s = h->getSegment(x, this->context);
									if (s) {
										auto bvf = s->scalar->getOrCreateFeature<dataaccessor::BoundedValuesFeature>();
										minY = (std::min)(minY, bvf->minValue());
										maxY = (std::max)(maxY, bvf->maxValue());
									}
								}
							}
						}

						percentDraw->setLeftRightValues(segment->begin, segment->end);
						percentDraw->setPercentMode(true);
						qwtPlot->setAxisScale(QwtPlot::yLeft, minY, maxY);
						qwtPlot->setAxisScaleDiv(QwtPlot::xBottom, percentDraw->getScaleDiv());
					}
				} else {
					setGlobalScales(scaleToActive);
				}
			}
		}
    } else {
        setGlobalScales(scaleToActive);

    }

    adjustOffsetStep(shiftSpinX, QwtPlot::xBottom);
    adjustOffsetStep(shiftSpinY, QwtPlot::yLeft);
}

void NewChartVisualizer::setScale()
{
	if (customScale) {
			return;
		}
	INewChartSeriePrivate* serie = tryGetCurrentSerie();
    if (serie) {
		auto timeSerie = dynamic_cast<NewChartSerie*>(serie);
       if (timeSerie && isEventMode()) {
		   float x = timeSerie->getTime();
		   auto helper = timeSerie->getEventsHelper();
           if (helper) {
               EventsHelper::SegmentConstPtr segment = helper->getSegment(x, this->context);
               setScale(this->scaleToActive, segment ? true : false);
           } else {
               setGlobalScales(scaleToActive);
           }
       } else {
           setGlobalScales(scaleToActive);
       }
    }    
}

void NewChartVisualizer::setAxisScale(const Axis axis, const double min, const double max, unsigned int steps)
{
	customScale = true;
	qwtPlot->setAxisScale(axis == INewChartVisualizer::AxisY ? QwtPlot::yLeft : QwtPlot::xBottom, min, max);
}

void NewChartVisualizer::setGlobalScales(bool scaleToActive)
{
	if (customScale) {
			return;
		}
    UTILS_ASSERT(percentDraw);
    INewChartSeriePrivate* serie = tryGetCurrentSerie();
    if (serie) {
        percentDraw->setPercentMode(false);
        if (scaleToActive) {
            Scales s = serie->getScales();
            qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
            qwtPlot->setAxisScale(QwtPlot::yLeft, s.getYMin(), s.getYMax());
        } else {
            qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
            qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
        }
    }
}

void NewChartVisualizer::onShiftX( double d )
{
    INewChartSeriePrivate* s = tryGetCurrentSerie();
    if (s) {
        s->setXOffset(d);
        qwtPlot->replot();
    }
}

void NewChartVisualizer::onShiftY( double d )
{
	INewChartSeriePrivate* s = tryGetCurrentSerie();
    if (s) {
        s->setYOffset(d);
        qwtPlot->replot();
    }
}

void NewChartVisualizer::onScaleX( double d )
{ 
	INewChartSeriePrivate* s = tryGetCurrentSerie();
    if (s && d != 0.0) {
       
        s->setXScale(d);
        qwtPlot->replot();
    }
}

void NewChartVisualizer::onScaleY( double d )
{
	INewChartSeriePrivate* s = tryGetCurrentSerie();
    if (s && d != 0.0) {
        s->setYScale(d);
        qwtPlot->replot();
    }
}

void NewChartVisualizer::refreshSpinBoxes()
{
	INewChartSeriePrivate* serie = tryGetCurrentSerie();
    if (serie) {
        shiftSpinX->blockSignals(true);
        shiftSpinX->setValue(serie->getXOffset());
        shiftSpinX->blockSignals(false);

        shiftSpinY->blockSignals(true);
        shiftSpinY->setValue(serie->getYOffset());
        shiftSpinY->blockSignals(false);

        scaleSpinX->blockSignals(true);
        scaleSpinX->setValue(serie->getXScale());
        scaleSpinX->blockSignals(false);

        scaleSpinY->blockSignals(true);
        scaleSpinY->setValue(serie->getYScale());
        scaleSpinY->blockSignals(false);
    }
}

void NewChartVisualizer::adjustOffsetStep( QDoubleSpinBox* spinBox, QwtPlot::Axis axis)
{
    double upper = qwtPlot->axisScaleDiv(axis).upperBound();
    double lower = qwtPlot->axisScaleDiv(axis).lowerBound();
    if (lower < upper) {
        spinBox->setSingleStep((upper - lower) / 50.0);
    }

}



void NewChartVisualizer::removeCurrentSerie()
{
	auto serie = tryGetCurrentSerie();
	if (serie) {
		removeSerie(dynamic_cast<plugin::IVisualizer::ISerie*>(serie));
	}
}

bool NewChartVisualizer::isCurveFromSerie(const QwtPlotCurve* curve) const
{
    for (auto it = series.begin(); it != series.end(); ++it) {
        if ((*it)->getCurve() == curve) {
            return true;
        }
    }

    return false;
}

void NewChartVisualizer::setLabelsVisible( bool val )
{
    for (auto serie = series.begin(); serie != series.end(); ++serie) {
        for (auto it = statesMap.begin(); it != statesMap.end(); ++it) {
            NewChartLabelStatePtr labels = utils::dynamic_pointer_cast<NewChartLabelState>(it->second);
            if (labels) {
                labels->setVisible(*serie, val && (*serie)->isVisible());
            }
        }
    }
}



boost::iterator_range<std::vector<INewChartSeriePrivate*>::const_iterator> NewChartVisualizer::getSeries() const
{
    return boost::make_iterator_range(series.cbegin(), series.cend());
}

QPixmap NewChartVisualizer::takeScreenshot() const 
{
    return QPixmap::grabWidget(qwtPlot);
}

NewChartLegendItem* NewChartVisualizer::getLegendLabel( QwtPlotCurve* curve )
{
    QVariant itemInfo = qwtPlot->itemToInfo(curve);
    QWidgetList items = legend->legendWidgets(itemInfo);
    for (auto it = items.begin(); it != items.end(); ++it) {
        NewChartLegendItem* itm = qobject_cast<NewChartLegendItem *>(*it);
        if (itm) {
            return itm;
        }
    }
    UTILS_ASSERT(false);
    return nullptr;
}

void NewChartVisualizer::initHierarchyProvider(core::IHierarchyProviderProxyPtr proxy)
{
	this->proxy = proxy;
}

void NewChartVisualizer::disconnectedFromHierarchy()
{
	this->proxy = core::IHierarchyProviderProxyPtr();
}

void NewChartVisualizer::addToHierarchy()
{
	auto serie = tryGetCurrentSerie();
	if (serie && proxy) {
		auto data = serie->asISerie()->getData();
		core::HierarchyHelperPtr helper = utils::make_shared<NewChartItemHelper>(data);

		auto dataItem = core::HierarchyItemPtr(new core::HierarchyDataItem(
			data, QIcon(), QString::fromStdString(serie->asISerie()->getName()), QString("Generated in chart visualizer"), helper));

		proxy->appendToHierarchy(dataItem);
	}
}

INewChartSeriePrivate* NewChartVisualizer::generateCurve(const INewChartCurveGenerator* generator, const INewChartCurveGenerator::Params& params)
{
	auto origin = tryGetCurrentOriginCurve();

	if (origin) {
		c3dlib::ScalarChannelReaderInterfaceConstPtr data;
		origin->asISerie()->getData()->tryGet(data);
		//auto timeWindow = this->movingAvgSpin->value();
		auto generated = generator->generate(data, params);
		auto wrapper = core::Variant::wrap <c3dlib::ScalarChannelReaderInterface>(generated.first);
		auto cserie = createSerie(typeid(c3dlib::ScalarChannelReaderInterface), wrapper);
		cserie->setName(generated.second);
		setActiveSerie(cserie);
		auto serie = dynamic_cast<INewChartSeriePrivate*>(cserie);
		additionalCurve2Origin[serie] = origin;

		return serie;
	}

	return nullptr;
}

INewChartSeriePrivate* NewChartVisualizer::tryGetCurrentOriginCurve()
{
	INewChartSeriePrivate* origin = tryGetCurrentSerie();
	auto it = additionalCurve2Origin.find(origin);
	if (it != additionalCurve2Origin.end()) {
		origin = it->second;
	}
	return origin;
}

GeneratorHelper::GeneratorHelper(NewChartVisualizer* v) : visualizer(v)
{

}

QList<QAction*> GeneratorHelper::getActions()
{
	QList<QAction*> actions;
	for (auto& generator : INewChartVisualizer::generators) {
		auto section = QString::fromStdString(generator->getName());
		coreUI::CoreAction * generatorAction = new coreUI::CoreAction(section, QIcon(":/newChart/icons/charts.png"), tr("Generator"), nullptr, coreUI::CoreTitleBar::Left);

		auto fun1 = [=]() {
			auto params = getCurrentParams(generator);
			INewChartSeriePrivate* serie = visualizer->generateCurve(generator.get(), params);
			if (serie) {
				generator2Generated[generator].insert(serie);
			}
		};
		void (QAction::* triggerPtr)(bool) = &QAction::triggered;
		connect(generatorAction, triggerPtr, this, fun1);
		actions.push_back(generatorAction);
		for (auto& arg : generator->getDefaultParams()) {
			// TODO : widget generowany na podstawie QVariant posiadajacy jeden sygnal informujacy o zmianie stanu
			auto tupl = LabeledEditor::create(QString::fromStdString(arg.name), arg.value);
			QWidget* editorWidget = std::get<2>(tupl);
			LabeledEditor* editor = std::get<3>(tupl);
			auto generatorP = [=]() -> INewChartCurveGenerator::Param {
				return INewChartCurveGenerator::Param(arg.name, editor->getValue());
			};
			this->generatorParameterCreator[generator].push_back(generatorP);

			auto updater = [=](){ 
				auto it = visualizer->additionalCurve2Origin.find(visualizer->tryGetCurrentSerie());
				if (it != visualizer->additionalCurve2Origin.end()) {
					c3dlib::ScalarChannelReaderInterfaceConstPtr data;
					it->second->asISerie()->getData()->tryGet(data);
					auto generated = generator->generate(data, getCurrentParams(generator));
					auto newChannel = generated.first;
					INewChartSeriePrivate* mav = it->first;
					auto wrapper = core::Variant::wrap(utils::const_pointer_cast<c3dlib::ScalarChannelReaderInterface>(newChannel));
					std::string newName = generated.second;
					mav->asISerie()->setName(newName);
					mav->updateData(wrapper);
				}
			};
			void (LabeledEditor::* valueChangePtr)() = &LabeledEditor::valueChanged;
			connect(editor, valueChangePtr, this, updater);
			
			coreUI::CoreWidgetAction * argumentWidget = new coreUI::CoreWidgetAction(nullptr, section, coreUI::CoreTitleBar::Right);
			argumentWidget->setDefaultWidget(std::get<0>(tupl));
			actions.push_back(argumentWidget);
		}
	}
	return actions;
}



INewChartCurveGenerator::Params GeneratorHelper::getCurrentParams(INewChartCurveGeneratorConstPtr g)
{
	auto it = generatorParameterCreator.find(g);
	if (it != generatorParameterCreator.end()) {
		INewChartCurveGenerator::Params params;
		for (auto fun : it->second) {
			params.push_back(fun());
		}

		return params;
	}

	throw std::logic_error("generator was not registered");
}
