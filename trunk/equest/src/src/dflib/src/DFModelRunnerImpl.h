/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   23:05
	filename: DFModelRunnerImpl.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___DFMODELRUNNERIMPL_H__
#define HEADER_GUARD___DFMODELRUNNERIMPL_H__

//#include <dflib/DFModelRunner.h>
//#include <dflib/MRModelInterfaceVerifier.h>
//#include <dflib/IDFLogger.h>
//
//#include <list>
//#include <vector>
//#include <map>
//#include <mutex>
//#include <condition_variable>
//
//namespace df{
//	class IPin;
//	class INode;
//
//	class IInputPin;
//	class IOutputPin;
//
//	class ISourceNode;
//	class ISinkNode;
//	class IProcessingNode;
//
//	class IDFInput;
//	class IDFOutput;
//
//	class IDFSource;
//	class IDFSink;
//	class IDFProcessor;
//
//	class IModelReader;
//}
//
//class MRInputPin;
//class MROutputPin;
//
//class IMRNode;
//class IMRSourceNode;
//class IMRSinkNode;
//class IMRProcessingNode;
//
//class df::DFModelRunner::DFModelRunnerImpl
//{
//private:
//
//	class INodeRunner
//	{
//	public:
//
//		INodeRunner(DFModelRunnerImpl * runner, df::IDFLogger * logger);
//
//		void run();
//
//	private:
//		virtual const bool dataflow() = 0;
//
//	protected:
//		unsigned int i;
//		unsigned int stage;
//		df::IDFLogger * logger_;
//		DFModelRunnerImpl * runner_;
//	};
//
//	friend class INodeRunner;
//
//	class SinkNodeRunner : public INodeRunner
//	{
//	public:
//		SinkNodeRunner(IMRSinkNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger);
//
//	private:
//		virtual const bool dataflow();
//
//	private:
//		IMRSinkNode * node_;
//	};
//
//	friend class SinkNodeRunner;
//
//	class SourceNodeRunner : public INodeRunner
//	{
//	public:
//		SourceNodeRunner(IMRSourceNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger);
//
//	private:
//		virtual const bool dataflow();
//
//	private:
//		IMRSourceNode * node_;
//	};
//
//	friend class SourceNodeRunner;
//
//	class ProcessingNodeRunner : public INodeRunner
//	{
//	public:
//		ProcessingNodeRunner(IMRProcessingNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger);
//
//	private:
//		virtual const bool dataflow();
//
//	private:
//		IMRProcessingNode * node_;
//	};
//
//	friend class ProcessingNodeRunner;
//
//	struct SourceNodeWrapData
//	{
//		IMRSourceNode * node;
//		df::IDFSource * dfSource;
//	};
//
//	typedef std::vector<SourceNodeWrapData> WrapedSources;
//	typedef std::vector<IMRSinkNode *> WrapedSinks;
//	typedef std::vector<IMRProcessingNode*> WrapedProcessors;
//
//	typedef std::vector<threadingUtils::IRunnablePtr> Runnables;
//	typedef std::list<df::IDFLoggerHelper*> LoggerHelpers;
//	typedef std::vector<INodeRunner*> NodeRunners;
//
//	typedef std::map<df::INode*, IMRNode *> NodesMapping;
//
//	typedef std::map<df::IInputPin*, MRInputPin*> InputPinsMapping;
//	typedef std::map<df::IOutputPin*, MROutputPin*> OutputPinsMapping;
//
//private:
//
//	void markFailure(const std::string & message);
//
//	void cleanUpDataflow();
//
//	void wrapSourceNode(const MRModelInterfaceVerifier::SourceVerificationData & sourceData, OutputPinsMapping & outputMapping);
//	void wrapSinkNode(const MRModelInterfaceVerifier::SinkVerificationData & sinkData, InputPinsMapping & inputMapping);
//	void wrapProcessorNode(const MRModelInterfaceVerifier::ProcessorVerificationData & processorData, InputPinsMapping & inputMapping, OutputPinsMapping & outputMapping);
//
//	void wrapInputPins(IMRSinkNode * sink, const MRModelInterfaceVerifier::InputVerification & inputData, InputPinsMapping & inputMapping);
//	void wrapOutputPins(IMRSourceNode * source, const MRModelInterfaceVerifier::OutputVerification & outputData, OutputPinsMapping & outputMapping);
//
//	void reset();
//
//	void resetDataflowStatus();
//
//	void resetDataflowElements();
//	void startDataflow();
//
//	void increaseDataToProcess();
//	void decreaseDataToProcess();
//
//	//! \return Czy źródła mają jeszcze dane
//	const bool sourcesHaveMore() const;
//
//	//! Meldujemy zakończenie pracy elementu nie będącego źródłem
//	void waitForAllSourcesStart();
//	void waitForAllSourcesFinish();
//	bool sourceFinished();
//	bool sourceStarted();
//	void nonSourceFinished();
//
//	void finishDataflow();
//	void stopDataflow();
//
//	//! kończymy przepływ danych - czyścimy wszystko
//	const bool dataflowFinished() const;
//
//	//! \param reader Obiekt pozwalajacy czytać strukturę naszego modelu
//	//! \return Czy model można przetwarzać
//	static const bool verifyModel(df::IModelReader * reader, MRModelInterfaceVerifier::ModelVerificationData & interfaceVerifier);
//
//	void tryPause();
//
//	//! Metoda mapuje elementy modelu do obiektów wrappujących, które obsługują logikę przepływu danych
//	//! \param reader Obiekt pozwalajacy czytaę strukturę naszego modelu
//	void wrapModelElements(df::IModelReader * model, const MRModelInterfaceVerifier::ModelVerificationData & modelElements);
//
//private:
//	std::mutex failureMessageSync;
//
//	std::mutex sourcesFinishSync;
//	std::condition_variable sourcesFinishWait;
//	std::mutex sourcesStartSync;
//	std::condition_variable sourcesStartWait;
//	std::mutex dataflowPauseSync;
//	std::mutex nonSourcesSync;
//	std::mutex dataToProcessSync;
//
//	WrapedSources sources_;
//	WrapedSinks sinks_;
//	WrapedProcessors processors_;
//	Runnables runnables_;
//	threadingUtils::IThreadPool::Threads threads_;
//	LoggerHelpers loggerHelpers_;
//	NodeRunners nodeRunners_;
//
//	NodesMapping nodesMapping;
//	NodesMapping pausedNodes;
//
//	unsigned int finishedRunners;
//	unsigned int dataToProcess;
//	unsigned int nonSourceElements;
//	unsigned int sourcesFinished_;
//	unsigned int sourcesStarted_;
//	unsigned int nonSourcesFinished_;
//
//	volatile bool sourcesEmpty_;
//	bool dataflowFinished_;
//
//	bool failure_;
//	std::list<std::string> failureMessages_;
//
//	bool paused_;
//
//	df::IDFLogger * logger_;
//
//	threadingUtils::IThreadPool * threadPool;
//
//public:
//
//	DFModelRunnerImpl();
//
//	//! \param reader Obiekt pozwalajacy czytać strukturę naszego modelu
//	//! \return Czy model można przetwarzać
//	static const bool verifyModel(const df::IModelReader * reader);
//
//	void start(df::IModelReader * model, df::IDFLogger * logger, threadingUtils::IThreadPool * tPool);
//	void stop();
//
//	void pause(df::INode * node);
//	void resume(df::INode * node);
//	const bool paused(df::INode * node);
//
//	void pause();
//	void resume();
//	const bool paused() const;
//
//	void join();
//};

#endif	//	HEADER_GUARD___DFMODELRUNNERIMPL_H__
