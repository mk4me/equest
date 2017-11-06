#include "PythonPCH.h"
#include "plugins/python/python/PythonEditor.h"
#include "QtWidgets/QFileDialog"
#include "PythonHighlighter.h"
#include <QtWidgets/QAction>
#include <QtCore/QTextStream>
#include "QtWidgets/QMessageBox"

#define signals Q_SIGNALS
#define slots Q_SLOTS

#include "coreui/CoreAction.h"

using namespace python;

QTextEdit* python::PythonEditor::createPythonEditor()
{
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);

	PythonEditor* editor = new PythonEditor;
	editor->setFont(font);

	QSyntaxHighlighter* highlighter = new PythonHighlighter(editor->document());
	highlighter->setParent(editor);
	//editor->setMinimumSize(600, 600);
	editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	editor->setStyleSheet("border: 1px solid red");

	const QString rsrcPath = ":/coreUI/icons/textedit";
	QList<QAction*> actions;


	QAction* undo = new coreUI::CoreAction(tr("Edit"), QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png")), tr("&Undo"), editor, coreUI::CoreTitleBar::Left);
	undo->setShortcut(QKeySequence::Undo);
	connect(undo, SIGNAL(triggered()), editor, SLOT(undo()));
	actions.push_back(undo);

	QAction* redo = new coreUI::CoreAction(tr("Edit"), QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png")), tr("&Redo"), editor);
	redo->setShortcut(QKeySequence::Redo);
	connect(redo, SIGNAL(triggered()), editor, SLOT(redo()));
	actions.push_back(redo);

	QIcon newIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/filenew.png"));
	QAction* aNew = new coreUI::CoreAction(tr("File"), newIcon, tr("&New"), editor);
	aNew->setShortcut(QKeySequence::New);
	connect(aNew, SIGNAL(triggered()), editor, SLOT(fileNew()));

	QAction* save = new coreUI::CoreAction(tr("File"), QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png")), tr("&Save"), editor);
	save->setShortcut(QKeySequence::Save);
	connect(save, SIGNAL(triggered()), editor, SLOT(save()));
	actions.push_back(save);

	QAction* saveAs = new coreUI::CoreAction(tr("File"), QIcon(rsrcPath + "/filesave.png"), tr("Save &As..."), editor);
	connect(saveAs, SIGNAL(triggered()), editor, SLOT(saveAs()));
	actions.push_back(saveAs);

	QAction* open = new coreUI::CoreAction(tr("File"), QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png")),
		tr("&Open..."), editor);
	open->setShortcut(QKeySequence::Open);
	connect(open, SIGNAL(triggered()), editor, SLOT(open()));
	actions.push_back(open);
	editor->addActions(actions);

	return editor;
}

void python::PythonEditor::save()
{
	if (filename.isEmpty()) {
		saveAs();
	} else {
		QFile outfile;
		outfile.setFileName(filename);
		outfile.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream out(&outfile);
		out << toPlainText() << endl;
	}
}

void python::PythonEditor::saveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
		QString(),
		tr("Python scripts (*.py *.txt )"));

	if (!filename.isEmpty()) {
		this->filename = filename;
		save();
	}
}

void python::PythonEditor::open()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
		QString(),
		tr("Python scripts (*.py *.txt )"));

	if (!filename.isEmpty()) {

		try {
			QFile inFile;
			inFile.setFileName(filename);
			inFile.open(QIODevice::ReadOnly | QIODevice::Text);
			QTextStream in(&inFile);
			this->setPlainText(in.readAll());
		}	catch (...) {
			QMessageBox::critical(this, tr("Error"), tr("Unable to load file %1").arg(filename));
			throw;
		}
		this->filename = filename;
	}
}

void python::PythonEditor::fileNew()
{
	 QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Question"), tr("Are you sure? Allchanges will be lost."),	QMessageBox::Yes | QMessageBox::No);
	 if (reply == QMessageBox::Yes) {
		 setPlainText(QString());
		 filename = QString();
	 }
}
