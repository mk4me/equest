#include <timelinelib/Controller.h>

static const unsigned int DefaultDelay = 1000000 / 60;

namespace timeline {

	Controller::Controller() : IController(&dirtyState),
		model(new Model()), constModel(model), timeUpdateMode(AsynchTimeUpdate),
		playbackDirection(PlayForward), timer(new Timer(DefaultDelay)),
		timeGenerator(&Controller::forwardTimeUpdate),
		loopbackMode(LoopbackOff)
	{
		timer->setController(this);
	}

	Controller::~Controller()
	{

	}

	const ModelConstPtr & Controller::getModel() const
	{
		return constModel;
	}

	double Controller::getNormalizedTime(double time) const
	{
		double ret = 0;
		if (model->getLength() > 0){
			ret = std::max(0.0, std::min(1.0, (time - model->getBeginTime()) / model->getLength()));
		}

		return ret;
	}

	double Controller::getNormalizedLength(double length) const
	{
		double ret = 0;
		if (model->getLength() > 0){
			ret = std::max(0.0, std::min(1.0, length / model->getLength()));
		}

		return ret;
	}

	std::pair<double, double> Controller::getNormalizedRange(double begin, double length) const
	{
		std::pair<double, double> ret(0, 0);
		if (model->getLength() > 0){
			ret.first = getNormalizedTime(begin);
			ret.second = getNormalizedLength(length);
			if (ret.first + ret.second > 1.0){
				ret.second = 1.0;
			}
		}

		return ret;
	}

	double Controller::getAbsoluteTime(double normalizedTime) const
	{
		UTILS_ASSERT((normalizedTime >= 0.0 && normalizedTime <= 1.0), "Błędny czas - poza zakresem 0-1");
		return normalizedTime * model->getLength() + model->getBeginTime();
	}

	double Controller::getAbsoluteLength(double normalizedLength) const
	{
		UTILS_ASSERT((normalizedLength >= 0.0 && normalizedLength <= 1.0), "Błędny czas - poza zakresem 0-1");
		return normalizedLength * model->getLength();
	}

	std::pair<double, double> Controller::getAbsoluteRange(double begin, double length) const
	{
		UTILS_ASSERT((begin >= 0.0 && begin <= 1.0), "Błędny czas początku - poza zakresem 0-1");
		UTILS_ASSERT((length >= 0.0 && length <= 1.0), "Błędna długość - poza zakresem 0-1");

		std::pair<double, double> ret(0, 0);

		ret.first = getAbsoluteTime(begin);
		ret.second = getAbsoluteLength(length);
		if (ret.first + ret.second > model->getLength()){
			ret.second = model->getLength() - ret.first;
		}

		return ret;
	}

	void Controller::setTimeUpdateMode(TimeUpdateMode timeUpdateMode)
	{
		ScopedLock lock(stateMutex);
		this->timeUpdateMode = timeUpdateMode;
	}

	Controller::TimeUpdateMode Controller::getTimeUpdateMode() const
	{
		ScopedLock lock(stateMutex);
		return timeUpdateMode;
	}

	void Controller::setPlaybackDirection(PlaybackDirection playbackDirection)
	{
		ScopedLock lock(stateMutex);
		this->playbackDirection = playbackDirection;
		timeGenerator = (playbackDirection == PlayForward) ? &Controller::forwardTimeUpdate : &Controller::backwardTimeUpdate;
	}

	Controller::PlaybackDirection Controller::getPlaybackDirection() const
	{
		ScopedLock lock(stateMutex);
		return playbackDirection;
	}

	//! \param loopbackMode
	void Controller::setLoopbackMode(LoopbackMode loopbackMode)
	{
		ScopedLock lock(stateMutex);
		this->loopbackMode = loopbackMode;
	}

	//! \return
	Controller::LoopbackMode Controller::getLoopbackMode() const
	{
		ScopedLock lock(stateMutex);
		return loopbackMode;
	}

	void Controller::run()
	{
		runningThreadID = std::this_thread::get_id();
		timer->run();
	}

	void Controller::finalize()
	{
		if (timer != nullptr){
			timer->finalize();
		}

		runningThreadID = std::thread::id();

		{
			ScopedLock lock(stateMutex);

			State state = getState();

			state.isPlaying = false;

			setState(state);
		}

		cv.notify_all();
	}

	void Controller::play()
	{
		ScopedLock lock(stateMutex);

		State state = getState();

		if (state.isPlaying == true){
			return;
		}

		state.isPlaying = true;
		setState(state);

		cv.notify_all();
	}

	void Controller::pause()
	{
		ScopedLock lock(stateMutex);

		State state = getState();

		if (state.isPlaying == false){
			return;
		}

		state.isPlaying = false;
		setState(state);
	}

	bool Controller::isPlaying() const
	{
		return getState().isPlaying;
	}

	double Controller::getLength() const
	{
		return getState().length;
	}

	double Controller::getTime() const
	{
		return getState().time;
	}

	double Controller::getNormalizedTime() const
	{
		return getState().normalizedTime;
	}

	double Controller::getTimeScale() const
	{
		return getState().timeScaleFactor;
	}

	void Controller::setTimeScale(double timeScale)
	{
		ScopedLock lock(stateMutex);

		// nowy stan
		State state = getState();
		state.timeScaleFactor = timeScale;
		setState(state);
	}

	void Controller::setTime(double time)
	{
		ScopedLock lock(stateMutex);

		// nowy stan
		State state = getState();
		state.time = time;
		state.normalizedTime = time / getLength();
		setState(state);
		timeDirty = true;
	}

	void Controller::setNormalizedTime(double normTime)
	{
		setTime(normTime * getLength());
	}

	State Controller::getState() const
	{
		if (isWriteEnabled()) {
			return model->getState();
		}
		else {
			return dirtyState;
		}
	}

	void Controller::setState(const State& state)
	{
		if (isWriteEnabled()) {
			ScopedLock lock(modelMutex);
			busy = true;
			model->setState(state);
			dirty = false;
			busy = false;
			timeDirty = false;
		}
		else {
			dirty = true;
		}
		dirtyState = state;
		notify();
	}

	bool Controller::isWriteEnabled() const
	{
		bool ret = false;

		if (std::this_thread::get_id() == runningThreadID || model->getState().isPlaying == false || timeUpdateMode == AsynchTimeUpdate) {
			ret = true;
		}

		return ret;
	}

	bool Controller::isBusy() const
	{
		ScopedLock lock(stateMutex);
		return busy;
	}

	bool Controller::isDirty() const
	{
		ScopedLock lock(stateMutex);
		return dirty;
	}

	bool Controller::isTimeDirty() const
	{
		ScopedLock lock(stateMutex);
		return timeDirty;
	}

	void Controller::addChannel(const std::string & path, const IChannelPtr & channel)
	{
		ScopedLock lock(stateMutex);
		ScopedLock lock2(modelMutex);
		model->addChannel(path, channel);
		State state = getState();
		double newLength = model->getLength();
		if (state.length != newLength){
			state.length = newLength;
		}

		setState(state);
	}

	void Controller::addChannels(const std::map<std::string, IChannelPtr> & channels)
	{
		ScopedLock lock(stateMutex);
		ScopedLock lock2(modelMutex);
		model->addChannels(channels);
		State state = getState();
		double newLength = model->getLength();
		if (state.length != newLength){
			state.length = newLength;
		}

		setState(state);
	}


	void Controller::removeChannel(const std::string & path)
	{
		ScopedLock lock(stateMutex);
		ScopedLock lock2(modelMutex);
		model->removeChannel(path);
		State state = getState();
		double newLength = model->getLength();
		if (state.length != newLength){
			state.length = newLength;
		}

		setState(state);
	}

	void Controller::removeChannels(const std::set<std::string> & paths)
	{
		ScopedLock lock(stateMutex);
		ScopedLock lock2(modelMutex);
		model->removeChannels(paths);
		State state = getState();
		double newLength = model->getLength();
		if (state.length != newLength){
			state.length = newLength;
		}

		setState(state);
	}


	double Controller::forwardTimeUpdate(unsigned long int realTimeDeltaMS)
	{
		const auto nextTime = getTime() + (double)realTimeDeltaMS / 1000000.0;
		return (loopbackMode == LoopbackOff) ?
			std::min(model->getEndTime(), nextTime) :
			((nextTime > model->getEndTime()) ? model->getBeginTime() : nextTime);
	}

	double Controller::backwardTimeUpdate(unsigned long int realTimeDeltaMS)
	{
		const auto nextTime = getTime() - (double)realTimeDeltaMS / 1000000.0;
		return (loopbackMode == LoopbackOff) ?
			std::max(model->getBeginTime(), nextTime) :
			((nextTime < model->getBeginTime()) ? model->getEndTime() : nextTime);
	}

	bool Controller::setNextTime(unsigned long int realTimeDeltaMS)
	{
		ScopedLock lock(stateMutex);

		bool ret = false;

		State state = getState();

		if (state.length > 0){

			state.time = (this->*timeGenerator)(realTimeDeltaMS);
			state.normalizedTime = state.time / state.length;

			if (loopbackMode == LoopbackOff){

				if (getPlaybackDirection() == PlayForward){
					if (getTime() == model->getEndTime()){
						ret = true;
					}
				}
				else if (getTime() == model->getBeginTime()){
					ret = true;
				}

				if (ret == true){
					state.isPlaying = false;
				}
			}

			setState(state);
		}

		return ret;
	}

	void Controller::setChannelActive(const std::string & path, bool active)
	{
		ScopedLock lock(stateMutex);
		model->setChannelActive(path, active);
	}

	Model::TChannelConstPtr Controller::findChannel(const std::string & path) const
	{
		ScopedLock lock(stateMutex);
		return model->findChannel(path);
	}

	Controller::Timer::Timer(unsigned long int delay)
		: delay(delay), finalize_(false)
	{
		UTILS_ASSERT((delay != 0), "Zły czas pomiędzy tikami tegara");
	}

	Controller::Timer::~Timer()
	{

	}

	void Controller::Timer::finalize()
	{
		finalize_ = true;
		controller->cv.notify_all();
	}

	void Controller::Timer::run()
	{
		if (controller == nullptr){
			throw std::runtime_error("Wrong controller to update");
		}

		while (finalize_ == false){
			if (controller->isPlaying() == false){
				// czy nie ma pauzy
				std::unique_lock<std::mutex> lock(controller->pauseMutex);
				controller->cv.wait(lock, [this]() { return controller->isPlaying() == true || finalize_ == true; });
			}

			controller->setNextTime(delay);

			//zasypiamy
			std::this_thread::sleep_for(std::chrono::microseconds(delay));
		}
	}

	void Controller::Timer::setController(Controller * controller)
	{
		UTILS_ASSERT((controller != nullptr), "Błędny kontroler do aktualizacji");
		this->controller = controller;
	}

	void Controller::Timer::setDelay(unsigned long int delay)
	{
		UTILS_ASSERT((delay > 0), "Blędne opóźnienie zegara");
		this->delay = delay;
	}

	unsigned long int Controller::Timer::getDelay() const
	{
		return delay;
	}

}