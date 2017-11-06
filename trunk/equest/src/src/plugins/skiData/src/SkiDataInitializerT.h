/********************************************************************
	created:  2016/07/18
	filename: SkiDataInitializerT.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__SKIDATAINITIALIZERT_H__
#define __HEADER_GUARD_SKIDATA__SKIDATAINITIALIZERT_H__

#include <corelib/Variant.h>
#include <dataaccessorlib/Wrappers.h>

namespace skidata
{
	template<typename Extractor>
	class SkiDataInitializerT : public core::IVariantInitializer
	{
	public:

		SkiDataInitializerT(core::VariantConstPtr rawData,
			const dataaccessor::UniformArgumentsGenerator<float> & uag,
			const std::vector<dataaccessor::IFeaturePtr> & features)
			: rawData(rawData), uag(uag), features(features)
		{

		}

		virtual ~SkiDataInitializerT() {}

		//! \param object Obiekt który inicjujemy
		virtual void initialize(core::Variant * object) override
		{
			utils::shared_ptr<const SkiData> sd;
			if (rawData->tryGet(sd) == true) {								
				auto data = dataaccessor::wrap(sd, uag, Extractor());
				for (const auto & f : features) {
					data->attachFeature(f);
				}
				object->set(utils::ObjectWrapper::wrap(data));
			}
		}

		//! \return Kopia inicjalizatora
		virtual IVariantInitializer * clone() const override
		{
			return new SkiDataInitializerT<Extractor>(rawData, uag, features);
		}

	private:
		const core::VariantConstPtr rawData;
		const dataaccessor::UniformArgumentsGenerator<float> uag;
		const std::vector<dataaccessor::IFeaturePtr> features;
	};
}

#endif  // __HEADER_GUARD_SKIDATA__SKIDATAINITIALIZERT_H__