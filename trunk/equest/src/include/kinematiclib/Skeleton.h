#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETON_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETON_H__

#include <hanimlib/Humanoid.h>
#include <acclaimformatslib/Skeleton.h>
#include <biovisionformatslib/Skeleton.h>
#include <kinematicUtils/Types.h>
#include <kinematiclib/Topology.h>

namespace kinematic
{
	/// \brief  Klasa przechowuje zunifokowaną hierarchie stawów,
	//! w tej konwencji każda kość musi kończyć się stawem bez dzieci,
	//! tzw. end effector
	//! \version 0.9.1
	class Skeleton
	{
	public:

		//! Klasa reprezentująca staw
		class JointData
		{
		private:
			//! Forward declaration
			class JointDataImpl;

			friend class Skeleton;

		public:

			//! \param other Kopiowane dane innego węzła			
			JointData(const JointData & other);
			//! \param name Nazwa węzła odpowiadająca szkieletowi
			//! \param translation Pozycja|Translacja lokalna stawu
			//! \param rotation Orientacja|Rotacja lokalna stawu			
			JointData(const std::string & name,
				const osg::Vec3d & translation = osg::Vec3d(0,0,0),
				const osg::Quat & rotation = osg::Quat(0,0,0,1));

			//! \param name Nazwa węzła odpowiadająca szkieletowi
			//! \param translation Pozycja|Translacja lokalna stawu
			//! \param rotation Orientacja|Rotacja lokalna stawu			
			JointData(JointData & parent, const std::string & name,
				const osg::Vec3d & translation = osg::Vec3d(0, 0, 0),
				const osg::Quat & rotation = osg::Quat(0, 0, 0, 1));

			//! \param parent Rodzic naszego węzła
			//! \param other Kopiowane dane innego węzła		
			JointData(JointData & parent, const JointData & other);

			//! Konstruktor przenoszący
			JointData(JointData && Other);

			//! Destruktor
			~JointData();
			//! \return Nazwa stawu
			std::string name() const;

			//! \return Globalna pozycja
			osg::Vec3d globalPosition() const;
			//! \return Globalna orientacja
			osg::Quat globalOrientation() const;

			//! \return Lokalna pozycja względem rodzica
			osg::Vec3d localPosition() const;
			//! \return Lokalna orientacja względem rodzica
			osg::Quat localOrientation() const;

			//! \param translation Lokalne przesunięcie
			void localUpdate(const osg::Vec3 & translation);
			//! \param translation Globalne przesunięcie
			void globalUpdate(const osg::Vec3d & translation);
			//! \param position Globalna pozycja
			void setGlobal(const osg::Vec3d & position);
			//! \param position Lokalna pozycja
			void setLocal(const osg::Vec3d & position);

			//! \param rotation Rotacja lokalna
			void localUpdate(const osg::Quat & rotation);
			//! \param rotation Rotacja globalna
			void globalUpdate(const osg::Quat & rotation);
			//! \param orientation Globalna orientacja
			void setGlobal(const osg::Quat & orientation);
			//! \param orientation Lokalna orientacja
			void setLocal(const osg::Quat & orientation);

			//! \param translation Lokalne przesunięcie
			//! \param rotation Rotacja lokalna
			void localUpdate(const osg::Vec3d & translation, const osg::Quat & rotation);
			//! \param translation Globalne przesunięcie
			//! \param rotation Rotacja globalna
			void globalUpdate(const osg::Vec3d & translation, const osg::Quat & rotation);

			//! \param position Globalna pozycja
			//! \param orientation Globalna orientacja
			void setGlobal(const osg::Vec3d & position, const osg::Quat & orientation);
			//! \param position Lokalna pozycja
			//! \param orientation Lokalna orientacja
			void setLocal(const osg::Vec3d & position, const osg::Quat & orientation);
		private:
			//! Prywatna implementacja
			utils::shared_ptr<JointDataImpl> impl;
		};

		//! Typ jointa stanu
		using Joint = treeContainer::NodeT<JointData>;
		//! Typ wskaźników do jointów
		DEFINE_SMART_POINTERS(Joint);

		//! Mapa nazw jointów do ich kolejności rotacji
		using JointsAxis = std::map<std::string, kinematicUtils::AxisOrder::Type>;

	public:

		//! Konstruktor domyślny
		Skeleton();
		//! Konstruktor kopiujący
		//! \param other Kopiowany szkielet - klonujemy węzły
		Skeleton(const Skeleton & other);
		//! Konstruktor przenoszący
		//! \param other przenoszony szkielet
		Skeleton(Skeleton && other);

		Skeleton(JointPtr && root);
		//! Destruktor wirtualny
		~Skeleton();
		//! Staw rodzic reprezentujący cały model
		JointPtr root();
		//! Staw rodzic reprezentujący cały model
		JointConstPtr root() const;

		//! Operator przenoszenia
		Skeleton & operator=(Skeleton && Other);
		//! Operator przypisania
		Skeleton & operator=(const Skeleton & Other);

		//! Konwersja z popularnych formatów
		//! \param srcSkeleton Szkielet acclaim
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const acclaim::Skeleton & srcSkeleton, Skeleton & destSkeleton);
		//! Konwersja z popularnych formatów
		//! \param srcSkeleton Szkielet acclaim
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const Skeleton & srcSkeleton, acclaim::Skeleton & destSkeleton,
			const kinematicUtils::AxisOrder::Type axisOrder,
			const kinematicUtils::AngleUnitType angleUnit,
			const JointsAxis & jointsAxis = JointsAxis());
		//! \param srcSkeleton Szkielet biovision
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const biovision::Skeleton & srcSkeleton, Skeleton & destSkeleton);
		//! \param srcSkeleton Szkielet hAnim
		//! \param destSkeleton [out] Uniwersalny format szkieletu
		//! \return Czy udało się konwertować
		static bool convert(const hAnim::Humanoid & srcSkeleton, Skeleton & destSkeleton);
		//! \param topology Topologia dla której tworzymy szkielet
		//! \return Szkielet na bazie topologii
		static bool convert(TopologyNodeConstPtr topology, Skeleton & skeleton);
		//! \tparam JPtr Typ wskaźnika na joint
		template<typename JPtr>
		//! \param root Głowa struktury szkieletu
		//! \return Mapowanie nazw jointów do ich wskaźników
		static std::map<std::string, JPtr> joints(JPtr root)
		{
			std::map<std::string, JPtr> ret;

			auto v = [&ret](JPtr joint)
			{
				ret.insert({ joint->value().name(), joint });
			};

			treeContainer::VisitPolicies::Node::PreOrder::visit(root, v);

			return ret;
		}

	private:
		//! Root struktury
		JointPtr root_;
	};	
	DEFINE_SMART_POINTERS(Skeleton);
}
#endif
