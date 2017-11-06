#include "CorePCH.h"
#include "StreamDataManager.h"
#include "DataManager.h"
#include "ApplicationCommon.h"
#include "ParserManager.h"
#include <utils/Push.h>
#include <fstream>
#include <utils/SmartPtr.h>

using namespace core;

class StreamDataManager::StreamTransaction : public IStreamDataManager::ITransaction
{
private:

	struct Modyfication
	{
		IStreamManagerReader::ModificationType modyfication;
		VariantsList objects;
	};

	typedef std::map<std::string, Modyfication> StreamModyfications;

private:
	IDataManager::TransactionPtr mdmTransaction;
	StreamDataManager * sdm;
	utils::shared_ptr<std::lock_guard<std::recursive_mutex>> lock;
	StreamModyfications modyfications;
	bool transactionRolledback;
	bool oldSkipUpdate;

public:
	StreamTransaction(StreamDataManager * sdm) : mdmTransaction(getDataManager()->transaction()),
		sdm(sdm), lock(new std::lock_guard<std::recursive_mutex>(sdm->sync)),
		transactionRolledback(false), oldSkipUpdate(sdm->skipUpdate)
	{
		sdm->skipUpdate = true;
	}

	~StreamTransaction()
	{
		//wymuszam najpierw commit transakcji po stronie MemoryDataManagera
		//potem sam notyfikuje o zmianach w plikach
		mdmTransaction.reset();

		if (isRolledback() == false) {
			if (modyfications.empty() == false) {
				StreamDataManager::ChangeList changes;
				for (auto it = modyfications.begin(); it != modyfications.end(); ++it) {
					StreamDataManager::Change change;
					change.stream = it->first;
					change.modyfication = it->second.modyfication;
					changes.push_back(change);
				}

				sdm->updateObservers(changes);
			}

			sdm->skipUpdate = oldSkipUpdate;
		}
	}

public:

	virtual const bool isRolledback() const
	{
		return transactionRolledback;
	}

	virtual void rollback()
	{
		transactionRolledback = true;
		mdmTransaction->rollback();

		for (auto it = modyfications.begin(); it != modyfications.end(); ++it) {
			switch (it->second.modyfication) {
			case IStreamManagerReader::ADD_STREAM:
			{
													 VariantsList toRemove;
													 sdm->rawGetObjects(it->first, toRemove);
													 //usuń obiekty z MemoryDM w transakcji
													 for (auto IT = toRemove.begin(); IT != toRemove.end(); ++IT) {
														 //TODO
														 //przywrócić poprzedni jeśli był
														 (*IT)->setInitializer(VariantInitializerPtr());
													 }
													 sdm->objectsByStreams.erase(it->first);
			}
				break;

			case IStreamManagerReader::REMOVE_STREAM:
			{
														//TODO
														//przywrócić LazyInitializer!!
														auto insRes = sdm->objectsByStreams.insert(StreamDataManager::ObjectsByStreams::value_type(it->first, it->second.objects));
														if (insRes.second == false) {
															insRes.first->second.insert(insRes.first->second.end(), it->second.objects.begin(), it->second.objects.end());
														}
			}
				break;
			}
		}

		sdm->skipUpdate = oldSkipUpdate;
		mdmTransaction.reset();
		lock.reset();
	}

	//! \param files Lista plików dla których zostaną utworzone parsery i z których wyciągnięte dane
	//! będą dostępne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void addStream(const StreamGrabberPtr stream)
	{
		verifyRollback();

		if (stream == nullptr || stream->name().empty() == true) {
			rollback();
			throw std::runtime_error("Uninitialized or anonymous stream");
		}

		if (sdm->rawIsManaged(stream->name()) == true) {
			rollback();
			throw std::runtime_error("Stream transaction tried to add stream already managed by manager");
		}

		rawAddStream(stream);
	}

	//! \param files Lista plików które zostaną usunięte z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void removeStream(const std::string & stream)
	{
		verifyRollback();

		if (sdm->rawIsManaged(stream) == false) {
			rollback();
			throw std::runtime_error("Stream transaction tried to modify stream not managed by manager");
		}

		rawRemoveStream(stream);
	}

	virtual const bool tryAddStream(const StreamGrabberPtr stream)
	{
		if (transactionRolledback == true || stream == nullptr || stream->name().empty() == true || sdm->rawIsManaged(stream->name()) == true) {
			return false;
		}

		rawAddStream(stream);

		return true;
	}

	virtual const bool tryRemoveStream(const std::string & stream)
	{
		if (transactionRolledback == true || sdm->rawIsManaged(stream) == false) {
			return false;
		}

		rawRemoveStream(stream);

		return true;
	}

	virtual void getStreams(StreamsList & streams) const
	{
		verifyRollback();

		sdm->rawGetStreams(streams);
	}

	virtual const bool isManaged(const std::string & stream) const
	{
		verifyRollback();

		return sdm->rawIsManaged(stream);
	}

	virtual const bool isLoadedCompleately(const std::string & stream) const
	{
		verifyRollback();

		return sdm->rawIsLoadedCompleately(stream);
	}

	virtual void getObjects(const std::string & stream, ConstVariantsList & objects) const
	{
		verifyRollback();

		sdm->rawGetObjects(stream, objects);
	}

	virtual void getObjects(const std::string & stream, VariantsCollection & objects) const
	{
		verifyRollback();

		sdm->rawGetObjects(stream, objects);
	}

private:

	inline void verifyRollback() const
	{
		if (isRolledback() == true) {
			throw std::runtime_error("Stream transaction already rolled-back");
		}
	}

	void rawAddStream(const StreamGrabberPtr stream)
	{
		//dodajemy dane do dm
		sdm->rawAddStream(stream, mdmTransaction);
		//aktualizujemy liste zmian
		Modyfication mod;
		mod.modyfication = IStreamManagerReader::ADD_STREAM;
		modyfications.insert(StreamModyfications::value_type(stream->name(), mod));
	}

	void rawRemoveStream(const std::string & stream)
	{
		Modyfication mod;
		mod.modyfication = IStreamManagerReader::REMOVE_STREAM;
		sdm->rawGetObjects(stream, mod.objects);
		//dodajemy dane do dm
		sdm->rawRemoveStream(stream, mdmTransaction);
		//aktualizujemy liste zmian
		modyfications.insert(StreamModyfications::value_type(stream, mod));
	}
};

class StreamDataManager::StreamReaderTransaction : public IStreamManagerReader::IOperations
{
public:
	StreamReaderTransaction(StreamDataManager * sdm) : sdm(sdm)
	{
		sdm->sync.lock();
	}

	~StreamReaderTransaction()
	{
		sdm->sync.unlock();
	}

public:

	virtual void getStreams(StreamsList & streams) const
	{
		sdm->rawGetStreams(streams);
	}

	virtual const bool isManaged(const std::string & stream) const
	{
		return sdm->rawIsManaged(stream);
	}

	virtual const bool isLoadedCompleately(const std::string & stream) const
	{
		return sdm->rawIsLoadedCompleately(stream);
	}

	virtual void getObjects(const std::string & stream, ConstVariantsList & objects) const
	{
		sdm->rawGetObjects(stream, objects);
	}

	virtual void getObjects(const std::string & stream, VariantsCollection & objects) const
	{
		sdm->rawGetObjects(stream, objects);
	}

private:
	StreamDataManager * sdm;
};

namespace core {
//! Wewnętrzna reprezentacja parsera używana przez DataManagera.
class StParser
{
private:
	//! Prawdziwy wewnętrzny parser.
	const plugin::IParserPtr parser;
	//! Opis źródła danych
	const IStreamDataManager::StreamGrabberPtr sg;
	//! Czy przeparsowano plik?
	bool parsed;
	//! Czy użyto parsera do przeparsowania?
	bool used;

private:

	virtual void _parseStream() = 0;

public:
	//! \param parser Faktyczny parser. To ten obiekt kontroluje jego
	//!     czas życia.
	//! \param resource Czy parser jest związany z zasobami stałymi?
	StParser(plugin::IParser * parser, const IStreamDataManager::StreamGrabberPtr sg) :
		parser(parser), parsed(false), used(false), sg(sg)
	{
		UTILS_ASSERT(parser != nullptr);
		UTILS_ASSERT(sg != nullptr);
	}

	//! Destruktor drukujący wiadomość o wyładowaniu pliku.
	virtual ~StParser()
	{
		if (isParsed()) {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading parser for stream: " << sg->name());
		} else if (isUsed()) {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading invalid parser for stream: " << sg->name());
		} else {
			CORE_LOG_NAMED_DEBUG("parser", "Unloading unused parser for stream: " << sg->name());
		}
	}

	//! \return Czy użyto tego parsera?
	inline const bool isUsed() const
	{
		return used;
	}
	//! \return Czy udało się przeparsować plik?
	inline const bool isParsed() const
	{
		return parsed;
	}

	inline void reset()
	{
		parser->reset();
		used = false;
		parsed = false;
	}

	const IStreamDataManager::IStreamPtr getStream() const
	{
		return sg->stream();
	}

	const std::string getStreamName() const
	{
		return sg->name();
	}

	//! \return
	inline plugin::IParserPtr getParser() const
	{
		return parser;
	}

	void parseStream()
	{
		UTILS_ASSERT(!isUsed());
		CORE_LOG_NAMED_DEBUG("parser", "Parsing stream: " << sg->name());
		used = true;
		_parseStream();
		parsed = true;
		CORE_LOG_NAMED_DEBUG("parser", "Stream parsed OK: " << sg->name());
	}

	//! Nie rzuca wyjątkami.
	//! \return Czy udało się przeparsować?
	const bool tryParse()
	{
		bool ret = false;
		try {
			parseStream();
			ret = true;
		}
		catch (const std::exception& e) {
			CORE_LOG_NAMED_ERROR("parser", "Error during parsing stream " << sg->name() << ": " << e.what());
		}
		catch (...) {
			CORE_LOG_NAMED_ERROR("parser", "UNKNOWN Error during parsing stream " << sg->name());
		}

		return ret;
	}

	inline void getObject(Variant& object, const VariantsVector::size_type idx) const
	{
		parser->getObject(object, idx);
	}
};
}

//! Wewnętrzna reprezentacja parsera używana przez DataManagera.
class StreamParser : public core::StParser
{
private:
	//! Prawdziwy wewnętrzny parser.
	plugin::IStreamParser * streamParser;

public:
	//! \param parser Faktyczny parser. To ten obiekt kontroluje jego
	//!     czas życia.
	//! \param resource Czy parser jest związany z zasobami stałymi?
	StreamParser(plugin::IParser * parser, const IStreamDataManager::StreamGrabberPtr sg)
		: StParser(parser, sg), streamParser(nullptr)
	{

	}

	//! Destruktor drukujący wiadomość o wyładowaniu pliku.
	virtual ~StreamParser()
	{

	}

private:

	virtual void _parseStream()
	{
		if (streamParser == nullptr) {
			streamParser = dynamic_cast<plugin::IStreamParser*>(getParser().get());
		}

		if (streamParser != nullptr) {
			auto stream = getStream();
			if (stream != nullptr) {
				streamParser->parse(stream, getStreamName());
			} else {
				std::string("Uninitialized stream for parsing");
			}
		} else {
			throw std::runtime_error("Parser is expected to implement plugin::ISourceParser interface but failed to cast");
		}
	}
};

//! Wewnętrzna reprezentacja parsera używana przez DataManagera.
class FileStreamParser : public StParser
{
private:
	//! Prawdziwy wewnętrzny parser.
	plugin::ISourceParser * sourceParser;
	utils::Filesystem::Path tmpFilePath;

public:
	//! \param parser Faktyczny parser. To ten obiekt kontroluje jego
	//!     czas życia.
	//! \param resource Czy parser jest związany z zasobami stałymi?
	FileStreamParser(plugin::IParser * parser, const IStreamDataManager::StreamGrabberPtr sg)
		: StParser(parser, sg), sourceParser(nullptr)
	{

	}

	//! Destruktor drukujący wiadomość o wyładowaniu pliku.
	virtual ~FileStreamParser()
	{
		if (tmpFilePath.empty() == false) {
			try {
				utils::Filesystem::deleteFile(tmpFilePath);
			}
			catch (...) {
			}
		}
	}

private:

	virtual void _parseStream()
	{
		if (sourceParser == nullptr) {
			sourceParser = dynamic_cast<plugin::ISourceParser*>(getParser().get());
		}

		if (sourceParser != nullptr) {
			auto stream = getStream();
			if (stream != nullptr) {

				const auto tmpPath = plugin::getPaths()->getTempFilePath();
				std::ofstream output(tmpPath.string(), std::ios_base::out | std::ios_base::binary);

				if (output.is_open() == true) {

					tmpFilePath = tmpPath;

					static unsigned int BufferSize = 1024 * 1024 * 5;
					utils::unique_ptr<char[]> buffer(new char[BufferSize] {0});

					std::streamsize read = 0;
					while ((read = stream->readsome(buffer.get(), BufferSize)) > 0) { output.write(buffer.get(), read); }

					output.flush();
					output.close();

					sourceParser->parse(tmpPath.string());
				} else {
					throw std::runtime_error("Failed to create temporary file for stream");
				}
			} else {
				std::string("Uninitialized stream for parsing");
			}
		} else {
			throw std::runtime_error("Parser is expected to implement plugin::ISourceParser interface but failed to cast");
		}
	}
};

namespace core {
class SimpleInitializer : public IVariantInitializer
{
public:
	SimpleInitializer(const utils::shared_ptr<StParser> & parser, const int idx)
		: parser(parser), idx(idx) {}
	virtual ~SimpleInitializer() {}
	virtual void initialize(Variant * object) {
		parser->parseStream();
		parser->getObject(*object, idx);
		parser->reset();
	}
	virtual IVariantInitializer * clone() const { return new SimpleInitializer(parser, idx); }
	virtual bool isEqual(const IVariantInitializer &) const { return false; }

private:
	utils::shared_ptr<StParser> parser;
	const int idx;
};
}


StreamDataManager::StreamDataManager() : skipUpdate(false)
{

}

StreamDataManager::~StreamDataManager()
{
	skipUpdate = true;
	auto mt = getDataManager()->transaction();
	auto tmpObjectsByStreams = objectsByStreams;
	for (auto it = tmpObjectsByStreams.begin(); it != tmpObjectsByStreams.end(); ++it) {
		rawRemoveStream(it->first, mt);
	}
}

void StreamDataManager::rawRemoveStream(const std::string & stream, const IDataManager::TransactionPtr & memTransaction)
{
	bool ok = true;
	VariantsList toRemove;
	//pobieramy obiekty do usuniecia z DM
	rawGetObjects(stream, toRemove);
	//usuń obiekty z MemoryDM w transakcji
	for (auto it = toRemove.begin(); it != toRemove.end(); ++it) {
		//TODO
		//przywrócić poprzedni jeśli był
		(*it)->setInitializer(VariantInitializerPtr());
		if (memTransaction->tryRemoveData(*it) == false) {
			ok = false;
		}
	}

	//usuń plik	
	objectsByStreams.erase(stream);
	streams_.left.erase(stream);
	if (ok == true) {
		CORE_LOG_INFO("Stream " << stream << " successfully removed from manager");
	} else {
		CORE_LOG_DEBUG("There were some problems while removing data from memory data manager for stream " << stream << ". Not all objects have been removed properly");
	}
}

void StreamDataManager::rawAddStream(const StreamGrabberPtr stream, const IDataManager::TransactionPtr & memTransaction)
{
	const auto streamName = stream->name();
	IParserManagerReader::ParserPrototypes sourceParsers;
	getParserManager()->sourceParsers(streamName, sourceParsers);
	IParserManagerReader::ParserPrototypes streamParsers;
	getParserManager()->streamParsers(streamName, streamParsers);

	//Z listy z własnym I/O usuwam te które są na liście z mozliwosciami strumienia - nie musze wypakowywac danych do pliku
	std::set<plugin::IParserConstPtr> sourceParsersSet;
	std::set<plugin::IParserConstPtr> streamParsersSet;

	sourceParsersSet.insert(sourceParsers.begin(), sourceParsers.end());
	streamParsersSet.insert(streamParsers.begin(), streamParsers.end());
	IParserManagerReader::ParserPrototypes sourcesLeft(sourceParsersSet.size());

	auto lastIT = std::set_difference(sourceParsersSet.begin(), sourceParsersSet.end(), streamParsersSet.begin(), streamParsersSet.end(), sourcesLeft.begin());

	sourcesLeft.erase(lastIT, sourcesLeft.end());
	//obiekty wyciągnięte z parserów
	VariantsList objects;
	//preferuje uzycie parserów z własną obsługą I/O - wierze że zrobią to maksymalnie wydajnie wg własnych zasad
	if (streamParsers.empty() == false) {
		initializeParsers<StreamParser>(streamParsers, stream, objects);
	}
	//teraz uzywam parserów strumieniowych - sam dostarczę im strumieni
	if (sourceParsers.empty() == false) {
		initializeParsers<FileStreamParser>(sourcesLeft, stream, objects);
	}

	if (objects.empty() == true) {
		CORE_LOG_DEBUG("Any of known parsers did not provide any valid data for stream: " << streamName);
	} else {

		int idx = 0;
		VariantsList objectsAdded;

		for (auto it = objects.begin(); it != objects.end(); ++it) {
			if ((*it)->existMetadata("core/name") == false) {
				std::stringstream name;
				name << streamName << "_" << idx++;
				(*it)->setMetadata("core/name", name.str());
			}

			if ((*it)->existMetadata("core/source") == false) {
				(*it)->setMetadata("core/source", streamName);
			}

			memTransaction->addData(*it);
			objectsAdded.push_back(*it);
		}

		// jak wszystko ok to zapamiętuje
		objectsByStreams.insert(ObjectsByStreams::value_type(streamName, objectsAdded));

		streams_.insert(Streams::value_type(streamName, stream));

		CORE_LOG_INFO("Stream: " << streamName << " succesfully loaded to manager. Extracted data : " << objects.size() << "| data loaded to memory data manager : " << objectsAdded.size());
	}
}

const bool StreamDataManager::rawIsManaged(const std::string & stream) const
{
	return streams_.left.find(stream) != streams_.left.end();
}

const bool StreamDataManager::rawIsLoadedCompleately(const std::string & stream) const
{
	return missingObjects.find(stream) == missingObjects.end();
}

void StreamDataManager::rawGetStreams(StreamsList & streams) const
{
	for (auto it = streams_.left.begin(); it != streams_.left.end(); ++it) {
		streams.push_back(it->get_left());
	}
}

void StreamDataManager::rawGetObjects(const std::string & stream, ConstVariantsList & objects) const
{
	auto it = objectsByStreams.find(stream);
	if (it != objectsByStreams.end()) {
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	} else {
		throw std::runtime_error("Stream not managed");
	}
}

void StreamDataManager::rawGetObjects(const std::string & stream, VariantsList & objects)
{
	auto it = objectsByStreams.find(stream);
	if (it != objectsByStreams.end()) {
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	} else {
		throw std::runtime_error("Stream not managed");
	}
}

void StreamDataManager::rawGetObjects(const std::string & stream, VariantsCollection & objects) const
{
	auto it = objectsByStreams.find(stream);

	for (auto objectIT = it->second.begin(); objectIT != it->second.end(); ++objectIT) {
		if (objects.exactTypes() == true) {
			if ((*objectIT)->data()->getTypeInfo() == objects.getTypeInfo()) {
				objects.push_back(*objectIT);
			}
		} else if ((*objectIT)->data()->isSupported(objects.getTypeInfo()) == true) {
			objects.push_back(*objectIT);
		}
	}
}

void StreamDataManager::updateObservers(const ChangeList & changes)
{
	for (auto it = observers.begin(); it != observers.end(); ++it) {
		try {
			(*it)->observe(changes);
		}
		catch (...) {
			//TODO
			//rozwinąć obserwatorów aby się jakoś identyfikowali!! ewentualnie robić to przez własną implementację dostarczaną konretnym obiektom
			//(osobne interfejsy readerów dla każdego elemnentu ładowanego do aplikacji - service, source, datasink, itp)
			CORE_LOG_WARNING("Error while updating stream data manager observer");
		}
	}
}

void StreamDataManager::removeStream(const std::string & stream)
{
	ScopedLock lock(sync);

	if (rawIsManaged(stream) == false) {
		throw std::runtime_error("Stream not managed");
	}

	{
		utils::Push _skipUpdate(skipUpdate, true);
		//usuń plik
		rawRemoveStream(stream, getDataManager()->transaction());
	}
	//notyfikuj
	ChangeList changes;
	Change change;
	change.stream = stream;
	change.modyfication = IStreamManagerReader::REMOVE_STREAM;
	changes.push_back(change);
	updateObservers(changes);
}

void StreamDataManager::addStream(const StreamGrabberPtr stream)
{
	ScopedLock lock(sync);

	auto streamName = stream->name();

	if (rawIsManaged(streamName) == true) {
		throw std::runtime_error("Stream already managed");
	}

	if (getParserManager()->sourceIsAccepted(streamName) == false) {
		throw std::runtime_error("Source not accepted by any parser");
	}

	{
		utils::Push _skipUpdate(skipUpdate, true);
		//dodaj plik
		rawAddStream(stream, getDataManager()->transaction());
	}

	//notyfikuj
	ChangeList changes;
	Change change;
	change.stream = streamName;
	change.modyfication = IStreamManagerReader::ADD_STREAM;
	changes.push_back(change);
	updateObservers(changes);
}

const bool StreamDataManager::tryAddStream(const StreamGrabberPtr stream)
{
	bool ret = true;
	try {
		addStream(stream);
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

const bool StreamDataManager::tryRemoveStream(const std::string & stream)
{
	bool ret = true;
	try {
		removeStream(stream);
	}
	catch (...) {
		ret = false;
	}

	return ret;
}

const IStreamDataManager::TransactionPtr StreamDataManager::transaction()
{
	return IStreamDataManager::TransactionPtr(new StreamTransaction(this));
}

const IStreamManagerReader::TransactionPtr StreamDataManager::transaction() const
{
	return IStreamManagerReader::TransactionPtr(new StreamReaderTransaction(const_cast<StreamDataManager*>(this)));
}

void StreamDataManager::observe(const IDataManagerReader::ChangeList & changes)
{
	if (skipUpdate == true) {
		return;
	}

	std::set<std::string> streamsToCheck;
	for (auto it = changes.begin(); it != changes.end(); ++it) {
		if ((*it).modyfication == IDataManagerReader::REMOVE_OBJECT &&
			(*it).previousValue != nullptr &&
			(*it).previousValue->initializer() != nullptr) {

			auto cinitializer = utils::dynamic_pointer_cast<const CompoundInitializer>((*it).previousValue->initializer());
			if (cinitializer != nullptr) {
				auto initializer = utils::const_pointer_cast<CompoundInitializer>(cinitializer);
				auto parser = initializer->innerParser();
				if (parser != nullptr) {
					const auto & stream = parser->getStreamName();
					streamsToCheck.insert(stream);
					objectsByStreams[stream].remove(utils::const_pointer_cast<Variant>((*it).previousValue));

					const auto idx = initializer->objectIdx((*it).previousValue);

					if ((*it).previousValue->initialized() == false ||
						(*it).previousValue->getRawPtr() != nullptr) {

						missingObjects[stream][initializer][idx] = (*it).previousValue->data()->getTypeInfo();
					} else {
						missingObjects[stream][initializer].erase(idx);

						if (missingObjects[stream][initializer].empty() == true) {
							missingObjects[stream].erase(initializer);
							if (missingObjects[stream].empty() == true) {
								missingObjects.erase(stream);
							}
						}
					}
				}
			}
		}
	}

	ChangeList schanges;

	for (auto it = streamsToCheck.begin(); it != streamsToCheck.end(); ++it) {
		tryRemoveUnusedStream(*it, schanges);
	}

	if (schanges.empty() == false) {
		updateObservers(schanges);
	}
}

void StreamDataManager::addObserver(const ObserverPtr & streamWatcher)
{
	ScopedLock lock(sync);
	if (std::find(observers.begin(), observers.end(), streamWatcher) != observers.end()) {
		throw std::runtime_error("Watcher already registered");
	}

	observers.push_back(streamWatcher);
}

void StreamDataManager::removeObserver(const ObserverPtr & streamWatcher)
{
	ScopedLock lock(sync);
	auto it = std::find(observers.begin(), observers.end(), streamWatcher);
	if (it == observers.end()) {
		throw std::runtime_error("Watcher not registered");
	}

	observers.erase(it);
}

void StreamDataManager::getStreams(StreamsList & streams) const
{
	ScopedLock lock(sync);
	for (auto it = streams_.left.begin(); it != streams_.left.end(); ++it) {
		streams.push_back(it->get_left());
	}
}

const bool StreamDataManager::isManaged(const std::string & stream) const
{
	ScopedLock lock(sync);
	return rawIsManaged(stream);
}

const bool StreamDataManager::isLoadedCompleately(const std::string & stream) const
{
	ScopedLock lock(sync);
	return rawIsLoadedCompleately(stream);
}

void StreamDataManager::getObjects(const std::string & stream, ConstVariantsList & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(stream, objects);
}

void StreamDataManager::getObjects(const std::string & stream, VariantsCollection & objects) const
{
	ScopedLock lock(sync);
	rawGetObjects(stream, objects);
}

void StreamDataManager::tryRemoveUnusedStream(const std::string & stream, ChangeList & changeList)
{
	auto it = objectsByStreams.find(stream);
	//sprawdzam czy nie moge juz usunąć strumienia i notyfikowć o zmianie
	if (it != objectsByStreams.end() && it->second.empty() == true) {
		CORE_LOG_INFO("Removing unused stream " << stream << " because of lack of delivered objects to memory data manager.");
		Change change;
		change.stream = it->first;
		change.modyfication = IStreamManagerReader::REMOVE_UNUSED_STREAM;
		changeList.push_back(change);

		objectsByStreams.erase(it);
		streams_.left.erase(stream);
	}
}

void core::StreamDataManager::CompoundInitializer::initialize(Variant * object)
{
	auto mt = getDataManager()->transaction();
	std::list<int> toDelete;
	if (data->parser->tryParse() == false) {
		CORE_LOG_NAMED_INFO("parser", "Parser " << data->parser->getParser()->ID() << " (" << data->parser->getParser()->shortName() << ") failed for stream " << data->parser->getStreamName());
		for (auto it = data->objects.begin(); it != data->objects.end(); ++it) {
			auto ow = it->second.lock();
			if (ow != nullptr && ow->data()->getRawPtr() == nullptr) {
				toDelete.push_back(it->first);
			}
		}
	} else {
		for (auto it = data->objects.begin(); it != data->objects.end(); ++it) {
			auto ow = it->second.lock();
			if (ow != nullptr) {
				if (ow->initialized() == false) {
					data->parser->getObject(*ow, it->first);
					if (ow->data()->getRawPtr() == nullptr) {
						CORE_LOG_NAMED_INFO("parser", "Object of type " << ow->data()->getTypeInfo().name()
											<< " not initialized properly by parser ID: " << data->parser->getParser()->ID()
											<< " (" << data->parser->getParser()->shortName() << ") for stream " << data->parser->getStreamName());
						toDelete.push_back(it->first);
					}
				}
			} else {
				toDelete.push_back(it->first);
			}
		}
	}

	data->parser->reset();

	for (auto it = toDelete.begin(); it != toDelete.end(); ++it) {
		auto oit = data->objects.find(*it);
		mt->tryRemoveData(oit->second.lock());
		data->objects.erase(oit);
	}

	if (data->objects.empty() == true) {
		CORE_LOG_NAMED_INFO("parser", "Releasing unused parser ID: " << data->parser->getParser()->ID() << " (" << data->parser->getParser()->shortName() << ") for stream " << data->parser->getStreamName());
	}
}

core::IVariantInitializer* core::StreamDataManager::CompoundInitializer::clone() const
{
	return new SimpleInitializer(data->parser, idx);
}

core::StreamDataManager::CompoundInitializer::CompoundDataPtr core::StreamDataManager::CompoundInitializer::details()
{
	return data;
}

utils::shared_ptr<const StParser> core::StreamDataManager::CompoundInitializer::innerParser() const
{
	return ((data != nullptr) ? data->parser : utils::shared_ptr<const StParser>());
}

const int core::StreamDataManager::CompoundInitializer::objectIdx(const VariantConstPtr object) const
{
	int ret = -1;

	if (data != nullptr) {

		for (auto it = data->objects.begin(); it != data->objects.end(); ++it) {
			if (it->second.lock() == object) {
				ret = it->first;
				break;
			}
		}
	}

	return ret;
}

const plugin::IParserPtr core::StreamDataManager::CompoundInitializer::parser() const
{
	plugin::IParserPtr ret;

	if (data != nullptr && data->parser != nullptr) {
		ret = data->parser->getParser();
	}

	return ret;
}

bool core::StreamDataManager::CompoundInitializer::isEqual(const IVariantInitializer &) const
{
	return false;
}

core::StreamDataManager::CompoundInitializer::~CompoundInitializer()
{

}

core::StreamDataManager::CompoundInitializer::CompoundInitializer(const CompoundDataPtr & data, const int idx) : data(data), idx(idx)
{

}
