#include "CoreUiPCH.h"
#include <coreui/MoveCommand.h>
#include <QtWidgets/QGraphicsItem>

using namespace coreUI;

MoveCommand::MoveCommand( QGraphicsItem* item, const QPointF& newP, const QPointF& oldP ) :
item(item),
    newPosition_(newP),
    oldPosition_(oldP)
{

}

void MoveCommand::doIt()
{
    item->setPos(newPosition_);
}

void MoveCommand::undoIt()
{
    item->setPos(oldPosition_);
}