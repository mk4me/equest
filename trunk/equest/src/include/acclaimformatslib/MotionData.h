#ifndef HEADER_GUARD_acclaimDATA__SKELETALMODEL_H__
#define HEADER_GUARD_acclaimDATA__SKELETALMODEL_H__

#include <acclaimformatslib/Bone.h>

namespace acclaim
{
	//! klasa przechowuje wczytane dane animacji
	struct MotionData
	{
	public:

		//! Agregat danych kanału
		using ChannelValues = std::vector<double>;

		/// \brief  Struktura zawiera wartości kanałów dla
		/// 	    konkretnej kości i konkretnego kanału
		struct BoneData
		{
			//! nazwa stawu
			std::string name;
			/// \brief  wartości kanałów dla tej stawu (ich liczba powinna być rowna stopniom swobody stawu)
			ChannelValues channelValues;
		};

		//! Agregat danych kości
		using BonesData = std::vector<BoneData>;

		/// \brief  struktura przechowuje wartości kanałów dla wszystkich kości w pojedynczej klatce animacji
		struct FrameData
		{
			unsigned int id; //!< numer klatki animacji
			BonesData bonesData; //!< wartości kanałów			
		};

		using FramesData = std::vector<FrameData>;

	public:
		//! \param frameTime Czas trwania ramki w sekundach
		MotionData(const double frameTime = 0.01) :
			frameTime(frameTime)
		{}

		MotionData(const MotionData & Other) : frameTime(Other.frameTime),
			frames(Other.frames){}

		MotionData(MotionData && Other) : frameTime(Other.frameTime),
			frames(std::move(Other.frames)) {}

		//! Destruktor
		~MotionData() {}

		//! kolecja z danymi
		FramesData frames;
		////! Czas trwania pojedynczej ramki
		const double frameTime;
	};
	DEFINE_SMART_POINTERS(MotionData);
}
#endif
