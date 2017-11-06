#include "NewChartPCH.h"
#include "NewChartLabelState.h"
#include "NewChartVisualizer.h"
#include "NewChartLabel.h"
#include "NewChartDot.h"
#include "INewChartSeriePrivate.h"

NewChartLabelState::NewChartLabelState(NewChartVisualizer* visualizer) :
    NewChartState(visualizer)
{
}

void NewChartLabelState::move( const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label )
{
	//label->setShift(pos - label->getPoint1Transformed(curve));
	auto br = label->boundingRect();
	label->setShift(pos - label->getPoint1Transformed(curve) - QPoint(br.width(), br.height()));
}

NewChartLabelState::LabelDataConstPtr NewChartLabelState::getLabel( const QPoint& pos, const QwtPlotCurve* curve )
{
    const QwtPlotItemList& itmList = plot->itemList();
    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it ) {
        NewChartLabel* label = dynamic_cast<NewChartLabel*>(*it);
        if (label && label->getPoint1() && label->isInsideLabel(pos, curve)) {            
            LabelDataConstPtr data = getLabel(label);
            if (data) {
                return data;
            }
        }
    }

    return LabelDataConstPtr();
}

NewChartLabelState::LabelDataConstPtr NewChartLabelState::getLabel( const QPoint& pos )
{
    auto range = visualizer->getSeries();
    for (auto it = range.begin(); it != range.end(); it++) {
        LabelDataConstPtr l = getLabel(pos, (*it)->getCurve());
        if (l) {
            return l;
        }
    }

    return LabelDataConstPtr();
}

NewChartLabelState::LabelDataConstPtr NewChartLabelState::getLabel( const NewChartLabel* label ) const
{
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        if ((*it)->label == label) {
            return *it;
        }
    }

    return LabelDataConstPtr();
}

NewChartLabelState::SeriePointDist NewChartLabelState::getClosestPoint(const QPoint& pos) const
{
    double min = (std::numeric_limits<double>::max)();
    const INewChartSeriePrivate* chosenSerie = nullptr;
    QPointF ret;
    auto series = visualizer->getSeries();
    for (auto serie : series) {
        double dist;
		if (serie->isVisible()) {
			const QwtPlotCurve* c = serie->getCurve();
			int pointIndex = c->closestPoint(pos, &dist);
			if (dist < min) {
				min = dist;
				chosenSerie = serie;
				ret = c->sample( pointIndex );
			}
		}
    }
    
	return boost::make_tuple(chosenSerie, ret, min);
}

void NewChartLabelState::draw( QPainter * painter)
{
    //throw std::exception("The method or operation is not implemented.");
}

void NewChartLabelState::stateBegin()
{
    canvas->setMouseTracking(true);
	canvas->setCursor(Qt::ArrowCursor);
}

void NewChartLabelState::stateEnd()
{
    //marker.detach();
}

void NewChartLabelState::removeLabel(LabelDataPtr data)
{
	if (data->dot1) {
		data->dot1->detach();
	}
	if (data->dot2) {
		data->dot2->detach();
	}
	if (data->label) {
		data->label->detach();
		data->label->getConnectionsItem()->detach();
	}
}

void NewChartLabelState::removeSerieLabels( const INewChartSeriePrivate* serie )
{
	std::for_each(labels.begin(), labels.end(), [&](LabelDataPtr& data)
	{
		removeLabel(data);
	});
    /*for (auto it = labels.begin(); it != labels.end(); ++it) {
        LabelDataPtr data = *it;
        if (data->serie == serie) {
            if (data->dot1) {
                data->dot1->detach();
            }
            if (data->dot2) {
                data->dot2->detach();
            }
            if (data->label) {
                data->label->detach();
            }
        }
    }*/
}

void NewChartLabelState::setVisible( const INewChartSeriePrivate* serie, bool visible )
{
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        LabelDataPtr data = *it;
        if (data->serie == serie) {
            if (data->dot1) {
                data->dot1->setVisible(visible);
            }
            if (data->dot2) {
                data->dot2->setVisible(visible);
            }
            if (data->label) {
                data->label->setVisible(visible);
            }
        }
    }
}


                                         
                                 
