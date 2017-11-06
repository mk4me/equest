#include "NewVdfPCH.h"
#include "DarkPinsFactory.h"
#include "DarkPinStrategy.h"
#include "StyleInputPinNode.h"
#include "StyleOutputPinNode.h"
#include <plugins/newVdf/IVisualPin.h>

using namespace vdf;

IVisualInputPinPtr DarkPinsFactory::createInputPin() const
{
	auto pin = IVisualInputPinPtr(new StyleInputPinNode());
	auto str = IPinStrategyPtr(new DarkPinStrategy());
	str->setPin(pin);
	pin->setVisualStrategy(str);
	return pin;
}
	
IVisualOutputPinPtr DarkPinsFactory::createOutputPin() const
{
	auto pin = IVisualOutputPinPtr(new StyleOutputPinNode());
	auto str = IPinStrategyPtr(new DarkPinStrategy());
	str->setPin(pin);
	pin->setVisualStrategy(str);
	return pin;
}
	
const QUuid DarkPinsFactory::id() const
{
	return QUuid::createUuid();
}
	
const QString DarkPinsFactory::name() const
{
	return "Style pin";
}
	
const QString DarkPinsFactory::description() const
{
	return "Style pin desc";
}
