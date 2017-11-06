#include "WatcherConfig.h"
#include <fstream>

WatcherConfig::WatcherConfig(const utils::Filesystem::Path& path) :
	configPath(path)
{

}

void WatcherConfig::save()
{
	std::ofstream outfile(configPath.string());
	for (auto& p : processed) {
		outfile << p.string() << std::endl;
	}
	outfile.close();
}

void WatcherConfig::loadOrCreate()
{
	std::ifstream infile(configPath.string());
	std::string line;
	while (std::getline(infile, line)) {
		processed.insert(line);
	}
	infile.close();
}

bool WatcherConfig::wasProcessed(const utils::Filesystem::Path& path)
{
	return processed.find(path) != processed.end();
}

void WatcherConfig::addProcessed(const utils::Filesystem::Path& p)
{
	processed.insert(p);
}
