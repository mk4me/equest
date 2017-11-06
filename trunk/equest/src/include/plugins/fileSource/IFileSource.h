/********************************************************************
	created:	2015/07/17
	created:	12:59
	filename: 	IFileSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_FILE_SOURCE__IFileSource_H__
#define HEADER_GUARD_FILE_SOURCE__IFileSource_H__


namespace fileSource {
	class IFileSource {
		public:
			virtual ~IFileSource() {}
			virtual void addFile(const utils::Filesystem::Path& path) = 0;
	};
	DEFINE_SMART_POINTERS(IFileSource);
}

#endif // HEADER_GUARD 
