
/********************************************************************
	created:	2014/06/20
	created:	20:6:2014   11:10
	filename: 	IMUCFGParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMU__IMUCFGPARSER_H__
#define HEADER_GUARD_IMU__IMUCFGPARSER_H__

#include <utils/SmartPtr.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <vector>
#include <osg/Quat>
#include "corelib/IParser.h"

namespace IMU {
    struct IMUConfig {
		IMUConfig() : imusCount(-1), imuID(-1), param1(-1), param2(-1) {}
        struct Joint {
            std::vector<osg::Quat> offsets;
            std::string name;
            int idx;
            int parentIdx;
        };
        int imusCount;
        int imuID;
        int param1;
        int param2;
        std::vector<Joint> joints;
    };
	DEFINE_SMART_POINTERS(IMUConfig);

	class IMUCFGParser : public plugin::ISourceParser
	{
		UNIQUE_ID("340EDB58-26D3-4FE1-943F-5817E9495211")
		CLASS_DESCRIPTION("IMU cfg parser", "IMU cfg parser")
    public:
        virtual ~IMUCFGParser() {}

	public:
		virtual void acceptedExpressions(Expressions & expressions) const;
		virtual IParser* create() const;
		virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
		virtual void reset();
		virtual void parse(const std::string& path);

	public:
		static IMUConfigPtr rawParse(const std::string& path);

	private:
		utils::ObjectWrapperPtr cfgWrapper;
    }; 

}
DEFINE_WRAPPER(IMU::IMUConfig, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);

#endif
