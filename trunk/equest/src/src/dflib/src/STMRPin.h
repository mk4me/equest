/********************************************************************
	created:  2012/12/10
	created:  10:12:2012   16:30
	filename: MRPin.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___STMRPIN_H__
#define HEADER_GUARD___STMRPIN_H__

#include <vector>
#include <dflib/IDFFeatures.h>
#include <dflib/IDFPin.h>
#include <dflib/IPin.h>

class ISTMRSource;
class ISTMRSink;
class STMRConnection;

class ISTMRPin : public df::IDFResetable
{
public:
	typedef unsigned int size_type;

public:
	virtual void update() = 0;
	virtual const bool updated() const = 0;

	virtual void consumeData() = 0;
	virtual const bool dataConsumed() const = 0;

	virtual STMRConnection * connection(size_type idx) = 0;
	virtual const STMRConnection * connection(size_type idx) const = 0;

	virtual size_type size() const = 0;
	virtual const bool empty() const = 0;

};

class STMRInputPin : public ISTMRPin
{
public:
	STMRInputPin(ISTMRSink * sink, df::IInputPin * pin, df::IDFInput * dfInput);
	~STMRInputPin();

	ISTMRSink * sink();
	const ISTMRSink * sink() const;

	virtual void reset();

	virtual void update();
	virtual const bool updated() const;

	virtual void consumeData();
	virtual const bool dataConsumed() const;

	virtual STMRConnection * connection(size_type idx);
	virtual const STMRConnection * connection(size_type idx) const;

	void addConnection(STMRConnection * connection);

	virtual size_type size() const;
	virtual const bool empty() const;

	df::IInputPin * innerPin();
	const df::IInputPin * innerPin() const;

	df::IDFInput * dfInput();
	const df::IDFInput * dfInput() const;

private:
	ISTMRSink * sink_;
	df::IInputPin * pin_;
	df::IDFInput * dfInput_;
	bool consumed_;
	std::vector<STMRConnection*> connections;
	unsigned int outputsReady;
};

class STMROutputPin : public ISTMRPin
{
public:
	STMROutputPin(ISTMRSource * source, df::IOutputPin * pin, df::IDFOutput * dfOutput);
	~STMROutputPin();

	ISTMRSource * source();
	const ISTMRSource * source() const;

	virtual void reset();

	virtual void update();
	virtual const bool updated() const;

	virtual void consumeData();
	virtual const bool dataConsumed() const;

	virtual STMRConnection * connection(size_type idx);
	virtual const STMRConnection * connection(size_type idx) const;

	void addConnection(STMRConnection * connection);

	virtual size_type size() const;
	virtual const bool empty() const;

	df::IOutputPin * innerPin();
	const df::IOutputPin * innerPin() const;

	df::IDFOutput * dfOutput();
	const df::IDFOutput * dfOutput() const;

private:
	ISTMRSource * source_;
	df::IOutputPin * pin_;
	df::IDFOutput * dfOutput_;
	bool updated_;
	std::vector<STMRConnection*> connections;
	unsigned int inputsConsumed;
};

#endif	//	HEADER_GUARD___STMRPIN_H__
