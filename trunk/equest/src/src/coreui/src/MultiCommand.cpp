#include "CoreUiPCH.h"
#include <coreui/MultiCommand.h>

using namespace coreUI;

MultiCommand::MultiCommand( const std::vector<utils::ICommandPtr>& c ) :
    commands(c) 
{

}

void MultiCommand::doIt()
{
    for (auto it = commands.begin(); it != commands.end(); ++it) {
        (*it)->doIt();
    }
}

void MultiCommand::undoIt()
{
    for (int i = commands.size() - 1; i >= 0; --i) {
        commands[i]->undoIt();
    }
}
