/********************************************************************
	created:	2014/07/27
	created:	16:22
	filename: 	PythonConsole.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PythonConsole_H__
#define HEADER_GUARD_PYTHON__PythonConsole_H__

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include "utils/SmartPtr.h"
#include "PythonLogic.h"


namespace python {
	class PythonConsole : public QWidget 
	{	
		Q_OBJECT;
	public:
		PythonConsole(PythonLogicPtr logic, QWidget *parent = 0);
		void append(const QString& text);

	private Q_SLOTS:
		void runCommand();

	private:
		PythonLogic::object global;
		QTextEdit* output;
		QLineEdit* edit;
		PythonLogicPtr logic;
	};
	DEFINE_SMART_POINTERS(PythonConsole);
}

#endif // HEADER_GUARD 
