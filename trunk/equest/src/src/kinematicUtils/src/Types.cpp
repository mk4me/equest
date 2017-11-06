#include <kinematicUtils/Types.h>
#include <ctype.h>
#include <algorithm>
#include <utils/StringConversionTools.h>

using namespace kinematicUtils;

std::string ChannelType::toString(const Type type)
{
	std::string ret = "TX";

	switch (type)
	{
	case TX:
		ret = "TX";
		break;
	case TY:
		ret = "TY";
		break;
	case TZ:
		ret = "TZ";
		break;
	case RX:
		ret = "RX";
		break;
	case RY:
		ret = "RY";
		break;
	case RZ:
		ret = "RZ";
		break;
	default:
		ret = "Custom channel: " + utils::Convert::toStdString<Type>(type);
		break;
	}

	return ret;
}

ChannelType::Type ChannelType::fromString(const std::string & name)
{
	std::string s(name);
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);

	ChannelType::Type ret = TX;

	if (s == "TX"){
		ret = TX;
	}
	else if (s == "TY"){
		ret = TY;
	}
	else if (s == "TZ"){
		ret = TZ;
	}
	else if (s == "RX"){
		ret = RX;
	}
	else if (s == "RY"){
		ret = RY;
	}
	else if (s == "RZ"){
		ret = RZ;
	}
	else{
		throw std::runtime_error("Unrecognized channel type");
	}

	return ret;
}

std::string AxisOrder::toString(const AxisOrder::Type axisOrder)
{
	std::string ret = "XYX";

	switch (axisOrder)
	{
	case AxisOrder::XYX:
		ret = "XYX";
		break;
	case AxisOrder::XYZ:
		ret = "XYZ";
		break;
	case AxisOrder::XZX:
		ret = "XZX";
		break;
	case AxisOrder::XZY:
		ret = "XZY";
		break;
	case AxisOrder::YXY:
		ret = "YXY";
		break;
	case AxisOrder::YXZ:
		ret = "YXZ";
		break;
	case AxisOrder::YZX:
		ret = "YZX";
		break;
	case AxisOrder::YZY:
		ret = "YZY";
		break;
	case AxisOrder::ZXY:
		ret = "ZXY";
		break;
	case AxisOrder::ZXZ:
		ret = "ZXZ";
		break;
	case AxisOrder::ZYX:
		ret = "ZYX";
		break;
	case AxisOrder::ZYZ:
		ret = "ZYZ";
		break;

	default:
		throw std::runtime_error("Unrecognized axis order");
	}

	return ret;
}

AxisOrder::Type AxisOrder::fromString(const std::string & axisOrderName)
{
	std::string s(axisOrderName);
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);

	AxisOrder::Type ret = XYX;

	if (s == "XYX"){
		ret = XYX;
	}
	else if (s == "XYZ"){
		ret = XYZ;
	}
	else if (s == "XZX"){
		ret = XZX;
	}
	else if (s == "XZY"){
		ret = XZY;
	}
	else if (s == "YXY"){
		ret = YXY;
	}
	else if (s == "YXZ"){
		ret = YXZ;
	}
	else if (s == "YZX"){
		ret = YZX;
	}
	else if (s == "YZY"){
		ret = YZY;
	}
	else if (s == "ZXY"){
		ret = ZXY;
	}
	else if (s == "ZXZ"){
		ret = ZXZ;
	}
	else if (s == "ZYX"){
		ret = ZYX;
	}
	else if (s == "ZYZ"){
		ret = ZYZ;
	}
	else{
		throw std::runtime_error("Unrecognized axis order");
	}

	return ret;
}

AxisOrder::Type AxisOrder::fromChannelTriplet(const ChannelTriplet & channelTriplet)
{
	AxisOrder::Type ret = AxisOrder::XYZ;

	if (channelTriplet[0] == ChannelType::RX && channelTriplet[1] == ChannelType::RY && channelTriplet[2] == ChannelType::RZ){
		ret = AxisOrder::XYZ;
	}
	else if (channelTriplet[0] == ChannelType::RX && channelTriplet[1] == ChannelType::RY && channelTriplet[2] == ChannelType::RX){
		ret = AxisOrder::XYX;
	}
	else if (channelTriplet[0] == ChannelType::RX && channelTriplet[1] == ChannelType::RZ && channelTriplet[2] == ChannelType::RX){
		ret = AxisOrder::XZX;
	}
	else if (channelTriplet[0] == ChannelType::RX && channelTriplet[1] == ChannelType::RZ && channelTriplet[2] == ChannelType::RY){
		ret = AxisOrder::XZY;
	}
	else if (channelTriplet[0] == ChannelType::RY && channelTriplet[1] == ChannelType::RX && channelTriplet[2] == ChannelType::RY){
		ret = AxisOrder::YXY;
	}
	else if (channelTriplet[0] == ChannelType::RY && channelTriplet[1] == ChannelType::RX && channelTriplet[2] == ChannelType::RZ){
		ret = AxisOrder::YXZ;
	}
	else if (channelTriplet[0] == ChannelType::RY && channelTriplet[1] == ChannelType::RZ && channelTriplet[2] == ChannelType::RX){
		ret = AxisOrder::YZX;
	}
	else if (channelTriplet[0] == ChannelType::RY && channelTriplet[1] == ChannelType::RZ && channelTriplet[2] == ChannelType::RY){
		ret = AxisOrder::YZY;
	}
	else if (channelTriplet[0] == ChannelType::RZ && channelTriplet[1] == ChannelType::RX && channelTriplet[2] == ChannelType::RY){
		ret = AxisOrder::ZXY;
	}
	else if (channelTriplet[0] == ChannelType::RZ && channelTriplet[1] == ChannelType::RX && channelTriplet[2] == ChannelType::RZ){
		ret = AxisOrder::ZXZ;
	}
	else if (channelTriplet[0] == ChannelType::RZ && channelTriplet[1] == ChannelType::RY && channelTriplet[2] == ChannelType::RX){
		ret = AxisOrder::ZYX;
	}
	else if (channelTriplet[0] == ChannelType::RZ && channelTriplet[1] == ChannelType::RY && channelTriplet[2] == ChannelType::RZ){
		ret = AxisOrder::ZYZ;
	}
	else {
		throw std::runtime_error("Invalid channel type or axis configuration for decoding axis order: " + boost::lexical_cast<std::string>(channelTriplet[0]));
	}

	return ret;
}

ChannelTriplet AxisOrder::toChannelTriplet(const AxisOrder::Type axisOrder)
{
	ChannelTriplet ret = { ChannelType::RX, ChannelType::RY, ChannelType::RZ };

	switch (axisOrder)
	{
	case AxisOrder::XYX:
		ret[0] = ChannelType::RX;
		ret[1] = ChannelType::RY;
		ret[2] = ChannelType::RX;
		break;

	case AxisOrder::XYZ:
		ret[0] = ChannelType::RX;
		ret[1] = ChannelType::RY;
		ret[2] = ChannelType::RZ;
		break;

	case AxisOrder::XZX:
		ret[0] = ChannelType::RX;
		ret[1] = ChannelType::RZ;
		ret[2] = ChannelType::RX;
		break;

	case AxisOrder::XZY:
		ret[0] = ChannelType::RX;
		ret[1] = ChannelType::RZ;
		ret[2] = ChannelType::RY;
		break;

	case AxisOrder::YXY:
		ret[0] = ChannelType::RY;
		ret[1] = ChannelType::RX;
		ret[2] = ChannelType::RY;
		break;

	case AxisOrder::YXZ:
		ret[0] = ChannelType::RY;
		ret[1] = ChannelType::RX;
		ret[2] = ChannelType::RZ;
		break;

	case AxisOrder::YZX:
		ret[0] = ChannelType::RY;
		ret[1] = ChannelType::RZ;
		ret[2] = ChannelType::RX;
		break;

	case AxisOrder::YZY:
		ret[0] = ChannelType::RY;
		ret[1] = ChannelType::RZ;
		ret[2] = ChannelType::RY;
		break;

	case AxisOrder::ZXY:
		ret[0] = ChannelType::RZ;
		ret[1] = ChannelType::RX;
		ret[2] = ChannelType::RY;
		break;

	case AxisOrder::ZXZ:
		ret[0] = ChannelType::RZ;
		ret[1] = ChannelType::RX;
		ret[2] = ChannelType::RZ;
		break;

	case AxisOrder::ZYX:
		ret[0] = ChannelType::RZ;
		ret[1] = ChannelType::RY;
		ret[2] = ChannelType::RX;
		break;

	case AxisOrder::ZYZ:
		ret[0] = ChannelType::RZ;
		ret[1] = ChannelType::RY;
		ret[2] = ChannelType::RZ;
		break;

	default:
		throw std::runtime_error("Unrecognized axis order");
	}

	return ret;
}