/********************************************************************
	created:	2015/03/20
	created:	8:58
	filename: 	C3DWriter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DWriter_H__
#define HEADER_GUARD_C3DLIB__C3DWriter_H__

#include <utils/SmartPtr.h>
#include <c3dlib/Export.h>
#include <c3dlib/C3DTypes.h>

namespace c3dlib {
	class C3DLIB_EXPORT C3DWriter 
	{
	public:
		C3DWriter() {}
		virtual ~C3DWriter() {}

	public:
		void addScalarChannel(ScalarChannelReaderInterfaceConstPtr channel);
		void addVectorChannel(VectorChannelReaderInterfaceConstPtr channel);
		void write(const std::string& path);

	private:
		std::vector<ScalarChannelReaderInterfaceConstPtr> scalars;
		std::vector<VectorChannelReaderInterfaceConstPtr> vectors;
	};
	DEFINE_SMART_POINTERS(C3DWriter);
}

#endif // HEADER_GUARD 
