#include <dflib/MRModelInterfaceVerifier.h>

#include <dflib/IPin.h>
#include <dflib/IDFPin.h>
#include <dflib/INode.h>
#include <dflib/IDFNode.h>
#include <dflib/IModel.h>

const bool MRModelInterfaceVerifier::verifyModelInterface(df::IModelReader * model, ModelVerificationData & modelVerification)
{
	for(unsigned int i = 0; i < model->sourcesSize(); ++i)
	{
		auto * node = model->source(i);
		if(node->connected() == true){
			df::IDFSource * src = dynamic_cast<df::IDFSource*>(node);
			if(src != nullptr)
			{
				SourceVerificationData srcVerificationData;
				srcVerificationData.node = node;
				srcVerificationData.source = src;

				verifyOutputPinsInterface(node, srcVerificationData.outputVerication, modelVerification.incompatiblePins);

				modelVerification.sourceVerification.push_back(srcVerificationData);
			}else{
				modelVerification.incompatibleNodes.push_back(node);
			}
		}
	}

	for(unsigned int i = 0; i < model->sinksSize(); ++i)
	{
		auto * node = model->sink(i);
		if(node->connected() == true){
			df::IDFSink * snk = dynamic_cast<df::IDFSink*>(node);
			if(snk != nullptr)
			{
				SinkVerificationData snkVerificationData;
				snkVerificationData.node = node;
				snkVerificationData.sink = snk;

				verifyInputPinsInterface(node, snkVerificationData.inputVerication, modelVerification.incompatiblePins);

				modelVerification.sinkVerification.push_back(snkVerificationData);
			}else{
				modelVerification.incompatibleNodes.push_back(node);
			}
		}
	}

	for(unsigned int i = 0; i < model->processorsSize(); ++i)
	{
		auto * node = model->processor(i);
		if(node->connected() == true){
			df::IDFProcessor * proc = dynamic_cast<df::IDFProcessor*>(node);
			if(proc != nullptr)
			{
				ProcessorVerificationData procVerificationData;
				procVerificationData.node = node;
				procVerificationData.processor = proc;

				verifyInputPinsInterface(node, procVerificationData.inputVerication, modelVerification.incompatiblePins);
				verifyOutputPinsInterface(node, procVerificationData.outputVerication, modelVerification.incompatiblePins);

				modelVerification.processorVerification.push_back(procVerificationData);
			}else{
				modelVerification.incompatibleNodes.push_back(node);
			}
		}
	}

	return modelVerification.incompatibleNodes.empty() && modelVerification.incompatiblePins.empty();
}

void MRModelInterfaceVerifier::verifyInputPinsInterface(df::ISinkNode * node, InputVerification & inputVerification, IncompatiblePins & incompatiblePins)
{
	for(unsigned int i = 0; i < node->inputSize(); ++i)
	{	
		auto * pin = node->inputPin(i);

		if(pin->connected() == true){
			df::IDFInput * input = dynamic_cast<df::IDFInput*>(pin);
			if(input != nullptr)
			{
				inputVerification.push_back(InputVerificationData(pin, input));
			}else{
				incompatiblePins.push_back(pin);
			}
		}
	}
}

void MRModelInterfaceVerifier::verifyOutputPinsInterface(df::ISourceNode * node, OutputVerification & outputVerification, IncompatiblePins & incompatiblePins)
{
	for(unsigned int i = 0; i < node->outputSize(); ++i)
	{
		auto * pin = node->outputPin(i);
		if(pin->connected() == true){
			df::IDFOutput * output = dynamic_cast<df::IDFOutput*>(pin);
			if(output != nullptr)
			{
				outputVerification.push_back(OutputVerificationData(pin, output));
			}else{
				incompatiblePins.push_back(pin);
			}
		}
	}
}