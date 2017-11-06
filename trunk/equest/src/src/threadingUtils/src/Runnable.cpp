#include <threadingUtils/Runnable.h>

using namespace threadingUtils;

FunctorRunnable::FunctorRunnable(const Functor & func)
: func(func)
{
}

FunctorRunnable::~FunctorRunnable()
{
}

void FunctorRunnable::run()
{
	if (func){
		func();
	}
}