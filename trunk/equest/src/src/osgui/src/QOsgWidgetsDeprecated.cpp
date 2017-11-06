/* OpenSceneGraph example, osganimate.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgui/QOsgWidgets.h>

#define USE_QT4 0
#define USE_QT5 1

#if USE_QT5

    #include <QtCore/QString>
    #include <QtCore/QTimer>
    #include <QtGui/QKeyEvent>
    #include <QtWidgets/QApplication>
    #include <QtOpenGL/QGLWidget>
    #include <QtWidgets/QMainWindow>
    #include <QtWidgets/QMdiSubWindow>
    #include <QtWidgets/QMdiArea>
    
    using Qt::WindowFlags;

#elif USE_QT4
    #include <QtCore/QString>
    #include <QtCore/QTimer>
    #include <QtGui/QKeyEvent>
    #include <QtGui/QApplication>
    #include <QtOpenGL/QGLWidget>
    #include <QtGui/QMainWindow>
    #include <QtGui/QMdiSubWindow>
    #include <QtGui/QMdiArea>

    using Qt::WindowFlags;
#else

    class QWidget;
    #include <qtimer.h>
    #include <qgl.h>
    #include <qapplication.h>

    #define WindowFlags WFlags

#endif

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

QOsgDeprecatedWidget::QOsgDeprecatedWidget( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f):
#if defined USE_QT4 || defined USE_QT5
    QGLWidget(parent, shareWidget, f)
#else
    QGLWidget(parent, name, shareWidget, f)
#endif
{
    graphicsWindow = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
#if defined USE_QT4 || defined USE_QT5
    setFocusPolicy(Qt::WheelFocus);
#else
    setFocusPolicy(QWidget::ClickFocus);
#endif
    setMouseTracking(true);
}

void QOsgDeprecatedWidget::resizeGL( int width, int height )
{
    graphicsWindow->getEventQueue()->windowResize(0, 0, width, height );
    graphicsWindow->resized(0,0,width,height);
}

void QOsgDeprecatedWidget::keyPressEvent( QKeyEvent* event )
{
#if defined USE_QT4 || defined USE_QT5
    graphicsWindow->getEventQueue()->keyPress( translateKey(event) );
#else
    graphicsWindow->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) event->ascii() );
#endif
}

void QOsgDeprecatedWidget::keyReleaseEvent( QKeyEvent* event )
{
#if defined USE_QT4 || defined USE_QT5
    graphicsWindow->getEventQueue()->keyRelease( translateKey(event) );
#else
    graphicsWindow->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) event->ascii() );
#endif
}

void QOsgDeprecatedWidget::mousePressEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    graphicsWindow->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void QOsgDeprecatedWidget::mouseReleaseEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    graphicsWindow->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}

void QOsgDeprecatedWidget::mouseMoveEvent( QMouseEvent* event )
{
    graphicsWindow->getEventQueue()->mouseMotion(event->x(), event->y());
}

void QOsgDeprecatedWidget::wheelEvent( QWheelEvent* event )
{
    using namespace osgGA;
    GUIEventAdapter::ScrollingMotion motion = event->delta() > 0 ? GUIEventAdapter::SCROLL_UP : GUIEventAdapter::SCROLL_DOWN;    
    graphicsWindow->getEventQueue()->mouseScroll(motion);
}

osgGA::GUIEventAdapter::KeySymbol QOsgDeprecatedWidget::translateKey( QKeyEvent* event )
{
    int key = event->key();
    if ( key == Qt::Key_Alt ) {
        return osgGA::GUIEventAdapter::KEY_Alt_L;
    } else if ( key == Qt::Key_Control ) {
        return osgGA::GUIEventAdapter::KEY_Control_L;
    } else if ( key == Qt::Key_Shift ) {
        return osgGA::GUIEventAdapter::KEY_Shift_L;
    } else {
        return static_cast<osgGA::GUIEventAdapter::KeySymbol>(*event->text().toStdString().c_str());
    }
}

QOsgDeprecatedWidget::~QOsgDeprecatedWidget()
{

}


/*
int mainQOSGWidget(QApplication& a, osg::ArgumentParser& arguments)
{
    // load the scene.
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);
    if (!loadedModel)
    {
        std::cout << arguments[0] <<": No data loaded." << std::endl;
        return 1;
    }
    
    std::cout<<"Using QOsgDeprecatedWidget - QGLWidget subclassed to integrate with osgViewer using its embedded graphics window support."<<std::endl;
    
    if (arguments.read("--CompositeViewer"))
    {
        CompositeViewerQT* viewerWindow = new CompositeViewerQT;

        unsigned int width = viewerWindow->width();
        unsigned int height = viewerWindow->height();
        
        {
            osgViewer::View* view1 = new osgViewer::View;
            view1->getCamera()->setGraphicsContext(viewerWindow->getGraphicsWindow());
            view1->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width)/static_cast<double>(height/2), 1.0, 1000.0);
            view1->getCamera()->setViewport(new osg::Viewport(0,0,width,height/2));
            view1->setCameraManipulator(new osgGA::TrackballManipulator);
            view1->setSceneData(loadedModel.get());
            
            viewerWindow->addView(view1);
        }
        
        {
            osgViewer::View* view2 = new osgViewer::View;
            view2->getCamera()->setGraphicsContext(viewerWindow->getGraphicsWindow());
            view2->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width)/static_cast<double>(height/2), 1.0, 1000.0);
            view2->getCamera()->setViewport(new osg::Viewport(0,height/2,width,height/2));
            view2->setCameraManipulator(new osgGA::TrackballManipulator);
            view2->setSceneData(loadedModel.get());
            
            viewerWindow->addView(view2);
        }

        viewerWindow->show();
#if USE_QT4
    }
    else if (arguments.read("--mdi")) {
          std::cout<<"Using ViewetQT MDI version"<<std::endl;
         
         //Following problems are found here:
         //- miminize causes loaded model to disappear (some problem with Camera matrix? - clampProjectionMatrix is invalid)
         
         ViewerQT* viewerWindow = new ViewerQT;
         viewerWindow->setCameraManipulator(new osgGA::TrackballManipulator);
         viewerWindow->setSceneData(loadedModel.get());
 
         QMainWindow* mw = new QMainWindow();
         QMdiArea* mdiArea = new QMdiArea(mw);
         mw->setCentralWidget(mdiArea);

         QMdiSubWindow *subWindow = mdiArea->addSubWindow(viewerWindow);
         subWindow->showMaximized();
         subWindow->setWindowTitle("New Window");
         mw->show();
#endif // USE_QT4
    } else {
        ViewerQT* viewerWindow = new ViewerQT;

        viewerWindow->setCameraManipulator(new osgGA::TrackballManipulator);
        viewerWindow->setSceneData(loadedModel.get());

        viewerWindow->show();
    }    
    
    
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
   
    return a.exec();
}/**/

/*EOF*/

QOsgDeprecatedViewer::QOsgDeprecatedViewer( QWidget * parent /*= 0*/, const char * name /*= 0*/, const QGLWidget * shareWidget /*= 0*/, Qt::WindowFlags f /*= 0*/ ) : 
QOsgDeprecatedWidget( parent, name, shareWidget, f )
{
    getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
    getCamera()->setProjectionMatrixAsPerspective(45.0, double(width())/height(), 1.0, 10000.0);
    getCamera()->setGraphicsContext(getGraphicsWindow());
    //setThreadingModel(osgViewer::Viewer::SingleThreaded);
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
    updateTimer.start(10);
}

void QOsgDeprecatedViewer::paintGL()
{
    if ( isVisible() ) {
        frame();
    }
}

void QOsgDeprecatedViewer::hideEvent( QHideEvent * event )
{
    QWidget::hideEvent(event);
}

QOsgDeprecatedViewer::~QOsgDeprecatedViewer()
{

}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
