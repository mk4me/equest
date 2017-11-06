#include <utils/Filesystem.h>
#include <chrono>
#include <thread>
#include <boost/lexical_cast.hpp>

////////////////////////////////////////////////////////////////////////////////
using namespace utils;
////////////////////////////////////////////////////////////////////////////////
using namespace boost::filesystem;

void Filesystem::createDirectory(const Path& path)
{
    if(path.empty())
    {
        return;
    }

    if(isDirectory(path) == false)
    {
        create_directories(path);
    }
}

void Filesystem::deleteDirectory(const Path& path)
{
	if(path.empty())
	{
		return;
	}

	if(isDirectory(path) == true)
	{
		remove_all(path);
	}
}

void Filesystem::deleteFile(const Path& path)
{
	if(path.empty())
	{
		return;
	}

	if(pathExists(path) == true && isDirectory(path) == false)
	{
		remove(path);
	}
}

void Filesystem::move(const Path& pathOld, const Path& pathNew)
{
	if(pathOld.empty() || pathNew.empty())
	{
		return;
	}

	Filesystem::copy(pathOld, pathNew);
    remove(pathOld);
}

void Filesystem::copy(const Path& pathOld, const Path& pathNew)
{
	if(pathOld.empty() || pathNew.empty())
	{
		return;
	}

	boost::filesystem::copy(pathOld, pathNew);
}

Filesystem::PathsList Filesystem::listFiles(const Path& path, bool recursive)
{
	PathsList files;

    if(pathExists(path) == true)
    {
        if(recursive)
        {
			PathsList dirs = listSubdirectories(path);
            for(const auto & dir : dirs)
            {
				PathsList subfiles = listFiles(dir, recursive);
                for(const auto & file : subfiles)
                {
                    files.push_back(file);
                }
            }
        }
        Iterator end;
        for(Iterator iter(path) ; iter != end ; ++iter)
        {
            if(isDirectory(iter->path()) == false)
            {
                files.push_back(iter->path());
            }
        }
    }
    return files;
}

Filesystem::PathsList Filesystem::listFiles(const Path& path, bool recursive, const std::string& mask)
{
	PathsList files;

	if(pathExists(path) == true)
	{
		if(recursive)
		{
			PathsList dirs = listSubdirectories(path);
			for (const auto & dir : dirs)
			{
				PathsList subfiles = listFiles(dir, recursive, mask);
				for (const auto & file : subfiles)
				{
					files.push_back(file);
				}
			}
		}
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			if (isDirectory(iter->path()) == false && mask.compare(iter->path().extension().string()) == 0)
			{
				files.push_back(iter->path());
			}
		}
	}
	return files;
}

Filesystem::PathsList Filesystem::listFiles(const Path& path, bool recursive, const std::vector<std::string>& masks)
{
	PathsList files;

	if(pathExists(path) == true)
	{
		if(recursive)
		{
			PathsList dirs = listSubdirectories(path);
			for (const auto & dir : dirs)
			{
				PathsList subfiles = listFiles(dir, recursive, masks);
				for (const auto & file : subfiles)
				{
					files.push_back(file);
				}
			}
		}
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			for(const auto & mask : masks)
			{
				if (isDirectory(iter->path()) == false && mask.compare(iter->path().extension().string()) == 0)
				{
					files.push_back(iter->path());
				}
			}
		}
	}
	return files;
}

Filesystem::PathsList Filesystem::listSubdirectories(const Path& path)
{
	PathsList subdirs;

	if(pathExists(path) == true)
	{
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			if(isDirectory(iter->path()) == true)
			{
				subdirs.push_back(iter->path());
			}
		}
	}
	return subdirs;
}

bool Filesystem::isRegularFile(const Path & path)
{
    return is_regular_file(path);
}

bool Filesystem::isSymbolicLink(const Path & path)
{
    return is_symlink(path);
}

bool Filesystem::isDirectory(const Path & path)
{
    return is_directory(path);
}

bool Filesystem::pathExists(const Path & path)
{
    return exists(path);
}

Filesystem::size_t Filesystem::availableSpace(const Path & path)
{
	Filesystem::size_t ret = 0;

	try{
		if (isRegularFile(path) == true){
			ret = boost::filesystem::space(path.parent_path()).available;			
		}
		else{
			ret = boost::filesystem::space(path).available;			
		}
		
	}
	catch (...){

	}

	return ret;
}

Filesystem::size_t Filesystem::capacity(const Path & path)
{
	Filesystem::size_t ret = -1;

	try{
		ret = boost::filesystem::space(path).capacity;		
	}
	catch (...){

	}

	return ret;
}

Filesystem::size_t Filesystem::freeSpace(const Path & path)
{
	Filesystem::size_t ret = -1;

	try{
		ret = boost::filesystem::space(path).free;		
	}
	catch (...){

	}

	return ret;
}

Filesystem::size_t Filesystem::size(const Path & path)
{
	Filesystem::size_t ret = -1;

	try{

		if (isRegularFile(path) == true){
			ret = boost::filesystem::file_size(path);
		}
		else if (isDirectory(path) == true){
			ret = 0;
			for (boost::filesystem::recursive_directory_iterator it(path);
				it != boost::filesystem::recursive_directory_iterator();
				++it)
			{
				if (isDirectory(*it) == false){
					ret += boost::filesystem::file_size(*it);
				}					
			}
		}

	}
	catch (...){

	}

	return ret;
}

std::string Filesystem::temporaryFileName()
{
	return boost::lexical_cast<std::string>(std::chrono::system_clock::now().time_since_epoch().count()) + "_" + boost::lexical_cast<std::string>(std::this_thread::get_id());
}

std::string Filesystem::temporaryFileExtension()
{
	return std::string("tmp");
}

Filesystem::Path Filesystem::temporaryFile()
{
	return Path(temporaryFileName() + "." + temporaryFileExtension());
}