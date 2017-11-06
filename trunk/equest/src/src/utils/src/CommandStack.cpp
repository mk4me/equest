#include <utils/CommandStack.h>

using namespace utils;

void CommandStack::addCommand( ICommandPtr command )
{
	command->doIt();
	if (isRedoPossible()) {
		commands.erase(++currentCommand, commands.end());
	}
	commands.push_back(command);
	currentCommand = --commands.end();
	emit changed();
}

void CommandStack::undo()
{
	if (isUndoPossible()) {
		(*currentCommand)->undoIt();
		--currentCommand;
		emit changed();
	}
}

void CommandStack::redo()
{
	if (isRedoPossible()) {
		currentCommand++;
		(*currentCommand)->doIt();
		emit changed();
	}
}

void CommandStack::clear()
{
    commands.clear();
    currentCommand = commands.end();
}


bool CommandStack::isRedoPossible() const
{
	return	!commands.empty() && 
			currentCommand != commands.end() && 
			currentCommand != (--commands.end());
}

CommandStack::CommandStack() 
{
	currentCommand = commands.end();
}

bool CommandStack::isUndoPossible() const
{
	return	!commands.empty() && 
		currentCommand != commands.begin();
}

