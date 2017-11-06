#include "NewVdfPCH.h"
#include "ConnectionsFactory.h"
#include "SimpleConnection.h"
#include "StyleConnection.h"
#include <plugins/newVdf/IVisualPin.h>

using namespace vdf;

IVisualConnectionPtr ConnectionsFactory::createConnection() const
{
    return IVisualConnectionPtr(new SimpleConnection());
}

const QUuid ConnectionsFactory::id() const
{
    return QUuid::createUuid();
}

const QString ConnectionsFactory::name() const
{
    return "Simple";
}

const QString ConnectionsFactory::description() const
{
    return "Simple line";
}
