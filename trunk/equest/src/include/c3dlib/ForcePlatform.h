/********************************************************************
	created:	2012/03/24
	created:	24:3:2012   13:12
	filename: 	ForcePlatform.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_ForcePlaform__FORCEPLATFORM_H__
#define HEADER_GUARD_ForcePlaform__FORCEPLATFORM_H__

#include <c3dlib/C3DParser.h>
#include <c3dlib/IForcePlatform.h>
#include <c3dlib/C3DTypes.h>
#include <c3dlib/C3DCollectionTypes.h>
#include <c3dlib/Export.h>

namespace c3dlib {
// Klasa reprezentuje platforme GRF
class C3DLIB_EXPORT ForcePlatform : public IForcePlatform
{
public:
    //! typ zdarzenia wczytanego z pliku C3D
    using IEvent = C3DParser::IEvent;
    //! kontekst zdarzenia (strona lewa, prawa, ... )
    using Context = C3DParser::IEvent::Context;

    //! Reprezentacja wykrytego kroku na płycie pomiarowej.
    //! W tym przypadku jako krok, bierze się moment, w którym cała stopa spoczywa na płycie pomiarowej
    class Step : public IStep
    {
    public:
        //! domyślny konstruktor, krok będzie nie zainicjalizowany
        Step() : startTime(-1.0f), endTime(-1.0f), context(ForcePlatform::IEvent::General) {}
        //! Kontruktor, który tworzy zainicjalizowana instancje kroku
        //! \param t1 czas rozpoczęcia kroku
        //! \param t2 czas zakończenia kroku
        //! \param c konkekst kroku (lewy , prawy)
        Step(float t1, float t2, ForcePlatform::Context c) : startTime(t1), endTime(t2), context(c) {}

    public:
        //! \return czas rozpoczęcia kroku
        virtual float getStartTime() const { return startTime; }
        //! \param val czas rozpoczęcia kroku
        virtual void setStartTime(float val) { startTime = val; }
        //! \return czas zakończenia kroku
        virtual float getEndTime() const { return endTime; }
        //! \param val czas zakończenia kroku
        virtual void setEndTime(float val) { endTime = val; }
        //! \return punkt, w którym znajduje się koniec stopy (pięta)
        virtual osg::Vec3 getStartPoint() const { return startPoint; }
        //! \param val punkt, w którym znajduje się koniec stopy (pięta)
        virtual void setStartPoint(const osg::Vec3& val) { startPoint = val; }
        //! \return punkt, w którym znajduje się początek stopy (palce)
        virtual osg::Vec3 getEndPoint() const { return endPoint; }
        //! \param val punkt, w którym znajduje się początek stopy (palce)
        virtual void setEndPoint(const osg::Vec3& val) { endPoint = val; }
        //! \return czy dane zawarte w klasie są poprawne
        virtual bool isValid() const { return startTime >= 0.0f && endTime >= 0.0f; }
        //! \return konkekst kroku (lewy , prawy)
        ForcePlatform::Context getContext() const { return context; }
        //! \param val konkekst kroku (lewy , prawy)
        void setContext(ForcePlatform::Context val) { context = val; }

    private:
        //! czas rozpoczęcia kroku
        float startTime;
        //! czas zakończenia kroku
        float endTime;
        //! punkt, w którym znajduje się koniec stopy (pięta)
        osg::Vec3 startPoint;
        //! punkt, w którym znajduje się początek stopy (palce)
        osg::Vec3 endPoint;
        //! konkekst kroku (lewy , prawy)
        Context context;
    };

public:
    //! Konstruktor, inicjalizuje klasę na podstawie surowych danych platformy
    //! \param platform platforma dostarczona przez parser c3d
	ForcePlatform(ForcePlatformStructPtr platform);

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const;
    //! \return środek płyty pomiarowej
    virtual osg::Vec3 getCenter() const;
    //! \return szerokość płyty pomiarowej
    virtual float getWidth() const;
    //! \return długość płyty pomiarowej
    virtual float getLength() const;
    //! Metoda pozwala określić orientację płyty pomiarowej
    //! \return kolejność wystąpienia wierzchołków (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const;
    //! Metoda pozwala określić orientację płyty pomiarowej
    //! \return kolejność wystąpienia wierzchołków (góra dol = 1, dol góra = -1)
    virtual float getSignY() const;
    //! Algorytm stara się wykryć wszystkie kroki(w rozumieniu Step) zwiazene z płyta GRF
    //! \param markers kolekcja z markerami, potrzebna aby pobrać markery związane ze stopą
    //! \param events kolekcja ze zdarzeniami, pomocne przy wykrywaniu kroków 
    void computeSteps( MarkerCollectionPtr markers, EventsCollectionConstPtr events );
    //! \return kanał z odpowiadająca płycie siłą
    virtual GRFChannelConstPtr getForceChannel() const;
    //! \return kanał z odpowiadająca płycie momentem siły
    virtual GRFChannelConstPtr getMomentChannel() const;
    //! ustawia kanał z odpowiadająca płycie siłą
    virtual void setForceChannel(GRFChannelConstPtr val)  { force = val; }
    //! ustawia kanał z odpowiadająca płycie momentem siły
    virtual void setMomentChannel(GRFChannelConstPtr val) { moment = val; }

private:
    //! Sprawdza czy punkt znajduje się w obrębie płyty (wysokość nie ma znaczenia)
    //! \param v sprawdzany punkt
    bool isInsideXY( const osg::Vec3& v) const;

private:
    //! obiekt pochodzący wprost z parsera c3d, surowe dane platformy
    ForcePlatformStruct platform;
    //! kolekcja z krokami (w rozumieniu klasy Step)
    Steps steps;
    //! kanał z odpowiadająca płycie siła
    GRFChannelConstPtr force;
    //! kanał z odpowiadająca płycie momentem siły
    GRFChannelConstPtr moment;
};
using ForcePlatformPtr = utils::shared_ptr<ForcePlatform>;
using ForcePlatformConstPtr = utils::shared_ptr<const ForcePlatform>;
}

#endif
