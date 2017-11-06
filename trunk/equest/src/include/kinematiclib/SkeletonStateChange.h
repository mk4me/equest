/********************************************************************
	created:  2015/04/29	15:12:21
	filename: SkeletonStateChange.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__SKELETONSTATECHANGE_H__
#define __HEADER_GUARD_KINEMATIC__SKELETONSTATECHANGE_H__

#include <kinematiclib/SkeletonState.h>

namespace kinematic
{
	//! \version 0.9.1
	struct SkeletonStateChange
	{
	public:
		//! Kompletne rotacje szkieletu wg kolejności aktywnego mapowania liniowego
		using CompleteRotations = std::vector < osg::Quat > ;
		//! Częsciowe rotacje szkieletu
		using PartialRotations = std::map < LinearizedSkeleton::NodeIDX, osg::Quat > ;
		//! Struktura opisująca prosta zmianę w szkielecie - translacja roota i lokalne rotacje stawów
		//! pozycje rotacji w wektorze wg mapowania
		struct RigidCompleteStateChange
		{
			//! Translacja roota
			osg::Vec3 translation;
			//! Lokalne rotacje wszystkich stawów
			CompleteRotations rotations;
		};

		using RigidCompleteStateChangeLocal = LocalData < RigidCompleteStateChange > ;
		using RigidCompleteStateChangeGlobal = GlobalData < RigidCompleteStateChange >;

		//! Struktura opisująca prosta zmianę w szkielecie - translacja roota i lokalne rotacje stawów
		//! rotacje wg mapowania
		struct RigidPartialStateChange
		{
			//! Translacja roota
			osg::Vec3 translation;
			//! Lokalne rotacje wybranych stawów
			PartialRotations rotations;
		};

		using RigidPartialStateChangeLocal = LocalData < RigidPartialStateChange >;
		using RigidPartialStateChangeGlobal = GlobalData < RigidPartialStateChange >;

		//! Struktura opisująca zmianę stawu
		struct NonRigidJointStateChange
		{
			//! Translacja
			osg::Vec3 translation;
			//! Rotacja
			osg::Quat rotation;
		};

		//! Typ opisujący pełną zmianę stanu szkieletu
		//! pozycja zmian wg mapowania
		using NonRigidCompleteStateChange = std::vector<NonRigidJointStateChange>;

		using NonRigidCompleteStateChangeLocal = LocalData < NonRigidCompleteStateChange >;
		using NonRigidCompleteStateChangeGlobal = GlobalData < NonRigidCompleteStateChange >;

		//! Typ opisujący pełną zmianą stanu szkieletu
		//! zmiany wg mapowania
		using NonRigidPartialStateChange = std::map<LinearizedSkeleton::NodeIDX, NonRigidJointStateChange>;

		using NonRigidPartialStateChangeLocal = LocalData < NonRigidPartialStateChange >;
		using NonRigidPartialStateChangeGlobal = GlobalData < NonRigidPartialStateChange >;

		//! Metody wyznaczają różnicę pomiędzy kolejnymi stanami szkieletu
		static RigidCompleteStateChange difference(const SkeletonState::RigidCompleteState & src, const SkeletonState::RigidCompleteState & dest);
		static RigidPartialStateChange difference(const SkeletonState::RigidCompleteState & src, const SkeletonState::RigidPartialState & dest);
		static RigidPartialStateChange difference(const SkeletonState::RigidPartialState & src, const SkeletonState::RigidPartialState & dest);
		static RigidPartialStateChange difference(const SkeletonState::RigidPartialState & src, const SkeletonState::RigidCompleteState & dest);

		static NonRigidCompleteStateChange difference(const SkeletonState::NonRigidCompleteState & src, const SkeletonState::NonRigidCompleteState & dest);
		static NonRigidPartialStateChange difference(const SkeletonState::NonRigidCompleteState & src, const SkeletonState::NonRigidPartialState & dest);
		static NonRigidPartialStateChange difference(const SkeletonState::NonRigidPartialState & src, const SkeletonState::NonRigidPartialState & dest);
		static NonRigidPartialStateChange difference(const SkeletonState::NonRigidPartialState & src, const SkeletonState::NonRigidCompleteState & dest);

		//! Metody aktualizują aktualny stan szkieletu o zadaną zmianę
		static void applyLocalChange(Skeleton & skeleton, const RigidCompleteStateChange & stateChange);
		static void applyLocalChange(Skeleton & skeleton, const RigidPartialStateChange & stateChange);
		static void applyLocalChange(Skeleton & skeleton, const NonRigidCompleteStateChange & stateChange);
		static void applyLocalChange(Skeleton & skeleton, const NonRigidPartialStateChange & stateChange);

		static void applyGlobalChange(Skeleton & skeleton, const RigidCompleteStateChange & stateChange);
		static void applyGlobalChange(Skeleton & skeleton, const RigidPartialStateChange & stateChange);
		static void applyGlobalChange(Skeleton & skeleton, const NonRigidCompleteStateChange & stateChange);
		static void applyGlobalChange(Skeleton & skeleton, const NonRigidPartialStateChange & stateChange);

		template<typename StateChangeType>
		static inline void applyChange(Skeleton & skeleton, const StateChangeType & stateChange)
		{
			applyChange(skeleton, stateChange, StateChangeType::type == Local);
		}

	private:

		template<typename StateChangeType>
		static inline void applyChange(Skeleton & skeleton, const StateChangeType & stateChange, std::true_type)
		{
			applyLocalChange(skeleton, stateChange);
		}

		template<typename StateChangeType>
		static inline void applyChange(Skeleton & skeleton, const StateChangeType & stateChange, std::false_type)
		{
			applyGlobalChange(skeleton, stateChange);
		}
	};
}

#endif	// __HEADER_GUARD_KINEMATIC__SKELETONSTATECHANGE_H__