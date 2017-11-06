#include "MdePCH.h"
#include <core/AppInitializer.h>
#include "MdeMainWindow.h"

int main(int argc, char* argv[])
{
	return core::AppInitializer::runApp<MdeMainWindow>(argc, argv, "IMU-MDE");
}
