#include "NewChartPCH.h"
#include "NewChartState.h"
#include "NewChartVisualizer.h"

NewChartState::NewChartState( NewChartVisualizer* visualizer ) : 
QObject((QObject*)visualizer),
    visualizer(visualizer),
    plot(visualizer->getPlot())
{
    UTILS_ASSERT(visualizer && plot && plot->canvas());
    canvas = qobject_cast<QwtPlotCanvas *>(plot->canvas());
}
