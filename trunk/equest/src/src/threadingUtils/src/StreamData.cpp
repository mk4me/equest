#include <threadingUtils/StreamData.h>
#include <algorithm>
#include <atomic>

using namespace threadingUtils;

class ResetableStreamStatusObserver::ResetableStreamStatusImpl
{
public:
	ResetableStreamStatusImpl() : modified_(false) {}
	~ResetableStreamStatusImpl() {}

	bool modified() const
	{		
		return modified_.exchange(false);
	}

	void update()
	{
		modified_ = true;
	}

private:
	mutable std::atomic<bool> modified_;
};

ResetableStreamStatusObserver::ResetableStreamStatusObserver()
: impl(new ResetableStreamStatusImpl)
{
}

ResetableStreamStatusObserver::~ResetableStreamStatusObserver()
{
}

bool ResetableStreamStatusObserver::modified() const
{
	return impl->modified();
}

void ResetableStreamStatusObserver::update()
{
	impl->update();
}

StreamBase::StreamBase()
{
}

void StreamBase::notify()
{
	std::lock_guard<std::recursive_mutex> lock(synch_);
	for (auto it = observers.begin(); it != observers.end(); ++it){
		(*it)->update();
	}
}

StreamBase::~StreamBase()
{
}

void StreamBase::attach(StreamStatusObserverPtr observer)
{
	std::lock_guard<std::recursive_mutex> lock(synch_);
	if (observer == nullptr){
		throw std::invalid_argument("Empty observer");
	}

	if (std::find(observers.begin(), observers.end(), observer) == observers.end()){
		observers.push_back(observer);
	}
}

void StreamBase::detach(StreamStatusObserverPtr observer)
{
	std::lock_guard<std::recursive_mutex> lock(synch_);
	observers.remove(observer);
}
