#include "SkiDataParser.h"
#include "SkiData.h"
#include <fstream>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace skidata;

static const unsigned char skipLines = 1;

SkiDataParser::SkiDataParser()
{

}

SkiDataParser::~SkiDataParser()
{

}

void SkiDataParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "Ski data format";
	expDesc.objectsTypes.push_back(typeid(SkiData));
	expressions.insert(Expressions::value_type(".*\\.csv$", expDesc));
}

plugin::IParser* SkiDataParser::create() const
{
	return new SkiDataParser();
}

void SkiDataParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(data);
}

void SkiDataParser::reset()
{
	data.reset();
}

static const std::regex r = std::regex("\;+");

static inline osg::Vec3 unpackVec3(const std::vector<std::string> & data, std::size_t idx)
{
	osg::Vec3 ret;
	ret[0] = boost::lexical_cast<float>(data[idx + 0]);
	ret[1] = boost::lexical_cast<float>(data[idx + 1]);
	ret[2] = boost::lexical_cast<float>(data[idx + 2]);
	return ret;
}

static inline ProcessedIMU unpackIMU(const std::vector<std::string> & data, std::size_t idx)
{
	ProcessedIMU ret;
	ret.orientation = unpackVec3(data, idx + 0);
	ret.accelerometer = unpackVec3(data, idx + 4) * 9.80665f / 1000.0f;
	ret.gyroscope = unpackVec3(data, idx + 7);
	ret.magnetometer = unpackVec3(data, idx + 10) / 10.0f;
	return ret;
}

static inline GPSData unpackGPS(const std::vector<std::string> & data, std::size_t idx)
{
	GPSData ret;
	ret.position.latitude = boost::lexical_cast<float>(data[idx + 0]);
	ret.position.longnitude = boost::lexical_cast<float>(data[idx + 1]);
	ret.position.height = boost::lexical_cast<float>(data[idx + 2]) / 1000.0;
	ret.speed.speed2D = boost::lexical_cast<float>(data[idx + 3]) / 10.0f;
	ret.speed.speed3D = boost::lexical_cast<float>(data[idx + 4]) / 10.0f;
	ret.position.heading = boost::lexical_cast<float>(data[idx + 5]);
	return ret;
}

static inline EnvironmentData unpackEnvironment(const std::vector<std::string> & data, std::size_t idx)
{
	EnvironmentData ret;
	ret.temperature = boost::lexical_cast<float>(data[idx + 0]);
	ret.preasure = boost::lexical_cast<float>(data[idx + 1]);	
	return ret;
}

void SkiDataParser::parse(const std::string & source)
{
	std::ifstream in(source);

	if (in.is_open() == true) {

		std::string str;
		{
			std::stringstream strStream;
			strStream << in.rdbuf();//read the file
			str = strStream.str();
		}
		
		std::stringstream strStream(str);


		SkiData localData;
		std::string line;
		for (auto i = skipLines; i > 0; --i) { std::getline(strStream, line); }
		while (std::getline(strStream, line)) {
			if (line.empty() == true) {
				throw std::runtime_error("Empty line in data");
			}

			boost::replace_all(line, ",", ".");

			std::sregex_token_iterator it(line.begin(), line.end(), r, -1);
			std::sregex_token_iterator end;
			std::vector<std::string> lineData;
			lineData.reserve(50);

			while (it != end) {
				lineData.push_back(*it);
				++it;
			}

			if (lineData.size() < 15) {
				throw std::runtime_error("Incomplete line data");
			}

			SkiDataFrame frame;
			frame.imu.chest = unpackIMU(lineData, 0);
			frame.environment = unpackEnvironment(lineData, 13);
			frame.gps = unpackGPS(lineData, 15);
			frame.imu.leftSki = unpackIMU(lineData, 21);
			frame.imu.rightSki = unpackIMU(lineData, 34);

			localData.push_back(frame);
		}

		this->data = utils::ObjectWrapper::wrap(utils::make_shared<SkiData>(std::move(localData)));
	}
}