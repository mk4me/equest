/********************************************************************
	created:  2010/10/15
	created:  15:10:2010   15:04
	filename: QOsgDeprecatedWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__QOSGWIDGET_H__
#define __HEADER_GUARD__QOSGWIDGET_H__

#include <QtOpenGL/QGLWidget>
#include <QtCore/QTimer>
#include <osgViewer/Viewer>
#include <osgViewer/GraphicsWindow>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////


/**
 *	Adapter pozwalający na włączenie widgetów z Qt do logiki Osg.
 */
class QOsgDeprecatedWidget : public QGLWidget
{
protected:
    //! Okno z punktu widzenia Osg.
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow;

public:
    //! Konstruktor w postaci właściwej Qt.
    QOsgDeprecatedWidget( QWidget * parent = 0, const char * name = 0, 
        const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 );

    //! Pusty polimorficzny destruktor.
    virtual ~QOsgDeprecatedWidget();

public:
    //! \return Okno z punktu widzenia Osg.
    inline osgViewer::GraphicsWindow* getGraphicsWindow() 
    { 
        return graphicsWindow.get(); 
    }
    //! \return Okno z punktu widzenia Osg.
    inline const osgViewer::GraphicsWindow* getGraphicsWindow() const 
    { 
        return graphicsWindow.get(); 
    }

protected:
    virtual void resizeGL( int width, int height );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void keyReleaseEvent( QKeyEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent* event );

    osgGA::GUIEventAdapter::KeySymbol translateKey( QKeyEvent* event );
};

class QOsgDeprecatedViewer : public QOsgDeprecatedWidget, public osgViewer::Viewer
{
private:
    QTimer updateTimer;

public:
    QOsgDeprecatedViewer(QWidget * parent = 0, const char * name = 0, 
        const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);

    virtual ~QOsgDeprecatedViewer();

    //! Metoda usprawniająca testowanie różnych wariantów łączenia OSG/QT.
    void onInit(osgViewer::CompositeViewer* composite)
    {
    }

    void setRenderingEnabled(bool enabled)
    {
        if ( !enabled ) {
            updateTimer.stop();
        } else {
            updateTimer.start();
        }
    }

protected:
    virtual void paintGL();
    virtual void hideEvent ( QHideEvent * event ) ;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__QOSGWIDGET_H__
