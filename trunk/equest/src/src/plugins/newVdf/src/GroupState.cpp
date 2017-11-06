#include "NewVdfPCH.h"
#include "GroupState.h"
#include "SceneStateMachine.h"
#include "VdfScene.h"
#include <QtWidgets/QGraphicsSceneEvent>

using namespace vdf;

GroupState::GroupState( SceneStateMachine* scene ) :
    ISceneState(scene),
    tempRect(nullptr)
{
}
    
void GroupState::begin( ISceneStateConstPtr lastState )
{
	startPos = PointPtr();
    auto nodes = stateMachine->getSceneModel()->getVisualItems<IVisualNodePtr>();
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        (*it)->setSelection(false);
    }

    auto connections = stateMachine->getSceneModel()->getVisualItems<IVisualConnectionPtr>();
    for (auto it = connections.begin(); it != connections.end(); ++it) {
        (*it)->setSelected(false);
    }
}

void GroupState::end()
{
	stateMachine->getScene()->changeCursor(Qt::ArrowCursor);
	delete tempRect;
	tempRect = nullptr;
}

bool GroupState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	if (!startPos) {
		startPos = PointPtr(new QPointF(e->scenePos()));
		tempRect = stateMachine->getScene()->addRect(startPos->x(), startPos->y(), 1, 1);
	}
	tempRect->setRect(getRect(*startPos, e->scenePos()));
	return true;
}

bool GroupState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	stateMachine->setState(stateMachine->getNormalState());
	return true;
}

QRectF GroupState::getRect( const QPointF& startPos, const QPointF& endPos )
{
	auto x1 = startPos.x();
	auto y1 = startPos.y();
	auto x2 = endPos.x();
	auto y2 = endPos.y();
	auto x = (std::min)(x1, x2);
	auto y = (std::min)(y1, y2);
	auto w = (std::abs)(x1 - x2);
	auto h = (std::abs)(y1 - y2);
	return QRectF(x, y, w, h);
}

GroupSelectedState::GroupSelectedState( SceneStateMachine* scene ) :
	ISceneState(scene)
{
}

void GroupSelectedState::begin( ISceneStateConstPtr lastState )
{
	auto groupState = stateMachine->getGroupState();
	UTILS_ASSERT(groupState == lastState);
	selectedNodes = groupState->getSelectedNodes();
	for (auto it = selectedNodes.begin(); it != selectedNodes.end(); ++it) {
		(*it)->setSelection(true);
	}
}

void GroupSelectedState::end()
{
}

bool GroupSelectedState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	return false;
}

bool GroupSelectedState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	return false;
}

bool vdf::GroupState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
	// gdy startPos jest puste, oznacza to, ze w normalnym stanie po prostu kliknieto w pusty obszar
	if (startPos && e->button() == Qt::LeftButton) {
		QRectF rect = getRect(*startPos, e->scenePos());
		auto selected = stateMachine->getScene()->items(rect, Qt::ContainsItemBoundingRect, Qt::AscendingOrder);
		selectedNodes = stateMachine->getScene()->getSceneModel()->getVisualItems<IVisualNodePtr>(selected);
		if (!selectedNodes.empty()) {
			for (auto it = selectedNodes.begin(); it != selectedNodes.end(); ++it) {
				(*it)->setSelection(true);
			}
		}
	} 

	stateMachine->setState(stateMachine->getNormalState());
	return true;
}
