
/********************************************************************
	created:	2014/07/27
	created:	16:28
	filename: 	PythonProcessingBlock.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PythonProcessingBlock_H__
#define HEADER_GUARD_PYTHON__PythonProcessingBlock_H__


#include <plugins/newVdf/UniversalInputPin.h>
#include <plugins/newVdf/UniversalOutputPin.h>
#include <plugins/dfElements/DFPins.h>
#include <dflib/Node.h>
#include <dflib/IDFNode.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <QtWidgets/QTextEdit>
#include "boost/python/object_core.hpp"

namespace python {

	class PythonProcessingBlock : public df::ProcessingNode, public df::IDFProcessor, public vdf::INodeConfiguration
	{
	public:
		PythonProcessingBlock();

	public:
		virtual void reset();
		virtual void process();

		std::vector<float> extractFloatVector(const boost::python::object& obj, const char* filed);

		virtual QWidget* getConfigurationWidget();

	private:
		VectorInputPin* inPinA;
		VectorInputPin* inPinB;
		VectorOutputPin* outPinA;
		std::string name;
		QTextEdit* editor;
	};
	DEFINE_SMART_POINTERS(PythonProcessingBlock);
}

#endif // HEADER_GUARD 
