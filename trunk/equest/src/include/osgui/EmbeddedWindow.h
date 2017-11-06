/********************************************************************
	created:  2010/11/29
	created:  29:11:2010   16:42
	filename: EmbeddedWindow.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__EMBEDDEDWINDOW_H__
#define HEADER_GUARD__EMBEDDEDWINDOW_H__

#include <string>
#include <type_traits>
#include <osg/CopyOp>
#include <osgWidget/Window>
#include <utils/Debug.h>
#include <osgui/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

template <class T>
class Embedded;

/**
 *	Generalizacja osgWidget::Window::EmbeddedWindow
 */
class EmbeddedWindow : public osgWidget::Window::EmbeddedWindow
{
public:
    typedef osgWidget::Window::EmbeddedWindow Base;
    //META_Object(osgui, EmbeddedWindow);
	META_Widget(osgui, EmbeddedWindow);

    //! \param name
    //! \param w
    //! \param h
    EmbeddedWindow( const std::string& name = "", osgWidget::point_type w = 0, osgWidget::point_type h = 0 );
    //! \param name
    //! \param window
    EmbeddedWindow( const std::string& name, osgWidget::Window* window );
    //! Konstruktor kopiujący EmbeddedWindow nie ma konstruktora kopiującego!
    //! \param widget
    //! \param copyop
    EmbeddedWindow( const EmbeddedWindow& widget, const osg::CopyOp& copyop);

    //! Kopia metody osgWidget::Window::embed
    //! \param window
    //! \param newName
    //! \param layer
    //! \param offset
    template <class T>
    static Embedded<T>* embed( T* window, 
        const std::string& newName = "", 
        osgWidget::Widget::Layer layer = osgWidget::Widget::LAYER_MIDDLE, 
        unsigned layerOffset = 0 );
};

/**
 *	Generalizacja osgWidget::Window::EmbeddedWindow. Automatycznie ustawia sobie kolor tła na (0,0,0,0).
 */
template <class T>
class Embedded : public EmbeddedWindow
{
public:
    typedef EmbeddedWindow Base;
    META_Object(osgui, Embedded);

    //! \param name
    //! \param w
    //! \param h
    Embedded( const std::string& name = "", osgWidget::point_type w = 0, osgWidget::point_type h = 0 ) :
    Base(name, w, h)
    {}

    //! \param name
    //! \param window
    Embedded( const std::string& name, T* window ) :
    Base(name, window)
    {}

    //! Konstruktor kopiujący EmbeddedWindow nie ma konstruktora kopiującego!
    //! \param widget
    //! \param copyop
    Embedded( const Embedded& widget, const osg::CopyOp& copyop) :
    Base(widget, copyop)
    {}

public:
    //! \return Wewnętrzne okno.
    T* getOrCreateWindow()
    {
        if ( Base::getWindow() ) {
            setWindow( new T(getName() + "Window") );
        }
        return getWindow();
    }
    //! \return Wewnętrzne okno.
    T* getWindow()
    {
        return dynamic_cast<T*>(Base::getWindow());
    }
    //! \return Wewnętrzne okno.
    const T* getWindow() const
    {
        return dynamic_cast<const T*>(Base::getWindow());
    }
    //! \param window Wewnętrzne okno.
    void setWindow(T* window)
    {
        Base::setWindow(window);
    }
};

template <class T>
static Embedded<T>* EmbeddedWindow::embed( T* window, 
                                 const std::string& newName /*= ""*/, 
                                 osgWidget::Widget::Layer layer /*= osgWidget::Widget::LAYER_MIDDLE*/, 
                                 unsigned layerOffset /*= 0*/ )
{
    static_assert((std::is_base_of<osgWidget::Window, T>::value), "Base class should inherit from osgWidget::Window");
    UTILS_ASSERT(window);
    Embedded<T>* ew = new Embedded<T>( 
        newName.size() > 0 ? newName : window->getName() + "Embedded",
        window->getWidth(),
        window->getHeight()
    );
    ew->setWindow(window);
    ew->setSize( window->getWidth(), window->getHeight() );
    ew->setCanFill(true);
    ew->setLayer(layer, layerOffset);
    return ew;
}


////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__EMBEDDEDWINDOW_H__
