BEGIN_PROJECT("static")

# Opcje konfiguracji
CONFIG_OPTION(UTILS_ENABLE_PROFILER "Enable profiling?" OFF)
if(WIN32)
	CONFIG_OPTION(UTILS_ENABLE_LEAK_DETECTION_VLD "Enable leak detection with VLD? (http://sites.google.com/site/dmoulding/vld)" OFF)
endif()
CONFIG_OPTION(UTILS_ENABLE_PRIVATE_TESTS "Enable private class members testing?" OFF)

SET_PUBLIC_HEADERS(
	MemberExtractor.h
	array_traits.h
	container_traits.h
	Clamping.h
	DummyWrap.h
	pointer_traits.h
	StreamTools.h
	ValueCarrier.h
	Align.h
	ClonePolicies.h
	Pool.h
	Enum.h
	NamedTree.h
	Filesystem.h
	PtrPolicyRaw.h
	PtrPolicyBoost.h
	PtrPolicyStd.h
	PtrPolicyHelper.h
	PtrWrapper.h
	ObserverPattern.h
	Macros.h
	Utils.h
	Debug.h
	Test.h
	Profiler.h
	System.h
	LeakDetection.h
	Push.h
	Serializable.h
	TypeInfo.h
	TimeUtils.h
	Export.h
	SmartPtr.h
	ICommand.h
	ICommandStack.h
	CommandStack.h
	dynamictreemodel.h
	modeltest.h
	StringConversionTools.h
	SamplesStatus.h
	memory.h
	function_traits.h
)

#------------------------------------------------------------------------------
# Ÿród³a
SET_SOURCE_FILES(
	Enum.cpp
	Debug.cpp
	NamedTree.cpp
	Profiler.cpp
	TypeInfo.cpp
	Filesystem.cpp
	CommandStack.cpp
	dynamictreemodel.cpp
	modeltest.cpp
	TimeUtils.cpp
	SamplesStatus.cpp
	memory.cpp
)

SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)

SET_MOC_FILES(
	CommandStack.h
	dynamictreemodel.h
	modeltest.h
)

CONFIGURE_PUBLIC_HEADER("Config.h.in" "Config.h")

END_PROJECT()