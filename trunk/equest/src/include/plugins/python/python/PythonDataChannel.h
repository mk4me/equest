/********************************************************************
	created:	2014/07/27
	created:	16:27
	filename: 	PythonDataChannel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PYTHONDATACHANNEL_H__
#define HEADER_GUARD_PYTHON__PYTHONDATACHANNEL_H__

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/python/python/Export.h>
#include <dataaccessorlib/Wrappers.h>

namespace python {
	template<class ChannelPtr>
	class PythonDataChannel
	{
	public:
		typedef ChannelPtr ptr_type;
		typedef typename ChannelPtr::element_type element_type;
		typedef typename element_type::value_type value_type;
		typedef typename element_type::argument_type argument_type;

	public:
		static PythonDataChannel convert(ChannelPtr channel)
		{
			UTILS_ASSERT(channel);
			PythonDataChannel<ChannelPtr> c;
			auto count = channel->size();
			
			for (auto i = 0; i < count; ++i) {
				c.data.push_back(channel->value(i));
			}
			
			auto adf = channel->feature<dataaccessor::DescriptorFeature>();
			if (adf != nullptr) {
				c.name = adf->name();
			}
			
			auto uaf = channel->getOrCreateFeature<dataaccessor::UniformArgumentsFeature>();
			if (uaf != nullptr) {
				c.frequency = 1.0 / uaf->argumentsInterval();
			}
			return c;
		}
		static ChannelPtr convert(const PythonDataChannel& obj)
		{
			auto name = obj.getName();
			
			if (name.empty() == true) {
				name = "Result";
			}
			
			auto wdata = obj.getData();
			const auto size = wdata.size();
			auto uaf = utils::make_shared<dataaccessor::UniformArgumentsFeature<argument_type>>(1.0 / obj.getFrequency());
			auto baf = utils::make_shared<dataaccessor::BoundedArgumentsFeature<argument_type>>(0.0, (size - 1) / obj.getFrequency());
			auto ff = dataaccessor::FunctionFeature::feature(true);
			
			utils::shared_ptr<dataaccessor::DescriptorFeature> df(dataaccessor::DescriptorFeature::create<value_type, argument_type>(name, "", ""));
			
			auto channel = dataaccessor::wrap(std::move(wdata), dataaccessor::UniformArgumentsGenerator<argument_type>(uaf->argumentsInterval(), size));
			channel->attachFeature(uaf);
			channel->attachFeature(baf);
			channel->attachFeature(ff);
			channel->attachFeature(df);
			return channel;
		}

	public:
		PythonDataChannel() : frequency(0.0f) {};
		virtual ~PythonDataChannel() {}

	public:
		std::vector<value_type> getData() const
		{
			return data;
		}
		
		float getFrequency() const
		{
			return frequency;
		}
		
		void setFrequency(float frq)
		{
			this->frequency = frq;
		}
		
		std::string getName() const
		{
			return name;
		}
		
		void setData(const std::vector<value_type>& data)
		{
			this->data = data;
		}
		
		void setName(const std::string& name)
		{
			this->name = name;
		}

	private:
		std::vector<value_type> data;
		float frequency;
		std::string name;
	};
	typedef PythonDataChannel<c3dlib::VectorChannelReaderInterfaceConstPtr> PythonVectorChannel;
	DEFINE_SMART_POINTERS(PythonVectorChannel);					  
	typedef PythonDataChannel<c3dlib::ScalarChannelReaderInterfaceConstPtr> PythonScalarChannel;
	DEFINE_SMART_POINTERS(PythonScalarChannel);
}

#endif // HEADER_GUARD 
