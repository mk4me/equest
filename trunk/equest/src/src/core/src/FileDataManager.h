/********************************************************************
created:  2013/01/14
created:  14:1:2013   16:13
filename: FileDataManager.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD___FILEDATAMANAGER_H__
#define HEADER_GUARD___FILEDATAMANAGER_H__

#include <map>
#include <list>
#include <mutex>
#include "ApplicationCommon.h"
#include "ParserManager.h"
#include "RegisteredDataTypesManager.h"
#include <corelib/IFileManagerReader.h>
#include <corelib/IFileDataManager.h>
#include <corelib/IDataManagerReader.h>
#include <corelib/IParserManagerReader.h>

namespace core {

	class FParser;
	class FileDataManager : public IFileDataManager, public IDataManagerReader::IObserver
	{
	private:
		//! Forward declaration
		class FileTransaction;
		friend class FileTransaction;
		//! Forward declaration
		class FileReaderTransaction;
		friend class FileReaderTransaction;

		class CompoundInitializer : public IVariantInitializer
		{
		public:

			struct CompoundData {
				utils::shared_ptr<FParser> parser;
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

			utils::shared_ptr<const FParser> innerParser() const;

			CompoundDataPtr details();

		private:
			CompoundDataPtr data;
			const int idx;
		};

		//! Słownik aktualnie obsługiwanych plików i skojarzonych z nimi parserów
		typedef std::map<utils::Filesystem::Path, ConstVariantsList> ObjectsByFiles;
		//! Mapowanie indeksu danych parsera do typu pod tym indeksem
		typedef std::map<int, utils::TypeInfo> ParserTypes;
		//! Mapowanie inicjalizatora do brakują		cych danych
			typedef std::map<utils::shared_ptr<CompoundInitializer>, ParserTypes> ObjectsByParser;
		//! Mapowanie plików do brakują		cych danych
			typedef std::map<utils::Filesystem::Path, ObjectsByParser> MissingObjects;

		//! Polityka synchronizacji
		typedef std::recursive_mutex SyncPolicy;
		typedef std::lock_guard<SyncPolicy> ScopedLock;

	private:

		//! Mapa parserów wg plików
		ObjectsByFiles objectsByFiles;
		//! Dane wyładowane przez Memory DM
		MissingObjects missingObjects;

		//! Obiekt na potrzeby synchronizacji
		mutable SyncPolicy sync;

		//! Obiekty obserwują		ce stan DM
			std::list<ObserverPtr> observers;

		//! Zmienna pozwalają		ca szybko opuscic update memory data managera
			volatile bool skipUpdate;

	private:

		template<typename ParserT>
		void initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const std::string & path, VariantsList & objects);

		void rawRemoveFile(const utils::Filesystem::Path & file, const IDataManager::TransactionPtr & memTransaction);

		void rawAddFile(const utils::Filesystem::Path & file, const IDataManager::TransactionPtr & memTransaction);

		void rawReloadFile(const utils::Filesystem::Path & file, const bool compleately, const IDataManager::TransactionPtr & memTransaction);

		const bool rawIsManaged(const utils::Filesystem::Path & file) const;

		const bool rawIsLoadedCompleately(const utils::Filesystem::Path & file) const;

		void updateObservers(const ChangeList & changes);

		void rawGetFiles(utils::Filesystem::FilesSet & files) const;

		void rawGetObjects(const utils::Filesystem::Path & file, ConstVariantsList & objects) const;

		void rawGetObjects(const utils::Filesystem::Path & file, VariantsCollection & objects) const;

		void rawGetObjects(const utils::Filesystem::Path & file, VariantsList & objects);

	public:
		//IFileDataManager API

		//! \param files Lista plików które zostaną usunięte z aplikacji a wraz z nimi skojarzone parsery i dane
		virtual void removeFile(const utils::Filesystem::Path & file);

		//! \param files Lista plików dla których zostaną utworzone parsery i z których wyciągnięte dane
		//! będą dostępne poprzez DataMangera LENIWA INICJALIZACJA
		virtual void addFile(const utils::Filesystem::Path & file);

		//! \param file Ścieżka pliku do przeładowania
		//! \param complete Czy plik ma zostać całkowicie przeładowany czy tylko brakujące dane mają zostać doładowane
		virtual void reloadFile(const utils::Filesystem::Path & file,
			const bool complete);


		virtual const bool tryAddFile(const utils::Filesystem::Path & file);

		virtual const bool tryRemoveFile(const utils::Filesystem::Path & file);

		//! \param file Ścieżka pliku do przeładowania
		//! \param complete Czy plik ma zostać całkowicie przeładowany czy tylko brakujące dane mają zostać doładowane
			//! \return Prawda jeśli plik pomyślnie przeładowano
		virtual const bool tryReloadFile(const utils::Filesystem::Path & file,
			const bool complete);

		//! \return Nowa transakcja
		virtual const TransactionPtr transaction();

	public:

		//IFileManagerReader API

		virtual void addObserver(const ObserverPtr & fileWatcher);
		virtual void removeObserver(const ObserverPtr & fileWatcher);

		//! \param files Zbiór plików ktrymi aktualnie zarządza ten DataManager
		virtual void getFiles(utils::Filesystem::FilesList & files) const;

		//! \param file Plik który weryfikujemy czy jest zarządzany przez DM
		//! \return Prawda jeżeli plik jest zarządzany przez ten DM
		virtual const bool isManaged(const utils::Filesystem::Path & file) const;

		//! \param file Plik kótry weryfikujemy czy jest w pełni załadowany
		//! \return Prawda jeśli plik jest w pełni załadowany
		virtual const bool isLoadedCompleately(const utils::Filesystem::Path & file) const;

		//! \param files Zbior plików dla których chcemy pobrać listę obiektów
		//! \return Mapa obiektów względem plików z których pochodza
		virtual void getObjects(const utils::Filesystem::Path & file, ConstVariantsList & objects) const;

		//! \param files Zbior plików dla których chcemy pobrać listę obiektów
		//! \return Mapa obiektów względem plików z których pochodza
		virtual void getObjects(const utils::Filesystem::Path & file, VariantsCollection & objects) const;

		virtual const TransactionConstPtr transaction() const;

	public:

		//IDataManagerReader::IObjectObserver API
		virtual void observe(const IDataManagerReader::ChangeList & changes);

	public:

		FileDataManager();

		virtual ~FileDataManager();

	private:
		//! \param path Ścieżka pliku który weryfikujemy pod ką		tem dostępności danych,
			//! jeśli brak to usuwamy nieużywany plik
			void tryRemoveUnusedFile(const utils::Filesystem::Path & file,
			ChangeList & changes);
	};


	template<typename ParserT>
	void FileDataManager::initializeParsers(const IParserManagerReader::ParserPrototypes & parsers, const std::string & path, VariantsList & objects)
	{
		auto pm = core::getParserManager();
		auto hm = core::getRegisteredDataTypesManager();

		//jeżeli pliku nie ma dodaj go, stwórz parsery i rozszerz dostępne dane wraz z ich opisem
		for (auto parserIT = parsers.begin(); parserIT != parsers.end(); ++parserIT) {
			// tworzymy współdzielone dane dla inicjalizatorów
			CompoundInitializer::CompoundDataPtr cid(new CompoundInitializer::CompoundData);
			cid->parser.reset(new ParserT((*parserIT)->create(), path));

			auto parserTypesMap = pm->parserTypes((*parserIT)->ID(), path);

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

#endif	//	HEADER_GUARD___FILEDATAMANAGER_H__
