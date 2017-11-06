#include "NewChartPCH.h"
#include "NewChartValueMarker.h"
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"
#include "NewChartLabel.h"
#include "NewChartDot.h"
#include "NewChartDotFloating.h"

static const double MIN_DIST = 20.0;

NewChartValueMarker::NewChartValueMarker( NewChartVisualizer* visualizer ) : 
    NewChartLabelState(visualizer)
{
    marker.setLerpRatios(1.0f);
}

bool NewChartValueMarker::stateEventFilter( QObject *object, QEvent *event )
{
    const INewChartSeriePrivate* currentSerie = visualizer->tryGetCurrentSerie();
    if (object != canvas || !currentSerie ) {
        return false;
    }

    QEvent::Type eventType = event->type();

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() == Qt::LeftButton) {
            currentLabel = getLabel(mouseEvent->pos(), currentSerie->getCurve());
			if (currentLabel && currentLabel->label->isInsideClose(mouseEvent->pos())) {
				removeLabel(utils::const_pointer_cast<LabelData>(currentLabel));
				currentLabel = LabelDataConstPtr();
				return true;
			}
        } else {
            currentLabel = LabelDataConstPtr();
        }
        
        if (!currentLabel && mouseEvent->button() == Qt::LeftButton) {
            SeriePointDist closest = getClosestPoint(mouseEvent->pos());
            if (closest.get<2>() < MIN_DIST) {
                insertNewMarker(closest.get<1>(), closest.get<0>()->getColor());
            }
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        currentLabel = LabelDataConstPtr();
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;
        
        if (currentLabel) {
            marker.setVisible(false);
            move(mouseEvent->pos(), currentSerie->getCurve(), currentLabel->label);
            currentLabel->label->itemChanged();
        } else {
            currentLabel = LabelDataConstPtr();
            SeriePointDist spd = getClosestPoint(mouseEvent->pos());
            if (spd.get<2>() < MIN_DIST) {
                marker.setValue(spd.get<1>());
                marker.setVisible(true);
            } else {
                marker.setVisible(false);
            }
        }
    } else if (event->type() == QEvent::Paint) {
        updateLabels();
        
    }

    return QObject::eventFilter( object, event );
}

void NewChartValueMarker::stateBegin()
{
    NewChartLabelState::stateBegin();
    canvas->setCursor(Qt::ArrowCursor);
    visualizer->setManipulation(false);
    marker.attach(plot);
}

void NewChartValueMarker::stateEnd()
{
    marker.detach();
}

void NewChartValueMarker::insertNewMarker( const QPointF& point, const QColor& color )
{
	const INewChartSeriePrivate* serie = visualizer->tryGetCurrentSerie();
    NewChartDot* dot = (new NewChartDotFloating(point, serie));
    NewChartLabel* label = (new NewChartLabel(QObject::tr("Time: %1\nValue: %2").arg(point.x()).arg(point.y())));
    label->setPen(QPen(color));
    dot->attach(plot);
    label->attach(plot);
	label->getConnectionsItem()->attach(plot);
    label->connectDot(dot);

    LabelDataPtr data(new LabelData());
    data->dot1 = dot;
    data->label = label;
    data->serie = serie;

    labels.push_back(data);
}

void NewChartValueMarker::updateLabels()
{
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        const QPointF& p = (*it)->dot1->getPosition();
		QString text1 = QObject::tr("Time: %1").arg(p.x());
		QString text2 = QObject::tr("Value: %1").arg(p.y());
		auto text = text1 + "\n" + text2;
        (*it)->label->setText(text);
    }
}


