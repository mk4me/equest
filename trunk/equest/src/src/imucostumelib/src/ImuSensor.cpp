#include <imucostumelib/ImuSensor.h>

using namespace imuCostume;

IMUSensor::IMUSensor(CANopenSensor * sensor)
	: sensor_(sensor)
{
}

IMUSensor::~IMUSensor()
{
}

CANopenSensor * IMUSensor::sensor() const
{
	return sensor_;
}