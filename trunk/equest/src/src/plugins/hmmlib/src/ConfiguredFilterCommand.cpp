#include "HmmLibPCH.h"
#include "ConfiguredFilterCommand.h"
#include <plugins/subject/Types.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>

__Helper::__Helper( std::function<void (const QString&, int)> function ) :
function(function) 
{
}

void __Helper::checkBoxChanged( int state )
{
    QCheckBox* checkBox = qobject_cast<QCheckBox *>(sender());
    function(checkBox->text(), state);
}


void __Helper::onElementHovered( const QString& name, bool selected ) {
    ConfigurationWidget* widget = qobject_cast<ConfigurationWidget*>(sender());
    if (selected) {
        auto it = namesDictionary.find(name);
        if (it != namesDictionary.end()) {
            widget->setText(it->second.second);
        } else {
            // ?
            widget->setText("");
        }
    } else {
        widget->setText("");
    }
}

void __Helper::onItemSelected( const QString& name, bool selected )
{
    UTILS_ASSERT(namesDictionary.empty() == false);
    auto it = namesDictionary.find(name);
    if (it != namesDictionary.end()) {
        function(it->second.first, selected ? 1 : 0);
    } else {
        // ?
    }
}
