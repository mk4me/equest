#include "HmmLibPCH.h"
#include "IllnessUnitFilter.h"
#include <corelib/HierarchyItem.h>


core::IHierarchyItemPtr EndoFilter::getFilteredTree( core::IHierarchyItemConstPtr root )
{
    return utils::make_shared<core::HierarchyItem>(root->getName(), root->getDescription(), root->getIcon());
}

void EndoFilter::reset()
{

}
