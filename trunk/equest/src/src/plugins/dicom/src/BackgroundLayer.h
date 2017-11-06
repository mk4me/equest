/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:32
	filename: 	BackgroundLayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__BACKGROUNDLAYER_H__
#define HEADER_GUARD_DICOM__BACKGROUNDLAYER_H__

#include <plugins/dicom/ILayer.h>

#include <boost/serialization/nvp.hpp>
#include <QtWidgets/QGraphicsPixmapItem>
#include "qstring_serialization.h"
#include "Serializers.h"

namespace dicom {

//! Layer z obrazem t³a
class BackgroundLayer : public ILayerGraphicItem
{
public:
    BackgroundLayer(const QPixmap& p, const QString& name = QString("Background"));
    BackgroundLayer(const QString& pixmapPath, const QString& name = QString("Background"));
	virtual ~BackgroundLayer();

public:
    virtual QGraphicsItem* getItem();
    virtual QString getName() const;
	virtual QSize getSize() const;
	virtual std::vector<QPointF> getPointsCloud(int density = 0, int normalizeLength = -1) const;
	virtual std::vector<QPointF> getPixelPointsCloud() const;

    QRect getCrop() const;
    void setCrop(const QRect& val);
    const QPixmap& getPixmap() const;

    virtual BackgroundLayer* clone() const;

private:
    BackgroundLayer() {}
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ILayerItem);
        ar & BOOST_SERIALIZATION_NVP(name);
        ar & BOOST_SERIALIZATION_NVP(pixmapPath);
    }

    virtual void setName( const QString& name );

    virtual bool getSelected() const;

    virtual void setSelected( bool val );

private:
    void lazy() const;


private:
    QString pixmapPath;
    QString name;
    mutable QGraphicsPixmapItem* pixmapItem;
    mutable QRect crop;
    mutable QPixmap pixmap;
    mutable QPixmap cropped;
};
typedef boost::shared_ptr<BackgroundLayer> BackgroundLayerPtr;
typedef boost::shared_ptr<const BackgroundLayer> BackgroundLayerConstPtr;


}

#endif
