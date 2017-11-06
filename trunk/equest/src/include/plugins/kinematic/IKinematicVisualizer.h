/********************************************************************
	created:	2015/09/04
	created:	10:42
	filename: 	IKinematicVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__IKinematicVisualizer_H__
#define HEADER_GUARD_KINEMATIC__IKinematicVisualizer_H__


namespace  kinematic {
	class IKinematicVisualizer {
		public:
			virtual ~IKinematicVisualizer() {}

			virtual bool getAxisXYZ() const = 0;
			virtual void setAxisXYZ(bool val) = 0;
	};
	DEFINE_SMART_POINTERS(IKinematicVisualizer);
}

#endif // HEADER_GUARD 
