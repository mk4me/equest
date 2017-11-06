#include "NewVdfPCH.h"
#include "NodesFactory.h"
#include "SimpleItem.h"
#include "SimpleSinkNode.h"
#include "SimpleSourceNode.h"
#include "SimpleProcessingNode.h"
#include "StyleItem.h"
#include "StyleSinkNode.h"
#include "StyleSourceNode.h"
#include "StyleProcessingNode.h"

using namespace vdf;

IVisualSinkNodePtr NodesFactory::createSinkNode() const
{
    return IVisualSinkNodePtr(new SimpleSinkNode());
}

IVisualSourceNodePtr NodesFactory::createSourceNode() const
{
    return IVisualSourceNodePtr(new SimpleSourceNode());
}

IVisualProcessingNodePtr NodesFactory::createProcessingNode() const
{
    return IVisualProcessingNodePtr(new SimpleProcessingNode());
}

const QUuid NodesFactory::id() const
{
    return QUuid::createUuid();
}

const QString NodesFactory::name() const
{
    return "Simple node";
}

const QString NodesFactory::description() const
{
    return "Simple node description";
}


