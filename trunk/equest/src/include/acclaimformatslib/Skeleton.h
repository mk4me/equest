#ifndef HEADER_GUARD_acclaimMODEL__SKELETON_H__
#define HEADER_GUARD_acclaimMODEL__SKELETON_H__

#include <map>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <utils/SmartPtr.h>
#include <acclaimformatslib/Bone.h>
#include <acclaimformatslib/DegreeOfFreedom.h>
#include <kinematicUtils/RotationConverter.h>
#include <osg/Vec3>

namespace acclaim
{
	/// \brief  Klasa przechowuje hierarchie kości
	struct Skeleton
	{
		//! Typ indeksujący kości wg ich identyfikatorów
		using Bones = std::map<Bone::ID, Bone>;
		//! Typ opisujący hierarchię kości - relacja rodzic <-> dziecko
		using Hierarchy = boost::bimap<boost::bimaps::multiset_of<Bone::ID>, boost::bimaps::set_of<Bone::ID>>;

		//! Typ indeksujący kości wg ich identyfikatorów
		using Mapping = boost::bimap < Bone::ID, std::string >;

		//! Dane pomocnicze dla formatu acclaim
		using HelperMotionData = std::map < Bone::ID, Bone::HelperMotionData > ;

		//! Struktura opisująca sekcję Units
		struct Units
		{
			//! Mapa klucz - opis
			std::map<std::string, std::string> typeValueUnits;
			//! Mapa klucz - wartość
			std::map<std::string, double> defaultValues;
			//! Konstruktor domyślny
			Units();

			Units(const Units & Other);
			Units(Units && Other);

			//! Destruktor
			~Units();

			Units& operator=(const Units& Other);
			Units& operator=(Units&& Other);

			//! \return Czy kąt podawany jest w radianach
			const bool isAngleInRadians() const;
			//! \param angleType Typ kąta
			void setAngleType(const kinematicUtils::AngleUnitType angleType);
		};

		//! Konstruktor domyślny
		Skeleton();

		Skeleton(const Skeleton & Other);
		Skeleton(Skeleton && Other);

		//! Destruktor
		~Skeleton();

		Skeleton& operator=(const Skeleton& Other);
		Skeleton& operator=(Skeleton&& Other);

		//! Wersja formatu
		std::string version;
		//! Nazwa szkieletu
		std::string name;
		//! Dokumentacja
		std::string documentation;
		//! Jednostki i opis
		Units units;
		//! korzeń, czyli kość od której zaczyna się hierarchia.
		Bone::ID root;
		//! Kości
		Bones bones;
		//! Hierarchia
		Hierarchy hierarchy;
		//! Początkowa pozycja w globalnym układzie odniesienia
		osg::Vec3d position;
		//! Początkowa orientacja w globalnym układzie odniesienia
		osg::Vec3d orientation;
		//! Kolejność rotacji (sekcja axis)
		kinematicUtils::AxisOrder::Type axisOrder;
		//! Kolejność rotacji (sekcja order)
		std::vector<kinematicUtils::Channel> dataOrder;
		//! \param skeleton Szkielet
		//! \return Kolejność rotacji
		static const kinematicUtils::AxisOrder::Type rotationOrder(const Skeleton & skeleton);
		//! \param skeleton Szkielet
		//! \return Aktywne kości (razem z rootem)
		static const std::size_t activeBones(const Skeleton & skeleton);
		//! \param skeleton Szkielet
		//! \return Dane pomocnicze dla wyznaczania ruchu
		static HelperMotionData helperMotionData(const Skeleton & skeleton);
		//! \param bones Kości
		//! \return Mapowanie kości
		static Mapping createMapping(const Bones & bones);

	};
	DEFINE_SMART_POINTERS(Skeleton);
}
#endif
