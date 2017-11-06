#include "CoreUiPCH.h"
#include <coreui/AbstractTab.h>

using namespace coreUI;

AbstractTab::AbstractTab( const QIcon& icon, const QString& label ):
    icon(icon),
    label(label),
    active(false),
    enabled(true)
{

}


void AbstractTab::attachChildContext( coreUI::IAppUsageContextPtr child )
{
    throw std::logic_error("The method or operation is not implemented.");
}

void AbstractTab::setEnabled( bool enabled )
{
    this->enabled = enabled;
    emit tabEnabled(enabled);
}

bool AbstractTab::isEnabled() const
{
    return enabled;
}


QIcon AbstractTab::getIcon() const
{
    return icon;
}

QString AbstractTab::getLabel() const
{
    return label;
}

bool AbstractTab::isActive() const
{
    return active;
}

void AbstractTab::setActive( bool val )
{
    this->active = val;
    emit tabActivated(val);
}
