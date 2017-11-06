#include "CoreLibPCH.h"
#include <corelib/FilterBundle.h>

void core::FilterBundle::addFilterCommand( core::IFilterCommandPtr command )
{
    commands.push_back(command);
}

core::IFilterCommandPtr core::FilterBundle::getFilter( int idx )
{
    return commands[idx];
}

int core::FilterBundle::genNumFilters() const
{
    return commands.size();
}


