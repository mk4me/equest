#include "NewChartPCH.h"
#include "NewChartVerticals.h"
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"
#include "NewChartDotFloating.h"

static const double MIN_DIST = 20.0;

NewChartVerticals::NewChartVerticals(NewChartVisualizer* visualizer, NewChartVerticalsConnection::ConnectionStyle style) :
    NewChartLabelState(visualizer),
    labelMarker(new NewChartLabel()),
    style(style)
{
	UTILS_ASSERT(style == NewChartVerticalsConnection::Horizontal || style == NewChartVerticalsConnection::Vertical);
    labelMarker->attach(plot);
	labelMarker->getConnectionsItem()->attach(plot);
    labelMarker->setVisible(false);
    marker.setLerpRatios(1.0f);
}

bool NewChartVerticals::stateEventFilter( QObject *object, QEvent *event )
{
    if (object != canvas) {
        return false;
    }
    
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() == Qt::LeftButton) {
            currentLabel = getLabel(mouseEvent->pos());
			if (currentLabel && currentLabel->label->isInsideClose(mouseEvent->pos())) {
				removeLabel(utils::const_pointer_cast<LabelData>(currentLabel));
				currentLabel = LabelDataConstPtr();
				return true;
			}
        } else {
            currentLabel = LabelDataConstPtr();
        }

        if (!currentLabel && mouseEvent->button() == Qt::LeftButton) {
            SeriePointDist cpd = getClosestPoint(mouseEvent->pos());
            if (point1) {
                insertNewMarker(point1->second, cpd.get<1>(), cpd.get<0>(), cpd.get<0>()->getColor());
                point1.reset();
            } else {
                if (cpd.get<2>() < MIN_DIST) {
					point1.reset(new std::pair<const INewChartSeriePrivate*, QPointF>(cpd.get<0>(), cpd.get<1>()));
                }
            }
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        currentLabel = LabelDataConstPtr();
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;

        if (currentLabel) {
            labelMarker->setVisible(false);
            move(mouseEvent->pos(), currentLabel->serie->getCurve(), currentLabel->label);
            currentLabel->label->itemChanged();
        } else {
            currentLabel = LabelDataConstPtr();
            SeriePointDist cpd = getClosestPoint(mouseEvent->pos());
            if (!point1 && cpd.get<2>() < MIN_DIST) {
                marker.setVisible(true);
                marker.setValue(cpd.get<1>());
            } else {  
                marker.setVisible(false);
            }
            if ( point1) {
				QString serieName = (style == NewChartVerticalsConnection::Horizontal) ? "t" : point1->first->asISerie()->getName().c_str();
				double diff = (style == NewChartVerticalsConnection::Horizontal) ? (cpd.get<1>().x() - point1->second.x()) : (cpd.get<1>().y() - point1->second.y());
                labelMarker->setText(QString("%1%2: %3").arg(QChar(0x394)).arg(serieName).arg(diff));
                labelMarker->connectDots(cpd.get<1>(), point1->second, style);
                labelMarker->setVisible(true);
                labelMarker->itemChanged();
            } else {
                labelMarker->setVisible(false);
            }
        }
    } else  if (event->type() == QEvent::Paint) {
        updateLabels();
    }

    return QObject::eventFilter( object, event );
}

void NewChartVerticals::stateBegin()
{
    NewChartLabelState::stateBegin();
    visualizer->setManipulation(false);
    canvas->setCursor(Qt::DragLinkCursor);
    labelMarker->attach(plot);
    marker.attach(plot);
}

void NewChartVerticals::stateEnd()
{
    labelMarker->detach();
	labelMarker->getConnectionsItem()->detach();
    marker.detach();
}

void NewChartVerticals::insertNewMarker(const QPointF& point1, const QPointF& point2, const INewChartSeriePrivate* currentSerie, const QColor& color)
{
    UTILS_ASSERT(currentSerie);
    NewChartDotPtr dot1(new NewChartDotFloating(point1, currentSerie));
    NewChartDotPtr dot2(new NewChartDotFloating(point2, currentSerie));
	double delta = (style == NewChartVerticalsConnection::Vertical) ? (point2.y() - point1.y()) : (point2.x() - point1.x());
	QString serieName = (style == NewChartVerticalsConnection::Horizontal) ? "t" : currentSerie->asISerie()->getName().c_str();
    NewChartLabelPtr label(new NewChartLabel( QString("%1%2: %3").arg(QChar(0x394)).arg(serieName).arg(delta)));
    label->setPen(QPen(color));
    dot1->attach(plot);
    dot2->attach(plot);
    label->attach(plot);
	label->getConnectionsItem()->attach(plot);
    label->connectDots(dot2, dot1, style);

    LabelDataPtr data(new LabelData);
    data->dot1 = dot1;
    data->dot2 = dot2;
    data->label = label;
    data->serie = currentSerie;
    labels.push_back(data);
}

void NewChartVerticals::updateLabels()
{
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        QPointF p1 = (*it)->dot1->getPosition();
        QPointF p2 = (*it)->dot2->getPosition();
		double delta = (style == NewChartVerticalsConnection::Vertical) ? (p2.y() - p1.y()) : (p2.x() - p1.x());
		QString serieName = (style == NewChartVerticalsConnection::Horizontal) ? "t" : (*it)->serie->asISerie()->getName().c_str();
        (*it)->label->setText(QString("%1%2: %3").arg(QChar(0x394)).arg(serieName).arg(delta));
    }
}


