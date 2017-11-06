#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/CANopenSensor.h>
#include <imucostumelib/ProtocolReadBufferHelper.h>
#include <utils/Debug.h>
#include <vector>

using namespace imuCostume;

bool CostumeCANopenIO::send(const uint8_t seqNumber, const void * data,
	const uint16_t length, const uint16_t timeout, CostumeRawIO & costume)
{
	const auto totalLength = sizeof(Header) + length;
	utils::unique_ptr<uint8_t[]> dataToTransmit(new uint8_t[totalLength]);

	// nagłówek ramki - wymusza wysłanie reszty na CAN
	dataToTransmit[0] = 0x02;
	// mój unikalny identyfikator - dostanę go w potwierdzeniu
	dataToTransmit[1] = seqNumber;
	//przepisuje ramkę CANopen
	std::memcpy(dataToTransmit.get() + sizeof(Header), data, length);

	return costume.send(dataToTransmit.get(), totalLength, timeout);	
}

bool CostumeCANopenIO::verifyReceiveConfirmation(const uint8_t seqNumber,
	const void * data, const uint16_t length)
{
	const uint8_t * locFrame = static_cast<const uint8_t*>(data);

	if (length > 1 && locFrame[0] == (uint8_t)0x82 &&
		locFrame[1] == (uint8_t)seqNumber){
		return true;
	}

	return false;
}

CostumeCANopenIO::Data CostumeCANopenIO::extractData(const void * data, const uint16_t length)
{
	Data ret;
	ret.timestamp = 0;
	bool wasTime = false;

	auto rh = ProtocolReadBufferHelper::create(data, length);

	//filtrujemy ramki CANopen i grupujemy je wg czujnika
	for (unsigned int i = 0; i < rh.size(); ++i){		

		auto pd = rh[i];
		const auto cobID = pd.header().cobID();
		auto msgid = cobID.value;

		if (cobID.value == 0xFFF){
			wasTime = true;
			std::memcpy(&ret.timestamp, pd.data(), 4);
			
		} else
		{
			CANopenFrame frame = { 0 };
			frame.structure.cobID = cobID;
			auto length = pd.header().length();
			UTILS_ASSERT(length <= CANopenFrame::SizeLimits::MaxSize);
			std::memcpy(frame.structure.data.data(), pd.data(), length);
			ret.CANopenFrames.push_back(frame);
		}
	}

	if (wasTime == false){
		ret.CANopenFrames.resize(0);
	}

	return ret;
}

void CostumeCANopenIO::prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer)
{	
	CANopenFrame frame = { 0 };
	CANopenSensor::formatNMTFrame(0x01, 0x00);
	sendBuffer.add(frame, 4);	
}
