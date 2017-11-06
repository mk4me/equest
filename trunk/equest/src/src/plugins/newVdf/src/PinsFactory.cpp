#include "NewVdfPCH.h"
#include "PinsFactory.h"
#include "SimpleItem.h"
#include "SimpleInputPinNode.h"
#include "SimpleOutputPinNode.h"
#include "StyleInputPinNode.h"
#include "StyleOutputPinNode.h"

using namespace vdf;

IVisualInputPinPtr PinsFactory::createInputPin() const
{
    return IVisualInputPinPtr(new SimpleInputPinNode());
}

IVisualOutputPinPtr PinsFactory::createOutputPin() const
{
    return IVisualOutputPinPtr(new SimpleOutputPinNode());
}

const QUuid PinsFactory::id() const
{
    return QUuid::createUuid();
}

const QString PinsFactory::name() const
{
    return "Simple pin";
}

const QString PinsFactory::description() const
{
    return "Simple pin desc";
}

