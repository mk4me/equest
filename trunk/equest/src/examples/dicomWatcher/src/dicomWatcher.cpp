#include "DicomWatcher.h"
#include <QtCore/QFileSystemWatcher>
#include <dicomLib/Dicom.h>
#include "dicomLib/DicomImporter.h"
#include "dicomLib/DicomInternalStruct.h"
#include <quazip/JlCompress.h>
#include "loglib/Logger.h"
#include <QtWidgets/QApplication>
#include <QtCore/QThread>
#include <QtCore/QFileInfo>

DicomWatcher::DicomWatcher(const utils::Filesystem::Path& outputDir, const utils::Filesystem::Path& configPath, bool singleOutputFolder, bool runOnce) :
config(configPath),
outputDir(outputDir),
singleOutputFolder(singleOutputFolder),
runOnce(runOnce)
{
	//! wczytanie informacji o przetworzonych plikach
	config.loadOrCreate();
}

void DicomWatcher::import(const utils::Filesystem::Path& from, const utils::Filesystem::Path& to)
{
	dicomImporter::DicomImporter importer(1, !singleOutputFolder);
	importer.setCallBack([&](const std::string& s) {
		UTILS_LOG_INFO("processing: " << s);
	});

	// wyszukuje g³ownego pliku dicom, jeœli takiego nie ma to rzucany jest wyj¹tek
	auto rootDir = importer.findDicomRootDir(from);
	// wczytanie struktury, wyj¹tek przy niepowodzeniu
	auto inter = importer.import(rootDir);
	// jeœli wczytana struktura ma w sobie kilka sesji to nastêpuje rozdzielenie na wiele struktur z jednosesyjnych
	std::vector<dicomImporter::DicomInternalStructPtr> splits = importer.split(inter);

	dicomImporter::DicomSaver saver;
	for (auto it = splits.begin(); it != splits.end(); ++it) {
		// konwersja i zapis obrazów
		importer.convertImages(*it, rootDir, to);
		// zapis xml-a z metadanymi
		saver.save(to, *it, !singleOutputFolder);
	}
}

class SyncFileInfo 
{
public:
	SyncFileInfo(const QString& filePath) :
		path(filePath) {}
	qint64 size() const {
		QFileInfo fi(path);
		return fi.size();
	}
private:
	QString path;
};

void DicomWatcher::waitTillReadyThanExtract(const utils::Filesystem::Path &file, const utils::Filesystem::Path &out)
{
	int timeout = 1000;
	QThread::msleep(10);
	SyncFileInfo fileInfo(QString::fromStdString(file.string()));
	int prevSize = fileInfo.size();

	// jeœli do pliku nie da siê zapisaæ, to mo¿e oznaczaæ, ¿e jest on dopiero tworzony
	while (true) { //(!fileInfo.isWriteable()) {
		//std::cout << "not writeable" << std::endl;
		QThread::msleep(100);
		int size = fileInfo.size();
		//std::cout << "prev - " << prevSize << ", size - " << size << std::endl;
		if (size == prevSize) {
			QThread::msleep(timeout);
			// mo¿e siê jednak okazaæ, ¿e po prostu nie mamy prawa do zapisu na takim pliku
			// wtedy tak czy siak bêdziemy go próbowaæ rozpakowaæ
			if (prevSize == fileInfo.size()) {
				break;
			}
		} else {
			prevSize = size;
		}
	}
	for (int i = 0; ; ++i) {
		if (i == 5) {
			throw std::runtime_error("A problem occurred while extracting the archive");
		}
		QStringList sl = JlCompress::extractDir(QString::fromStdString(file.string()), QString::fromStdString(out.string()));
		if (!sl.empty()) {
			break;
		}
		QThread::msleep(100);
	}
}

void DicomWatcher::dirChanged(const QString & path)
{
	try {
		// ta petla wynika z tego, ze metoda wyzwalana jest gdy skopiowany jest pierwszy plik
		// gdy ustawiona jest flaga run_once, to powinny zostac przetworzone wszystkie pliki
		// zostalo zrobione naiwne zalozenie, ze rozpakowywanie i konwertowanie dicoma bedzie trwalo dluzej niz jego kopiowanie
		bool processed = true;
		while (processed) {
			processed = false;
			// wszystkie nieprzetworzone pliki *.zip z foleru, bez przeszukiwania podfolderów
			auto files = listFilesToProcess(path.toStdString());
			for (auto& f : files) {
				processed = runOnce;
				UTILS_LOG_INFO("extracting: " << f.string());
				utils::Filesystem::Path out1(outputDir / f.stem() / "raw");
				utils::Filesystem::Path out2 = singleOutputFolder ? (outputDir / "converted") : (outputDir / f.stem() / "converted");
				//! wypakowanie archiwum, jeœli nie ma folderu, to zostanie utworzony
				waitTillReadyThanExtract(f, out1);

				UTILS_LOG_INFO("processing: " << f.string());
				//! przetworzenie rozpakowanego dicoma (folder out1) na obrazy png, wynik trafia do folderu out2
				import(out1, out2 );
				//! aktualizacja statusów
				config.addProcessed(f);
				config.save();
				UTILS_LOG_INFO("processing: " << f.string() << " done.");
			}
		}

		exitIfRequested();
	}
	catch (std::exception& e) {
		UTILS_LOG_ERROR(e.what());
		exitIfRequested(1);
	} 
	catch (...) {
		UTILS_LOG_ERROR("Unknown error");
		exitIfRequested(1);
	}
}

std::vector<utils::Filesystem::Path> DicomWatcher::listFilesToProcess(const std::string& path)
{

	auto files = utils::Filesystem::listFiles(path, false, ".zip");
	std::vector<utils::Filesystem::Path> ret;
	std::copy_if(files.begin(), files.end(), std::back_inserter(ret), [&](const utils::Filesystem::Path& p) { return !config.wasProcessed(p); });
	return ret;
}

void DicomWatcher::exitIfRequested(int errorCode)
{
	if (runOnce) {
		QApplication::exit(errorCode);
		exit(errorCode);
	}
}



