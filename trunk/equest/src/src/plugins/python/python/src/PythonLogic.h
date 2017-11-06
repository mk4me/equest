/********************************************************************
	created:	2012/12/05
	created:	5:12:2012   10:20
	filename: 	PythonLogic.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PYTHONLOGIC_H__
#define HEADER_GUARD_PYTHON__PYTHONLOGIC_H__

#include <boost/python.hpp>
#include <map>
#include <utils/SmartPtr.h>
#include <boost/circular_buffer.hpp>

namespace python {

	//! Klasa dziêki której realizowane jest przekierowanie wyjœcia z pythona.
	//! kod z http://onegazhang.wordpress.com/2008/09/30/redirect-python-stdoutstderr-via-boost-python/
	class PythonStdIoRedirect {
	public:
		typedef boost::circular_buffer<std::string> ContainerType;
		void Write(std::string const& str);
		static std::string GetOutput();
	private:
		static ContainerType m_outputs; // must be static, otherwise output is missing
	};

	//! Klasa zarz¹dza pythonem z poziomu c++
	class PythonLogic
	{
	public:
		PythonLogic(const std::string& pythonPath);
		virtual ~PythonLogic();
		typedef boost::python::object object;

	public:
		std::string run(const std::string& script);
		std::pair<std::string, object> runStatement(const std::string& script, object global = object(), object local = object());
		object& getDict();
		object createGlobal();

	private:
		boost::python::object mainModule;
		boost::python::object mainNamespace;

		PythonStdIoRedirect python_stdio_redirector;
	};
	typedef utils::shared_ptr<PythonLogic> PythonLogicPtr;
	typedef utils::shared_ptr<const PythonLogic> PythonLogicConstPtr;

	
}

#endif
