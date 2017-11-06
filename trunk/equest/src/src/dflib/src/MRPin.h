/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   16:30
	filename: MRPin.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___MRPIN_H__
#define HEADER_GUARD___MRPIN_H__

#include <mutex>
#include <vector>
#include <dflib/IDFFeatures.h>
#include <dflib/IDFPin.h>
#include <dflib/IPin.h>

class IMRSource;
class IMRSink;
class MRConnection;

class IMRPin : public df::IDFResetable
{
public:
	typedef unsigned int size_type;

public:
	virtual void update() = 0;
	virtual const bool updated() const = 0;

	virtual void consumeData() = 0;
	virtual const bool dataConsumed() const = 0;

	virtual MRConnection * connection(size_type idx) = 0;
	virtual const MRConnection * connection(size_type idx) const = 0;

	virtual size_type size() const = 0;
	virtual const bool empty() const = 0;

protected:

	mutable std::mutex mutex;
};

class MRInputPin : public IMRPin
{
public:
	MRInputPin(IMRSink * sink, df::IInputPin * pin, df::IDFInput * dfInput);
	~MRInputPin();

	IMRSink * sink();
	const IMRSink * sink() const;

	virtual void reset();

	virtual void update();
	virtual const bool updated() const;

	virtual void consumeData();
	virtual const bool dataConsumed() const;

	virtual MRConnection * connection(size_type idx);
	virtual const MRConnection * connection(size_type idx) const;

	void addConnection(MRConnection * connection);

	virtual size_type size() const;
	virtual const bool empty() const;

	df::IInputPin * innerPin();
	const df::IInputPin * innerPin() const;

	df::IDFInput * dfInput();
	const df::IDFInput * dfInput() const;

private:
	IMRSink * sink_;
	df::IInputPin * pin_;
	df::IDFInput * dfInput_;
	bool consumed_;
	std::vector<MRConnection*> connections;
	unsigned int outputsReady;
};

class MROutputPin : public IMRPin
{
public:
	MROutputPin(IMRSource * source, df::IOutputPin * pin, df::IDFOutput * dfOutput);
	~MROutputPin();

	IMRSource * source();
	const IMRSource * source() const;

	virtual void reset();

	virtual void update();
	virtual const bool updated() const;

	virtual void consumeData();
	virtual const bool dataConsumed() const;

	virtual MRConnection * connection(size_type idx);
	virtual const MRConnection * connection(size_type idx) const;

	void addConnection(MRConnection * connection);

	virtual size_type size() const;
	virtual const bool empty() const;

	df::IOutputPin * innerPin();
	const df::IOutputPin * innerPin() const;

	df::IDFOutput * dfOutput();
	const df::IDFOutput * dfOutput() const;

private:
	IMRSource * source_;
	df::IOutputPin * pin_;
	df::IDFOutput * dfOutput_;
	bool updated_;
	std::vector<MRConnection*> connections;
	unsigned int inputsConsumed;
};

#endif	//	HEADER_GUARD___MRPIN_H__
