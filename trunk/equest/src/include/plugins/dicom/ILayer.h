/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:27
	filename: 	ILayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ILAYER_H__
#define HEADER_GUARD_DICOM__ILAYER_H__

#include <utils/SmartPtr.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

class QPainter;
class QRect;
class QGraphicsItem;

namespace dicom {

//! Podstawowy element warstwy wizualizatora
class ILayerItem
{
public:
    ILayerItem() : adnotationIdx(-1) {}

	ILayerItem(const int val) : adnotationIdx(val) {}
	virtual ~ILayerItem() {}

    
    virtual QString getName() const = 0;
    virtual void setName(const QString& name) = 0;

    //! \return G³êboka kopia elementu
    virtual ILayerItem* clone() const = 0;

    // TODO : dobrze byloby przeniest to do jakiegos dekoratora...
    int getAdnotationIdx() const { return adnotationIdx; }

private:
	//! Index typu adnotacji
    int adnotationIdx;

    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    { 
        ar & BOOST_SERIALIZATION_NVP(adnotationIdx);
    }
};
typedef boost::shared_ptr<ILayerItem> ILayerItemPtr;
typedef boost::shared_ptr<const ILayerItem> ILayerItemConstPtr;

class ILayerGraphicItem : public ILayerItem
{
public:
	ILayerGraphicItem() : ILayerItem() {}

	ILayerGraphicItem(const int val) : ILayerItem(val) {}
	virtual ~ILayerGraphicItem() {}

	virtual QSize getSize() const = 0;

	virtual bool getSelected() const = 0;
	virtual void setSelected(bool val) = 0;

	virtual QGraphicsItem* getItem() = 0;

	virtual ILayerGraphicItem* clone() const = 0;

    virtual std::vector<QPointF> getPointsCloud(int density = 0, int normalizeLength = -1) const = 0;
	virtual std::vector<QPointF> getPixelPointsCloud() const = 0;

};
typedef boost::shared_ptr<ILayerGraphicItem> ILayerGraphicItemPtr;
typedef boost::shared_ptr<const ILayerGraphicItem> ILayerGraphicItemConstPtr;


class ILayer
{
public:
    virtual ~ILayer() {}

    virtual void addItem(ILayerItemPtr) = 0;
    virtual void removeItem(ILayerItemPtr) = 0;
    virtual void setVisible(bool) = 0;
};

}

#endif
