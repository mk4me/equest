BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	Features.h
	AccessorType.h
	Accessors.h
	ArgumentTracker.h
	DescriptorFeature.h
	AccessorsCollection.h	
	Statistics.h
	WrapperHelpers.h
	BoundedArgumentsFeature.h
	BoundedValuesFeature.h
	FunctionFeature.h		
	FunctionDescriptionFeature.h		
	PeriodicFunctionFeature.h
	UniformArgumentsFeature.h	
	Types.h
	Adapters.h
	Extrapolators.h
	Interpolators.h	
	Wrappers.h
	CurrentValueExtractor.h
	Traits.h
	SafeAccessorWrapper.h
	NearestArgumentsFinder.h
	UniformArgumentsGenerator.h
)

SET_SOURCE_FILES(
	FunctionFeature.cpp
)

END_PROJECT()