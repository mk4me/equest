#include "DicomPCH.h"
#include "AbstractEditState.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsRectItem>
#include <QtWidgets/QGraphicsLineItem>
#include <QtWidgets/QMenu>
#include <coreui/AbstractStateMachine.h>
#include <coreui/MoveCommand.h>
#include <coreui/MultiCommand.h>
#include "LayeredStateMachine.h"
#include "BackgroundLayer.h"
#include "LayeredSerie.h"
#include "PointsLayer.h" 
#include <plugins/dicom/Annotations.h>


using namespace dicom;



QString dicom::AddPointCommand::name()
{
    return QString(typeid(this).name());
}

void dicom::AddPointCommand::undoIt()
{
    auto point = layer->removePoint(idx);
    removedPoint = std::unique_ptr<QGraphicsItem>(point);
    removedPoint->setVisible(false);
    layer->refresh();
}

void dicom::AddPointCommand::doIt()
{
    if (removedPoint) {
        removedPoint->setVisible(true);
        layer->addPoint(removedPoint.release());
        removedPoint = std::unique_ptr<QGraphicsItem>();
    } else {
        idx = bestFit ? getBestIdx() : layer->getNumPoint();
        layer->addPoint(newP, idx);
    }

    layer->refresh();
}

dicom::AddPointCommand::AddPointCommand( PointsLayerPtr layer, const QPointF& newP, bool bestFit ) : 
layer(layer),
    newP(newP),
    idx(-1),
    removedPoint(nullptr),
    bestFit(bestFit)
{

}

float magnitude(const QPointF &l1,  const QPointF &l2)
{
    QPointF ofs = l2 - l1;
    return sqrt(ofs.x() * ofs.x() + ofs.y() * ofs.y());
}


float distancePointLine( const QPointF &p, const QPointF &l1,  const QPointF &l2)
{
    float lineMag;
    float u;
    QPoint intersectPnt;

    lineMag = magnitude( l2, l1 );

    u = ( ( ( p.x() - l1.x() ) * ( l2.x() - l1.x() ) ) +
        ( ( p.y() - l1.y() ) * ( l2.y() - l1.y() ) ) ) /
        ( lineMag * lineMag );

    if ( u < 0.0f) {
        return magnitude(p, l1);
    }
    if ( u > 1.0f ) {
        return magnitude(p, l2);
    }

    intersectPnt.setX( int( l1.x() + u * ( l2.x() - l1.x() ) ) );
    intersectPnt.setY( int( l1.y() + u * ( l2.y() - l1.y() ) ) );

    return magnitude( p, intersectPnt );
}
float getDistance2(const QPointF& p, const QPointF& newP)
{
    QPointF ofs = p - newP;
    return ofs.x() * ofs.x() + ofs.y() * ofs.y();
}
int getBestFitIdx(const PointsLayer* layer, const QPointF& newP)
{
	int count = layer->getNumPoint();
	if (count < 2) {
		return count;
	}
	float minDist2 = std::numeric_limits<float>::max();
	int minIdx = -1;
	for (int i = 0; i < count - 1; ++i) {

		float dist2 = distancePointLine(newP, layer->getPoint(i)->pos(), layer->getPoint(i + 1)->pos());
		if (dist2 < minDist2) {
			minDist2 = dist2;
			minIdx = i + 1;
		}
	}
	float endD = sqrt(getDistance2(layer->getPoint(count - 1)->pos(), newP));
	float startD = sqrt(getDistance2(layer->getPoint(0)->pos(), newP));
	if (endD - minDist2 < 0.001f) {
		minIdx = count;
		minDist2 = endD;
	}

	if (startD - minDist2 < 0.001f) {
		minIdx = 0;
	}
	return minIdx;
}

int dicom::AddPointCommand::getBestIdx()
{
	return getBestFitIdx(layer.get(), newP);
}



//////////////////////////////////////////////////////////////////////////

QString dicom::RemovePointCommand::name()
{
    return QString(typeid(this).name());
}

void dicom::RemovePointCommand::undoIt()
{
    removedPoint->setVisible(true);
    layer->addPoint(removedPoint.release(), idx);
}

void dicom::RemovePointCommand::doIt()
{
    auto point = layer->removePoint(idx);
    removedPoint = std::unique_ptr<QGraphicsItem>(point);
    removedPoint->setVisible(false);
    layer->refresh();
}

dicom::RemovePointCommand::RemovePointCommand( PointsLayerPtr layer, int idx ) : 
    layer(layer),
    idx(idx),
    removedPoint(nullptr)
{

}

void dicom::MovePathCommand::undoIt()
{
    MoveCommand::undoIt();
    layer->refresh();
}

void dicom::MovePathCommand::doIt()
{
    MoveCommand::doIt();
    layer->refresh();
}

dicom::MovePathCommand::MovePathCommand( PointsLayerPtr layer, QGraphicsItem* item, const QPointF& newP, const QPointF& oldP ) : 
MoveCommand(item, newP, oldP),
    layer(layer)
{

}


dicom::AbstractEditState::AbstractEditState(LayeredStateMachine* machine) :
coreUI::AbstractState(machine),
machine(machine),
possibleMove(false)
{

}


bool dicom::AbstractEditState::keyReleaseEvent(QKeyEvent *event)
{
	return true;
}


bool dicom::AbstractEditState::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
	auto numPoints = layer->getNumPoint();
	if (numPoints > 2) {
		auto mpos = e->scenePos();
		int idx = getBestFitIdx(layer.get(), mpos) % numPoints;
		int idx2 = (idx - 1) % numPoints; 
		idx2 = idx2 < 0 ? numPoints - 1 : idx2;
		QGraphicsItem* p = layer->getPoint(idx);
		line[0].setLine(p->scenePos().x(), p->scenePos().y(), mpos.x(), mpos.y());

		QGraphicsItem* p2 = layer->getPoint(idx2);
		line[1].setLine(p2->scenePos().x(), p2->scenePos().y(), mpos.x(), mpos.y());
	}
	if (possibleMove) {
		layer->refresh();
	}
	return false;
}


bool dicom::AbstractEditState::focusOutEvent( QFocusEvent * event )
{
    if (!rightClickMenu.lock()) {
        machine->setState(machine->getNormalState());
        return true;
    }
    return false;
}

void dicom::AbstractEditState::begin(coreUI::AbstractStateConstPtr lastState)
{
	//machine->getGraphicsScene()->addItem(&line[0]);
	//machine->getGraphicsScene()->addItem(&line[1]);
	line[0].setPen(QPen(Qt::gray, 1, Qt::SolidLine));
	line[1].setPen(QPen(Qt::gray, 1, Qt::SolidLine));
	line[0].setZValue(2.0);
	line[1].setZValue(2.0);
}

void dicom::AbstractEditState::end()
{
	machine->getGraphicsScene()->removeItem(&line[0]);
	machine->getGraphicsScene()->removeItem(&line[1]);
}

QGraphicsItem* dicom::AbstractEditState::extractItem(QGraphicsSceneMouseEvent* e)
{
	auto items = machine->getGraphicsScene()->items(e->scenePos());

	for (auto& i : items) {
		if (i != &line[0] && i != &line[1]) {
			return i;
		}
	}


	return nullptr;
}

