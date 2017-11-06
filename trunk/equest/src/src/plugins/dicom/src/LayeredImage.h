/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:36
	filename: 	LayeredImage.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDIMAGE_H__
#define HEADER_GUARD_DICOM__LAYEREDIMAGE_H__


#include <plugins/dicom/ILayer.h>
#include <plugins/dicom/ILayeredImage.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include "BackgroundLayer.h"


namespace dicom {

class LayeredImage : public ILayeredImage
{
public:
    LayeredImage();
    LayeredImage(const QPixmap& pixmap);
    LayeredImage(const std::string& pixmap);
	virtual ~LayeredImage();

public:
    virtual void addLayer( ILayerItemPtr layer, const std::string& layerName);
    virtual void removeLayer( ILayerItemConstPtr layer );
    virtual layers_const_range getLayerItems(const std::string& layerName) const;
    virtual QPixmap getPixmap() const;
	virtual int getNumLayerItems(const std::string& layerName) const;
	virtual int getNumGraphicLayerItems(const std::string& layerName) const;
	virtual ILayerItemConstPtr getLayerItem(const std::string& layerName,int idx) const;
	virtual ILayerItemPtr getLayerItem(const std::string& layerName, int idx);
	virtual ILayerGraphicItemConstPtr getLayerGraphicItem(const std::string& layerName,int idx) const;
	virtual ILayerGraphicItemPtr getLayerGraphicItem(const std::string& layerName, int idx);
    QSize getSize() const;

    // workaround
    std::vector<ILayerItemPtr> getLayersToSerialize(const std::string& tag) const;
    dicom::ILayerGraphicItemPtr getBackgroundLayer() const;
    void setBackgroundLayer(dicom::BackgroundLayerPtr val);
    virtual int getNumTags() const;
    virtual std::string getTag( int idx ) const;
    virtual tags_range getTags() const;
    virtual bool getTagVisible(const std::string& tag) const;
    virtual void setTagVisible(const std::string& tag, bool val);
    virtual ILayeredImage* clone() const;
	virtual const bool isPowerDoppler() const;
	void setIsPowerDoppler(const bool val);

	virtual const int getTrialID() const;
	void setTrialID(const int trialID);

private:
    BackgroundLayerPtr backgroundLayer;
    LayersMap layers;
	GraphicLayersMap graphicLayers;
    // TODO : przyda sie refaktoring, string juz nie starcza
    std::set<std::string> tags;
    std::map<std::string, bool> tagsVisibility;
	bool isPowerDoppler_;
	int trialID;

private:
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("layers", layers);
    }
};
typedef boost::shared_ptr<LayeredImage> LayeredImagePtr;
typedef boost::shared_ptr<const LayeredImage> LayeredImageConstPtr;
}

DEFINE_WRAPPER_INHERITANCE(dicom::LayeredImage, dicom::ILayeredImage);


#endif
