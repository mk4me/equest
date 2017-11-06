#ifndef HEADER_GUARD_VICONFORMATS__VSKPARSER_H__
#define HEADER_GUARD_VICONFORMATS__VSKPARSER_H__

#include <string>
#include <vector>
#include <viconformatslib/Stick.h>
#include <viconformatslib/Marker.h>
#include <utils/SmartPtr.h>

namespace vicon
{
	//! Dane wczytane z pliku vsk
	struct Vsk {

		Vsk() {}
		Vsk(const Vsk & Other) : sticks(Other.sticks), markers(Other.markers) {}
		Vsk(Vsk && Other) : sticks(std::move(Other.sticks)), markers(std::move(Other.markers)) {}

		std::vector<Stick> sticks;
		std::vector<Marker> markers;
	};
	DEFINE_SMART_POINTERS(Vsk);

	/// \brief Klasa realizuje odczyt plików *.vsk
	class VskParser
	{
	public:
		/// \brief  Parsuje podany plik.
		/// \param  filename  Nazwa pliku do parsowania.
		static Vsk parse(std::istream& in);
	};
}

#endif
