#ifndef HEADER_GUARD_acclaimMODEL__ASFPARSER_H__
#define HEADER_GUARD_acclaimMODEL__ASFPARSER_H__

#include <string>
#include <acclaimformatslib/Skeleton.h>

namespace acclaim
{
	/*!
	 * \brief dzięki tej klasie możliwe jest wczytanie i zapisywanie plików *.asf
	 */
	class AsfParser
	{
	public:
		//! \param stream Strumień który parsujemy
		//! \param createUniqueIndicies Czy generować unikalne indeksy kości czy używać tych z pliku
		static Skeleton parse(std::istream & stream, const bool createUniqueIndicies);
		//! \param stream Strumień do którego zapisujemy szkielet
		//! \param model Model szkieletu który zapisujemy
		static void serialize(std::ostream & stream, const Skeleton & model);
	};
}

#endif
