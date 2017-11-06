/********************************************************************
	created:	2012/12/13
	created:	13:12:2012   11:03
	filename: 	ExampleItems.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__EXAMPLEITEMS_H__
#define HEADER_GUARD_NEWVDF__EXAMPLEITEMS_H__

#include <corelib/PluginCommon.h>
#include <dflib/Pin.h>
#include <dflib/IDFPin.h>
#include <dflib/IDFNode.h>
#include <dflib/Node.h>
#include <functional>
#include <loglib/ILog.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <QtWidgets/QLabel>

namespace vdf {

class IntOutputPin : public df::OutputPin, public df::IDFOutput
{
public:
    IntOutputPin(df::ISourceNode * node) : df::OutputPin(node), val(0) {}

    const int value() const { return val; }
    void value(int val) { this->val = val; }

    virtual void reset()
    {
        val = 0;
    }

private:
    int val;
};


class IntInputPin : public df::InputPin, public df::IDFInput
{
public:

    IntInputPin(df::ISinkNode * node) : df::InputPin(node), val(0) {}

    virtual void copyData(const df::IDFOutput * pin)
    {
        val = dynamic_cast<const IntOutputPin*>(pin)->value();
    }

    virtual void reset()
    {
        val = 0;
    }

    const int value() const { return val; }

private:

    int val;
};

class IntSink : public df::SinkNode, public df::IDFSink
{
public:
    typedef std::vector<int> Data;

public:

    IntSink(const std::string & name = "")
    {
        inPinA = new IntInputPin(this);
        addInputPin(inPinA);
    }

    virtual void reset() { Data().swap(data_); }

    virtual void consume()
    {
        data_.push_back(inPinA->value());
		//PLUGIN_LOG_INFO("wynik: " << (inPinA->value()));
    }

    const Data & data() const
    {
        return data_;
    }

private:
    std::string name;
    IntInputPin * inPinA;
    Data data_;
};


class IntProcessor : public df::ProcessingNode, public df::IDFProcessor, public INodeConfiguration
{
public:

    IntProcessor()
    {
        inPinA = new IntInputPin(this);
        inPinB = new IntInputPin(this);
        outPinA = new IntOutputPin(this);
        addInputPin(inPinA);
        addInputPin(inPinB);
        addOutputPin(outPinA);
        widget = new QLabel("The method or operation is not implemented.");
    }

    virtual ~IntProcessor() { delete widget; }

    virtual void reset() {}

    virtual void process()
    {
        outPinA->value(inPinA->value() + inPinB->value());
    }

    virtual QWidget* getConfigurationWidget()
    {
        return widget;
    }

    virtual void refreshConfiguration() 
    {
    }

private:
    IntInputPin * inPinA;
    IntInputPin * inPinB;
    IntOutputPin * outPinA;

    std::string name;
    QWidget* widget;
};



class IntSource : public df::SourceNode, public df::IDFSource
{
public:

    IntSource() : idx(0)
    {
        outPinA = new IntOutputPin(this);
        addOutputPin(outPinA);
    }

    virtual void reset() { idx = 0; }

    virtual const bool empty() const { return idx == 10; }

    virtual void produce()
    {
        outPinA->value(idx++);
    }

private:
    IntOutputPin * outPinA;
    int idx;
};

}

#endif
