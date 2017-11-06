#include <dfmlib/DFModel.h>
#include <dfmlib/DFSourceNode.h>
#include <dfmlib/Connection.h>
#include <algorithm>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFModel::DFModel(void) : Model(), running(false), paused(false), stopPending(false),
    pausePending(false), finishedLeafes(0), finished(false)
{
    //pauseMutex.lock();
    //modelRunner.reset(new ModelRunner(this));
}


DFModel::~DFModel(void)
{
    /*ScopedLock lock(runningMutex);
    if(modelRunner->isRunning() == true){
        modelRunner->finishProcessing();
        pauseMutex.unlock();
        modelRunner->join();
    }*/
}

bool DFModel::isModelChangeAllowed() const
{
    return Model::isModelChangeAllowed() == true && running == false;
}

bool DFModel::isFinished() const
{
    ScopedLock lock(runningMutex);
    
    return finished;
}

bool DFModel::isRunning() const
{
    ScopedLock lock(runningMutex);

    return running;
}

bool DFModel::isPaused() const
{
    ScopedLock lock(runningMutex);

    return paused;
}

void DFModel::run()
{
    ScopedLock lock(runningMutex);
    editMutex.lock();

    //wznów odtwarzanie po pause
    if(paused == true){
        paused = false;
        notifySources();
        notify();
        return;
    }
    
    if(running == true){
        throw std::runtime_error("Model already running!");
    }

    if(isModelValid() == false){
        throw std::runtime_error("Can not run invalid model!");
    }

    running = true;

    if(sourcesHaveMoreData() == true){
        notifySources();
        
        if(finished == false){
            notify();
        }
    }else{
        finished = true;
        notify();
    }    
}

void DFModel::pause()
{
    ScopedLock lock(runningMutex);

    if(running == false){
        throw std::runtime_error("Can not pause - Model is not running!");
    }

    pausePending = true;
}

void DFModel::stop()
{
    ScopedLock lock(runningMutex);

    if(running == false){
        throw std::runtime_error("Can not stop model - model is not running!");
    }

    stopPending = true;
}

void DFModel::reset()
{
    ScopedLock lock(runningMutex);

    if(finished == false){
        throw std::runtime_error("Can not reset model that has not finished!");
    }

    resetPinStates();
    resetNodeStates();

    running = false;
    paused = false;
    finished = false;

    pausePending = false;
    stopPending = false;

    editMutex.unlock();

    notify();
}

bool DFModel::sourcesHaveMoreData() const
{
	bool ret = true;

	for(auto it = sourceNodes.begin(); it != sourceNodes.end(); ++it){
		if((*it)->hasMoreData() == false){
			ret = false;
			break;
		}
	}

	return ret;
}

bool DFModel::additionalConnectRules(const CPinPtr & src, const CPinPtr & dest) const
{
	bool ret = true;

	if( src->getParent() == dest->getParent() || dest->empty() == false || createCycle(src, dest) == true){
		ret = false;
	}

	return ret;
}

void DFModel::notifySources()
{
	for(auto it = sourceNodes.begin(); it != sourceNodes.end(); ++it){
		(*it)->notify();
	}
}

bool DFModel::isNodeSupported(const NPtr & node) const
{
	if(Model::isNodeSupported(node) && DFNode::getDFNode(node) != nullptr){
		return true;
	}

	return false;
}

void DFModel::leafHasProcessedData()
{
    {
        //ScopedLock lock(leavesMutex);
        ++finishedLeafes %= innerGetLeafNodes().size();
    }
    
    //sprawdź czy wszystkie liście przetworzyły dane
	if( finishedLeafes == 0 ){
        ScopedLock _lock(runningMutex);
        if(stopPending == true){
            finished = true;
            notify();
        }else if(pausePending == false){
            if(sourcesHaveMoreData() == true){
                resetPinStates();
                //resetNodeStates();
                notifySources();
            }else{
                finished = true;
                notify();
            }
        }else{
            paused = true;
        }
	}
}

void DFModel::resetPinStates()
{
	for(auto it = getNodes().begin(); it != getNodes().end(); ++it){
		DFNPtr node(DFNode::getDFNode(*it));
		if(node != 0){
			for(auto iT = node->beginIn(); iT != node->endIn(); ++iT){
				resetPinState(*iT);
			}

			for(auto iT = node->beginOut(); iT != node->endOut(); ++iT){
				resetPinState(*iT);
			}
		}
	}
}

void DFModel::resetPinState(const PinPtr & pin)
{
	DFPinPtr p(DFPin::getDFPin(pin));
	//only DFPins are supported
	if(p != nullptr){
		p->reset();
	}
}

void DFModel::resetNodeStates()
{
	for(auto it = getNodes().begin(); it != getNodes().end(); ++it){
		DFNPtr node(DFNode::getDFNode(*it));
		if(node != nullptr){
			node->reset();
		}
	}
}

bool DFModel::additionalModelValidation() const
{
    if(getNodes().empty() == true || sourceNodes.empty() == true || (getNodes().size() - sourceNodes.size()) == 0 ){
        return false;
    }	

    return true;
}

const DFModel::SourceNodes & DFModel::getSourceNodes() const
{
	return sourceNodes;
}

void DFModel::afterNodeAdd(const NPtr & node)
{
    Model::afterNodeAdd(node);

    DFSNPtr sourceNode(utils::dynamic_pointer_cast<DFSourceNode>(node));
    if(sourceNode != nullptr){
        sourceNodes.insert(sourceNode);
    }
}

void DFModel::beforeNodeRemove(const NPtr & node)
{
    Model::beforeNodeRemove(node);

	DFSNPtr sourceNode(utils::dynamic_pointer_cast<DFSourceNode>(node));
    if(sourceNode != nullptr){
        sourceNodes.erase(sourceNode);
    }
}

void DFModel::afterLeafAdd(const NPtr & node)
{
    DFNode::getDFNode(node)->onLeafProcessedCallback = DFNode::OnLeafProcessedCallback(std::bind(&DFModel::leafHasProcessedData, this));
}

void DFModel::beforeLeafRemove(const NPtr & node)
{
    DFNode::getDFNode(node)->onLeafProcessedCallback = DFNode::OnLeafProcessedCallback();
}

//DFModel::ModelRunner::ModelRunner(DFModel * model) : model(model), finish(false)
//{
//
//}
//
//DFModel::ModelRunner::~ModelRunner()
//{
//
//}
//
//void DFModel::ModelRunner::run()
//{
//    while(true){
//        {
//            ScopedLock lock(model->pauseMutex);
//            if(finish == true){
//                return;
//            }
//        }
//
//        model->pauseMutex.lock();
//
//        model->notifySources();        
//    }
//}
//
//void DFModel::ModelRunner::finishProcessing()
//{
//    finish = true;
//}

}
