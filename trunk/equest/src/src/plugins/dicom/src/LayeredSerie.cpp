#include "DicomPCH.h"
#include "LayeredImageVisualizer.h"
#include <fstream>
#include <plugins/dicom/ILayeredImage.h>
#include <QtGui/QPixmap>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGraphicsPixmapItem>
#include "LayeredImageVisualizerView.h"
#include <QtWidgets/QApplication>
#include "BackgroundLayer.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "LayeredImage.h"
#include <utils/ICommand.h>
#include <QtWidgets/QScrollBar>
#include <boost/format.hpp>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <corelib/IPlugin.h>
#include <corelib/ISourceManager.h>


using namespace dicom;

class RemoveLayerCommand : public utils::ICommand
{
public:
    RemoveLayerCommand(LayeredSerie* serie, ILayeredImagePtr img, ILayerItemPtr layer) :
      img(img),
      layer(layer),
      serie(serie),
	  wasEdited(false)
    {
    }

public:
    virtual void doIt() 
    {
		wasEdited = serie->isEdited();

        img->removeLayer(layer);

		auto glayer = boost::dynamic_pointer_cast<ILayerGraphicItem>(layer);

        if (glayer) {
            glayer->getItem()->setVisible(false);
        }
        serie->refresh();

		serie->markAsEdited(true);
    }
    //! cofa wykonane ju¿ polecenie
    virtual void undoIt() 
    {
        img->addLayer(layer, serie->getCurrentLayerUserName());
		auto glayer = boost::dynamic_pointer_cast<ILayerGraphicItem>(layer);

		if (glayer) {
			glayer->getItem()->setVisible(true);
		}
        serie->refresh();

		serie->markAsEdited(wasEdited);
    }

    //! \return nazwa polecenia (dla ewentualnej reprezentacji stosu poleceñ)
    virtual QString name() { return QString(typeid(this).name()); }

private:
    ILayeredImagePtr img;
    ILayerItemPtr layer;
    LayeredSerie* serie;
	bool wasEdited;
};

hmdbCommunication::IHMDBShallowCopyContext * LayeredSerie::sourceContext() const
{
	return shallowContext_;
}

hmdbCommunication::IHMDBShallowCopyContext * LayeredSerie::sourceContextForData(const core::VariantConstPtr data)
{
	hmdbCommunication::IHMDBShallowCopyContext * ret = nullptr;

	auto source = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (source != nullptr){
		auto srcContext = source->shallowContextForData(data);
		ret = srcContext.get();
	}

	return ret;
}

void LayeredSerie::setupData( const core::VariantConstPtr & data )
{
	core::VariantPtr cloneWrp = data->clone();
    dicom::ILayeredImagePtr img = cloneWrp->get();
    this->image = img;

	this->shallowContext_ = sourceContextForData(data);

    if (image) {

        graphicsScene->addItem(image->getBackgroundLayer()->getItem());

        for(auto tag : image->getTags()) {
            int count = image->getNumGraphicLayerItems(tag);
            for (int i = 0; i < count; ++i) {
                auto vec = image->getLayerGraphicItem(tag, i);
                if (vec && vec->getItem() != nullptr) {
                    graphicsScene->addItem(vec->getItem());
                }
            }
        }

		std::string xmlName;        
        if (data->getMetadata("DICOM_XML", xmlName) == true) {
            utils::Filesystem::Path xml = xmlName;
            setName(xml.stem().string());
        }
        this->data = data;
        switchCrop(); //domyslny cropping
    }
}

QPixmap dicom::LayeredSerie::getPixmap() const
{
    return image->getPixmap();
}

QSize dicom::LayeredSerie::getSize() const
{
    return image->getSize();
}

dicom::ILayeredImageConstPtr dicom::LayeredSerie::getImage() const
{
    return image;
}

dicom::ILayeredImagePtr dicom::LayeredSerie::getImage()
{
    return image;
}

dicom::LayeredSerie::LayeredSerie(LayeredImageVisualizer* visualizer, LayeredImageVisualizerView * view) :
    visualizer(visualizer),
	view(view),
    //pixmapItem(nullptr),
    initialized(false),
    layersModel(visualizer),
    commandStack(new utils::CommandStack),
	shallowContext_(nullptr)
{
    stateMachine = utils::make_shared<LayeredStateMachine>(this, commandStack);
    graphicsScene = new coreUI::GraphicSceneWithStateMachine(stateMachine);
    graphicsView = new coreUI::WheelGraphicsView(graphicsScene);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
	graphicsView->setMouseTracking(true);
    graphicsScene->setSceneRect( graphicsView->rect() );

    QObject::connect(graphicsScene, SIGNAL(selectionChanged()), stateMachine.get(), SLOT(selectionChanged()));
}

const bool dicom::LayeredSerie::editionEnable() const
{
	using namespace hmdbServices::xmlWsdl;
	auto status = view->annotationStatus(false).status;
	return  status == AnnotationStatus::UnderConstruction || status == AnnotationStatus::Unspecified || status == AnnotationStatus::Rejected;
	//auto actions = view->actions();
	//return actions.empty() == false ? actions.first()->isEnabled() : false;
}

coreUI::WheelGraphicsView* dicom::LayeredSerie::getGraphicsView() const
{
    return graphicsView;
}

coreUI::GraphicSceneWithStateMachine* dicom::LayeredSerie::getGraphicsScene() const
{
    return graphicsScene;
}

void dicom::LayeredSerie::refresh()
{
    getGraphicsView()->setSceneRect(0, 0, getSize().width(), getSize().height());
    getGraphicsScene()->setSceneRect( getGraphicsView()->rect() );
    for(auto tag : image->getTags()) {
		int count = image->getNumGraphicLayerItems(tag);
		for (int i = 0; i < count; ++i) {
			auto vec = image->getLayerGraphicItem(tag, i);
            if (vec && vec->getItem() != nullptr && graphicsScene != vec->getItem()->scene()) {
                graphicsScene->addItem(vec->getItem());
            }
        }
    }
    layersModel.setImage(getImage());
}

std::string dicom::LayeredSerie::getXmlOutputFilename() const
{
	std::string xmlPattern;
	data->getMetadata("DICOM_XML_PATTERN",xmlPattern);

    return boost::str(boost::format(xmlPattern) % visualizer->getCurrentLayerUserName());
}

dicom::LayeredModelView* dicom::LayeredSerie::getLayersModel()
{
    return &layersModel;
}

void dicom::LayeredSerie::undo()
{
    if (commandStack->isUndoPossible()) {
        commandStack->undo();
    }
}

void dicom::LayeredSerie::redo()
{
    if (commandStack->isRedoPossible()) {
        commandStack->redo();
    }
}

const bool dicom::LayeredSerie::isEdited() const
{
	auto w = visualizer->createWidget();
	if(w != nullptr){
		return w->isWindowModified();
	}

	return false;
}

void dicom::LayeredSerie::markAsEdited(const bool edited)
{
	auto w = visualizer->createWidget();
	if(w != nullptr){
		w->setWindowModified(edited);
	}
}

void dicom::LayeredSerie::setNormalState()
{
    stateMachine->setState(stateMachine->getNormalState());
}

void dicom::LayeredSerie::save()
{
    std::ofstream ofs(getXmlOutputFilename());
    assert(ofs.good());
    boost::archive::xml_oarchive oa(ofs);
    oa.register_type<BackgroundLayer>();
    oa.register_type<PointsLayer>();
	oa.register_type<BloodLevelLayer>();
	oa.register_type<InflammatoryLevelLayer>();
	oa.register_type<FingerTypeLayer>();
	oa.register_type<JointTypeLayer>();
	oa.register_type<ImageQualityLayer>();
    LayeredImageConstPtr l = boost::const_pointer_cast<const LayeredImage>(boost::dynamic_pointer_cast<LayeredImage>(getImage()));
    auto v = l->getLayersToSerialize(visualizer->getCurrentLayerUserName());
    oa << boost::serialization::make_nvp("layers", v);
    ofs.close();
}

void dicom::LayeredSerie::removeLayer( int tagIdx, int idx )
{
    auto img = this->getImage();
    auto tag = img->getTag(tagIdx);
    removeLayer(tag, idx);
}

void dicom::LayeredSerie::removeLayer( const std::string& tag, int idx )
{
    auto img = this->getImage();
    if (idx >= 0 && idx < img->getNumLayerItems(tag)) {
        auto layer = img->getLayerItem(tag, idx);
        commandStack->addCommand(utils::make_shared<RemoveLayerCommand>(this, img, layer));    
    }
}

void dicom::LayeredSerie::editLayer( int tagIdx, int idx )
{
    auto img = this->getImage();

    auto tag = img->getTag(tagIdx);
    if (idx >= 0 && idx < img->getNumLayerItems(tag)) {
        auto layer = img->getLayerItem(tag, idx);
        auto pointsLayer = boost::dynamic_pointer_cast<PointsLayer>(layer);
        if (pointsLayer) {
            stateMachine->getEditState()->setLayerToEdit(pointsLayer);
            stateMachine->setState(stateMachine->getEditState());
        }
    }
}

void dicom::LayeredSerie::switchCrop()
{
    ILayeredImagePtr img = this->getImage();
    //int count = img->getNumLayerItems(TODO);
    BackgroundLayerPtr background = boost::dynamic_pointer_cast<BackgroundLayer>(img->getBackgroundLayer());
    if (background) {
        if (background->getPixmap().rect() == background->getCrop()) {
            //background->setCrop(QRect(56,135, 761, 493));
            background->setCrop(QRect(4,133, 850, 645));
        } else {
            background->setCrop(background->getPixmap().rect());
        }
    }
}

void dicom::LayeredSerie::setMoveState()
{
    stateMachine->setState(stateMachine->getMoveState());
}

std::string dicom::LayeredSerie::getLoggedUserName() const
{
    return visualizer->getUserName();
}

std::string dicom::LayeredSerie::getCurrentLayerUserName() const
{
    return visualizer->getCurrentLayerUserName();
}



void dicom::LayeredSerie::setBoneState()
{
    stateMachine->setState(stateMachine->getBoneState());
}

void dicom::LayeredSerie::setSkinState()
{
    stateMachine->setState(stateMachine->getSkinState());
}

void dicom::LayeredSerie::setTendonState()
{
    stateMachine->setState(stateMachine->getTendonState());
}

void dicom::LayeredSerie::setJointState()
{
    stateMachine->setState(stateMachine->getJointState());
}

void dicom::LayeredSerie::setInflamatoryState()
{
    stateMachine->setState(stateMachine->getInflamatoryState());
}

void dicom::LayeredSerie::setNoiseState()
{
    stateMachine->setState(stateMachine->getNoiseState());
}

void dicom::LayeredSerie::fitToSize()
{
    ILayeredImagePtr img = this->getImage();
    BackgroundLayerPtr background = boost::dynamic_pointer_cast<BackgroundLayer>(img->getBackgroundLayer());
    if (background) {
        auto rect = background->getCrop();
        auto rectW = getGraphicsView()->rect();
        
        auto h = (float)rectW.height() / rect.height();
        auto w = (float)rectW.width() / rect.width();

        if (w > 1.0f) {
            w *= 0.99f;
            getGraphicsView()->scale(w, w);
            auto sb = getGraphicsView()->verticalScrollBar();
            sb->setValue(sb->maximum() - 1);
        }
    }
}

void dicom::LayeredSerie::init()
{
    initialized = true;
    fitToSize();
}

bool dicom::LayeredSerie::wasInitialized() const
{
    return initialized;
}

std::string dicom::LayeredSerie::getImageFilename() const
{
    std::string sourceFile;
    if (data->getMetadata("core/source", sourceFile)) {
        return sourceFile;
    }

    throw loglib::runtime_error("Unable to find image file name");
}




