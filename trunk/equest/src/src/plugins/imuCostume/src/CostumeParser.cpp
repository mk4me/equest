#include "PCH.h"
#include "CostumeParser.h"
#include <boost/algorithm/hex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <vector>

using namespace IMU;

void CostumeParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "Costume data format";
	expDesc.objectsTypes.push_back(typeid(IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT));
	expressions.insert(Expressions::value_type(".*\\.cdata$", expDesc));
}

plugin::IParser* CostumeParser::create() const
{
	return new CostumeParser();
}

void CostumeParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(dataWrapper);
}

void CostumeParser::reset()
{
	dataWrapper.reset();
}

void CostumeParser::parse(const std::string& path)
{
	std::ifstream file(path);

	if (file.is_open() == true)
	{
		IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT data;

		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty() == false){				
				const unsigned int counter = boost::lexical_cast<unsigned int>(line);

				IIMUDataSource::CostumesDataFrame frame;

				for (unsigned int i = 0; i < counter; ++i)
				{
					if (std::getline(file, line)){
						std::vector<std::string> strs;
						boost::split(strs, line, boost::is_any_of(" "));
						if (strs.size() != 3){
							throw std::runtime_error("Wrong file format - expected 3 values");
						}
						else{
							IIMUDataSource::CostumeID id;
							id.ip = strs[0];
							id.port = boost::lexical_cast<unsigned int>(strs[1]);

							std::vector<uint8_t> rawData;
							boost::algorithm::unhex(strs[2].begin(), strs[2].end(), std::back_inserter(rawData));

							imuCostume::ProtocolSendBufferHelper::Buffer data;
							data.length = rawData.size();
							data.buffer.reset(new uint8_t[data.length], utils::array_deleter<uint8_t>());
							std::memcpy(data.buffer.get(), rawData.data(), data.length);

							//frame.insert(IIMUDataSource::CostumesDataFrame::value_type(id, data));
						}
					}
					else{
						throw std::runtime_error("Wrong file format - line expected");
					}
				}

				data.push_back(std::move(frame));
			}
			else{
				throw std::runtime_error("Wrong file format - line expected");
			}
		}

		dataWrapper = utils::ObjectWrapper::create<IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT>();

		dataWrapper->set(utils::make_shared<IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT>(std::move(data)));
	}	
}

static void serialize(std::ostream & stream, const osg::Vec3 & vec)
{
	stream << vec.x() << " " << vec.y() << " " << vec.z();
}

static void serialize(std::ostream & stream, const osg::Quat & orient)
{
	stream << orient.x() << " " << orient.y() << " " << orient.z() << " " << orient.w();
}

void CostumeParser::save(std::ostream & stream,
	const IIMUDataSource::CostumesDataFrame & data,
	const CostumesMappings & mapping)
{
	for (const auto & val : data)
	{
		auto it = mapping.find(val.first);

		if (it == mapping.end()){
			throw std::runtime_error("Costume not found in mappings");
		}

		stream << val.first.ip << " " << val.first.port << " " << val.second.timestamp << " ";
		serialize(stream, val.second.skeletonData[0].globalState.position);

		for (const auto & sData : val.second.sensorsData)
		{
			stream << " " << std::to_string(sData.first) << " ";
			serialize(stream, sData.second.accelerometer);
			stream << " ";
			serialize(stream, sData.second.gyroscope);
			stream << " ";
			serialize(stream, sData.second.magnetometer);
			stream << " ";
			serialize(stream, sData.second.orientation);
			stream << " ";
			serialize(stream, sData.second.estimatedOrientation);
			stream << " ";
			serialize(stream, val.second.skeletonData[it->second.find(sData.first)->second].localState.orientation);
		}
	}
}

void CostumeParser::save(std::ostream & stream,
	const IIMUDataSource::CostumesRecordingDataBuffer::ListT & data,
	const CostumesMappings & mapping,
	std::size_t idx)
{
	for (auto const & val : data)
	{
		stream << idx++ << " " << val.size() << std::endl;
		save(stream, val, mapping);
		stream << std::endl;
	}
}

void CostumeParser::save(const std::string & path,
	const IIMUDataSource::CostumesRecordingDataBuffer::ListT & data,
	const CostumesMappings & mapping)
{
	std::ofstream out(path);
	if (out.is_open() == true){
		save(out, data, mapping, 0);
	}

	out.flush();
	out.close();
}
