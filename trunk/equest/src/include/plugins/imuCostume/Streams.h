/********************************************************************
	created:  2015/01/06	00:09:20
	filename: Streams.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__STREAMS_H__
#define __HEADER_GUARD_IMU__STREAMS_H__

#include <kinematiclib/SkeletonState.h>
#include <plugins/imuCostume/Types.h>
#include <utils/SmartPtr.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <threadingUtils/StreamData.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <kinematiclib/Skeleton.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
#include <plugins/newChart/Wrappers.h>

namespace IMU
{
	//! \tparam T Typ danych strumienia czasowego
	//! \tparam TimeType Typ czasu
	template<typename T, typename TimeType = float>
	//! Typ danych strumienia czasowego
	using TimeData = std::pair < TimeType, T > ;

	//! \tparam T Typ danych strumienia czasowego
	//! \tparam TimeType Typ czasu
	template<typename T, typename TimeType = float>
	//! Typ interfejsu strumienia czasowego
	using ITimeStream = threadingUtils::IStreamT < TimeData<T, TimeType> > ;

	//! Strumieñ opisuj¹cy jointy
	typedef ITimeStream<JointData> JointStream;

	DEFINE_SMART_POINTERS(JointStream);

	//! Seria danych szkieletu
	typedef ITimeStream<kinematic::SkeletonState::NonRigidCompleteStateLocal> SkeletonStateStream;
	DEFINE_SMART_POINTERS(SkeletonStateStream);

	//! Strumieñ danych jednego czujnika IMU
	typedef ITimeStream<SensorData> IMUStream;

	DEFINE_SMART_POINTERS(IMUStream);

	//! Strumieñ danych 3D
	typedef ITimeStream<osg::Vec3d> Vec3Stream;

	DEFINE_SMART_POINTERS(Vec3Stream);

	//! Strumieñ danych 4D
	typedef ITimeStream<osg::Quat> QuatStream;

	DEFINE_SMART_POINTERS(QuatStream);

	//! Strumieñ surowych danych
	typedef threadingUtils::StreamT<imuCostume::ProtocolSendBufferHelper::Buffer> RawDataStream;

	DEFINE_SMART_POINTERS(RawDataStream);

	//! Strumieñ rozpakowanych danych do formatu CANopen
	typedef threadingUtils::IStreamT<imuCostume::CostumeCANopenIO::Data> CANopenFramesStream;

	DEFINE_SMART_POINTERS(CANopenFramesStream);

	//! Strumieñ wypakowanych danych z czujników
	typedef threadingUtils::IStreamT<imuCostume::Costume::Data> CostumeStream;

	DEFINE_SMART_POINTERS(CostumeStream);

	//! Strumieñ wypakowanych danych z czujników
	typedef threadingUtils::IStreamT<SensorsStreamData> SensorsStream;

	DEFINE_SMART_POINTERS(SensorsStream);

	typedef ITimeStream<kinematic::SkeletonState::RigidCompleteStateLocal> MotionStream;

	DEFINE_SMART_POINTERS(MotionStream);

	struct CostumeSkeletonMotion
	{
		kinematic::SkeletonConstPtr skeleton;
		kinematic::LinearizedSkeleton::GlobalMapping mapping;
		MotionStreamPtr stream;
	};

	DEFINE_SMART_POINTERS(CostumeSkeletonMotion);
}

//! -------------------- Wrappery ---------------------------------

DEFINE_WRAPPER(IMU::IMUStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::Vec3Stream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::QuatStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::SensorsStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

DEFINE_WRAPPER(threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER_INHERITANCE(IMU::RawDataStream, threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>);
DEFINE_WRAPPER(IMU::CANopenFramesStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::CostumeStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::MotionStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::JointStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::CostumeSkeletonMotion, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(IMU::SkeletonStateStream, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif	// __HEADER_GUARD_IMU__STREAMS_H__