/********************************************************************
created:  2013/01/17
created:  17:1:2013   16:47
filename: StreamDataManager.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD___STREAMDATAMANAGER_H__
#define HEADER_GUARD___STREAMDATAMANAGER_H__

#include <map>
#include <list>
#include <mutex>
#include "ApplicationCommon.h"
#include "ParserManager.h"
#include "RegisteredDataTypesManager.h"
#include <corelib/IParserManagerReader.h>
#include <corelib/IStreamDataManager.h>
#include <corelib/IStreamManagerReader.h>

//TODO
//implementacja, wyciągnięcie części wspólnej fileDM i streamDM -> parsery i większa częśc logiki parsowania i inicjalizacji

#include <boost/bimap.hpp>
#include <corelib/IDataManagerReader.h>

namespace core {

	class StParser;
	class SimpleInitializer;
	class StreamDataManager : public IStreamDataManager, public IDataManagerReader::IObserver
	{
	private:

		class StreamTransaction;
		friend class StreamTransaction;

		class StreamReaderTransaction;
		friend class StreamReaderTransaction;

		class CompoundInitializer : public IVariantInitializer
		{
		public:

			struct CompoundData {
				utils::shared_ptr<StParser> parser;
				std::map<int, VariantWeakPtr> objects;
			};

			typedef utils::shared_ptr<CompoundData> CompoundDataPtr;

		public:

			CompoundInitializer(const CompoundDataPtr & data, const int idx);

			virtual ~CompoundInitializer();

			virtual void initialize(Variant * object);

			virtual IVariantInitializer * clone() const;
			virtual bool isEqual(const IVariantInitializer &) const;

			const plugin::IParserPtr parser() const;

			//! \param object Obiekt o który pytamy
			//! \return Indeks obiektu inicjalizowany przez parser
			const int objectIdx(const VariantConstPtr object) const;

			utils::shared_ptr<const StParser> innerParser() const;

			CompoundDataPtr details();

		private:
			CompoundDataPtr data;
			const int idx;
		};

		//! Słownik aktualnie obsługiwanych plików i skojarzonych z nimi parserów
		typedef boost::bimap<std::string, StreamGrabberPtr> Streams;
		//! Mapowanie indeksu danych parsera do typu pod tym indeksem
		typedef std::map<int, utils::TypeInfo> ParserTypes;
		//! Mapowanie inicjalizatora do brakujących danych
		typedef std::map<utils::shared_ptr<CompoundInitializer>, ParserTypes> ObjectsByParser;
		//! Mapowanie plików do brakujących danych
		typedef std::map<std::string, ObjectsByParser> MissingObjects;

		//!
		typedef std::map<std::string, VariantsList> ObjectsByStreams;

		typedef std::recursive_mutex SyncPolicy;
		typedef std::lock_guard<SyncPolicy> ScopedLock;

	private:
		//! Zarejestrowane strumienie
		Streams streams_;

		//! Mapa parserów wg plików
		ObjectsByStreams objectsByStreams;

		//! Dane wyładowane przez Memory DM
		MissingObjects missingObjects;

		//! Obiekt na potrzeby synchronizacji
		mutable SyncPolicy sync;

		//! Obiekty obserwujące stan DM
		std::list<ObserverPtr> observers;

		//! Zmienna pozwalająca szybko opuscic update memory data managera
		volatile bool skipUpdate;

	private:

		void rawRemoveStream(const std::string & stream, const IDataManager::TransactionPtr & memTransaction);

		void rawAddStream(const StreamGrabberPtr stream, const IDataManager::TransactionPtr & memTransaction);

		const bool rawIsManaged(const std::string & stream) const;

		void updateObservers(const ChangeList & changes);

		void rawGetStreams(StreamsList & streams) const;

		void rawGetObjects(const std::string & stream, ConstVariantsList & objects) const;

		void rawGetObjects(const std::string & stream, VariantsCollection & objects) const;

		void rawGetObjects(const std::string & stream, VariantsList & objects);

		const bool rawIsLoadedCompleately(const std::string & stream) const;

	public:

		StreamDataManager();

		virtual ~StreamDataManager();

		virtual const IStreamDataManager::TransactionPtr transaction();

		//! \data Dane wchodzące pod kontrolę DM
		virtual void addStream(const StreamGrabberPtr streamGrabber);

		//! Dane usuwane z DM
		virtual void removeStream(const std::string & stream);

		virtual const bool tryAddStream(const StreamGrabberPtr streamGrabber);
		//! \param stream Strumień usuwany z managera
		//! \return Prawda jeśli pomyślnie usunięto strumień
		virtual const bool tryRemoveStream(const std::string & stream);

		//! \param files Zbiór plików ktrymi aktualnie zarządza ten DataManager
		virtual void getStreams(StreamsList & streams) const;

		//! \param stream Nazwa strumienia
		//! \return Prawda jeśli strumień jest zarządzany przez ten DM
		virtual const bool isManaged(const std::string & stream) const;

		//! \param stream Strumień który weryfikujemy czy jest w pełni załadowany
		//! \return Prawda jeśli strumień jest w pełni załadowany
		virtual const bool isLoadedCompleately(const std::string & stream) const;

		//! \param stream Nazwa strumienia
		//! \return Mapa obiektów względem plików z których pochodza
		virtual void getObjects(const std::string & stream, ConstVariantsList & objects) const;
		//! \param stream Nazwa strumienia
		//! \return Mapa obiektów względem plików z których pochodza
		virtual void getObjects(const std::string & stream, VariantsCollection & objects) const;

		//! \param streamWatcher Rejestrowany obiekt obserwujący zmiany managera strumieni
		virtual void addObserver(const ObserverPtr & streamWatcher);
		//! \param streamWatcher Wyrejestrowywany obiekt obserwujący zmiany managera strumieni
		virtual void removeObserver(const ObserverPtr & streamWatcher);
		//! \return Nowa transakcja
		virtual const IStreamManagerReader::TransactionPtr transaction() const;

	public:

		//IDataManagerReader::IObjectObserver API
		virtual void observe(const IDataManagerReader::ChangeList & changes);

	private:
		//! \param stream Strumień który weryfikujemy pod kątem dostępności danych, jeśli brak to usuwamy nieużywany plik
		//! \param changeList Lista zmian
		void tryRemoveUnusedStream(const std::string & stream, ChangeList & changeList);

		template<typename ParserT>
		void initializeParsers(const IParserManagerReader::ParserPrototypes & parsers,
							   const IStreamDataManager::StreamGrabberPtr stream,
							   VariantsList & objects);
	};

	template<typename ParserT>
	void StreamDataManager::initializeParsers(const IParserManagerReader::ParserPrototypes & parsers,
											  const IStreamDataManager::StreamGrabberPtr stream, VariantsList & objects)
	{
		auto pm = core::getParserManager();
		auto hm = core::getRegisteredDataTypesManager();

		//jeżeli pliku nie ma dodaj go, stwórz parsery i rozszerz dostępne dane wraz z ich opisem
		for (auto parserIT = parsers.begin(); parserIT != parsers.end(); ++parserIT) {
			// tworzymy współdzielone dane dla inicjalizatorów
			CompoundInitializer::CompoundDataPtr cid = utils::make_shared<CompoundInitializer::CompoundData>();
			cid->parser.reset(new ParserT((*parserIT)->create(), stream));

			auto id = (*parserIT)->ID();
			auto parserTypesMap = pm->parserTypes(id, stream->name());

			//zarejestrowanie obiektów i ich związku z parserem i typami danych
			for (auto objectIT = parserTypesMap.begin(); objectIT != parserTypesMap.end(); ++objectIT) {
				//towrzymy taki obiekt
				auto ow = hm->createWrapper(objectIT->second);
				//aktualizuje ow dla CompoundInitializer
				cid->objects[objectIT->first] = ow;
				//inicjalizator na bazie parsera
				ow->setInitializer(VariantInitializerPtr(new CompoundInitializer(cid, objectIT->first)));
				objects.push_back(ow);
			}
		}
	}
}

#endif	//	HEADER_GUARD___STREAMDATAMANAGER_H__
