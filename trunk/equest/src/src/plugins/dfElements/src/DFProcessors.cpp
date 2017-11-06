#include <plugins/dfElements/DFProcessors.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <dataaccessorlib/UniformArgumentsFeature.h>

void VectorDiff::process()
{
    c3dlib::VectorChannelReaderInterfaceConstPtr signal1 = inPinA->getValue();
    c3dlib::VectorChannelReaderInterfaceConstPtr signal2 = inPinB->getValue();

    if (signal1 && signal2) {
		double step = 0.0;
		auto uaf = signal1->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
		if (uaf != nullptr){
			step = uaf->argumentsInterval();
		}

		const size_type count = (std::min)(signal1->size(), signal2->size());
		std::vector<c3dlib::VectorChannelReaderInterface::sample_type> data;
		data.reserve(count);
        
        for (size_type i = 0; i < count; ++i) {
            auto val = signal1->value(i) - signal2->value(i);
			data.push_back({ i * step, val });
        }

		auto channel = dataaccessor::wrap(std::move(data));
		channel->attachFeature(uaf);
		channel->attachFeature(utils::make_shared<dataaccessor::DescriptorFeature>("Result", "", "", "", ""));
        outPinA->setValue(channel);
    } else {

    }
}

void VectorDiff::reset()
{

}

VectorDiff::VectorDiff()
{
    inPinA = new VectorInputPin(this);
    inPinB = new VectorInputPin(this);
    outPinA = new VectorOutputPin(this);
    addInputPin(inPinA);
    addInputPin(inPinB);
    addOutputPin(outPinA);
}

VectorAdder::VectorAdder()
{
    inPinA = new VectorInputPin(this);
    inPinB = new VectorInputPin(this);
    outPinA = new VectorOutputPin(this);
    addInputPin(inPinA);
    addInputPin(inPinB);
    addOutputPin(outPinA);
}

void VectorAdder::reset()
{

}

void VectorAdder::process()
{
    c3dlib::VectorChannelReaderInterfaceConstPtr signal1 = inPinA->getValue();
    c3dlib::VectorChannelReaderInterfaceConstPtr signal2 = inPinB->getValue();

    if (signal1 && signal2) {
		double step = 0.0;
		auto uaf = signal1->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
		if (uaf != nullptr){
			step = uaf->argumentsInterval();
		}

		const size_type count = (std::min)(signal1->size(), signal2->size());
		std::vector<c3dlib::VectorChannelReaderInterface::sample_type> data;
		data.reserve(count);

		for (size_type i = 0; i < count; ++i) {
			auto val = signal1->value(i) + signal2->value(i);
			data.push_back({ i * step, val });
		}

		auto channel = dataaccessor::wrap(std::move(data));

		channel->attachFeature(uaf);
		channel->attachFeature(utils::make_shared<dataaccessor::DescriptorFeature>("Result", "", "", "", ""));
        outPinA->setValue(channel);
    } else {

    }
}

Vector2Scalar::Vector2Scalar()
{
    inPinA = new VectorInputPin(this);
    outPinX = new ScalarOutputPin(this);
    outPinY = new ScalarOutputPin(this);
    outPinZ = new ScalarOutputPin(this);
    addInputPin(inPinA);
    addOutputPin(outPinX);
    addOutputPin(outPinY);
    addOutputPin(outPinZ);
}

void Vector2Scalar::reset()
{

}

void Vector2Scalar::process()
{
	c3dlib::VectorChannelReaderInterfaceConstPtr signal1 = inPinA->getValue();
    if (signal1) {

		auto uaf = signal1->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();

		std::vector<c3dlib::ScalarChannelReaderInterface::sample_type> dataX, dataY, dataZ;
		dataX.reserve(signal1->size());
		dataY.reserve(signal1->size());
		dataZ.reserve(signal1->size());

		for (size_type i = 0; i < signal1->size(); ++i) {
			auto val = signal1->sample(i);
			dataX.push_back({ val.first, val.second[0] });
			dataY.push_back({ val.first, val.second[1] });
			dataZ.push_back({ val.first, val.second[2] });
		}

		auto channelX = dataaccessor::wrap(std::move(dataX));
		auto channelY = dataaccessor::wrap(std::move(dataY));
		auto channelZ = dataaccessor::wrap(std::move(dataZ));

		channelX->attachFeature(uaf);
		channelX->attachFeature(utils::make_shared<dataaccessor::DescriptorFeature>("X", "", "", "", ""));

		channelY->attachFeature(uaf);
		channelY->attachFeature(utils::make_shared<dataaccessor::DescriptorFeature>("Y", "", "", "", ""));

		channelZ->attachFeature(uaf);
		channelZ->attachFeature(utils::make_shared<dataaccessor::DescriptorFeature>("Z", "", "", "", ""));


        outPinX->setValue(channelX);
        outPinY->setValue(channelY);
        outPinZ->setValue(channelZ);
    } else {

    }
}

Scalar2Vector::Scalar2Vector()
{
    outPinA = new VectorOutputPin(this);
    inPinX = new ScalarInputPin(this);
    inPinY = new ScalarInputPin(this);
    inPinZ = new ScalarInputPin(this);
    addOutputPin(outPinA);
    addInputPin(inPinX);
    addInputPin(inPinY);
    addInputPin(inPinZ);
}

void Scalar2Vector::reset()
{

}

void Scalar2Vector::process()
{
    c3dlib::ScalarChannelReaderInterfaceConstPtr signal1 = inPinX->getValue();
    c3dlib::ScalarChannelReaderInterfaceConstPtr signal2 = inPinY->getValue();
    c3dlib::ScalarChannelReaderInterfaceConstPtr signal3 = inPinZ->getValue();
    if (signal1 && signal2 && signal3) {

		double step = 0.0;
		auto uaf = signal1->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
		if (uaf != nullptr){
			step = uaf->argumentsInterval();
		}

		size_type count = (std::min)(signal1->size(), signal2->size());
		count = (std::min<size_type>)(count, signal3->size());
		std::vector<c3dlib::VectorChannelReaderInterface::sample_type> data;
		data.reserve(count);

		for (size_type i = 0; i < count; ++i) {
			data.push_back({ i * step, osg::Vec3(signal1->value(i), signal2->value(i), signal3->value(i)) });
		}

		auto channel = dataaccessor::wrap(std::move(data));

		channel->attachFeature(uaf);
		channel->attachFeature(utils::make_shared<dataaccessor::DescriptorFeature>("Result", "", "", "", ""));
        
        outPinA->setValue(channel);
    } else {

    }
}

QWidget* ScalarTreshold::getConfigurationWidget()
{
    return widget;
}

void ScalarTreshold::refreshConfiguration()
{

}

ScalarTreshold::ScalarTreshold()
{
    QLayout* layout = new QHBoxLayout();
    QLabel* label = new QLabel(QObject::tr("Treshold: "));
    spinbox = new QDoubleSpinBox();
    layout->addWidget(label);
    layout->addWidget(spinbox);
    spinbox->setValue(1.0);
    widget = new QWidget();
    widget->setLayout(layout);
}
