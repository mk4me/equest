#include "CoreLibPCH.h"
#include <corelib/AbstractSerie.h>

plugin::AbstractSerie::AbstractSerie()
{

}

void plugin::AbstractSerie::setData( const utils::TypeInfo & requestedDataType, const core::VariantConstPtr & data )
{
    this->requestedType = requestedDataType;
    this->data = data;
    setupData(data);
}

const utils::TypeInfo & plugin::AbstractSerie::getRequestedDataType() const
{
    return requestedType;
}

const core::VariantConstPtr & plugin::AbstractSerie::getData() const
{
    return data;
}

const std::string plugin::AbstractSerie::getName() const
{
    return name;
}

void plugin::AbstractSerie::setName( const std::string & name )
{
    this->name = name;
}
