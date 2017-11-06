BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	osgVDFBaseModel.h
	osgVDFBaseNode.h
	osgVDFBasePin.h
	osgVDFModel.h
	osgVDFNode.h
	osgVDFNodeTypeDescriptor.h
	osgVDFPin.h
	osgVDFRectangleSelectionFactory.h
	osgVDFLinearConnectionFactory.h
	osgVDFBezieerConnectionFactory.h
)

SET_PRIVATE_HEADERS(
	StdAfx.h
)

SET_SOURCE_FILES(
	osgVDFBaseModel.cpp
	osgVDFBaseNode.cpp
	osgVDFBasePin.cpp
	osgVDFModel.cpp
	osgVDFNode.cpp
	osgVDFPin.cpp
	osgVDFNodeTypeDescriptor.cpp
	osgVDFRectangleSelectionFactory.cpp
	osgVDFLinearConnectionFactory.cpp
	osgVDFBezieerConnectionFactory.cpp
	StdAfx.cpp
)

SET_PRECOMPILED_HEADER(StdAfx.h StdAfx.cpp)

END_PROJECT()