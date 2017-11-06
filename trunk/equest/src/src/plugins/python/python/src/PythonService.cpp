#include "PythonPCH.h"
#include "PythonLogic.h"
#include "PythonService.h"
#include "PythonHighlighter.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtCore/QCoreApplication>
#include "QtCore/QCommandLineParser"
#include <QtGui/QFont>
#include "PythonConsole.h"
#include "QtWidgets/QAction"
#include "plugins/python/python/PythonEditor.h"

#define signals Q_SIGNALS
#define slots Q_SLOTS

#include "coreui/CoreAction.h"

using namespace python;

PythonService::PythonService() :
    editor(nullptr), 
	hierarchyManager(nullptr)
{

}

PythonService::~PythonService()
{

}


void PythonService::init(core::ISourceManager * sourceManager,
						 core::IVisualizerManager * visualizerManager,
						 core::IDataManager * memoryDataManager,
						 core::IStreamDataManager * streamDataManager,
						 core::IFileDataManager * fileDataManager,
						 core::IDataHierarchyManager * hierarchyDataManager)
{
	auto args = QCoreApplication::arguments();
	QCommandLineParser parser;
	QCommandLineOption homeOption(QStringList() << "python-home",
		QCoreApplication::translate("main", "Python home directory"),
		QCoreApplication::translate("main", "path"));
	parser.addOption(homeOption);
	QCommandLineOption script(QStringList() << "python-script",
		QCoreApplication::translate("main", "Python script to run"),
		QCoreApplication::translate("main", "path"));
	parser.addOption(script);
	if (!parser.parse(args)) {
		PLUGIN_LOG_INFO(parser.errorText().toStdString());
	}


	std::string path;
	if (parser.isSet(homeOption)) {
		path = parser.value(homeOption).toStdString();
	}

	if (parser.isSet(script)) {
		this->startingScriptPath = parser.value(script).toStdString();
	}
	logic = utils::make_shared<PythonLogic>(path);
	hierarchyManager = hierarchyDataManager;
}

QWidget* PythonService::getWidget( )
{
	if (!logic) {
		return nullptr;
	}
	if (editor == nullptr){
		console = new python::PythonConsole(logic);

		editor = PythonEditor::createPythonEditor();
		QList<QAction*> actions;

		coreUI::CoreAction*  run = new coreUI::CoreAction(tr("Run"), QIcon(":/newVdf/icons/run.png"), tr("Run"), this, coreUI::CoreTitleBar::Left);
		connect(run, SIGNAL(triggered()), this, SLOT(runScript()));
		actions.push_back(run);

		editor->addActions(actions);
		
		if (!startingScriptPath.empty()) {
			std::ifstream inFile(startingScriptPath);
			if (inFile.is_open()) {
				std::stringstream strStream;
				strStream << inFile.rdbuf();
				QString text = QString::fromStdString(strStream.str());
				inFile.close();
				editor->setPlainText(text);
				runScript();
			}
		}
		else {
			editor->setPlainText("print \"Hello world\"\nresult = 5 * 4");
		}
	}

    return editor;
}

void PythonService::runScript()
{
    int input = 10;
    logic->getDict()["input"] = input;
	const std::string prescript =
		"def mdeimport(plugin, star = 0) :										 \n"
		"	importCommand = 'import {0}' if star == 0 else 'from {0} import *'	 \n"
		"	try :																 \n"
		"		exec(importCommand.format(plugin), globals())					 \n"
		"	except ImportError :												 \n"
		"		try :															 \n"
		"			exec(importCommand.format(plugin + 'd'), globals())			 \n"
		"		except ImportError :											 \n"
		"			print 'Unable to load : ', plugin;							 \n";
    QString out = QString::fromStdString(logic->run(prescript + editor->toPlainText().toStdString()));
	//output->setText(out);
	if (out.isEmpty()) {
		out = QString("Done running script (there was no output)\n");
	}
	console->append(out);
}

const bool PythonService::lateInit()
{
    return true;
}

void PythonService::finalize()
{
 
}

void PythonService::update( double deltaTime )
{
 
}

QWidgetList PythonService::getPropertiesWidgets()
{
    QWidgetList l;
	console->setObjectName(QObject::tr("Console"));
    l.push_back(console);
	return l;
}

