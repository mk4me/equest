#include "CommunicationPCH.h"
#include "HMDBLocalContext.h"
#include "ShallowCopyUtils.h"
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <corelib/IParserManagerReader.h>

using namespace hmdbCommunication;

template<class Base>
class StreamWrapper : public Base
{
public:

	StreamWrapper(decltype(std::declval<Base>().rdbuf()) sb, IHMDBStorage::TransactionConstPtr transaction) : Base(sb), transaction(transaction) {}

	StreamWrapper(const StreamWrapper&) = delete;
	StreamWrapper& operator=(const StreamWrapper&) = delete;

	virtual ~StreamWrapper()
	{	// destroy the object
		auto toDelete = Base::rdbuf(nullptr);
		if (toDelete != nullptr){
			delete toDelete;
		}
	}

private:

	IHMDBStorage::TransactionConstPtr transaction;
};

class StorageStreamGrabber : public core::IStreamDataManager::IStreamGrabber
{
public:

	//! \param storage Obiekt z danymi
	//! \param name Nazwa strumienia do wypakowania
	StorageStreamGrabber(const IHMDBStorageConstPtr storage, const std::string & name)
		: storage(storage), name_(name)
	{

	}

	//! Destruktor wirtualny
	virtual ~StorageStreamGrabber()
	{

	}

	//! \return Strumieñ
	virtual const plugin::IStreamParser::IStreamPtr stream() const
	{
		auto t = storage->transaction();
		auto s = t->get(name_);
		return plugin::IStreamParser::IStreamPtr(new StreamWrapper<std::istream>(s->rdbuf(nullptr), t));
	}

	//! \return Nazwa, identyfikator strumienia
	virtual const std::string name() const
	{
		return core::IStreamManagerReader::streamName(storage->protocol(), name_);
	}

	//! \return Nazwa, identyfikator strumienia
	virtual const std::string rawName() const
	{
		return name_;
	}

private:
	//! Obiekt z danymi
	const IHMDBStorageConstPtr storage;
	//! Nazwa strumienia do wypakowania
	const std::string name_;
};

class HMDBLocalContextTransaction : public threadingUtils::TransactionImplHelper<IHMDBLocalContext::transaction_type, HMDBLocalContext::TransactionSharedState>
{
public:
	HMDBLocalContextTransaction(HMDBLocalContext::SharedStatePtr sharedState) : ImplType(sharedState),
		mdm(sharedState->innerState.mdm->transaction()),
		sdm(sharedState->innerState.sdm->transaction())
	{

	}

	virtual ~HMDBLocalContextTransaction()
	{

	}

	//! \return Czy dane o które pytamy pochodza z tego Ÿród³a
	virtual const bool isMyData(const core::VariantConstPtr data) const
	{
		return std::find(sharedState().myData_.begin(), sharedState().myData_.end(), data) != sharedState().myData_.end();
	}

	virtual const bool isMyData(const void * data) const
	{
		auto it = std::find_if(sharedState().myData_.begin(), sharedState().myData_.end(), [=](const core::VariantConstPtr d) -> bool {
			if (d->getRawPtr() == data){
				return true;
			}

			return false;
		});


		return it != sharedState().myData_.end();
	}

	//! \param fileName Nazwa pliku ze storage
	//! \return Dane skojarzone z tym plikiem
	virtual const core::ConstVariantsList data(const std::string & fileName) const
	{
		const auto sName = core::IStreamManagerReader::streamName(sharedState().dataContext_->storage()->protocol(), fileName);

		core::ConstVariantsList oList;

		if (sdm->isManaged(sName) == true){
			sdm->getObjects(sName, oList);
		}

		return oList;
	}

	//! \param fileName Plik jaki bêdê ³adowaæ ze storage do StreamManagera
	//! \return Czy uda³o siê za³adowac plik
	virtual const bool load(const std::string & fileName)
	{
		bool ret = false;

		if (isLoaded(fileName) == false){
			core::IStreamDataManager::StreamGrabberPtr sg(new StorageStreamGrabber(sharedState().dataContext_->storage(), fileName));
			sdm->addStream(sg);
			core::ConstVariantsList oList;
			sdm->getObjects(sg->name(), oList);
			sharedState().myData_.splice(sharedState().myData_.end(), oList);
			ret = true;
		}

		return ret;
	}
	//! \param fileName Plik jaki bêdê wy³adowywaæ ze StreamManagera
	//! \return Czy uda³o siê wy³adowac plik
	virtual const bool unload(const std::string & fileName)
	{
		bool ret = false;

		const auto sName = core::IStreamManagerReader::streamName(sharedState().dataContext_->storage()->protocol(), fileName);

		if (sdm->isManaged(sName) == true){
			sdm->removeStream(sName);
			ret = true;
		}

		return ret;
	}
	//! \param fileName Plik o który pytam czy jest za³¹dowany
	//! \return Czy plik jest za³adowany
	virtual const bool isLoaded(const std::string & fileName) const
	{
		const auto sName = core::IStreamManagerReader::streamName(sharedState().dataContext_->storage()->protocol(), fileName);
		return sdm->isManaged(sName);
	}

	//! \param data Dane jakie bêdê ³adowa³ do MemoryManagera
	//! \return Czy uda³o siê za³adowac dane
	virtual const bool load(const core::VariantPtr data)
	{
		bool ret = false;

		if (mdm->isManaged(data) == false){
			mdm->addData(data);
			sharedState().myData_.push_back(data);
			ret = true;
		}

		return ret;
	}
	//! \param data Dane jakie bêdê wy³adowa³ z MemoryManagera
	//! \return Czy uda³o siê wy³adowac dane
	virtual const bool unload(const core::VariantConstPtr data)
	{
		bool ret = false;

		auto it = std::find(sharedState().myData_.begin(), sharedState().myData_.end(), data);

		if (it != sharedState().myData_.end()){

			if (mdm->isManaged(data) == true){
				mdm->removeData(data);
			}
			else{
				sharedState().myData_.erase(it);
			}

			ret = true;
		}

		return ret;
	}

private:
	//! Transakcja MDM
	core::IDataManager::TransactionPtr mdm;
	//! Transakcja SDM
	core::IStreamDataManager::TransactionPtr sdm;
};

/*

//próbuje pliki vsk wypakowaæ z zip

auto zipName = sessionName + ".zip";
auto vskName = sessionName + ".vsk";

if (file->fileName == zipName){
	//mam zipa - szukam vsk w zip
	auto vskDestPath = pathsManager->filePath(vskName, sessionName);
	QuaZip zip(filePath.string().c_str());

	if (zip.open(QuaZip::mdUnzip) == true){
		//uda³o sie otworzyæ plik do wypakowywania
		auto files = zip.getFileNameList();
		if (files.contains(QString::fromUtf8(vskName.c_str())) == true &&
			zip.setCurrentFile(QString::fromUtf8(vskName.c_str())) == true){
			//znalaz³em vsk - próbujemy go wypakowaæ
			QuaZipFile inFile(&zip);
			if (inFile.open(QIODevice::ReadOnly) && inFile.getZipError() == UNZ_OK) {
				//tworze plik docelowy
				QFile outFile;
				outFile.setFileName(vskDestPath.string().c_str());
				if (outFile.open(QIODevice::WriteOnly)) {
					//uda³o siê - mogê kopiowaæ
					auto ok = copyData(inFile, outFile);
					ok &= (inFile.getZipError() == UNZ_OK);

					//zamykam plik docelowy
					outFile.close();

					//zamykam plik w archiwum zip
					inFile.close();

					if (ok == false || inFile.getZipError() != UNZ_OK) {
						utils::Filesystem::deleteFile(vskDestPath);
					}
				}
			}
		}
	}
}

*/

HMDBLocalContext::HMDBLocalContext(IHMDBDataContextPtr dataContext, core::IDataManager * mdm,
	core::IStreamDataManager * sdm) : sharedState(utils::make_shared<TransactionSharedState>())
{
	sharedState->innerState.dataContext_ = dataContext;
	sharedState->innerState.mdm = mdm;
	sharedState->innerState.sdm = sdm;
}

HMDBLocalContext::~HMDBLocalContext()
{

}

const IHMDBDataContextPtr HMDBLocalContext::dataContext()
{
	return sharedState->innerState.dataContext_;
}

const IHMDBDataContextConstPtr HMDBLocalContext::dataContext() const
{
	return sharedState->innerState.dataContext_;
}

HMDBLocalContext::TransactionPtr HMDBLocalContext::transaction()
{
	return TransactionPtr(new HMDBLocalContextTransaction(sharedState));
}

HMDBLocalContext::TransactionConstPtr HMDBLocalContext::transaction() const
{
	return TransactionConstPtr(new HMDBLocalContextTransaction(sharedState));
}

void HMDBLocalContext::observe(const core::IDataManagerReader::ChangeList & changes)
{
	for (auto it = changes.begin(); it != changes.end(); ++it){
		
		auto oc = *it;
		
		if (oc.modyfication == core::IDataManagerReader::REMOVE_OBJECT){
			sharedState->innerState.myData_.remove(oc.previousValue);
		}
	}
}