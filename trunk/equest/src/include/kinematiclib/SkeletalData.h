#ifndef HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__
#define HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__

#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletonState.h>
#include <threadingUtils/StreamData.h>

#include <utils/Debug.h>
namespace kinematic
{
	//! klasa przechowuje wczytane dane animacji
	struct SkeletalData
	{
		/// \brief  struktura przechowuje wartości kanałów dla wszystkich kości w pojedynczej klatce animacji
		struct Frame
		{
			//! Translacja całego szkieletu
			osg::Vec3 translation;
			//! Rotacje lokalne poszczególnych stawów indeskowane wg mapowania
			std::vector<osg::Quat> rotations;
		};

		/// \brief  struktura przechowuje wartości kanałów dla wszystkich kości w pojedynczej klatce animacji
		struct PartialFrame
		{
			//! Translacja całego szkieletu
			osg::Vec3 translation;
			//! Rotacje poszczególnych stawów wg nazw
			std::map<std::string, osg::Quat> rotations;
		};
	
		//! konstruktor, ustawia domyślny okres pojedynczej ramki (1/100)
		SkeletalData(const float duration = 0.01) :	frameTime(duration)	{}
		//! Destruktor
		~SkeletalData() {}
		
		//! kolecja z danymi
		std::vector<Frame>  frames;
		//! okres pojedynczej ramki
		float frameTime;
	};
	using SkeletalDataPtr = utils::shared_ptr<SkeletalData>;
	using SkeletalDataConstPtr = utils::shared_ptr<const SkeletalData>;

	
	struct StreamSkeletonDataFrame
	{
		//! Pozycja roota
		osg::Vec3 rootPosition;
		int frameNo; //!< numer klatki animacji
		std::vector<osg::Quat> rotations; //!< rotacje
	};
	DEFINE_SMART_POINTERS(StreamSkeletonDataFrame);


	using SkeletalDataStream = threadingUtils::StreamT<StreamSkeletonDataFrame>;

	using SkeletalDataStreamPtr = utils::shared_ptr<SkeletalDataStream>;
	using SkeletalDataStreamConstPtr = utils::shared_ptr<const SkeletalDataStream>;

	//! Klatka dla danych strumieniowych
	struct StreamSingleFrame
	{
		//! Pozycja roota
		osg::Vec3 rootPosition;
		int frameNo; //!< numer klatki animacji
		std::vector<StreamSkeletonDataFramePtr> jointsData; //!< wartości kanałów

		StreamSingleFrame* clone() const
		{
			std::unique_ptr<StreamSingleFrame> clone(new StreamSingleFrame());
			clone->rootPosition = this->rootPosition;
			clone->frameNo = this->frameNo;
			const int count = this->jointsData.size();
			clone->jointsData.resize(count);
			for (int i = 0; i < count; ++i) {
				clone->jointsData[i] = this->jointsData[i];
			}
			return clone.release();
		}
	};

	using SkeletalFramesStream = threadingUtils::StreamT<StreamSingleFrame>;
}
#endif
