/********************************************************************
	created:	2014/08/06
	created:	9:33
	filename: 	PythonEditor.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PythonEditor_H__
#define HEADER_GUARD_PYTHON__PythonEditor_H__

#include <QtWidgets/QTextEdit>
#include <plugins/python/python/Export.h>

namespace python {
	class PLUGIN_PYTHON_EXPORT PythonEditor : public QTextEdit
	{
		Q_OBJECT;
	public:
		static QTextEdit* createPythonEditor();
		virtual ~PythonEditor() {}

	private Q_SLOTS:
		void fileNew();
		void save();
		void saveAs();
		void open();

	private:
		QString filename;
	};
}

#endif // HEADER_GUARD 
