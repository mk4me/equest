#include "NewVdfPCH.h"
#include "DarkConnectionsFactory.h"
#include "DarkConnectionStrategy.h"
#include "StyleConnection.h"

using namespace vdf;

IVisualConnectionPtr DarkConnectionsFactory::createConnection() const
{
    auto connection = IVisualConnectionPtr(new StyleConnection());
	auto strategy = IConnectionStrategyPtr(new DarkConnectionStrategy());
    strategy->setConnection(connection);
	connection->setVisualStrategy(strategy);
    connection->visualItem()->setFlag(QGraphicsItem::ItemIsSelectable);
	return connection;
}

const QUuid DarkConnectionsFactory::id() const
{
    return QUuid::createUuid();
}

const QString DarkConnectionsFactory::name() const
{
    return "Curved";
}

const QString DarkConnectionsFactory::description() const
{
    return "Curved line";
}

