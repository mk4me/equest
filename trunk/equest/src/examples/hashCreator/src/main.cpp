#include <string>
#include <networkUtils/EncryptionUtils.h>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <iostream>

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("Hash creator");
	QCoreApplication::setApplicationVersion("0.1");

	QCommandLineParser parser;
	parser.setApplicationDescription("Hash creator");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("txt", QCoreApplication::translate("main", "Text to encrypt"));

	QCommandLineOption key1Option(QStringList() << "k1" << "key1",
		QCoreApplication::translate("main", "Key1 to encrypt (16 characters)."));
	parser.addOption(key1Option);

	QCommandLineOption key2Option(QStringList() << "k2" << "key2",
		QCoreApplication::translate("main", "Key2 to encrypt (16 characters)."));
	parser.addOption(key2Option);

	parser.process(app);

	const QStringList args = parser.positionalArguments();
	if (args.size() < 1) {
		parser.showHelp(1);
	}
	else {
		try {
			auto txt = args.at(0);
			if (parser.isSet(key1Option) && parser.isSet(key2Option)) {
				QString key1S = parser.value(key1Option);
				QString key2S = parser.value(key2Option);\
					auto key1 = networkUtils::EncryptionUtils::convertEncryptionKey(key1S);
				auto key2 = networkUtils::EncryptionUtils::convertEncryptionKey(key2S);
				std::cout << networkUtils::EncryptionUtils::encrypt(txt, key1, key2).toStdString();
			}
			else {
				std::cout << networkUtils::EncryptionUtils::encrypt(txt).toStdString();
			}
		} catch (const std::invalid_argument& e) {
			std::cerr << e.what();
		} catch (...) {
			std::cerr << "Unknown error";
		}
	}
	return 0;
}