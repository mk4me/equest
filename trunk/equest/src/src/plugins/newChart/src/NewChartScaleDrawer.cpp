#include "NewChartPCH.h"
#include "NewChartScaleDrawer.h"


PercentScaleDraw::PercentScaleDraw( double left, double right, bool percentMode /*= false*/ ) :
QwtScaleDraw(),
    left(left),
    right(right),
    percentMode(percentMode)
{
}

void PercentScaleDraw::drawLabel( QPainter *painter, double value ) const
{
    double val = percentMode ? ((value - left) / (right - left) * 100) : value;
    QwtText lbl = tickLabel( painter->font(), val);

    if ( lbl.isEmpty() )
        return;
    if (percentMode) {
        lbl.setText(lbl.text() + "%");
    }
    QPointF pos = labelPosition( value );

    QSizeF labelSize = lbl.textSize( painter->font() );

    const QTransform transform = labelTransformation( pos, labelSize );

    painter->save();
    painter->setWorldTransform( transform, true );

    lbl.draw ( painter, QRect( QPoint( 0, 0 ), labelSize.toSize() ) );

    painter->restore();

}

QwtScaleDiv PercentScaleDraw::getScaleDiv()
{
    QList<double> ticks[QwtScaleDiv::NTickTypes];

    QList<double> &majorTicks = ticks[QwtScaleDiv::MajorTick];
    QList<double> &mediumTicks = ticks[QwtScaleDiv::MediumTick];
    QList<double> &minorTicks = ticks[QwtScaleDiv::MinorTick];

    double diff = right - left;

    for (int i = 0; i <= 100; ++i) {
        if ((i % 25) == 0) {
            majorTicks += left + diff * (i / 100.0);
        } else if ((i % 5) == 0) {
            mediumTicks += left + diff * (i / 100.0);
        } else {
            minorTicks += left + diff * (i / 100.0);
        }
    }

    QwtScaleDiv scaleDiv( majorTicks.first(), majorTicks.last(), ticks );
    scaleDiv.setTicks(QwtScaleDiv::MediumTick, mediumTicks);
    return scaleDiv;
}

void PercentScaleDraw::setLeftRightValues( double left, double right )
{
    this->left = left;
    this->right = right;
}
