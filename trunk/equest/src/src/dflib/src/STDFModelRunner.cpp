#include <dflib/STDFModelRunner.h>
#include "STDFModelRunnerImpl.h"

namespace df {
	STDFModelRunner::STDFModelRunner() : privImpl(new STDFModelRunnerImpl())
	{
	}

	STDFModelRunner::~STDFModelRunner()
	{
		delete privImpl;
	}

	void STDFModelRunner::start(IModelReader * model, IDFLogger * logger)
	{
		privImpl->start(model, logger);
	}

	void STDFModelRunner::stop()
	{
		privImpl->stop();
	}



	const bool STDFModelRunner::verifyModel(const IModelReader * reader)
	{
		return STDFModelRunnerImpl::verifyModel(reader);
	}
}
