#include "CommunicationPCH.h"
#include "MemoryStorage.h"
#include <streambuf>
#include <sstream>
#include <utils/StreamTools.h>

using namespace hmdbCommunication;

const std::size_t MaxBufferSize = 1 << 20;

class MemoryStorageTransaction : public threadingUtils::TransactionImplHelper<hmdbCommunication::IHMDBStorageOperations, MemoryStorage::TransactionSharedState>
{
public:
	MemoryStorageTransaction(MemoryStorage::SharedStatePtr sharedState)
		: ImplType(sharedState)
	{

	}

	virtual ~MemoryStorageTransaction()
	{

	}

	virtual const bool exists(const std::string & key) const
	{
		return sharedState().find(key) != sharedState().end();
	}

	virtual const IOStreamPtr get(const std::string & key)
	{
		auto it = sharedState().find(key);
		if (it != sharedState().end()){
			return IHMDBStorageOperations::IOStreamPtr(new std::stringstream(it->second));
		}

		return IHMDBStorageOperations::IOStreamPtr();
	}

	virtual const IStreamPtr get(const std::string & key) const
	{
		auto it = sharedState().find(key);
		if (it != sharedState().end()){
			return IHMDBStorageOperations::IStreamPtr(new std::stringstream(it->second));
		}

		return IHMDBStorageOperations::IStreamPtr();
	}

	virtual const bool set(const std::string & key, IStreamPtr input)
	{
		auto str = utils::StreamTools::read(*input);
		auto it = sharedState().find(key);
		if (it != sharedState().end()){
			it->second = str;
		}
		else{
			sharedState().insert({ key, str });
		}

		return true;
	}

	virtual void set(const std::string & key, IStreamPtr input, IHMDBStorageProgress * progress)
	{
		const auto streamSize = utils::StreamTools::size(*input);

		if (streamSize == 0){
			progress->setError("Empty input stream");
			return;
		}

		const auto readSize = std::min((int)streamSize, (int)MaxBufferSize);

		utils::unique_ptr<char[]> memblock(new char[readSize] {0});
		input->seekg(0, std::ios::beg);
		int offset = 0;
		const unsigned int fullReads = (streamSize / readSize);
		const float progressPart = (float)readSize / (float)streamSize;

		std::string str;

		for (unsigned int i = 0; i < fullReads && progress->aborted() == false; ++i){
			input->read(memblock.get(), readSize);
			str.append(memblock.get(), readSize);
			offset += readSize;
			progress->setProgress(progressPart * (i + 1));
		}

		if (progress->aborted() == true){
			return;
		}

		const auto left = streamSize - offset;

		if (left > 0){
			input->read(memblock.get(), left);
			str.append(memblock.get(), left);
		}

		progress->setProgress(1.0);

		auto it = sharedState().find(key);
		if (it != sharedState().end()){
			it->second = str;
		}
		else{
			sharedState().insert({ key, str });
		}
	}

	virtual const bool remove(const std::string & key)
	{
		return sharedState().erase(key) > 0;
	}

	virtual const bool rename(const std::string & oldKey,
		const std::string & newKey, const bool overwrite = false)
	{
		auto oit = sharedState().find(oldKey);

		if (oit == sharedState().end()){
			return false;
		}

		auto it = sharedState().find(newKey);
		if (it != sharedState().end()){
			if (overwrite == false){
				return false;
			}

			it->second = oit->second;
		}
		else{
			sharedState().insert({ newKey, oit->second });
		}

		sharedState().erase(oit);

		return true;
	}

	virtual const Keys keys() const
	{
		IHMDBStorageOperations::Keys ret;

		for (auto it = sharedState().begin(); it != sharedState().end(); ++it){
			ret.insert(it->first);
		}

		return ret;
	}

	virtual const bool canStore(const unsigned long long size) const
	{
		//TODO
		//crossplatform free memory
		return true;
	}
};


MemoryStorage::MemoryStorage()
{

}


MemoryStorage::~MemoryStorage()
{

}

const bool MemoryStorage::isOpened() const
{
	return true;
}

void MemoryStorage::close()
{
	sharedState.reset();
}

const std::string MemoryStorage::name() const
{
	return std::string("MemoryStorage");
}

const std::string MemoryStorage::protocol() const
{
	return std::string("memory");
}

MemoryStorage::TransactionPtr MemoryStorage::transaction()
{
	return TransactionPtr(new MemoryStorageTransaction(sharedState));
}

MemoryStorage::TransactionConstPtr MemoryStorage::transaction() const
{
	return TransactionConstPtr(new MemoryStorageTransaction(sharedState));
}