#include <dflib/DFModelRunner.h>
#include <dflib/STDFModelRunner.h>
#include "STDFModelRunnerImpl.h"
#include <dflib/LoggerHelpers.h>
#include "STMRNode.h"
#include "STMRPin.h"
#include "STMRConnection.h"
#include <dflib/IModel.h>
#include <dflib/IDFPin.h>
#include <dflib/IDFNode.h>
#include <dflib/MRModelLogicVerifier.h>
#include <dflib/MRModelPinRequirementsVerifier.h>
#include <dflib/STDFModelRunner.h>

df::STDFModelRunner::STDFModelRunnerImpl::STDFModelRunnerImpl()
{
}

df::STDFModelRunner::STDFModelRunnerImpl::INodeRunner::INodeRunner(STDFModelRunnerImpl * runner, df::IDFLogger * logger) : runner_(runner), logger_(logger), i(0)
{
}

void df::STDFModelRunner::STDFModelRunnerImpl::INodeRunner::run()
{
	bool next = true;
	bool error = false;
	std::string errorMessage;

	i = 0;

	while (error == false && next == true){
		try{
			++i;
			stage = 0;
			next = dataflow();
		}
		catch (std::exception & e){
			error = true;
			errorMessage = std::string(e.what());
		}
		catch (...){
			error = true;
			errorMessage = "Unknown node runner error";
		}
	}

	if (error == true){
		runner_->markFailure(errorMessage);
	}
}

df::STDFModelRunner::STDFModelRunnerImpl::SourceNodeRunner::SourceNodeRunner(ISTMRSourceNode * node, STDFModelRunnerImpl * runner, df::IDFLogger * logger) : INodeRunner(runner, logger), node_(node)
{
}

const bool df::STDFModelRunner::STDFModelRunnerImpl::SourceNodeRunner::dataflow()
{
	if (runner_->dataflowFinished() == true)
	{
		return false;
	}

	if (runner_->sourceStarted() == false){
		stage++;
	}
	stage++;
	stage++;
	stage++;
	node_->process();
	stage++;
	if (runner_->sourceFinished() == false){
		stage++;
	}
	stage++;

	
	stage++;

	return true;
}

df::STDFModelRunner::STDFModelRunnerImpl::SinkNodeRunner::SinkNodeRunner(ISTMRSinkNode * node, STDFModelRunnerImpl * runner, df::IDFLogger * logger) : INodeRunner(runner, logger), node_(node)
{

}

const bool df::STDFModelRunner::STDFModelRunnerImpl::SinkNodeRunner::dataflow()
{
	stage++;
	if (runner_->dataflowFinished() == true)
	{
		return false;
	}

	stage++;
	stage++;
	node_->process();
	stage++;
	runner_->nonSourceFinished();
	stage++;
	return true;
}

df::STDFModelRunner::STDFModelRunnerImpl::ProcessingNodeRunner::ProcessingNodeRunner(ISTMRProcessingNode * node, STDFModelRunnerImpl * runner, df::IDFLogger * logger) : INodeRunner(runner, logger), node_(node)
{

}

const bool df::STDFModelRunner::STDFModelRunnerImpl::ProcessingNodeRunner::dataflow()
{
	stage++;

	if (runner_->dataflowFinished() == true)
	{
		return false;
	}

	stage++;
	stage++;
	node_->process();
	stage++;
	runner_->nonSourceFinished();
	stage++;
	stage++;
	return true;
}

//! \return Czy źródła mają jeszcze dane
const bool df::STDFModelRunner::STDFModelRunnerImpl::sourcesHaveMore() const
{
	bool ret = true;

	for (auto it = sources_.begin(); it != sources_.end(); ++it)
	{
		if ((*it).dfSource->empty() == true)
		{
			ret = false;
			break;
		}
	}

	return ret;
}

void df::STDFModelRunner::STDFModelRunnerImpl::increaseDataToProcess()
{
	++dataToProcess;
}

void df::STDFModelRunner::STDFModelRunnerImpl::decreaseDataToProcess()
{
	--dataToProcess;
}


bool df::STDFModelRunner::STDFModelRunnerImpl::sourceFinished()
{

	++sourcesFinished_;

	if (sourcesFinished_ == sources_.size())
	{
		if (sourcesHaveMore() == true){
			sourcesFinished_ = 0;
			//sourcesStartWait.lock();
			//sourcesFinishWait.notify_all();
		}
		else{
			//źródła skończyły przetwarzać - czekamy na innych żeby zakończyć dataflow
			sourcesEmpty_ = true;
			if (dataToProcess == 0){
				//stopDataflow();
			}
		}

		return true;
	}

	return false;
}

bool df::STDFModelRunner::STDFModelRunnerImpl::sourceStarted()
{
	++sourcesStarted_;

	if (sourcesStarted_ == sources_.size())
	{
		sourcesStarted_ = 0;
		increaseDataToProcess();
		//sourcesFinishWait.lock();
		//sourcesStartWait.notify_all();
		return true;
	}

	return false;
}

void df::STDFModelRunner::STDFModelRunnerImpl::nonSourceFinished()
{
	++nonSourcesFinished_;

	if (nonSourcesFinished_ == nonSourceElements)
	{
		nonSourcesFinished_ = 0;
		decreaseDataToProcess();

		if (sourcesEmpty_ == true && dataToProcess == 0){
			//stopDataflow();
		}
	}
}

void df::STDFModelRunner::STDFModelRunnerImpl::finishDataflow()
{
	try{

		cleanUpDataflow();
	}
	catch (std::exception & e){
		markFailure(e.what());
	}
	catch (...){
		markFailure("Unknown error while finishing dataflow");
	}
}

const bool df::STDFModelRunner::STDFModelRunnerImpl::dataflowFinished() const
{
	return dataflowFinished_;
}

const bool df::STDFModelRunner::STDFModelRunnerImpl::verifyModel(df::IModelReader * reader, MRModelInterfaceVerifier::ModelVerificationData & interfaceVerifier)
{
	MRModelLogicVerifier::ModelVerificationData logicVerify;
	MRModelPinRequirementsVerifier::UnsatisfiedPins pinsVerify;

	bool logicOK = MRModelLogicVerifier::verifyModelLogic(reader, logicVerify);
	bool pinsOK = MRModelPinRequirementsVerifier::verifyModelPinRequirements(reader, pinsVerify);
	bool interfaceOK = MRModelInterfaceVerifier::verifyModelInterface(reader, interfaceVerifier);

	return (logicOK && pinsOK && interfaceOK);
}

const bool df::STDFModelRunner::STDFModelRunnerImpl::verifyModel(const df::IModelReader * reader)
{
	MRModelInterfaceVerifier::ModelVerificationData interfaceVerifier;
	return verifyModel(const_cast<df::IModelReader*>(reader), interfaceVerifier);
}


void df::STDFModelRunner::STDFModelRunnerImpl::start(df::IModelReader * model, df::IDFLogger * logger)
{
	MRModelInterfaceVerifier::ModelVerificationData interfaceVerification;
	if (verifyModel(model, interfaceVerification) == false){
		std::string message("Model verification failed:");

		if (interfaceVerification.incompatibleNodes.empty() == false){
			message += "Some nodes do not implement required functionality - IDFSource, IDFSink or IDFProcessor";
		}

		if (interfaceVerification.incompatiblePins.empty() == false){
			message += "Some pins do not implement required functionality - IDFInput or IDFOutput";
		}

		throw df::ModelVerificationException(message.c_str());
	}


	logger_ = logger;

	try{
		resetDataflowStatus();
		wrapModelElements(model, interfaceVerification);
		resetDataflowElements();
	}
	catch (std::exception & e){
		cleanUpDataflow();
		throw ModelRunInitializationException(e.what());
	}
	catch (...){
		cleanUpDataflow();
		throw ModelRunInitializationException("Unknown error while initializing dataflow");
	}

	try{
		startDataflow();
	}
	catch (std::exception & e){
		throw ModelStartException(e.what());
	}
	catch (...){
		throw ModelStartException("Unknown error while starting dataflow");
	}
}

void df::STDFModelRunner::STDFModelRunnerImpl::stop()
{
}


//void df::STDFModelRunner::STDFModelRunnerImpl::join()
//{
//	for (auto it = threads_.begin(); it != threads_.end(); ++it)
//	{
//		(*it)->join();
//	}
//
//	{
//		std::lock_guard<std::mutex> lock(failureMessageSync);
//
//		if (dataflowFinished_ == true){
//			dataflowFinished_ = false;
//
//			try{
//				cleanUpDataflow();
//			}
//			catch (std::exception & e){
//				markFailure(e.what());
//			}
//			catch (...){
//				markFailure("Unknown error while finishing dataflow");
//			}
//		}
//	}
//
//	if (failure_ == true){
//		std::string message = "Error(s) while model running: ";
//
//		for (auto it = failureMessages_.begin(); it != failureMessages_.end(); ++it){
//			message += *it + " | ";
//		}
//
//		throw ModelRunningException(message.c_str());
//	}
//}

void df::STDFModelRunner::STDFModelRunnerImpl::wrapModelElements(df::IModelReader * model, const MRModelInterfaceVerifier::ModelVerificationData & modelElements)
{
	InputPinsMapping inputMapping;
	OutputPinsMapping outputMapping;

	for (auto it = modelElements.sourceVerification.begin(); it != modelElements.sourceVerification.end(); ++it)
	{
		wrapSourceNode(*it, outputMapping);
	}

	for (auto it = modelElements.sinkVerification.begin(); it != modelElements.sinkVerification.end(); ++it)
	{
		wrapSinkNode(*it, inputMapping);
	}

	for (auto it = modelElements.processorVerification.begin(); it != modelElements.processorVerification.end(); ++it)
	{
		wrapProcessorNode(*it, inputMapping, outputMapping);
	}

	for (unsigned int i = 0; i < model->connectionsSize(); ++i)
	{
		auto c = model->connection(i);
		auto src = outputMapping[c->source()];
		auto dest = inputMapping[c->destination()];
		auto mrconnection = new STMRConnection(src, dest, c);

		src->addConnection(mrconnection);
		dest->addConnection(mrconnection);
	}

	nonSourceElements = sinks_.size() + processors_.size();
}

void df::STDFModelRunner::STDFModelRunnerImpl::wrapSourceNode(const MRModelInterfaceVerifier::SourceVerificationData & sourceData, OutputPinsMapping & outputMapping)
{
	SourceNodeWrapData srcWrapData;
	srcWrapData.node = new STMRSourceNode(sourceData.node, sourceData.source);
	srcWrapData.dfSource = sourceData.source;
	wrapOutputPins(srcWrapData.node, sourceData.outputVerication, outputMapping);

	auto runner = new SourceNodeRunner(srcWrapData.node, this, logger_);
	nodeRunners_.push_back(runner);



	df::IDFLoggable * loggable = dynamic_cast<df::IDFLoggable*>(sourceData.node);

	if (loggable != nullptr){
		auto loggerHelper = new df::SourceLoggerHelper(sourceData.node, logger_);
		loggable->initLogger(loggerHelper);
		loggerHelpers_.push_back(loggerHelper);
	}

	sources_.push_back(srcWrapData);
}

void df::STDFModelRunner::STDFModelRunnerImpl::wrapSinkNode(const MRModelInterfaceVerifier::SinkVerificationData & sinkData, InputPinsMapping & inputMapping)
{
	auto node = new STMRSinkNode(sinkData.node, sinkData.sink);
	wrapInputPins(node, sinkData.inputVerication, inputMapping);

	auto runner = new SinkNodeRunner(node, this, logger_);
	nodeRunners_.push_back(runner);



	df::IDFLoggable * loggable = dynamic_cast<df::IDFLoggable*>(sinkData.node);

	if (loggable != nullptr){
		auto loggerHelper = new df::SinkLoggerHelper(sinkData.node, logger_);
		loggable->initLogger(loggerHelper);
		loggerHelpers_.push_back(loggerHelper);
	}

	sinks_.push_back(node);
}

void df::STDFModelRunner::STDFModelRunnerImpl::wrapProcessorNode(const MRModelInterfaceVerifier::ProcessorVerificationData & processorData,
	InputPinsMapping & inputMapping, OutputPinsMapping & outputMapping)
{
	auto node = new STMRProcessingNode(processorData.node, processorData.processor);
	wrapInputPins(node, processorData.inputVerication, inputMapping);
	wrapOutputPins(node, processorData.outputVerication, outputMapping);

	INodeRunner * runner = nullptr;

	if (processorData.node->outputsConnected() == 0){
		runner = new SinkNodeRunner(node, this, logger_);
	}
	else{
		runner = new ProcessingNodeRunner(node, this, logger_);
	}

	nodeRunners_.push_back(runner);


	df::IDFLoggable * loggable = dynamic_cast<df::IDFLoggable*>(processorData.node);

	if (loggable != nullptr){
		auto loggerHelper = new df::ProcessingLoggerHelper(processorData.node, logger_);
		loggable->initLogger(loggerHelper);
		loggerHelpers_.push_back(loggerHelper);
	}

	processors_.push_back(node);
}

void df::STDFModelRunner::STDFModelRunnerImpl::wrapInputPins(ISTMRSinkNode * sink, const MRModelInterfaceVerifier::InputVerification & inputData, InputPinsMapping & inputMapping)
{
	for (auto it = inputData.begin(); it != inputData.end(); ++it)
	{
		auto p = new STMRInputPin(sink, (*it).first, (*it).second);
		sink->addInputPin(p);
		inputMapping[(*it).first] = p;
	}
}

void df::STDFModelRunner::STDFModelRunnerImpl::wrapOutputPins(ISTMRSourceNode * source, const MRModelInterfaceVerifier::OutputVerification & outputData, OutputPinsMapping & inputMapping)
{
	for (auto it = outputData.begin(); it != outputData.end(); ++it)
	{
		auto p = new STMROutputPin(source, (*it).first, (*it).second);
		source->addOutputPin(p);
		inputMapping[(*it).first] = p;
	}
}

void df::STDFModelRunner::STDFModelRunnerImpl::resetDataflowStatus()
{
	finishedRunners = 0;
	dataToProcess = 0;
	nonSourceElements = 0;
	sourcesFinished_ = 0;
	sourcesStarted_ = 0;
	nonSourcesFinished_ = 0;
	sourcesEmpty_ = false;
	failure_ = false;
	dataflowFinished_ = false;
	paused_ = false;
	//sourcesStartWait.lock();
}

void df::STDFModelRunner::STDFModelRunnerImpl::resetDataflowElements()
{
	for (auto it = sources_.begin(); it != sources_.end(); ++it)
	{
		(*it).node->reset();
	}

	for (auto it = sinks_.begin(); it != sinks_.end(); ++it)
	{
		(*it)->reset();
	}

	for (auto it = processors_.begin(); it != processors_.end(); ++it)
	{
		(*it)->reset();
	}
}

void df::STDFModelRunner::STDFModelRunnerImpl::startDataflow()
{
	bool hasStep = true;
	while (hasStep) {
		hasStep = false;
		for (auto& source : sources_) {
			if (source.node->hasSomethingToProcess()) {
				hasStep = true;
				source.node->process();
			}
		}
		for (auto& processor : processors_) {
			if (processor->hasSomethingToProcess()) {
				hasStep = true;
				processor->process();
			}
		}

		for (auto& sink : sinks_) {
			if (sink->hasSomethingToProcess()) {
				hasStep = true;
				sink->process();
			}
		}
	}
}

void df::STDFModelRunner::STDFModelRunnerImpl::cleanUpDataflow()
{
	for (auto it = sources_.begin(); it != sources_.end(); ++it)
	{
		delete (*it).node;
	}

	for (auto it = sinks_.begin(); it != sinks_.end(); ++it)
	{
		delete *it;
	}

	for (auto it = processors_.begin(); it != processors_.end(); ++it)
	{
		delete *it;
	}

	for (auto it = loggerHelpers_.begin(); it != loggerHelpers_.end(); ++it)
	{
		delete *it;
	}

	//threadingUtils::IThreadPool::Threads().swap(threads_);
	//Runnables().swap(runnables_);

	NodesMapping().swap(nodesMapping);
	NodesMapping().swap(pausedNodes);

	WrapedSources().swap(sources_);
	WrapedSinks().swap(sinks_);
	WrapedProcessors().swap(processors_);
	LoggerHelpers().swap(loggerHelpers_);

	resetDataflowStatus();

	sourcesEmpty_ = false;
	dataflowFinished_ = false;

	logger_ = nullptr;
}

void df::STDFModelRunner::STDFModelRunnerImpl::markFailure(const std::string & message)
{

	failureMessages_.push_back(message);
	if (failure_ == false)
	{
		failure_ = true;
		//stopDataflow();
	}
}


