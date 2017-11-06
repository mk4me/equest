#include "PythonProcessingBlock.h"
#include <plugins/python/python/PythonDataChannel.h>
#include "boost/python/import.hpp"
#include <plugins/python/python/PythonEditor.h>
#include "boost/python/exec.hpp"
#include <dataaccessorlib/Wrappers.h>

namespace py = boost::python;

python::PythonProcessingBlock::PythonProcessingBlock()
{
	inPinA = new VectorInputPin(this);
	outPinA = new VectorOutputPin(this);
	addInputPin(inPinA);
	addOutputPin(outPinA);

	editor = PythonEditor::createPythonEditor();
	QString script = 
		"result_x = [x for x in input_x]\n"
		"result_y = [y for y in input_y]\n"
		"result_z = [z for z in input_z]\n";
	editor->setPlainText(script);
}

void python::PythonProcessingBlock::reset()
{

}

void python::PythonProcessingBlock::process()
{
	c3dlib::VectorChannelReaderInterfaceConstPtr signal1 = inPinA->getValue();

	if (signal1) {
		size_type count = signal1->size();

		std::vector<float> vecX, vecY, vecZ;
		for (size_type i = 0; i < count; ++i) {
			vecX.push_back(signal1->value(i).x());
			vecY.push_back(signal1->value(i).y());
			vecZ.push_back(signal1->value(i).z());
		}
		boost::python::class_<std::vector<float> >("PyVec")
			.def(boost::python::vector_indexing_suite<std::vector<float> >());

		auto mainModule = py::import("__main__");
		auto globs = mainModule.attr("__dict__");
		globs["input_x"] = vecX;
		globs["input_y"] = vecY;
		globs["input_z"] = vecZ;
		py::str script = editor->toPlainText().toStdString().c_str();
		
		try {
			exec(script, globs);
		} catch (...) {
			PyErr_Print();
			throw;
		}
		try {
			vecX = extractFloatVector(globs, "result_x");
			vecY = extractFloatVector(globs, "result_y");
			vecZ = extractFloatVector(globs, "result_z");
		} catch (...) {
			PyErr_Print();
			throw;
		}

		count = vecX.size();
		if (count != vecY.size() || count != vecZ.size()) {
			throw std::runtime_error("Wrong output list size");
		}

		auto uaf = signal1->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
		auto baf = signal1->getOrCreateFeature<dataaccessor::BoundedArgumentsFeature>();
		auto ff = signal1->getOrCreateFeature<dataaccessor::FunctionFeature>();

		std::vector<c3dlib::VectorChannelReaderInterface::value_type> data;
		data.reserve(count);

		auto adf = signal1->feature<dataaccessor::DescriptorFeature>();
		utils::shared_ptr<dataaccessor::DescriptorFeature> df;
		if (adf != nullptr) {
			df = utils::make_shared<dataaccessor::DescriptorFeature>(adf->name() + " - Result", adf->valueType(),
				adf->valueUnit(), adf->argumentType(), adf->argumentUnit());

		}
		else {
			df.reset(dataaccessor::DescriptorFeature::create<c3dlib::VectorChannelReaderInterface::value_type,
				c3dlib::VectorChannelReaderInterface::argument_type>("Result", "", ""));
		}

		for (int i = 0; i < count; ++i) {
			data.push_back(osg::Vec3(vecX[i], vecY[i], vecZ[i]));
		}

		auto rc = dataaccessor::wrap(std::move(data), dataaccessor::UniformArgumentsGenerator<c3dlib::VectorChannelReaderInterface::argument_type>(uaf->argumentsInterval(), signal1->size()));
		rc->attachFeature(uaf);
		rc->attachFeature(baf);
		rc->attachFeature(ff);
		rc->attachFeature(df);

		outPinA->setValue(rc);
	}
	else {

	}
}

QWidget* python::PythonProcessingBlock::getConfigurationWidget()
{
	return editor;
}

std::vector<float> python::PythonProcessingBlock::extractFloatVector(const boost::python::object& obj, const char* filed)
{
	std::vector<float> res;
	boost::python::list lst = py::extract<boost::python::list>(obj[filed]);
	int count = boost::python::len(lst);
	for (int i = 0; i < count; ++i)	{
		res.push_back(py::extract<float>(lst[i]));
	}
	return res;
}
