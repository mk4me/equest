#include "VideoPCH.h"
#include "VideoVisualizer.h"

#include <osgWidget/Widget>
#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/Box>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgui/AspectRatioKeeper.h>
#include <osgui/OsgWidgetUtils.h>
#include <osgui/EventCallback.h>

#include <vidlib/osg/VideoImageStream.h>

#include <utils/StringConversionTools.h>
#include <corelib/PluginCommon.h>

#include <QtGui/QIcon>

using namespace core;

struct VideoVisualizer::Refresher
{
    VideoVisualizer* visualizer;
    void operator()(osg::Node* node, const osgGA::GUIEventAdapter* event)
    {
        UTILS_ASSERT(visualizer);
        visualizer->refresh( static_cast<float>(event->getWindowWidth()), static_cast<float>(event->getWindowHeight()) );
    }
};

struct VideoVisualizer::WidgetUpdater : public osg::Drawable::UpdateCallback
{
    VideoVisualizer* visualizer;

    WidgetUpdater(VideoVisualizer* visualizer = nullptr) :
    visualizer(visualizer)
    {

    }

    virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable)
    {
        UTILS_ASSERT(visualizer);
        visualizer->updateWidget();
    }
};


VideoVisualizer::VideoSerie::VideoSerie(VideoVisualizer * visualizer) :
	visualizer(visualizer),
    offset(0.0)
{

}

VideoVisualizer::VideoSerie::~VideoSerie()
{

}

void VideoVisualizer::VideoSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string VideoVisualizer::VideoSerie::getName() const
{
	return name;
}

void VideoVisualizer::VideoSerie::setData(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data)
{
	bool success = false;

    std::string delayIt;
    if (data->getMetadata("movieDelay", delayIt)) {
        try {
        	this->offset = boost::lexical_cast<double>(delayIt);
        } catch (const boost::bad_lexical_cast& ) {
        	std::replace(delayIt.begin(), delayIt.end(), ',', '.');
        	try {
        		this->offset =  boost::lexical_cast<double>(delayIt);
        	} catch (const boost::bad_lexical_cast& ) {
        		PLUGIN_LOG_WARNING("Unable to parse movie delay");
        	}
        }
    }
	if (data->data()->isSupported(typeid(VideoStream))) {
		auto clonedData = data->clone();
		visualizer->clear();
		success = clonedData->tryGet(visualizer->stream);
	}
	else if (data->data()->isSupported(typeid(VideoChannel))) {
		auto clonedData = data->clone();
		VideoChannelPtr channel = clonedData->get();
		if (channel) {
			visualizer->clear();
			visualizer->stream = channel->getVideoStream();
			success = (visualizer->stream != nullptr);
		}
	}

	this->data = data;
	this->requestedType = requestedType;

	// pobranie obrazka
	if ( success  == true && visualizer->stream != nullptr ) {
		if(visualizer->getImage() == true){
			visualizer->widget->setColor(1.0, 1.0, 1.0, 1.0);
			visualizer->refreshImage();
			//! Fix pierwszej ramki - wymuszam poprawny resize okienek OSG!!
			visualizer->viewer->getEventQueue()->windowResize(0, 0, visualizer->viewer->width(), visualizer->viewer->height());
			visualizer->viewer->frame();
		}
	}else{

		visualizer->refresh(visualizer->viewer->width(), visualizer->viewer->height());
	}
}

const utils::TypeInfo & VideoVisualizer::VideoSerie::getRequestedDataType() const
{
	return requestedType;
}

void VideoVisualizer::VideoSerie::safeSetTime(const double time)
{
	visualizer->currentStreamTime = std::min(std::max(0.0, time - offset), getLength());
}

void VideoVisualizer::VideoSerie::update()
{
	/*VideoStreamConstPtr stream;
	data->get(stream);*/
	//safeSetTime(visualizer->stream->getTime());
}

const core::VariantConstPtr & VideoVisualizer::VideoSerie::getData() const
{
	return data;
}

double VideoVisualizer::VideoSerie::getLength() const
{
	return visualizer->stream->getDuration();
}

double VideoVisualizer::VideoSerie::getBegin() const
{
	return 0.0;
}

double VideoVisualizer::VideoSerie::getEnd() const
{
	return getLength();
}

void VideoVisualizer::VideoSerie::setTime(double time)
{
	safeSetTime(time);
	requestUpdate();
}

void VideoVisualizer::VideoSerie::setOffset( double val )
{
    offset = val;
}

double VideoVisualizer::VideoSerie::getOffset() const
{
    return offset;
}

VideoVisualizer::VideoVisualizer() :
useTextureRect(true), prevStreamTime(-1), currentStreamTime(-1), prevStreamWidth(-1), currentSerie_(nullptr),
widget(nullptr)
{

}

VideoVisualizer::~VideoVisualizer()
{
/*
	std::cout << "deleting stream" << std::endl;
	stream.release();
	std::cout << "deleting streamImage" << std::endl;
	streamImage.release();
	std::cout << "deleting workspace" << std::endl;
	workspace.release();
	std::cout << "deleting ratioKeeper" << std::endl;
	ratioKeeper.release();
  */
}

plugin::IVisualizer* VideoVisualizer::create() const
{
    return new VideoVisualizer();
}

void VideoVisualizer::getSupportedTypes(utils::TypeInfoList & supportedTypes) const
{
    supportedTypes.push_back(typeid(VideoStream));
	supportedTypes.push_back(typeid(VideoChannel));
	supportedTypes.push_back(typeid(vidlib::Picture));
}

void VideoVisualizer::refresh( float width, float height )
{
    // aktualizacja współczynnika proporcji
    if ( streamImage ) {
        ratioKeeper->setAspectRatio( streamImage->getPixelAspectRatio() * streamImage->s() / streamImage->t() );
    }

    // aktualizacja rozmiaru tła
    workspace->setOrigin(0, 0);
    workspace->resize(width , height);
    workspace->update();
    // aktualizacja tekstury tła
    //workspace->getBackground()->setTexCoordRegion(0, height, width, -height);
	workspace->getBackground()->setTexCoordRegion(0, 0, width, height);

    // aktualizacja rozmiaru tekstury
    if ( streamImage ) {
        // odświeżenia danych zależnych od obrazka
        // resize obrazka
		if (widget->getWidth()) {
			streamImage->setMaxWidth(widget, widget->getWidth());
		}
		if(width > 0 && height > 0){
			workspace->resizeAdd();
		}
    }
}

void VideoVisualizer::updateWidget()
{
    // korekcja współrzędnych tekstury
    if ( streamImage ) {
        if ( useTextureRect ) {
            // sprawdzamy, czy trzeba odświeżyć współrzędne tekstury
            float s = static_cast<float>(streamImage->s());
            float t = static_cast<float>(streamImage->t());
            //
            auto ll = widget->getTexCoord(osgWidget::Widget::LOWER_LEFT);
            auto ur = widget->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
            float oldS = fabs( ll.x() - ur.x() );
            float oldT = fabs( ll.y() - ur.y() );

            // czy trzeba aktualizować?
            if ( oldS != s || oldT != t ) {
                widget->setTexCoord(0, t , osgWidget::Widget::UPPER_LEFT);
                widget->setTexCoord(0, 0 , osgWidget::Widget::LOWER_LEFT);
                widget->setTexCoord(s, t , osgWidget::Widget::UPPER_RIGHT);
                widget->setTexCoord(s, 0 , osgWidget::Widget::LOWER_RIGHT);
              osgui::correctTexCoords(widget);
            }
        }
    }
}

void VideoVisualizer::update( double deltaTime )
{
	if(currentSerie_ == nullptr){
		return;
	}

	bool needsUpdate = false;

	if ( currentStreamTime != prevStreamTime ) {
		prevStreamTime = currentStreamTime;
		needsUpdate = true;		

		stream->setTime(currentStreamTime);		
		getImage();
	}

    if ( streamImage ) {		
		refreshImage();

        int width = streamImage->s();

        if ( prevStreamWidth != width ) {
            prevStreamWidth = width;
            needsUpdate = true;
        }
    }

	if ( needsUpdate ) {		
		viewer->requestRedraw();
	}
}

bool VideoVisualizer::getImage()
{
	streamImage = stream->getImage(vidlib::PixelFormatRGB24);
	return streamImage != nullptr;
}

void VideoVisualizer::refreshImage()
{
	widget->setTexture( stream->getTexture(streamImage->getFormat(), useTextureRect), true, useTextureRect );
	osgui::correctTexCoords(widget, streamImage);
	refresh(viewer->width(), viewer->height());
}

QWidget* VideoVisualizer::createWidget()
{
    using namespace osg;
    using namespace osgWidget;
    using namespace osgui;

    viewer = new QOsgDefaultWidget();
	viewer->setMinimumSize(50,50);
	viewer->setKeyEventSetsDone(0);
    //viewer->setTimerActive(false);

    // pobranie cech kontekstu graficznego
    const GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    // konfiguracja kamery
    Camera* camera = viewer->getCamera();
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::PROTECTED | StateAttribute::OFF);
    camera->setProjectionMatrix(Matrix::ortho2D(0.0, double(traits->width), 0.0f, double(traits->height)));
    camera->setReferenceFrame(Transform::ABSOLUTE_RF);
    camera->setViewMatrix(Matrix::identity());
    camera->setClearColor(Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie WMa
    WindowManager* windowManager = new WindowManager(viewer, float(traits->width), float(traits->height), 0xFF/*, WindowManager::WM_PICK_DEBUG*/);
    viewer->setSceneData(windowManager);
    viewer->addEventHandler( new osgWidget::MouseHandler(windowManager) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(windowManager, camera) );

    // dodanie obszaru roboczego
    workspace = new osgWidget::Box();
    workspace->getBackground()->setImage(plugin::getResourcePath("images/transparent_background.png").string(), false, false);
    osg::Texture* texture = osgui::getTexture(workspace->getBackground());
	if (texture) {
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	}
    windowManager->addChild(workspace);

    // dodanie widgetu
    widget = new Widget("video");
    widget->setUpdateCallback( new WidgetUpdater(this) );
    // ratio keeper
    ratioKeeper = new AspectRatioKeeper(widget, 1);
    workspace->addWidget(ratioKeeper);
	widget->setMinimumSize(50, 50);

	//ratioKeeper->setTarget(widget);

    Refresher refresher = { this };
    workspace->addEventCallback(createEventCallback(osgGA::GUIEventAdapter::RESIZE, refresher));


    refresh(float(viewer->width()), float(viewer->height()));

//#ifdef _DEBUG

    //dodajemy tez event handler ze statystykami
    viewer->addEventHandler( new osgViewer::StatsHandler() );
    //viewer->setTimerActive(true);

//#endif

    viewer->setFocusPolicy(Qt::StrongFocus);

#ifdef __linux__
    // hack, bez tego tworzyl sie  kontekst graficzny w lewym gornym rogu..
    viewer->show();
#endif
    return viewer;
}

QIcon* VideoVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/video/icons/video.png"));
}

int VideoVisualizer::getMaxDataSeries() const
{
    return 1;
}

plugin::IVisualizer::ISerie* VideoVisualizer::createSerie(const utils::TypeInfo & requestedType, const VariantConstPtr & data)
{
    VideoSerie* ret = new VideoSerie(this);

    ret->setName("Video");
    ret->setData(requestedType, data);

    return ret;
}

plugin::IVisualizer::ISerie* VideoVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie)
{
    return nullptr;
}

plugin::IVisualizer::ISerie* VideoVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data)
{
	return nullptr;
}

void VideoVisualizer::removeSerie(plugin::IVisualizer::ISerie* serie)
{
    clear();
	currentSerie_ = nullptr;
}

void VideoVisualizer::clear()
{
	streamImage = nullptr;
	stream = nullptr;
	//ratioKeeper->setTarget(nullptr);
	widget->setColor(0, 0, 0, 0); 

	prevStreamTime = currentStreamTime = -1;

	prevStreamWidth = -1;

	refresh(viewer->width(), viewer->height());
}

QPixmap VideoVisualizer::takeScreenshot() const
{
	return QPixmap::fromImage(viewer->grabFrameBuffer(true));
}

void VideoVisualizer::setActiveSerie(plugin::IVisualizer::ISerie * serie)
{
	currentSerie_ = serie;
}

const plugin::IVisualizer::ISerie * VideoVisualizer::getActiveSerie() const
{
	return currentSerie_;
}

plugin::IVisualizer::ISerie * VideoVisualizer::getActiveSerie()
{
	return currentSerie_;
}
