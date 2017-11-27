#include "EquestPCH.h"
#include <core/AppInitializer.h>
#include "EquestMainWindow.h"

int main(int argc, char* argv[])
{
	return core::AppInitializer::runApp<MdeMainWindow>(argc, argv, "MDE");
}
