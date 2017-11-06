#ifndef HEADER_GUARD_acclaimMODEL__JOINT_H__
#define HEADER_GUARD_acclaimMODEL__JOINT_H__

#include <vector>
#include <osg/Vec3>
#include <utils/SmartPtr.h>
#include <boost/utility.hpp>
#include <acclaimformatslib/DegreeOfFreedom.h>
#include <kinematicUtils/RotationConverter.h>

namespace acclaim
{
	//! Struktura przechowuje informacje o stawie
	struct Bone
	{
		//! Typ identyfikatora kości
		using ID = int;
		//! Stopnie swobody
		using Dofs = std::vector < DegreeOfFreedom > ;

		//! Struktura z danymi pomocniczymi do wyznaczania ruchu
		struct HelperMotionData
		{
			osg::Quat c;
			osg::Quat cInv;
		};

		//! Domyślna orientacja kości
		static const osg::Vec3d defaultAxis;
		//! Domyślna kolejnośc osi
		static const kinematicUtils::AxisOrder::Type defaultAxisOrder = kinematicUtils::AxisOrder::XYZ;		
		//! Domyślna reprezentacja kątów
		static const kinematicUtils::AngleUnitType defaultAngleUnit = kinematicUtils::Deg;

		//! unikalny identyfikator stawu
		ID id;
		//! nazwa stawu
		std::string name;
		//! kierunek kości w globalnym/lokalnym? układzie odniesienia
		osg::Vec3d direction;
		//! długość kości
		double length;
		//! określa orientację referencyjną kości - obraca się wg orientacji danych z AMC
		osg::Vec3d axis;
		//! określa kolejność rotacji dla osi obrotu
		kinematicUtils::AxisOrder::Type axisOrder;		
		//! stopnie swobody dla kości (razem z limitami)
		Dofs dofs;
		//! \return Czy kość jest aktywna - będa dla niej dane w pliku AMC
		const bool isActive() const;
		//! \return Kolejność rotacji
		const kinematicUtils::AxisOrder::Type rotationOrder() const;
		//! \param bone Kość
		//! \param angleInRadians Czy kąty sa w radianach
		//! \return Dane pomocnicze przy przetwarzaniu danych ruchu
		static HelperMotionData helperData(const Bone & bone, const bool angleInRadians);
		//! \param channel Typ kanału, musi to być kanał z rotacjami
		//! \param angleUnit Jednostki dla kątów
		//! \return Domyslny stopień swobody dla kąta
		static DegreeOfFreedom defaultRotationDof(const kinematicUtils::Channel channel,
			const kinematicUtils::AngleUnitType angleUnit);
		//! \param angleUnit Jednostki dla kątów
		//! \return Domyslne stopnie swobody dla obracających się kości
		static Dofs defaultRotationDofs(const kinematicUtils::AngleUnitType angleUnit);
		//! \param angleUnit Jednostki dla kątów
		//! \return Domyslne stopnie swobody dla roota
		static Dofs defaultRootDofs(const kinematicUtils::AngleUnitType angleUnit);

		//! Domyslny konstruktor
		Bone();

		//! \param Other Kopiowana kość
		Bone(const Bone & Other);

		//! \param Other Przenoszona kość
		Bone(Bone && Other);

		//! Desturktor
		~Bone();
	};
}

#endif
