/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:28
	filename: 	ILayeredImage.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ILAYEREDIMAGE_H__
#define HEADER_GUARD_DICOM__ILAYEREDIMAGE_H__


#include <plugins/dicom/ILayer.h>
#include <vector>
#include <objectwrapperlib/ObjectWrapper.h>
#include <utils/PtrPolicyBoost.h>
#include <utils/ClonePolicies.h>
#include <boost/range/iterator_range_core.hpp>
#include <boost/serialization/access.hpp>
#include "boost/serialization/assume_abstract.hpp"

namespace dicom {

class ILayeredImage
{
public:
    typedef std::multimap<std::string, ILayerItemPtr> LayersMap;
	typedef std::multimap<std::string, ILayerGraphicItemPtr> GraphicLayersMap;
    typedef boost::iterator_range<LayersMap::const_iterator> layers_const_range;
	typedef boost::iterator_range<GraphicLayersMap::const_iterator> graphiclayers_const_range;
    typedef boost::iterator_range<std::set<std::string>::const_iterator> tags_range;

public:
    ILayeredImage() {}
	virtual ~ILayeredImage() {}
    virtual void addLayer(ILayerItemPtr layer, const std::string& layerName) = 0;
    virtual void removeLayer(ILayerItemConstPtr layer) = 0;
    virtual layers_const_range getLayerItems(const std::string& layerName) const = 0;
    virtual QPixmap getPixmap() const = 0;
    virtual QSize getSize() const = 0;
    virtual int getNumLayerItems(const std::string& layerName) const = 0;
	virtual int getNumGraphicLayerItems(const std::string& layerName) const = 0;
    virtual ILayerItemConstPtr getLayerItem(const std::string& layerName,int idx) const = 0;
    virtual ILayerItemPtr getLayerItem(const std::string& layerName, int idx) = 0;
	virtual ILayerGraphicItemConstPtr getLayerGraphicItem(const std::string& layerName,int idx) const = 0;
	virtual ILayerGraphicItemPtr getLayerGraphicItem(const std::string& layerName, int idx) = 0;
    virtual dicom::ILayerGraphicItemPtr getBackgroundLayer() const = 0;
    virtual int getNumTags() const = 0;
    virtual std::string getTag(int idx) const = 0;
    virtual tags_range getTags() const = 0;
    virtual bool getTagVisible(const std::string& tag) const = 0;
    virtual void setTagVisible(const std::string& tag, bool val) = 0;
	virtual const bool isPowerDoppler() const = 0;
	virtual const int getTrialID() const { return -1; }	

    virtual ILayeredImage* clone() const = 0;
};
typedef boost::shared_ptr<ILayeredImage> ILayeredImagePtr;
typedef boost::shared_ptr<const ILayeredImage> ILayeredImageConstPtr;

typedef std::vector<dicom::ILayerItemPtr> LayersVector;
typedef boost::shared_ptr<LayersVector> LayersVectorPtr;
typedef boost::shared_ptr<const LayersVector> LayersVectorConstPtr;
}

DEFINE_WRAPPER(dicom::ILayeredImage, utils::PtrPolicyBoost, utils::ClonePolicyVirtualCloneMethod);
DEFINE_WRAPPER(dicom::LayersVector, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);

#endif
