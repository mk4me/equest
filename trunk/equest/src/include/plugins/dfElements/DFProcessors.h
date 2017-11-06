/********************************************************************
	created:	2013/02/28
	created:	28:2:2013   18:31
	filename: 	DFProcessors.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DFELEMENTS__HMMPROCESSORS_H__
#define HEADER_GUARD_DFELEMENTS__HMMPROCESSORS_H__

#include <plugins/newVdf/UniversalInputPin.h>
#include <plugins/newVdf/UniversalOutputPin.h>
#include <plugins/dfElements/DFPins.h>
#include <QtWidgets/QDoubleSpinBox>
#include <dflib/Node.h>
#include <dflib/IDFNode.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>
#include <dataaccessorlib/DescriptorFeature.h>
#include <dataaccessorlib/Wrappers.h>

class VectorDiff : public df::ProcessingNode, public df::IDFProcessor
{
public:
    VectorDiff();

public:
    virtual void reset();
    virtual void process();

private:
    VectorInputPin* inPinA;
    VectorInputPin* inPinB;
    VectorOutputPin* outPinA;
    std::string name;
};

class VectorAdder : public df::ProcessingNode, public df::IDFProcessor
{
public:
    VectorAdder();

public:
    virtual void reset();
    virtual void process();

private:
    VectorInputPin* inPinA;
    VectorInputPin* inPinB;
    VectorOutputPin* outPinA;
    std::string name;
};

template <class InputPinT, class OutputPinT, class InputPtr, class OutputPtr>
class UniversalChannelProcessor : public df::ProcessingNode, public df::IDFProcessor
{
public:
    UniversalChannelProcessor()
    {
        inPinA = new InputPinT(this);
        outPinA = new OutputPinT(this);
        addInputPin(inPinA);
        addOutputPin(outPinA);
    }

public:
    virtual void reset() {}
    typedef typename OutputPtr::element_type::value_type SimpleOutT;
	typedef typename InputPtr::element_type::value_type SimpleInT;
    virtual SimpleOutT processSingleValue(const SimpleInT& val) = 0;
    virtual void process()
    {
        InputPtr signal1 = inPinA->getValue();

        if (signal1) {
			
			auto uaf = signal1->template getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
			
			std::vector<typename OutputPtr::element_type::sample_type> data;
			data.reserve(signal1->size());

			for (size_type i = 0; i < signal1->size(); ++i) {
				const auto sample = signal1->sample(i);
				data.push_back({ sample.first, processSingleValue(sample.second)});
			}

			auto channel = dataaccessor::wrap(std::move(data));

			channel->attachFeature(uaf);
			channel->attachFeature(utils::make_shared<dataaccessor::DescriptorFeature>("Result", "", "", "", ""));

            outPinA->setValue(channel);
        } else {

        }
    }

private:
    InputPinT * inPinA;
    OutputPinT * outPinA;
    std::string name;
};


template <class InputPinT, class OutputPinT, class InputPtr, class OutputPtr>
class UniversalChannel2Processor : public df::ProcessingNode, public df::IDFProcessor
{
public:
    UniversalChannel2Processor()
    {
        inPinA = new InputPinT(this);
        inPinB = new InputPinT(this);
        outPinA = new OutputPinT(this);
        addInputPin(inPinA);
        addInputPin(inPinB);
        addOutputPin(outPinA);
    }

public:
    virtual void reset() {}
    typedef typename OutputPtr::element_type::value_type SimpleOutT;
	typedef typename InputPtr::element_type::value_type SimpleInT;
    virtual SimpleOutT processSingleValue(const SimpleInT& v1, const SimpleInT& v2) = 0;
    virtual void process()
    {
        InputPtr signal1 = inPinA->getValue();
        InputPtr signal2 = inPinB->getValue();

        if (signal1 && signal2) {
			const size_type count = (std::min)(signal1->size(), signal2->size());
			std::vector<typename OutputPtr::element_type::sample_type> out(count);

			auto uaf = signal1->template getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();

			const auto interval = uaf->argumentsInterval();            

            for (size_type i = 0; i < count; ++i) {
                const auto val = processSingleValue(signal1->value(i), signal2->value(i));
				out[i] = { i * interval, val };
            }			

			auto df = signal1->template feature<dataaccessor::DescriptorFeature>();

			dataaccessor::IFeaturePtr description;

			if (df != nullptr){

				description = utils::make_shared<dataaccessor::DescriptorFeature>("Result",
					df->valueType(), df->valueUnit(),
					df->argumentType(), df->argumentUnit());
			}
			else{
				description.reset(dataaccessor::DescriptorFeature::create<typename OutputPtr::element_type::value_type,
					typename OutputPtr::element_type::argument_type>("Result",
					df->valueUnit(), df->argumentUnit()));
			}

			auto channel = dataaccessor::wrap(std::move(out));
			channel->attachFeature(uaf);
			channel->attachFeature(description);

            outPinA->setValue(channel);
        } else {

        }
    }

private:
   InputPinT * inPinA;
   InputPinT * inPinB;
   OutputPinT * outPinA;
   std::string name;
};

typedef UniversalChannelProcessor<ScalarInputPin, ScalarOutputPin, c3dlib::ScalarChannelReaderInterfaceConstPtr, c3dlib::ScalarChannelReaderInterfaceConstPtr> UniversalScalar;
typedef UniversalChannel2Processor<ScalarInputPin, ScalarOutputPin, c3dlib::ScalarChannelReaderInterfaceConstPtr, c3dlib::ScalarChannelReaderInterfaceConstPtr> Universal2Scalar;
typedef UniversalChannelProcessor<VectorInputPin, VectorOutputPin, c3dlib::VectorChannelReaderInterfaceConstPtr, c3dlib::VectorChannelReaderInterfaceConstPtr> UniversalVector;
typedef UniversalChannel2Processor<VectorInputPin, VectorOutputPin, c3dlib::VectorChannelReaderInterfaceConstPtr, c3dlib::VectorChannelReaderInterfaceConstPtr> Universal2Vector;

class ScalarDiff : public Universal2Scalar
{
public:
    virtual float processSingleValue(const float& v1, const float& v2) 
    {
        return v1 - v2;
    }
};

class ScalarAdd : public Universal2Scalar
{
public:
    virtual float processSingleValue(const float& v1, const float& v2) 
    {
        return v1 + v2;
    }
};

class ScalarTreshold : public UniversalScalar, public vdf::INodeConfiguration
{
public:
    ScalarTreshold();
    virtual float processSingleValue(const float& v1) 
    {
        float treshold = static_cast<float>(spinbox->value());
        return (std::min)(v1, treshold);
    }

    virtual QWidget* getConfigurationWidget();
    virtual void refreshConfiguration();
private:
    QWidget* widget;
    QDoubleSpinBox* spinbox;
};

class ScalarMul : public Universal2Scalar
{
public:
    virtual float processSingleValue(const float& v1, const float& v2) 
    {
        return v1 + v2;
    }
};

class ScalarInverter : public UniversalScalar
{
public:
    virtual float processSingleValue(const float& v1) 
    {
        return -v1;
    }
};

class ScalarDummy : public UniversalScalar
{
public:
    virtual float processSingleValue(const float& v1) 
    {
        return v1;
    }
};

class VectorMul : public Universal2Vector
{
public:
    virtual osg::Vec3 processSingleValue(const osg::Vec3& v1, const osg::Vec3& v2) 
    {
        return osg::Vec3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
    }
};

class Vector2Scalar : public df::ProcessingNode, public df::IDFProcessor
{
public:
    Vector2Scalar();

public:
    virtual void reset();
    virtual void process();

private:
    VectorInputPin * inPinA;
    ScalarOutputPin * outPinX;
    ScalarOutputPin * outPinY;
    ScalarOutputPin * outPinZ;
    std::string name;
};

class Scalar2Vector : public df::ProcessingNode, public df::IDFProcessor
{
public:
    Scalar2Vector();

public:
    virtual void reset();
    virtual void process();

private:
    VectorOutputPin * outPinA;
    ScalarInputPin * inPinX;
    ScalarInputPin * inPinY;
    ScalarInputPin * inPinZ;
    std::string name;
};

#endif
