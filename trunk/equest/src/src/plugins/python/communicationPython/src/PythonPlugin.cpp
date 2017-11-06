#include <corelib/IPlugin.h>
#include <corelib/BaseDataTypes.h>
#include "CommunicationPython.h"
#include "boost/python/class.hpp"
#include "boost/python/scope.hpp"
#include "boost/python/return_value_policy.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"

namespace py = boost::python;

MDE_PYTHON_MODULE(plugin_communicationPython)
{
	{
		typedef python::CommunicationPython::ContextConf Conf;
	
#define add_qstring_property(name, dest) add_property(name, \
			boost::python::make_getter(dest, py::return_value_policy<py::return_by_value>()),\
			boost::python::make_setter(dest))


		typedef hmdbCommunication::IHMDBSourceViewManager::StorageConfiguration StC;
		typedef hmdbCommunication::IHMDBSourceViewManager::RemoteConfiguration RC;
		typedef hmdbCommunication::IHMDBSourceViewManager::ServiceConfiguration SeC;
		typedef hmdbCommunication::IHMDBSourceViewManager::UserCredentialsConfiguration UC;

		py::class_<SeC>("ServiceConfiguration")
			.add_qstring_property("url", &SeC::url)
			.add_qstring_property("caPath", &SeC::caPath);
		py::class_<UC>("UserCredentialsConfiguration")
			.add_qstring_property("user", &UC::user)
			.add_qstring_property("password", &UC::password);
		py::class_<StC>("StorageConfiguration")
			.add_qstring_property("path", &StC::path)
			.add_qstring_property("password", &StC::password);
		py::class_<RC>("RemoteConfiguration")
			.add_property("serviceConfiguration", &RC::serviceConfiguration)
			.add_property("userConfiguration", &RC::userConfiguration);

		py::class_<Conf>("ContextConfiguration")
			.add_qstring_property("name", &Conf::name)
			.add_property("storageConfiguration", &Conf::storageConfiguration)
			.add_property("motionServicesConfiguration", &Conf::motionServicesConfiguration)
			.add_property("motionDataConfiguration", &Conf::motionDataConfiguration)
			.add_property("medicalServicesConfiguration", &Conf::medicalServicesConfiguration)
			.add_property("medicalDataConfiguration", &Conf::medicalDataConfiguration);
	}

	py::class_<std::vector<std::string>>("vectorOfStrings")
		.def(py::vector_indexing_suite<std::vector<std::string>>());

	py::class_<python::ShallowContext>("ShallowContext")
		.def("loadTrial", &python::ShallowContext::loadTrial)
		.def("listTrialsToLoad", &python::ShallowContext::listTrialsToLoad);

	py::class_<python::CommunicationPython>("CommunicationPython")
		.def("getContext", &python::CommunicationPython::getShallowContext)
		.def("login", &python::CommunicationPython::login)
		.def("loadConfig", &python::CommunicationPython::loadConf)
		.def("getCurrentConfig", &python::CommunicationPython::getCurrentConf)
		.def("shallowContextsCount", &python::CommunicationPython::shallowContextsCount);
}