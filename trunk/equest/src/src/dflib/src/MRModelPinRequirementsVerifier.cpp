#include <dflib/MRModelPinRequirementsVerifier.h>
#include <dflib/IPin.h>
#include <dflib/INode.h>
#include <dflib/IModel.h>

const bool MRModelPinRequirementsVerifier::verifyModelPinRequirements(df::IModelReader * model, UnsatisfiedPins & unsatisfiedPins)
{
	for(unsigned int i = 0; i < model->sourcesSize(); ++i)
	{
		verifyOutputPinRequirements(model->source(i), unsatisfiedPins.unsatisfiedOutputs);
	}

	for(unsigned int i = 0; i < model->sinksSize(); ++i)
	{
		verifyInputPinRequirements(model->sink(i), unsatisfiedPins.unsatisfiedInputs);
	}

	for(unsigned int i = 0; i < model->processorsSize(); ++i)
	{
		auto proc = model->processor(i);
		verifyInputPinRequirements(proc, unsatisfiedPins.unsatisfiedInputs);
		verifyOutputPinRequirements(proc, unsatisfiedPins.unsatisfiedOutputs);
	}

	return unsatisfiedPins.unsatisfiedInputs.empty() && unsatisfiedPins.unsatisfiedOutputs.empty();
}

void MRModelPinRequirementsVerifier::verifyInputPinRequirements(df::ISinkNode * node, UnsatisfiedInputs & unsatisfiedInputs)
{
	for(unsigned int i = 0; i < node->inputSize(); ++i)
	{
		auto * pin = node->inputPin(i);
		df::IExtendedInputPin * input = dynamic_cast<df::IExtendedInputPin*>(pin);
		if(input != nullptr && input->required() == true && input->connected() == false)
		{
			unsatisfiedInputs.push_back(input);
		}
	}
}

void MRModelPinRequirementsVerifier::verifyOutputPinRequirements(df::ISourceNode * node, UnsatisfiedOutputs & unsatisfiedOutputs)
{
	for(unsigned int i = 0; i < node->outputSize(); ++i)
	{
		auto * pin = node->outputPin(i);
		if(pin->connected() == true)
		{
			df::IExtendedOutputPin * output = dynamic_cast<df::IExtendedOutputPin*>(pin);
			if(output != nullptr && output->dependenciesSatisfied() == false)
			{
				unsatisfiedOutputs.push_back(output);
			}
		}
	}
}