/********************************************************************
	created:  2015/01/22	09:03:26
	filename: CostumeParser.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__COSTUMEPARSER_H__
#define __HEADER_GUARD_IMU__COSTUMEPARSER_H__

#include <ostream>
#include <corelib/IParser.h>
#include <utils/SmartPtr.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <utils/ClonePolicies.h>

namespace IMU
{
	class CostumeParser : public plugin::ISourceParser
	{
		UNIQUE_ID("33178925-FC36-4260-B0EC-25F5268BE1F2")
			CLASS_DESCRIPTION("Costume data parser", "Costume data parser")

	public:

		using SensorToJointMapping = std::map < imuCostume::Costume::SensorID, kinematic::LinearizedSkeleton::NodeIDX > ;
		using CostumesMappings = std::map < imuCostume::CostumeRawIO::CostumeAddress, SensorToJointMapping > ;

	public:
		virtual ~CostumeParser() {}
		CostumeParser() {}

	public:

		template<typename SensorsDescriptions>
		static SensorToJointMapping createMapping(const SensorsDescriptions & sds,
			const kinematic::LinearizedSkeleton::GlobalMapping & lm)
		{
			SensorToJointMapping ret;
			auto rIT = ret.end();

			for (const auto & sd : sds)
			{
				auto it = lm.data().right.find(sd.second.jointName);
				if (it == lm.data().right.end()){
					throw std::runtime_error("Joint name not found in linearized skeleton bimapping");
				}

				rIT = ret.insert(rIT, { sd.first, it->get_left() });
			}

			return ret;
		}

		virtual void acceptedExpressions(Expressions & expressions) const override;
		virtual IParser* create() const override;
		virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const override;
		virtual void reset() override;
		virtual void parse(const std::string& path) override;

		static void save(std::ostream & stream,
			const IIMUDataSource::CostumesDataFrame & data,
			const CostumesMappings & mapping);

		static void save(std::ostream & stream,
			const IIMUDataSource::CostumesRecordingDataBuffer::ListT & data,
			const CostumesMappings & mapping,
			std::size_t idx);

		static void save(const std::string & path,
			const IIMUDataSource::CostumesRecordingDataBuffer::ListT & data,
			const CostumesMappings & mapping);

	private:
		utils::ObjectWrapperPtr dataWrapper;
	};
}

DEFINE_WRAPPER(IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT, PtrPolicyStd, ClonePolicyCopyConstructor);

#endif	// __HEADER_GUARD_IMU__COSTUMEPARSER_H__