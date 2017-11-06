#include <fstream>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <dicomLib/Dicom.h>
#include "DicomWatcher.h"
#include "WatcherConfig.h"
#include "QtCore/QCommandLineParser"
#include "QtCore/QStringList"
#include "loglib/ILog.h"
#include "loglib/Logger.h"

#include <QtCore/QThread>
#include <QtCore/QFile>

// sk³adnia :
// dicomWatcher [opcje] inputFolder outputFolder 
// opcje :
// -s --status-file			-œcie¿ka do pliku z informacjami o przetworzonych plikach
// -v --version				-informacja o wersji
// -l --log-file			-œcie¿ka do pliku z logiem
// -p --print-on-console	-informacje bêd¹ logowane na konsolê
// -r --run_once			-aplikacja tylko raz czeka na dane wejœciowe, po ich przetworzeniu koñczy dzia³anie
// -f --single_folder		-dane wyjœciowe trafi¹ do pojedynczego foldera
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("Dicom Watcher");
	QCoreApplication::setApplicationVersion("0.14");

	QCommandLineParser parser;
	parser.setApplicationDescription("Dicom Watcher");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("WatchFolder", QCoreApplication::translate("main", "Directory to watch."));
	parser.addPositionalArgument("OutputFolder", QCoreApplication::translate("main", "Destination directory."));


	QCommandLineOption configOption(QStringList() << "s" << "status-file",
								   QCoreApplication::translate("main", "Status <file>."),
								   QCoreApplication::translate("main", "file"));
	parser.addOption(configOption);
	QCommandLineOption verboseOption(QStringList() << "p" << "print-on-console",
									 QCoreApplication::translate("main", "Prints informations on standard output"));
	parser.addOption(verboseOption);

	QCommandLineOption runOnceOption(QStringList() << "r" << "run-once",
		QCoreApplication::translate("main", "Prints informations on standard output"));
	parser.addOption(runOnceOption);

	QCommandLineOption singleFolderOption(QStringList() << "f" << "single-folder",
		QCoreApplication::translate("main", "Single, output folder."));
	parser.addOption(singleFolderOption);

	QCommandLineOption logOption(QStringList() << "l" << "log-file",
									QCoreApplication::translate("main", "Log <file>."),
									QCoreApplication::translate("main", "file"));
	parser.addOption(logOption);
	
	//QCommandLineOption xOption(QStringList() << "x" << "ximulateIN",
	//	QCoreApplication::translate("main", "Log <file>."));
	//parser.addOption(xOption);

	parser.process(app);


	utils::Filesystem::Path sourceDir("input");
	utils::Filesystem::Path destDir("output");
	
	const QStringList args = parser.positionalArguments();
	if (args.size() < 2) {
		parser.showHelp(1);
	} else {
		sourceDir = args.at(0).toStdString();
		destDir = args.at(1).toStdString();
	}

	

	/*if (parser.isSet(xOption)) {
		QFile outF(QString::fromStdString(destDir.string()));
		QFile inF(QString::fromStdString(sourceDir.string()));
		outF.open(QIODevice::WriteOnly);
		inF.open(QIODevice::ReadOnly);
		const int part = 10000;
		char buff[part];
		int rea = -1;
		int size = 0;
		do {
			rea = inF.read(buff, part);
			outF.write(buff, rea);
			size += rea;
			std::cout << "processing : " << size << std::endl;
			QThread::msleep(10);
		} while (rea > 0);
		outF.close();
		inF.close();
		return 0;
	}
*/

	utils::Filesystem::Path configFile("status.dw");
	if (parser.isSet(configOption)) {
		QString targetDir = parser.value(configOption);
		configFile = targetDir.toStdString();
	}

	std::vector<loglib::ILogPtr> loggers;
	if (parser.isSet(verboseOption)) {
		loggers.push_back(utils::make_shared<loglib::CoutLogger>());
	}

	utils::Filesystem::Path logFile("log.txt");
	if (parser.isSet(logOption)) {
		QString targetDir = parser.value(logOption);
		logFile = targetDir.toStdString();
	}
	if (utils::Filesystem::pathExists(logFile)) {
		utils::Filesystem::deleteFile(logFile);
	}
	loglib::FileLogger::setPath(logFile);
	loggers.push_back(utils::make_shared<loglib::FileLogger>());

	loglib::ILogPtr multiLog = utils::make_shared<loglib::MultiLogger>(std::move(loggers));
	loglib::Logger::setLog(multiLog);
	
	QFileSystemWatcher fileWatcher;
	QString observedPath = QString::fromStdString(sourceDir.string());
	bool test = fileWatcher.addPath(observedPath);

	bool runOneSet = parser.isSet(runOnceOption);
	DicomWatcher dicomWatcher(destDir, configFile, parser.isSet(singleFolderOption), runOneSet);
	if (runOneSet) {
		auto files = dicomWatcher.listFilesToProcess(observedPath.toStdString());
		if (!files.empty()) {
			dicomWatcher.dirChanged(observedPath);
		}
	}
	UTILS_LOG_INFO("Dicom watcher started");
	test = QObject::connect(&fileWatcher, SIGNAL(directoryChanged(const QString &)), &dicomWatcher, SLOT(dirChanged(const QString &)));

	return app.exec();
}


