#include "DicomPCH.h"
#include "BackgroundLayer.h"
#include <QtGui/QPainter>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

dicom::BackgroundLayer::BackgroundLayer( const QString& pixmapPath, const QString& name /*= QString("Background")*/ ) :
	ILayerGraphicItem(0),
    pixmapPath(pixmapPath),
    name(name),
    pixmapItem(new QGraphicsPixmapItem())
{
    
}

dicom::BackgroundLayer::BackgroundLayer( const QPixmap& p, const QString& name /*= QString("Background")*/ ) :
	ILayerGraphicItem(0),
    pixmap(p),
    name(name),
    pixmapItem(new QGraphicsPixmapItem())
{
    
}


dicom::BackgroundLayer::~BackgroundLayer()
{

}
    
QString dicom::BackgroundLayer::getName() const
{
    return name;
}


QSize dicom::BackgroundLayer::getSize() const
{
    return getCrop().size();
}

void dicom::BackgroundLayer::setName( const QString& name )
{
    this->name = name;
}

bool dicom::BackgroundLayer::getSelected() const
{
    return false;
}

void dicom::BackgroundLayer::setSelected( bool /*val*/ )
{

}

QRect dicom::BackgroundLayer::getCrop() const
{
    lazy();
    return crop;
}

void dicom::BackgroundLayer::setCrop( const QRect& val )
{
    if (crop != val) {
        crop = val;
        cropped = getPixmap().copy(crop);
        pixmapItem->setPixmap(cropped);
        pixmapItem->setOffset(crop.x(), crop.y());
    }
}

QGraphicsItem* dicom::BackgroundLayer::getItem()
{
    lazy();
    return pixmapItem;
}

const QPixmap& dicom::BackgroundLayer::getPixmap() const
{
    lazy();
    return pixmap;
}

void dicom::BackgroundLayer::lazy() const
{
    if (pixmap.isNull()) {
        pixmap = QPixmap(pixmapPath);
    }
    if (crop.isEmpty()) {

		if(pixmap.isNull() == true){
			PLUGIN_LOG_INFO("Background layer lazy initialization failed for pixmapPath: " << pixmapPath.toStdString());
		}else{
			crop = pixmap.rect();
			cropped = pixmap.copy(crop);
			pixmapItem->setPixmap(cropped);
		}
    }
}

dicom::BackgroundLayer* dicom::BackgroundLayer::clone() const
{
    BackgroundLayer* bl = new BackgroundLayer(pixmap, name);
    bl->pixmapPath = this->pixmapPath;
    bl->setCrop(this->getCrop());
    return bl;
}

std::vector<QPointF> dicom::BackgroundLayer::getPointsCloud(int density, int normalizeLength) const
{
	return std::vector<QPointF>();
}



std::vector<QPointF> dicom::BackgroundLayer::getPixelPointsCloud() const
{
	return std::vector<QPointF>();
}
