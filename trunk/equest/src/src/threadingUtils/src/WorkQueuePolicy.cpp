#include <threadingUtils/WorkQueuePolicy.h>

using namespace threadingUtils;

StealingMultipleWorkQueuePolicy::StealingMultipleWorkQueuePolicy()
{
	//initializeThread();
}

StealingMultipleWorkQueuePolicy::~StealingMultipleWorkQueuePolicy()
{
	//deinitializeThread();
}
			
void StealingMultipleWorkQueuePolicy::initializeThread()
{
	if (localWorkQueue() != nullptr){
		return;
	}

	utils::shared_ptr<WorkStealingQueue> localQueue(utils::make_shared<WorkStealingQueue>());
	const std::thread::id myID = std::this_thread::get_id();
	{
		std::lock_guard<std::mutex> lock(mutex);
		localQueues.insert({ myID, localQueue });
	}

	localWorkQueue() = localQueue.get();
}

void StealingMultipleWorkQueuePolicy::deinitializeThread()
{
	const std::thread::id myID = std::this_thread::get_id();			
	{
		std::lock_guard<std::mutex> lock(mutex);
		rescheduleLocalQueue();
		localQueues.erase(myID);
	}

	localWorkQueue() = nullptr;
}

StealingMultipleWorkQueuePolicy::size_type StealingMultipleWorkQueuePolicy::size() const
{			
	size_type ret = commonQueue.size();

	std::lock_guard<std::mutex> lock(mutex);
	for (auto lqd : localQueues){
		ret += lqd.second->size();
	}

	return ret;
}

bool StealingMultipleWorkQueuePolicy::tryGet(Task & task)
{
	return ((tryPopWorkFromLocalQueue(task) == true) ||
		(tryPopWorkFromCommonQueue(task) == true) ||
		(tryPopWorkFromOtherLocalQueue(task) == true));
}

void StealingMultipleWorkQueuePolicy::clearLocalQueue()
{
	if (localWorkQueue() != nullptr) { localWorkQueue()->clear(); }
}

bool StealingMultipleWorkQueuePolicy::empty() const
{
	return ((commonQueue.empty() == true) && (localQueuesEmpty() == true));
}

void StealingMultipleWorkQueuePolicy::rescheduleLocalQueue()
{
	const auto s = localQueues.size() - 1;
	if (s > 0){
		//jest komu przerzuciæ zadania
		std::vector<utils::shared_ptr<WorkStealingQueue>> rescheduledQueues;
		rescheduledQueues.reserve(s);

		for (auto lc : localQueues){
			if (lc.second.get() != localWorkQueue()){
				rescheduledQueues.push_back(lc.second);
			}
		}

		unsigned int i = 0;
		//TODO - dodaæ kolejkê priorytetow¹, gwarantuj¹c¹ rozdawanym zadaniom najwy¿szy priorytet
		while (localWorkQueue()->empty() == false){			
			FunctionWrapper functionWrapper;
			if (localWorkQueue()->trySteal(functionWrapper) == true){
				rescheduledQueues[i++]->push(std::move(functionWrapper));
				i = (i + 1) % s;
			}
		}
	}
}
		
bool StealingMultipleWorkQueuePolicy::localQueuesEmpty() const
{
	std::lock_guard<std::mutex> lock(mutex);
	for (auto lqd : localQueues){
		if (lqd.second->empty() == true){
			return true;
		}
	}

	return false;
}

bool StealingMultipleWorkQueuePolicy::tryPopWorkFromLocalQueue(Task & task)
{
	if ((localWorkQueue() != nullptr) && (localWorkQueue()->tryPop(task.functionWrapper) == true)){
		task.id = std::this_thread::get_id();
		return true;
	}

	return false;
}

bool StealingMultipleWorkQueuePolicy::tryPopWorkFromCommonQueue(Task & task)
{
	if(commonQueue.tryPop(task.functionWrapper) == true){
		task.id = std::this_thread::get_id();
		return true;
	}

	return false;
}

bool StealingMultipleWorkQueuePolicy::tryPopWorkFromOtherLocalQueue(Task & task)
{
	std::lock_guard<std::mutex> lock(mutex);
	for (auto lqd : localQueues){
		if (lqd.second->trySteal(task.functionWrapper) == true){
			task.id = lqd.first;
			return true;
		}
	}

	return false;
}

StealingQueue<FunctionWrapper> *& StealingMultipleWorkQueuePolicy::localWorkQueue()
{
	static thread_local StealingQueue<FunctionWrapper> * localWorkQueue_ = nullptr;
	return localWorkQueue_;
}