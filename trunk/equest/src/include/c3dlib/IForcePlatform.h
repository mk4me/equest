/********************************************************************
	created:	2012/03/23
	created:	23:3:2012   10:37
	filename: 	ForcePlatform.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__IFORCEPLATFORM_H__
#define HEADER_GUARD_C3DLIB__IFORCEPLATFORM_H__

#include <boost/range.hpp>
#include <osg/Vec3>
#include <utils/SmartPtr.h>
#include <c3dlib/C3DTypes.h>

namespace c3dlib {

//! Interfejs platformy GRF, obiekty tego typu zasilają system
class IForcePlatform
{
public:
    //! Interfejs wykrytego kroku na płycie pomiarowej.
    //! W tym przypadku jako krok, bierze się moment, w którym cała stopa spoczywa na płycie pomiarowej
    class IStep {
        public:
            virtual ~IStep() {}
            //! \return czas rozpoczęcia kroku
            virtual float getStartTime() const = 0;
            //! \return czas zakończenia kroku
            virtual float getEndTime() const = 0;
            //! \return punkt, w którym znajduje się koniec stopy (pięta)
            virtual osg::Vec3 getStartPoint() const = 0;
            //! \return punkt, w którym znajduje się początek stopy (palce)
            virtual osg::Vec3 getEndPoint() const = 0;
            //! \return czy dane zawarte w klasie są poprawne
            virtual bool isValid() const = 0;
    };
    using IStepPtr = utils::shared_ptr<IStep>;
    using IStepConstPtr = utils::shared_ptr<const IStep>;

    //! wektor z krokami
    using Steps = std::vector<IStepConstPtr>;
    //! niemodyfikowalny zakres kroków z kolekcji
    using StepsConstRange = boost::iterator_range<Steps::const_iterator>;
    //! wektor z kanałami GRF
    using GRFChannels = std::vector<GRFChannelConstPtr>;
    //! niemodyfikowalny zakres kanałów GRF
    using GRFChannelsConstRange = boost::iterator_range<GRFChannels::const_iterator>;

public:
	virtual ~IForcePlatform() {}

public:
    //! \return wykryte kroki (w rozumieniu IStep)
    virtual StepsConstRange getSteps() const = 0;
    //! \return środek płyty pomiarowej
    virtual osg::Vec3 getCenter() const = 0;
    //! \return szerokość płyty pomiarowej
    virtual float getWidth() const = 0;
    //! \return długość płyty pomiarowej
    virtual float getLength() const = 0;
    //! Metoda pozwala określić orientacje płyty pomiarowej
    //! \return kolejność wystąpienia wierzchołków (lewy prawy = 1, prawy lewy = -1)
    virtual float getSignX() const = 0;
    //! Metoda pozwala określić orientacje płyty pomiarowej
    //! \return kolejność wystąpienia wierzchołków (góra dol = 1, dol góra = -1)
    virtual float getSignY() const = 0;
    //! \return kanał z odpowiadająca płycie siła
    virtual GRFChannelConstPtr getForceChannel() const = 0;
    //! \return kanał z odpowiadająca płycie momentem siły
    virtual GRFChannelConstPtr getMomentChannel() const = 0;
    //! Metoda liczy odległość danego punktu od srodka płyty pomiarowej
    //! \param vec dany punkt
    //! \return zmierzona odległość
    float getDistanceToCenter(const osg::Vec3& vec) const {
        return (getCenter() - vec).length();
    }
};
using IForcePlatformPtr = utils::shared_ptr<IForcePlatform>;
using IForcePlatformConstPtr = utils::shared_ptr<const IForcePlatform>;
using IForcePlatformCollection = std::vector<IForcePlatformPtr>;

}

#endif
