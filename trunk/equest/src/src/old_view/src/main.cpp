#include "CorePCH.h"
#include "ToolboxMain.h"
#include <core/AppInitializer.h>

int main(int argc, char *argv[])
{
	return core::AppInitializer::runApp<ToolboxMain>(argc, argv, "Core");
}

