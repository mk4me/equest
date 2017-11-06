#include "HmmLibPCH.h"
#include <plugins/hmmlib/ConfigurationPainterAreas.h>
#include <QtGui/QPainter>
#include <QtCore/QFileInfo>
#include <QtGui/QPen>

IArea::IArea() : 
active(false),  
    scale(1.0f) 
{}

bool IArea::isInside( int x, int y ) const
{
    int x1 = getX();
    int x2 = x1 + getWidth();
    int y1 = getY();
    int y2 = y1 + getHeight();
    return (x1 <= x && x2 >= x && y1 <= y && y2 >= y);
}

void SinglePicture::setPixmapAlpha(QImage& pixmap, unsigned int alpha )
{
    QImage alphaChannel = pixmap;
    QPainter p(&alphaChannel);
    p.fillRect(alphaChannel.rect(), QColor(alpha, alpha, alpha));
    p.end();
    pixmap.setAlphaChannel(alphaChannel);
}

void SinglePicture::draw( QPainter& painter, bool selected )
{
    if (alwaysVisible || isActive()) {
        int shift = selected ? 2 : 0;
        painter.drawImage(QRect(getX() - shift, getY() - shift, getWidth() + shift, getHeight() + shift), *pixmap);
    } else {
        if (selected) {
            painter.drawImage(QRect(getX(), getY(), getWidth(), getHeight()), *alphaPixmap);
        }
    }
}

bool SinglePicture::isInside( int x, int y ) const
{
    if (IArea::isInside(x, y)) {
        int shiftX = (x - getX()) / getScale();
        int shiftY = (y - getY()) / getScale();
        const QRgb* data = reinterpret_cast<const QRgb*>(pixmap->constBits());

        int alpha = qAlpha(data[shiftX + shiftY * pixmap->width()]);

        if (alpha != 0) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

SinglePicture::SinglePicture( const QString& name, int x /*= 0*/, int y /*= 0*/, bool alwaysVisible /*= false*/ ) :
name(QFileInfo(name).baseName()),
    pixmap(new QImage(name)),
    alphaPixmap( new QImage(name)),
    x(x), y(y),
    alwaysVisible(alwaysVisible)
{
    setPixmapAlpha(*alphaPixmap, 100);
}

SinglePicture::SinglePicture( const QString& name, const QImagePtr& pixmap, int x /*= 0*/, int y /*= 0*/, bool alwaysVisible /*= false*/ ) :
name(name),
    pixmap(pixmap),
    alphaPixmap(new QImage(*pixmap)),
    x(x), y(y),
    alwaysVisible(alwaysVisible)
{
    setPixmapAlpha(*alphaPixmap, 100);
}

void SingleMarker::draw( QPainter& painter, bool selected ) 
{
    QPen pen;
    QBrush brush;
    brush.setStyle(Qt::Dense5Pattern);
    pen.setWidth(3);
    if (isActive()) {
        if (selected) {
            pen.setColor(QColor(100,255,10));
            brush.setColor(QColor(100,255,10,50));
        } else {
            pen.setColor(QColor(100,200,10));
            brush.setColor(QColor(100,200,10,50));
        }
    } else {
        if (selected) {
            pen.setColor(QColor(100,100,100));
            brush.setColor(QColor(100,100,100,50));
        } else {
            pen.setColor(QColor(100,100,100,60));
            brush.setColor(QColor(100,100,100,10));
        }
    }

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawEllipse(getX(), getY(), getWidth(), getHeight());
}

void SingleMarker::init( const QString& name, int posX, int posY, int r )
{
    this->name = name;
    this->posX = posX;
    this->posY = posY;
    this->r = r;
}

SingleMarker::SingleMarker( const QString& name, int posX, int posY )
{
    init(name, posX, posY, 15);
}

SingleMarker::SingleMarker( const QString& name, int posX, int posY, int r )
{
    init(name, posX, posY, r);
}

