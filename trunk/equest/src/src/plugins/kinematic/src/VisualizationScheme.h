/********************************************************************
	created:	2012/04/23
	created:	23:4:2012   12:45
	filename: 	VisualizationScheme.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__VISUALIZATIONSCHEME_H__
#define HEADER_GUARD_KINEMATIC__VISUALIZATIONSCHEME_H__

#include <viconformatslib/VskParser.h>

//! Pomocnicza klasa do obsługi plików vsk. Likwiduje konieczność ich wielokrotnego parsowania
class Vsk
{
public:
    //! Okresla przygotowane wczesniej zestawy danych
	enum MarkersCount
	{
        MarkersCount15 = 15,
        MarkersCount16 = 16,
		MarkersCount39 = 39,
		MarkersCount53 = 53
	};

public:
	//! dzięki tej metodzie można pobrać przeparsowany plik vsk z obsługiwanych schematów. W przypadku niepowodzenia rzucany jest wyjątek
	//! \param count dostępny schemat
    //! \return Parser z wczytaną strukturą
	static vicon::VskConstPtr get(MarkersCount count);

private:
    //! przygotowany schemat dla 16 markerów
	static vicon::VskPtr Count16;
    //! przygotowany schemat dla 39 markerów
	static vicon::VskPtr Count39;
    //! przygotowany schemat dla 53 markerów
	static vicon::VskPtr Count53;
};

#endif
