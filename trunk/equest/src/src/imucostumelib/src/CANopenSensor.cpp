#include <imucostumelib/CANopenSensor.h>
#include <utils/Utils.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <boost/detail/endian.hpp>
#include <imucostumelib/ProtocolReadBufferHelper.h>
#include <imucostumelib/CANopenFrame.h>

using namespace imuCostume;

CANopenFrame CANopenSensor::formatNMTFrame(const uint8_t messageID, const uint8_t nodeID)
{
	CANopenFrame ret = { 0 };
	ret.structure.data[0] = messageID;
	ret.structure.data[1] = nodeID;

	return ret;
}

CANopenSensor::ODataSize CANopenSensor::dataSize(const std::size_t size)
{
	if (size & 0x3){
		throw std::runtime_error("Invalid data size");
	}
	else if (size > 4){
		throw std::runtime_error("Data size limit exceeded");
	}
	else if (size == 4){
		return OSize4B;
	}
	else if (size == 2){
		return OSize2B;
	}
	else{
		return OSize1B;
	}
}

std::size_t CANopenSensor::dataBytes(const ODataSize size)
{
	std::size_t ret = 0;
	switch (size)
	{
	case OSize1B:
		ret = 1;
		break;
	case OSize2B:
		ret = 2;
		break;
	case OSize4B:
		ret = 4;
		break;
	}

	return ret;
}

CANopenFrame CANopenSensor::formatExpeditedSDOWriteFrame(const uint8_t nodeID, const uint16_t dictID, const uint8_t dictSubID, const uint32_t value, const ODataSize dataSize)
{
	uint32_t v = utils::EndianSwap(value);

	CANopenFrame ret = { 0 };
	ret.buffer[0] = nodeID;
	ret.buffer[1] = 0x06;
	ret.structure.data[0] = (uint8_t)dataSize;
	ret.structure.data[1] = (dictID & 0xFF);
	ret.structure.data[2] = (dictID >> 8);
	ret.structure.data[3] = dictSubID;
	std::memcpy(ret.structure.data.data() + 4, &v, dataBytes(dataSize));

	return ret;
}

CANopenFrame CANopenSensor::formatExpeditedSDOReadFrame(const uint8_t nodeID, const uint16_t dictID, const uint8_t dictSubID)
{
	CANopenFrame ret = { 0 };
	ret.buffer[0] = nodeID;
	ret.buffer[1] = 0x06;
	ret.structure.data[0] = 0x40;
	ret.structure.data[1] = (dictID & 0xFF);
	ret.structure.data[2] = (dictID >> 8);
	ret.structure.data[3] = dictSubID;

	return ret;
}