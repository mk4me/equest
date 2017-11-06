#include "NewChartPCH.h"
#include "NewChartSerie.h"
#include "NewChartDotFloating.h"

NewChartDotFloating::NewChartDotFloating( const QPointF& position, const INewChartSeriePrivate* relatedSerie, int size /*= 2*/ ) :
    NewChartDot(size),
    relatedSerie(relatedSerie)
{
    QPointF p(position.x() / relatedSerie->getXScale(), position.y() / relatedSerie->getYScale());
    p -= relatedSerie->getOffset();
    this->position = p;
}


QPointF NewChartDotFloating::getPosition() const
{
    QPointF p = position;
    p.setX(p.x() * relatedSerie->getXScale());
    p.setY(p.y() * relatedSerie->getYScale());
    return p + relatedSerie->getOffset();
}

void NewChartDotFloating::setPosition( const QPointF& val )
{
    position = val; 
}
