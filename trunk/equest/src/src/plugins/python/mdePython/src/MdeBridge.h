/********************************************************************
	created:	2014/07/31
	created:	11:27
	filename: 	MdeBridge.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__MdeBridge_H__
#define HEADER_GUARD_PYTHON__MdeBridge_H__

#include <plugins/python/python/PythonDataChannel.h>
#include <corelib/ISourceManager.h>
#include <corelib/IVisualizerManager.h>
#include <corelib/IDataManager.h>
#include <corelib/IStreamDataManager.h>
#include <corelib/IFileDataManager.h>
#include "plugins/python/python/PythonPluginUtils.h"
#include "corelib/HierarchyHelper.h"

namespace python {

	typedef std::pair<std::string, std::string> StringPair;
	typedef std::vector<StringPair> DataList;


	struct Helper
	{
		Helper() {}
		Helper(core::HierarchyHelperConstWeakPtr helper, core::IVisualizerManager* vm) : helper(helper), manager(vm) {}
		std::string name();
		void createVisualizer();

		bool operator==(const Helper& h)
		{
			return this->helper.lock() == h.helper.lock();
		}

		core::IVisualizerManager* manager;
		core::HierarchyHelperConstWeakPtr helper;
	};

	//! Klasa ³¹czy MDE z Pythonem. Zapewnia wymianê danych miêdzy tymi œrodowiskami
	class MdeBridge:  public PythonPluginUtils
	{
	public:
		virtual ~MdeBridge() {}
		
	public:
		//! 
		//! \param sessionDesc
		//! \param dataDesc
		//! \return 
		PythonVectorChannel getVectorChannel(const std::string& sessionDesc, const std::string& dataDesc);
		PythonScalarChannel getScalarChannel(const std::string& sessionDesc, const std::string& dataDesc);

		template <class PythonDataChannel>
		PythonDataChannel getDataChannel(const std::string& sessionDesc, const std::string& dataDesc)
		{
			auto transaction = getDataManagerReader()->transaction();
			core::ConstVariantsList objects;
			transaction->getObjects(objects, typeid(PythonDataChannel::element_type), false);

			PythonDataChannel::ptr_type channel;
			for (auto& var : objects) {
				std::string source; var->getMetadata("core/source", source);
				std::string name; var->getMetadata("core/name", name);
				if (source.find(sessionDesc) != std::string::npos && name.find(dataDesc) != std::string::npos) {
					channel = var->get();
					break;
				}
			}

			if (channel) {
				return PythonDataChannel::convert(channel);
			} else {
				throw std::runtime_error("No data");
			}
		}

		//! 
		//! \param channel
		//! \return 
		void addVectorChannel(const PythonVectorChannel& channel);
		void addScalarChannel(const PythonScalarChannel& channel);


		//! 
		//! \return 
		DataList listLoadedVectors();
		DataList listLoadedScalars();
		template <class T>
		DataList listLoadedData()
		{
			python::DataList dataList;
			auto transaction = getDataManagerReader()->transaction();
			core::ConstVariantsList objects;
			transaction->getObjects(objects, typeid(T), false);
			std::string source;
			std::string name;
			for (auto& var : objects) {
				var->getMetadata("core/source", source);
				var->getMetadata("core/name", name);
				dataList.push_back(std::make_pair(source, name));

			}
			return dataList;
		}

		PythonVectorChannel createVectorChannel();
		PythonScalarChannel createScalarChannel();


		std::vector<Helper> getHelpers();

		void addFile(const std::string& file);

		void close(int errorCode = 0);

		void log(const std::string& txt);

	};
	DEFINE_SMART_POINTERS(MdeBridge);
}

#endif // HEADER_GUARD 
