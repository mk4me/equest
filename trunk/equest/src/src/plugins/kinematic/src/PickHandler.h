/********************************************************************
	created:	2012/03/09
	created:	9:3:2012   12:10
	filename: 	PickHandler.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__PICKHANDLER_H__
#define HEADER_GUARD_KINEMATIC__PICKHANDLER_H__


#include <osg/Geode>
#include <osg/Drawable>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgViewer/View>

#include <boost/tuple/tuple.hpp>
#include <functional>
#include <list>

//! klasa pomocnicza, dzięki niej można łatwo podpiąć się pod system pickingu
class PickHandler : public osgGA::GUIEventHandler
{
public:
    //! wskaźnik do węzła przechowywującego geometrię
    typedef osg::Geode* GeodePtr;
    //! wskaźnik do obiektu, który może zostać odrysowany na scenie
    typedef osg::ref_ptr<osg::Drawable> DrawablePtr;
    //! krotka przechowująca : węzeł, jego obiekt, który został kliknięty, doległość od obserwatora
    typedef boost::tuple<GeodePtr, DrawablePtr, double> PickerTuple;
    //! lista obiektów, które zostały "kliknięte"
    typedef std::list<PickerTuple> PickerList;
    //! funktor realizujący jakąś akcję po kliknięciu
    typedef std::function<void (const PickerList& )> HandlerFunction;

public:
    //! Konstruktor
    //! \param callback funkcja, która zostanie wywołana za każdym razem, gdy kliknie się obiekt na scenie
    PickHandler(HandlerFunction callback);

public:
    //! Obsługa eventów
    //! \param ea eventy myszki, klawiatury lub okna
    //! \param aa interfejs obsługi akcji
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
    //! Metoda realizująca faktyczny picking
    //! \param view widok sceny
    //! \param ea eventy myszki, klawiatury lub okna
    virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

protected:
    //! funktor realiuzjący pewną akcję po kliknięciu
    HandlerFunction handlerFunction;
};



#endif
