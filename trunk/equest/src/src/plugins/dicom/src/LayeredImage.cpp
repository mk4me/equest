#include "DicomPCH.h"
#include "LayeredImage.h"
#include "BackgroundLayer.h"
#include <QtGui/QPainter>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <QtWidgets/QGraphicsItemGroup>
#include <boost/make_shared.hpp>


dicom::LayeredImage::LayeredImage( const QPixmap& pixmap ) : isPowerDoppler_(false), trialID(-1)
{
    //layers.push_back(utils::make_shared<BackgroundLayer>(pixmap));
    backgroundLayer = boost::make_shared<BackgroundLayer>(pixmap);
}

dicom::LayeredImage::LayeredImage( const std::string& pixmap ) : isPowerDoppler_(false), trialID(-1)
{
    //layers.push_back(utils::make_shared<BackgroundLayer>(QString::fromStdString(pixmap)));
    backgroundLayer = boost::make_shared<BackgroundLayer>(QString::fromStdString(pixmap));
}

dicom::LayeredImage::LayeredImage() : isPowerDoppler_(false), trialID(-1)
{

}

dicom::LayeredImage::~LayeredImage()
{

}

const bool dicom::LayeredImage::isPowerDoppler() const
{
	return isPowerDoppler_;
}

void dicom::LayeredImage::setIsPowerDoppler(const bool val)
{
	isPowerDoppler_ = val;
}

const int dicom::LayeredImage::getTrialID() const
{
	return trialID;
}

void dicom::LayeredImage::setTrialID(const int trialID)
{
	this->trialID = trialID;
}

void dicom::LayeredImage::addLayer( ILayerItemPtr layer, const std::string& layerName )
{
    layers.insert(std::make_pair(layerName, layer));
    tags.insert(layerName);

	auto graphicLayer = boost::dynamic_pointer_cast<ILayerGraphicItem>(layer);
	if(graphicLayer != nullptr){
		graphicLayers.insert(std::make_pair(layerName, graphicLayer));

		auto vis = tagsVisibility.find(layerName);
		if (vis == tagsVisibility.end()) {
			tagsVisibility[layerName] = true;
		}
	}
}

void dicom::LayeredImage::removeLayer( ILayerItemConstPtr layer )
{
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        if (it->second == layer) {
            layers.erase(it);
            break;
        }
    }

	auto graphicLayer = boost::dynamic_pointer_cast<const ILayerGraphicItem>(layer);
	if(graphicLayer != nullptr){
		for (auto it = graphicLayers.begin(); it != graphicLayers.end(); ++it) {
			if (it->second == layer) {
				graphicLayers.erase(it);
				break;
			}
		}
	}
}

dicom::LayeredImage::layers_const_range dicom::LayeredImage::getLayerItems( const std::string& layerName ) const
{
    return boost::make_iterator_range(layers.equal_range(layerName));
    //return boost::make_iterator_range(layers.cbegin(), layers.cend());
}


QPixmap dicom::LayeredImage::getPixmap() const
{
    throw loglib::runtime_error("Not implemented");
    /*QSize size = getSize();
    QRect rect(QPoint(0, 0), size);
    QPixmap pix(size);
    QPainter paint(&pix);
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        IRasterLayerItemPtr raster = utils::dynamic_pointer_cast<IRasterLayerItem>(*it);
        if (raster) {
            raster->render(&paint, &rect);
        }
    }
    return pix;*/
}

QSize dicom::LayeredImage::getSize() const
{
    QSize maxSize = backgroundLayer->getSize();
    for (auto it = graphicLayers.begin(); it != graphicLayers.end(); ++it) {
        QSize layerSize = (it->second)->getSize();
       
        if (maxSize.width() < layerSize.width()) {
            maxSize.setWidth(layerSize.width());
        }

        if (maxSize.height() < layerSize.height()) {
            maxSize.setHeight(layerSize.height());
        }
    }

    return maxSize;
}

int dicom::LayeredImage::getNumLayerItems( const std::string& layerName ) const
{
    return layers.count(layerName);
}

int dicom::LayeredImage::getNumGraphicLayerItems(const std::string& layerName) const
{
	return graphicLayers.count(layerName);
}

dicom::ILayerItemConstPtr dicom::LayeredImage::getLayerItem(const std::string& layerName, int idx ) const
{
    UTILS_ASSERT(idx >= 0 && idx < getNumLayerItems(layerName));
    auto it = layers.equal_range(layerName).first;
    std::advance(it, idx);
    return (it->second);
}

dicom::ILayerItemPtr dicom::LayeredImage::getLayerItem(const std::string& layerName,  int idx )
{
    UTILS_ASSERT(idx >= 0 && idx < getNumLayerItems(layerName));
    auto it = layers.equal_range(layerName).first;
    std::advance(it, idx);
    return (it->second);
}

dicom::ILayerGraphicItemConstPtr dicom::LayeredImage::getLayerGraphicItem(const std::string& layerName,int idx) const
{
	UTILS_ASSERT(idx >= 0 && idx < getNumGraphicLayerItems(layerName));
	auto it = graphicLayers.equal_range(layerName).first;
	std::advance(it, idx);
	return (it->second);
}

dicom::ILayerGraphicItemPtr dicom::LayeredImage::getLayerGraphicItem(const std::string& layerName, int idx)
{
	UTILS_ASSERT(idx >= 0 && idx < getNumGraphicLayerItems(layerName));
	auto it = graphicLayers.equal_range(layerName).first;
	std::advance(it, idx);
	return (it->second);
}

std::vector<dicom::ILayerItemPtr> dicom::LayeredImage::getLayersToSerialize( const std::string& tag ) const
{
    std::vector<ILayerItemPtr> ret;
	auto r = layers.equal_range(tag);
	for (auto it = r.first; it != r.second; ++it){
        ret.push_back(it->second);
    }
    return ret;
}

dicom::ILayerGraphicItemPtr dicom::LayeredImage::getBackgroundLayer() const
{
    return backgroundLayer;
}

void dicom::LayeredImage::setBackgroundLayer( dicom::BackgroundLayerPtr val )
{
    backgroundLayer = val;
}

int dicom::LayeredImage::getNumTags() const
{
    return tags.size();
}

std::string dicom::LayeredImage::getTag( int idx ) const
{
    auto it = tags.begin();
    std::advance(it, idx);
    return *it;
}

dicom::LayeredImage::tags_range dicom::LayeredImage::getTags() const
{
    return boost::make_iterator_range(tags.cbegin(), tags.cend());
}

bool dicom::LayeredImage::getTagVisible( const std::string& tag ) const
{
    auto vis = tagsVisibility.find(tag);
    if (vis != tagsVisibility.end()) {
        return vis->second;
    } else {
        throw loglib::runtime_error("Unknown tag");
    }
}

void dicom::LayeredImage::setTagVisible( const std::string& tag, bool val )
{
    tagsVisibility[tag] = val;
    for (int i = getNumGraphicLayerItems(tag) - 1; i >= 0; --i ) {
		getLayerGraphicItem(tag, i)->getItem()->setVisible(val);	
    }
}

dicom::ILayeredImage* dicom::LayeredImage::clone() const
{
    std::unique_ptr<LayeredImage> img(new LayeredImage());
    img->backgroundLayer = boost::dynamic_pointer_cast<BackgroundLayer>(ILayerItemPtr(backgroundLayer->clone()));
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        img->addLayer(ILayerItemPtr(it->second->clone()), it->first);
    }

    for (auto it = tagsVisibility.begin(); it != tagsVisibility.end(); ++it) {
        img->setTagVisible(it->first, it->second);
    }

	img->setIsPowerDoppler(this->isPowerDoppler());
	img->setTrialID(this->getTrialID());

    return img.release();
}


