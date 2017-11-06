/********************************************************************
	created:	2012/03/25
	created:	25:3:2012   20:51
	filename: 	EventSerieBase.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__EVENTSERIEBASE_H__
#define HEADER_GUARD_C3DPLUGIN__EVENTSERIEBASE_H__

#include <plugins/c3d/Export.h>
#include <corelib/IVisualizer.h>
#include <plugins/c3d/C3DCollections.h>

//! klasa dostarcza eventów do podstawowej serii danych.
//! jeśli seria danych ma je obsługiwać, powinna dziedziczyć po tej klasie
class PLUGINC3D_EXPORT EventSerieBase : public plugin::IVisualizer::ISerie, public plugin::IVisualizer::ITimeAvareSerieFeatures
{
public:

    //! Abstraktycja metoda, seria danych po jej wywołaniu powinna zainicjalizować sobie obsługę eventów
    //! \param val ustawiane eventy
    virtual void setEvents(c3dlib::EventsCollectionConstPtr val) = 0;
    //! \return kontekst, dla którego aktualnie rozpatrywane są jointy (typowo left, right)
	c3dlib::C3DEventsCollection::Context getContext() const;
    //! ustawia kontekst, dla którego aktualnie rozpatrywane są jointy 
    //! \param val kontekst, typowo left, right
	void setContext(c3dlib::C3DEventsCollection::Context val);

private:
    //! kontekst, typowo left, right
	c3dlib::C3DEventsCollection::Context context;

};
typedef utils::shared_ptr<EventSerieBase> EventSerieBasePtr;
typedef utils::shared_ptr<const EventSerieBase> EventSerieBaseConstPtr;

#endif
