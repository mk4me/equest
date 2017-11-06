#include "PythonPCH.h"
#include "PythonLogic.h"
#include <QtWidgets/QMessageBox>
#include <boost/python.hpp>
#include <osg/Vec3>
#include "loglib/Exceptions.h"
#include "plugins/python/python/PythonPluginUtils.h"


using namespace boost::python;


//////////////////////////////////////////////////////////////////////////
// zaczerpniete z : https://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/

struct QString_to_python_str
{
	static PyObject* convert(QString const& s)
	{
		return boost::python::incref(
			boost::python::object(
			s.toLatin1().constData()).ptr());
	}
};

struct QString_from_python_str
{
	QString_from_python_str()
	{
		boost::python::converter::registry::push_back(
			&convertible,
			&construct,
			boost::python::type_id<QString>());
	}

	// Determine if obj_ptr can be converted in a QString
	static void* convertible(PyObject* obj_ptr)
	{
		if (!PyString_Check(obj_ptr)) return 0;
		return obj_ptr;
	}

	// Convert obj_ptr into a QString
	static void construct(
		PyObject* obj_ptr,
		boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		// Extract the character data from the python string
		const char* value = PyString_AsString(obj_ptr);

		// Verify that obj_ptr is a string (should be ensured by convertible())
		assert(value);

		// Grab pointer to memory into which to construct the new QString
		void* storage = (
			(boost::python::converter::rvalue_from_python_storage<QString>*)
			data)->storage.bytes;

		// in-place construct the new QString using the character data
		// extraced from the python object
		new (storage)QString(value);

		// Stash the memory chunk pointer for later use by boost.python
		data->convertible = storage;
	}
};

void initializeConverters()
{
	using namespace boost::python;

	// register the to-python converter
	to_python_converter <
		QString,
		QString_to_python_str > ();

	// register the from-python converter
	QString_from_python_str();
}

// zaczerpniete z : https://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/
//////////////////////////////////////////////////////////////////////////


python::PythonStdIoRedirect::ContainerType python::PythonStdIoRedirect::m_outputs;

python::PythonLogic::PythonLogic(const std::string& pythonPath)
{
	if (!pythonPath.empty()) {
		Py_SetPythonHome(const_cast<char*>(pythonPath.c_str()));
	}
	char* home = Py_GetPythonHome();
	if (home == nullptr) {
		throw loglib::runtime_error("No python directory specified");
	}
	PLUGIN_LOG_INFO("Python home: " << Py_GetPythonHome());
	
	PLUGIN_LOG_INFO("Initializing python");
    Py_Initialize();

    mainModule = import("__main__");
	mainNamespace = createGlobal();
	mainNamespace["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
		.def("write", &PythonStdIoRedirect::Write);
	boost::python::import("sys").attr("stderr") = python_stdio_redirector;
	boost::python::import("sys").attr("stdout") = python_stdio_redirector;
	
	initializeConverters();

	

	//mainNamespace["mde"] = ptr(bridge.get());
	PLUGIN_LOG_INFO("Python started");
}

python::PythonLogic::~PythonLogic()
{
    Py_Finalize();
}




std::string python::PythonLogic::run(const std::string& script)
{
	str code(script.c_str());
    try {
		//PyRun_SimpleString("sys.stderr = cStringIO.StringIO()");
		
        exec(code, mainNamespace);
       
	}
	catch (std::exception& e) {
		
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
	} catch (...) {
        PyErr_Print();
        //PyErr_Clear();
    }

	return python_stdio_redirector.GetOutput();
}

boost::python::object& python::PythonLogic::getDict()
{
    return mainNamespace;
}

std::pair<std::string, python::PythonLogic::object> python::PythonLogic::runStatement(const std::string& script, object global /*= object()*/, object local /*= object()*/)
{
	str code(script.c_str());

	//global["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
	//	.def("write", &PythonStdIoRedirect::Write);
	object ret;
	try {
		//ret = exec_statement(code, global, local);
		PyRun_SimpleString(script.c_str());
	} catch (...) {
		PyErr_Print();
	}
	return std::make_pair(python_stdio_redirector.GetOutput(), ret);
}

python::PythonLogic::object python::PythonLogic::createGlobal()
{
	boost::python::object global = mainModule.attr("__dict__");
	//global["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
	//	.def("write", &PythonStdIoRedirect::Write);
	return global;
}



void python::PythonStdIoRedirect::Write(std::string const& str)
{
	if (m_outputs.capacity() < 100)
		m_outputs.resize(100);
	m_outputs.push_back(str);
}

std::string python::PythonStdIoRedirect::GetOutput()
{
	std::string ret;
	std::stringstream ss;
	for (boost::circular_buffer<std::string>::const_iterator it = m_outputs.begin();
		it != m_outputs.end();
		++it)
	{
		ss << *it;
	}
	m_outputs.clear();
	ret = ss.str();
	return ret;
}
