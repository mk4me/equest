/********************************************************************
	created:	2015/10/21
	created:	14:01
	filename: 	DicomWatcher.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM_WATCHER__DicomWatcher_H__
#define HEADER_GUARD_DICOM_WATCHER__DicomWatcher_H__

#include <QtCore/QObject>
#include <QtCore/QString>
#include <utils/Filesystem.h>
#include "WatcherConfig.h"

//! Klasa odbiera sygna³, ¿e folder siê zmieni³ i jeœli to mo¿liwe konwertuje dicom na nasz format
class DicomWatcher : public QObject 
{
	Q_OBJECT;
	public:
		//! Konstruktor
		//! \param outputDir folder wyjœciowy, jeœli nie istnieje, to zostanie utworzony
		//! \param configPath œcie¿ka do pliku ze statusami plików (informacja, czy zosta³y przetworzone). Jesli nie ma to zostanie utworzony
		//! \param singleOutputFolder czy wszystkie produkty maj¹ trafiæ do tego samego folderu?
		//! \param runOnce czy aplikacja ma przetwarzaæ pliki ci¹gle, czy tylko raz i koñczyæ dzia³anie?
		DicomWatcher(const utils::Filesystem::Path& outputDir, const utils::Filesystem::Path& configPath,bool singleOutputFolder = false, bool runOnce = false);
		virtual ~DicomWatcher() {}

	public slots:
		//! Sprawdza folder, jeœli pojawi³y siê nowe pliki zip, to rozpoczyna siê przetwarzanie
		//! \param path œcie¿ka do folderu, który uleg³ zmianie
		void dirChanged(const QString & path);

		

	public:
		//! \param path obserwowana œcie¿ka
		//! \return zwraca wektor plików zip, które jeszcze nie by³y wczeœniej przetwarzane
		std::vector<utils::Filesystem::Path> listFilesToProcess(const std::string& path);

	private:
		//! metoda zakoñczy dzia³anie aplikacji, jeœli ustawiony jest parametr runOnce
		void exitIfRequested(int errorCode = 0);
		//! 
		//! \param from
		//! \param to
		//! \return 
		void import(const utils::Filesystem::Path& from, const utils::Filesystem::Path& to);
		//! Metoda czeka, a¿ plik bêdzie gotowy do u¿ytku (np. skoñczy siê transfer), póŸniej go rozpakuje
		//! \param file plik do rozpakowania
		//! \param out1 folder docelowy
		void waitTillReadyThanExtract(const utils::Filesystem::Path &file, const utils::Filesystem::Path &out);

	private:
		//! przechowuje statusy plików
		WatcherConfig config;
		//! œcie¿ka do pliku ze statusami plików (informacja, czy zosta³y przetworzone)
		const utils::Filesystem::Path outputDir;
		//! czy aplikacja ma przetwarzaæ pliki ci¹gle, czy tylko raz i koñczyæ dzia³anie?
		bool runOnce;
		//! czy wszystkie produkty maj¹ trafiæ do tego samego folderu ?
		bool singleOutputFolder;
};



#endif // HEADER_GUARD 
