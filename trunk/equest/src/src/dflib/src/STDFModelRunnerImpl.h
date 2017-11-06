/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   23:05
	filename: STDFModelRunnerImpl.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___STDFModelRunnerIMPL_H__
#define HEADER_GUARD___STDFModelRunnerIMPL_H__

#include <dflib/STDFModelRunner.h>
#include <dflib/MRModelInterfaceVerifier.h>
#include <dflib/IDFLogger.h>

#include <list>
#include <vector>
#include <map>

namespace df{
	class IPin;
	class INode;

	class IInputPin;
	class IOutputPin;

	class ISourceNode;
	class ISinkNode;
	class IProcessingNode;

	class IDFInput;
	class IDFOutput;

	class IDFSource;
	class IDFSink;
	class IDFProcessor;

	class IModelReader;
}

class STMRInputPin;
class STMROutputPin;

class ISTMRNode;
class ISTMRSourceNode;
class ISTMRSinkNode;
class ISTMRProcessingNode;

class df::STDFModelRunner::STDFModelRunnerImpl
{
private:

	class INodeRunner
	{
	public:

		INodeRunner(STDFModelRunnerImpl * runner, df::IDFLogger * logger);

		void run();

	private:
		virtual const bool dataflow() = 0;

	protected:
		unsigned int i;
		unsigned int stage;
		df::IDFLogger * logger_;
		STDFModelRunnerImpl * runner_;
	};

	friend class INodeRunner;

	class SinkNodeRunner : public INodeRunner
	{
	public:
		SinkNodeRunner(ISTMRSinkNode * node, STDFModelRunnerImpl * runner, df::IDFLogger * logger);

		virtual const bool dataflow();

	private:
		ISTMRSinkNode * node_;
	};

	friend class SinkNodeRunner;

	class SourceNodeRunner : public INodeRunner
	{
	public:
		SourceNodeRunner(ISTMRSourceNode * node, STDFModelRunnerImpl * runner, df::IDFLogger * logger);

		virtual const bool dataflow();

	private:
		ISTMRSourceNode * node_;
	};

	friend class SourceNodeRunner;

	class ProcessingNodeRunner : public INodeRunner
	{
	public:
		ProcessingNodeRunner(ISTMRProcessingNode * node, STDFModelRunnerImpl * runner, df::IDFLogger * logger);

		virtual const bool dataflow();

	private:
		ISTMRProcessingNode * node_;
	};

	friend class ProcessingNodeRunner;

	struct SourceNodeWrapData
	{
		ISTMRSourceNode * node;
		df::IDFSource * dfSource;
	};

	typedef std::vector<SourceNodeWrapData> WrapedSources;
	typedef std::vector<ISTMRSinkNode *> WrapedSinks;
	typedef std::vector<ISTMRProcessingNode*> WrapedProcessors;

	typedef std::list<df::IDFLoggerHelper*> LoggerHelpers;
	typedef std::vector<INodeRunner*> NodeRunners;

	typedef std::map<df::INode*, ISTMRNode *> NodesMapping;

	typedef std::map<df::IInputPin*, STMRInputPin*> InputPinsMapping;
	typedef std::map<df::IOutputPin*, STMROutputPin*> OutputPinsMapping;

private:

	void markFailure(const std::string & message);

	void cleanUpDataflow();

	void wrapSourceNode(const MRModelInterfaceVerifier::SourceVerificationData & sourceData, OutputPinsMapping & outputMapping);
	void wrapSinkNode(const MRModelInterfaceVerifier::SinkVerificationData & sinkData, InputPinsMapping & inputMapping);
	void wrapProcessorNode(const MRModelInterfaceVerifier::ProcessorVerificationData & processorData, InputPinsMapping & inputMapping, OutputPinsMapping & outputMapping);

	void wrapInputPins(ISTMRSinkNode * sink, const MRModelInterfaceVerifier::InputVerification & inputData, InputPinsMapping & inputMapping);
	void wrapOutputPins(ISTMRSourceNode * source, const MRModelInterfaceVerifier::OutputVerification & outputData, OutputPinsMapping & outputMapping);

	void reset();

	void resetDataflowStatus();

	void resetDataflowElements();
	void startDataflow();

	void increaseDataToProcess();
	void decreaseDataToProcess();

	//! \return Czy źródła mają jeszcze dane
	const bool sourcesHaveMore() const;

	bool sourceFinished();
	bool sourceStarted();
	void nonSourceFinished();

	void finishDataflow();
	
	//! kończymy przepływ danych - czyścimy wszystko
	const bool dataflowFinished() const;

	//! \param reader Obiekt pozwalajacy czytać strukturę naszego modelu
	//! \return Czy model można przetwarzać
	static const bool verifyModel(df::IModelReader * reader, MRModelInterfaceVerifier::ModelVerificationData & interfaceVerifier);


	//! Metoda mapuje elementy modelu do obiektów wrappujących, które obsługują logikę przepływu danych
	//! \param reader Obiekt pozwalajacy czytać strukturę naszego modelu
	void wrapModelElements(df::IModelReader * model, const MRModelInterfaceVerifier::ModelVerificationData & modelElements);

private:

	WrapedSources sources_;
	WrapedSinks sinks_;
	WrapedProcessors processors_;
	LoggerHelpers loggerHelpers_;
	NodeRunners nodeRunners_;

	NodesMapping nodesMapping;
	NodesMapping pausedNodes;

	unsigned int finishedRunners;
	unsigned int dataToProcess;
	unsigned int nonSourceElements;
	unsigned int sourcesFinished_;
	unsigned int sourcesStarted_;
	unsigned int nonSourcesFinished_;

	volatile bool sourcesEmpty_;
	bool dataflowFinished_;

	bool failure_;
	std::list<std::string> failureMessages_;

	bool paused_;

	df::IDFLogger * logger_;


public:

	STDFModelRunnerImpl();

	//! \param reader Obiekt pozwalajacy czytać strukturę naszego modelu
	//! \return Czy model można przetwarzać
	static const bool verifyModel(const df::IModelReader * reader);

	void start(df::IModelReader * model, df::IDFLogger * logger);
	void stop();

};

#endif	//	HEADER_GUARD___STDFModelRunnerIMPL_H__
