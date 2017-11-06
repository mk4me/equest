#include "PythonPCH.h"
#include "PythonPCH.h"
#include "PythonConsole.h"
#include <QtWidgets/QScrollBar>

python::PythonConsole::PythonConsole(python::PythonLogicPtr logic, QWidget *parent /*= 0*/) : 
	QWidget(parent),
	logic(logic)
{
	output = new QTextEdit();
	edit = new QLineEdit();

	QString style = "background-color: black; 	color: grey;";

	output->setStyleSheet(style);
	output->setReadOnly(true);
	edit->setStyleSheet(style);
	edit->setPlaceholderText(">>> ");
	connect(edit, SIGNAL(returnPressed()), this, SLOT(runCommand()));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(output);
	layout->addWidget(edit);

	this->setLayout(layout);

	global = logic->createGlobal();
}

void python::PythonConsole::runCommand()
{
	std::string script = edit->text().toStdString();
	auto out = logic->runStatement(script, global);
	global = out.second;
	append(edit->text() + "\n" + QString::fromStdString(out.first));
	edit->setText(QString());
}

void python::PythonConsole::append(const QString& text)
{
	output->setText(output->toPlainText() + "\n" + text);
	QScrollBar* vScrollBar = output->verticalScrollBar();
	vScrollBar->triggerAction(QScrollBar::SliderToMaximum);
}
