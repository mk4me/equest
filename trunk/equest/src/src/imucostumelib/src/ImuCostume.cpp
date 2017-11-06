#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/ProtocolReadBufferHelper.h>
#include <imucostumelib/CANopenSensor.h>

using namespace imuCostume;

Costume::SensorsConfiguration Costume::sensorsConfiguration(const void * data, const uint16_t length)
{
	Costume::SensorsConfiguration localConfiguration;

	ProtocolReadBufferHelper pd = ProtocolReadBufferHelper::create(data, length);
	for (decltype(pd.size()) j = 0; j < pd.size(); ++j){
		auto d = pd[j];

		const auto cobID = d.header().cobID();

		if (cobID.value != 0xFFF && cobID.value >= 0x500 && cobID.value < 0x580){
			uint8_t nodeID = (cobID.value - 0x500) / 4;
			localConfiguration[Costume::IMU].insert(nodeID);
		}
	}

	return localConfiguration;
}

std::list<Costume::SensorDataPtr> Costume::extractSensorsData(const std::list<CANopenFrame> & data)
{
	std::list<Costume::SensorDataPtr> ret;

	std::map<Costume::SensorID, std::list<const CANopenFrame*>> groupedData;

	//filtrujemy ramki CANopen i grupujemy je wg czujnika
	for (const auto & f : data){
		
		const auto cobID = f.structure.cobID;

		auto msgid = cobID.value;
		if (msgid >= 0x500 && msgid < 0x580)
		{
			const int imuID = (msgid - 0x500) / 4;
			//const int imuID = cobID.nodeID();
			groupedData[imuID].push_back(&f);
		}
	}

	for (auto & sd : groupedData){

		int dataStatus = 0;

		osg::Vec3 acc;
		osg::Vec3 mag;
		osg::Vec3 gyro;
		osg::Quat orient;

		for (auto it = sd.second.rbegin(); it != sd.second.rend(); ++it){

			const auto fd = **it;
			const int data_type = (fd.structure.cobID.value - 0x500) % 4;

			if (data_type == 0 && !(dataStatus & Costume::IMUSensor::ACC_DATA))
			{
				dataStatus |= Costume::IMUSensor::ACC_DATA;
				acc[0] = int16_t((uint16_t(fd.structure.data[1]) << 8) | uint16_t(fd.structure.data[0])) / 1024.0f;
				acc[1] = int16_t((uint16_t(fd.structure.data[3]) << 8) | uint16_t(fd.structure.data[2])) / 1024.0f;
				acc[2] = int16_t((uint16_t(fd.structure.data[5]) << 8) | uint16_t(fd.structure.data[4])) / 1024.0f;
			}
			else if (data_type == 1 && !(dataStatus & Costume::IMUSensor::MAG_DATA))
			{
				dataStatus |= Costume::IMUSensor::MAG_DATA;
				mag[0] = int16_t((uint16_t(fd.structure.data[1]) << 8) | uint16_t(fd.structure.data[0])) / 1024.0f;
				mag[1] = int16_t((uint16_t(fd.structure.data[3]) << 8) | uint16_t(fd.structure.data[2])) / 1024.0f;
				mag[2] = int16_t((uint16_t(fd.structure.data[5]) << 8) | uint16_t(fd.structure.data[4])) / 1024.0f;
			}
			else if (data_type == 2 && !(dataStatus & Costume::IMUSensor::GYRO_DATA))
			{
				dataStatus |= Costume::IMUSensor::GYRO_DATA;
				gyro[0] = int16_t((uint16_t(fd.structure.data[1]) << 8) | uint16_t(fd.structure.data[0])) / 2048.0f;
				gyro[1] = int16_t((uint16_t(fd.structure.data[3]) << 8) | uint16_t(fd.structure.data[2])) / 2048.0f;
				gyro[2] = int16_t((uint16_t(fd.structure.data[5]) << 8) | uint16_t(fd.structure.data[4])) / 2048.0f;
			}
			else if (data_type == 3 && !(dataStatus & Costume::IMUSensor::ORIENT_DATA))
			{
				dataStatus |= Costume::IMUSensor::ORIENT_DATA;
				orient[3] = float(int16_t((uint16_t(fd.structure.data[1]) << 8) | uint16_t(fd.structure.data[0])));
				orient[0] = float(int16_t((uint16_t(fd.structure.data[3]) << 8) | uint16_t(fd.structure.data[2])));
				orient[1] = float(int16_t((uint16_t(fd.structure.data[5]) << 8) | uint16_t(fd.structure.data[4])));
				orient[2] = float(int16_t((uint16_t(fd.structure.data[7]) << 8) | uint16_t(fd.structure.data[6])));
				orient /= orient.length();
			}

			if (dataStatus == 0x7){
				break;
			}
		}

		ret.push_back(utils::make_shared<Costume::IMUSensor>(sd.first, dataStatus, acc, mag, gyro, orient));
	}

	return ret;
}

std::list<Costume::SensorDataPtr> Costume::extractSensorsData(const void * buffer, const uint16_t length)
{
	std::list<Costume::SensorDataPtr> ret;

	std::map<Costume::SensorID, std::list<unsigned int>> groupedData;

	auto rh = ProtocolReadBufferHelper::create(buffer, length);

	//filtrujemy ramki CANopen i grupujemy je wg czujnika
	for (unsigned int i = 0; i < rh.size(); ++i){

		auto pd = rh[i];
		const auto cobID = pd.header().cobID();
		auto msgid = cobID.value;
		if (msgid >= 0x500 && msgid < 0x580)
		{
			//const int imuID = (msgid - 0x500) / 4;
			const int imuID = cobID.nodeID();
			groupedData[imuID].push_back(i);
		}
	}

	for (auto & sd : groupedData){

		int dataStatus = 0;

		osg::Vec3 acc;
		osg::Vec3 mag;
		osg::Vec3 gyro;
		osg::Quat orient;

		for (auto it = sd.second.rbegin(); it != sd.second.rend(); ++it){

			const auto fd = rh[*it];
			const int data_type = (fd.header().cobID().value - 0x500) % 4;

			if (data_type == 0 && !(dataStatus & Costume::IMUSensor::ACC_DATA))
			{
				dataStatus |= Costume::IMUSensor::ACC_DATA;
				acc[0] = int16_t((uint16_t(fd.data()[1]) << 8) | uint16_t(fd.data()[0])) / 1024.0f;
				acc[1] = int16_t((uint16_t(fd.data()[3]) << 8) | uint16_t(fd.data()[2])) / 1024.0f;
				acc[2] = int16_t((uint16_t(fd.data()[5]) << 8) | uint16_t(fd.data()[4])) / 1024.0f;
			}
			else if (data_type == 1 && !(dataStatus & Costume::IMUSensor::MAG_DATA))
			{
				dataStatus |= Costume::IMUSensor::MAG_DATA;
				mag[0] = int16_t((uint16_t(fd.data()[1]) << 8) | uint16_t(fd.data()[0])) / 1024.0f;
				mag[1] = int16_t((uint16_t(fd.data()[3]) << 8) | uint16_t(fd.data()[2])) / 1024.0f;
				mag[2] = int16_t((uint16_t(fd.data()[5]) << 8) | uint16_t(fd.data()[4])) / 1024.0f;
			}
			else if (data_type == 2 && !(dataStatus & Costume::IMUSensor::GYRO_DATA))
			{
				dataStatus |= Costume::IMUSensor::GYRO_DATA;
				gyro[0] = int16_t((uint16_t(fd.data()[1]) << 8) | uint16_t(fd.data()[0])) / 2048.0f;
				gyro[1] = int16_t((uint16_t(fd.data()[3]) << 8) | uint16_t(fd.data()[2])) / 2048.0f;
				gyro[2] = int16_t((uint16_t(fd.data()[5]) << 8) | uint16_t(fd.data()[4])) / 2048.0f;
			}
			else if (data_type == 3 && !(dataStatus & Costume::IMUSensor::ORIENT_DATA))
			{
				dataStatus |= Costume::IMUSensor::ORIENT_DATA;
				orient[0] = float((uint16_t(fd.data()[1]) << 8) | uint16_t(fd.data()[0]));
				orient[1] = float((uint16_t(fd.data()[3]) << 8) | uint16_t(fd.data()[2]));
				orient[2] = float((uint16_t(fd.data()[5]) << 8) | uint16_t(fd.data()[4]));
				orient[3] = float((uint16_t(fd.data()[7]) << 8) | uint16_t(fd.data()[6]));
			}

			if (dataStatus == 0x7){
				break;
			}
		}

		ret.push_back(utils::make_shared<Costume::IMUSensor>(sd.first, dataStatus, acc, mag, gyro, orient));
	}

	return ret;
}

Costume::Data Costume::convert(const CostumeCANopenIO::Data & data)
{
	Data ret;
	ret.timestamp = data.timestamp;
	ret.sensorsData = extractSensorsData(data.CANopenFrames);

	return ret;
}

void Costume::prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer)
{
	CANopenFrame disableHeartbeatFrame = CANopenSensor::formatExpeditedSDOWriteFrame(0, CANopenSensor::PRODUCED_HEARTBEAT_TIME, 0, (uint32_t)0);
	CANopenFrame storeParametersFrame = CANopenSensor::formatExpeditedSDOWriteFrame(0, CANopenSensor::STORE_PARAMETERS, CANopenSensor::STORE_PARAMETERS_SAVE_ALL, (uint32_t)0x65766173);
	ProtocolSendBufferHelper::updateFrameLength(disableHeartbeatFrame, 10);
	ProtocolSendBufferHelper::updateFrameLength(storeParametersFrame, 10);

	for (uint8_t i = 0; i < 127; ++i){		

		auto dhf = disableHeartbeatFrame;
		auto spf = storeParametersFrame;
		dhf.buffer[0] = (dhf.buffer[0] & 0x80 | i & 0x7F);
		spf.buffer[0] = (spf.buffer[0] & 0x80 | i & 0x7F);
		sendBuffer.add(dhf);
		sendBuffer.add(spf);		
	}
}

void Costume::prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer,
	const SensorsConfiguration & sensorsConfiguration)
{
	if (sensorsConfiguration.empty() == false){
		CANopenFrame disableHeartbeatFrame = CANopenSensor::formatExpeditedSDOWriteFrame(0, CANopenSensor::PRODUCED_HEARTBEAT_TIME, 0, (uint32_t)0);
		CANopenFrame storeParametersFrame = CANopenSensor::formatExpeditedSDOWriteFrame(0, CANopenSensor::STORE_PARAMETERS, CANopenSensor::STORE_PARAMETERS_SAVE_ALL, (uint32_t)0x65766173);
		ProtocolSendBufferHelper::updateFrameLength(disableHeartbeatFrame, 10);
		ProtocolSendBufferHelper::updateFrameLength(storeParametersFrame, 10);

		for (auto & cc : sensorsConfiguration){
			for (auto sid : cc.second){

				auto dhf = disableHeartbeatFrame;
				auto spf = storeParametersFrame;
				dhf.buffer[0] = (dhf.buffer[0] & 0x80 | sid & 0x7F);
				spf.buffer[0] = (spf.buffer[0] & 0x80 | sid & 0x7F);
				sendBuffer.add(dhf);
				sendBuffer.add(spf);
			}
		}
	}
}

Costume::GenericSensor::GenericSensor(const SensorID id, const int type)
	: id_(id), type_(type)
{

}

Costume::GenericSensor::~GenericSensor()
{

}

Costume::SensorID Costume::GenericSensor::id() const
{
	return id_;
}	

int Costume::GenericSensor::type() const
{
	return type_;
}

Costume::IMUSensor::IMUSensor(const SensorID id, const int status,
	const osg::Vec3 & accelerometer, const osg::Vec3 & magnetometer,
	const osg::Vec3 & gyroscope, const osg::Quat & orientation)
	: Costume::GenericSensor(id, Costume::IMU),	dataStatus_(status),
	accelerometer_(accelerometer), gyroscope_(gyroscope),
	magnetometer_(magnetometer), orientation_(orientation)
{

}

Costume::IMUSensor::~IMUSensor()
{

}

//! \return Dane akcelerometru 
const osg::Vec3 & Costume::IMUSensor::accelerometer() const
{
	return accelerometer_;
}

const osg::Vec3 & Costume::IMUSensor::magnetometer() const
{
	return magnetometer_;
}

const osg::Vec3 & Costume::IMUSensor::gyroscope() const
{
	return gyroscope_;
}

const osg::Quat & Costume::IMUSensor::orientation() const
{
	return orientation_;
}

int Costume::IMUSensor::dataStatus() const
{
	return dataStatus_;
}

Costume::INSOLESensor::INSOLESensor(const SensorID id, const INSOLESData & insolesData)
	: Costume::GenericSensor(id, Costume::INSOLE), insolesData_(insolesData)
{

}

Costume::INSOLESensor::~INSOLESensor()
{

}

const Costume::INSOLESensor::INSOLESData & Costume::INSOLESensor::insolesData() const
{
	return insolesData_;
}