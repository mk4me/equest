#include <dflib/MRModelLogicVerifier.h>
#include <dflib/INode.h>
#include <dflib/IModel.h>

const bool MRModelLogicVerifier::verifyModelLogic(df::IModelReader * model, ModelVerificationData & logicVerification)
{
	for(unsigned int i = 0; i < model->nodesSize(); ++i)
	{
		auto node = model->node(i);
		if(node->connected() == false)
		{
			logicVerification.unconnectedNodes.push_back(node);
		}

		/*for(unsigned int i = 0; i < model->rootsSize(); ++i)
		{
			logicVerification.artificialSources.push_back(model->root(i));
		}*/
	}

	return logicVerification.artificialSources.empty() && logicVerification.unconnectedNodes.empty();
}