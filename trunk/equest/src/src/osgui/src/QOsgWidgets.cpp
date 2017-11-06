#include "PCH.h"
#include <osgViewer/Renderer>
#include <osgui/QOsgWidgets.h>
#include <osgui/DisableableRenderer.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

QOsgContextWidget::QOsgContextWidget( QWidget * parent, const osg::GraphicsContext::Traits* traits /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
osgQt::GLWidget( translateFormat( traits ? *traits : osg::GraphicsContext::Traits(osg::DisplaySettings::instance()) ), parent, NULL, f )
{
    osg::GraphicsContext::Traits* t = new osg::GraphicsContext::Traits;
    *t = traits ? *traits : osg::GraphicsContext::Traits(osg::DisplaySettings::instance());

    // korekcja rozmiaru
    if ( t->width == 0 && t->height == 0 ) {
        t->width = width();
        t->height = height();
    }
    UTILS_ASSERT(t->inheritedWindowData == NULL);
    t->inheritedWindowData = new osgQt::GraphicsWindowQt::WindowData( this );

    // wygląda dziwnie, ale w kontruktorze ustawiana jest zmienna _gw
    osgQt::GraphicsWindowQt* temp = new osgQt::GraphicsWindowQt(t);
    UTILS_ASSERT(temp == _gw);

    // wyłączenie odrysowywanie tła
    setAttribute( Qt::WA_OpaquePaintEvent );
}

QOsgContextWidget::~QOsgContextWidget()
{
}

QGLFormat QOsgContextWidget::translateFormat( const osg::GraphicsContext::Traits& traits )
{
    const osg::GraphicsContext::Traits* _traits = &traits;
    // kod zaczerpniety z osgQt/GraphicsWindowQt.cpp
    QGLFormat format( QGLFormat::defaultFormat() );
    format.setAlphaBufferSize( _traits->alpha );
    format.setRedBufferSize( _traits->red );
    format.setGreenBufferSize( _traits->green );
    format.setBlueBufferSize( _traits->blue );
    format.setDepthBufferSize( _traits->depth );
    format.setStencilBufferSize( _traits->stencil );
    format.setSampleBuffers( _traits->sampleBuffers );
    format.setSamples( _traits->samples );
    format.setAlpha( _traits->alpha>0 );
    format.setDepth( _traits->depth>0 );
    format.setStencil( _traits->stencil>0 );
    format.setDoubleBuffer( _traits->doubleBuffer );
    format.setSwapInterval( _traits->vsync ? 1 : 0 );
    return format;
}


QOsgView::QOsgView( QWidget * parent /*= 0*/, const osg::GraphicsContext::Traits* traits /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
QOsgContextWidget(parent, traits, f)
{
    init();
}

void QOsgView::init()
{
    int width = getGraphicsWindow()->getTraits()->width;
    int height = getGraphicsWindow()->getTraits()->height;

    _camera->setGraphicsContext( getGraphicsWindow() );
    _camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    _camera->setViewport( new osg::Viewport(0, 0, width, height) );
    _camera->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width)/height, 1.0f, 10000.0f );

}

//! \param camera
osg::GraphicsOperation* QOsgView::createRenderer(osg::Camera* camera)
{
    DisableableRenderer* render = new DisableableRenderer(camera);
    camera->setStats(new osg::Stats("Camera"));
    return render;
}

bool QOsgView::isRenderingEnabled() const
{
    if ( const osg::Camera* camera = getCamera() ) {
        if ( const DisableableRenderer* renderer = dynamic_cast<const DisableableRenderer*>(camera->getRenderer()) ){
            return renderer->isRenderingEnabled();
        }
    }
    return true;
}

void QOsgView::setRenderingEnabled( bool renderingEnabled )
{
    if ( osg::Camera* camera = getCamera() ) {
        if ( DisableableRenderer* renderer = dynamic_cast<DisableableRenderer*>(camera->getRenderer()) ){
            renderer->setRenderingEnabled(renderingEnabled);
        }
    }
}

QOsgViewer::QOsgViewer( QWidget * parent /*= 0*/, const osg::GraphicsContext::Traits* traits /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
QOsgContextWidget(parent, traits, f), skipFramesIfInvisible(true)
{
    init();
}

void QOsgViewer::init()
{
	setThreadingModel(osgViewer::Viewer::SingleThreaded);
	setRunFrameScheme(osgViewer::ViewerBase::ON_DEMAND);
	setKeyEventSetsDone(0);
    int width = getGraphicsWindow()->getTraits()->width;
    int height = getGraphicsWindow()->getTraits()->height;

    _camera->setGraphicsContext( getGraphicsWindow() );
    _camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    _camera->setViewport( new osg::Viewport(0, 0, width, height) );
    _camera->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width)/height, 1.0f, 10000.0f );

    frameTimer.setInterval(defaultInterval);    
    connect(&frameTimer, SIGNAL(timeout()), this, SLOT(update()));
    frameTimer.start();
}

//! \param camera
osg::GraphicsOperation* QOsgViewer::createRenderer(osg::Camera* camera)
{
    DisableableRenderer* render = new DisableableRenderer(camera);
    camera->setStats(new osg::Stats("Camera"));
    return render;
}

bool QOsgViewer::isRenderingEnabled() const
{
    if ( const osg::Camera* camera = getCamera() ) {
        if ( const DisableableRenderer* renderer = dynamic_cast<const DisableableRenderer*>(camera->getRenderer()) ){
            return renderer->isRenderingEnabled();
        }
    }
    return false;
}

void QOsgViewer::setRenderingEnabled( bool renderingEnabled )
{
    if ( osg::Camera* camera = getCamera() ) {
        if ( DisableableRenderer* renderer = dynamic_cast<DisableableRenderer*>(camera->getRenderer()) ){
            renderer->setRenderingEnabled(renderingEnabled);
        }
    }
}

QOsgViewer::~QOsgViewer()
{
}

void QOsgViewer::paintEvent( QPaintEvent* event )
{
    if (!skipFramesIfInvisible || (isVisible() && !isHidden()) ) {		
		
		// avoid excessive CPU loading when no frame is required in ON_DEMAND mode
		if (getRunFrameScheme() == osgViewer::ViewerBase::ON_DEMAND)
		{			
			if (_lastFrameStartTime.time_s() < 0.01)
				return;
		}

		// record start frame time
		_lastFrameStartTime.setStartTick();

		// make frame
		if (getRunFrameScheme() == osgViewer::ViewerBase::ON_DEMAND)
		{
			if (checkNeedToDoFrame())
			{
				frame();
			}
		}
		else
		{
			frame();
		}
    }
}

void QOsgViewer::setTimerActive( bool active )
{
    if ( active != isTimerActive() ) {
        if ( active ) {
            frameTimer.start();
        } else {
            frameTimer.stop();
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
