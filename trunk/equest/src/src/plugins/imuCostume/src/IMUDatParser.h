/********************************************************************
	created:	2014/06/20
	created:	20:6:2014   11:10
	filename: 	IMUDatParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMU__IMUDATPARSER_H__
#define HEADER_GUARD_IMU__IMUDATPARSER_H__

#include <cstdint> //TODO nie wiem czy to na linuxie przejdzie, jesli nie to trzeba zrobic ifdefa na stdint.h
#include <vector>
#include <utils/SmartPtr.h>
#include <utils/Filesystem.h>
#include "IMUCFGParser.h"

namespace IMU {

    struct ImuRawData {
        int16_t acc_x;
        int16_t acc_y;
        int16_t acc_z;
        int16_t rate_x;
        int16_t rate_y;
        int16_t rate_z;
        int16_t mag_x;
        int16_t mag_y;
        int16_t mag_z;
    };

    struct ImuData {
        ImuRawData raw;
        int16_t qx;
        int16_t qy;
        int16_t qz;
        int16_t qw;
    };

    typedef  std::vector<ImuData> Frame;
    
    typedef std::vector<Frame> Frames;
	DEFINE_SMART_POINTERS(Frames);
    
    class IMUDatParser
    {
    public:
        virtual ~IMUDatParser() {}
		static Frames parse(const utils::Filesystem::Path& path, int imusCount);
		static std::pair<Frames,int> parse(const utils::Filesystem::Path& path);
        static void save(const utils::Filesystem::Path& path, const Frames& frames);
    };
    DEFINE_SMART_POINTERS(IMUDatParser);

	class IMUDatParserCore : public plugin::ISourceParser
	{
		UNIQUE_ID("33179925-FC36-4260-B0EC-25F5268BE1F2")
		CLASS_DESCRIPTION("IMU dat parser", "IMU dat parser")
	public:
		virtual ~IMUDatParserCore() {}

	public:
		virtual void acceptedExpressions(Expressions & expressions) const;
		virtual IParser* create() const;
		virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
		virtual void reset();
		virtual void parse(const std::string& path);

	private:
		utils::ObjectWrapperPtr datWrapper;
	};

    class IMUDatSplitter
    {
    public:
        static std::vector<Frames> split(const Frames& frames, const std::vector<std::pair<int,int>>& splits);
    };

}

DEFINE_WRAPPER(IMU::Frames, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
#endif
