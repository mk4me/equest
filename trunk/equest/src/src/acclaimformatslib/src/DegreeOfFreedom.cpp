#include <acclaimformatslib/DegreeOfFreedom.h>
#include <algorithm>

using namespace acclaim;

std::string Axis::getAxisOrderName( kinematicUtils::AxisOrder::Type order ) {
    switch(order){
        case kinematicUtils::AxisOrder::XYZ : return "XYZ";
        case kinematicUtils::AxisOrder::XZY : return "XZY";
        case kinematicUtils::AxisOrder::YXZ : return "YXZ";
        case kinematicUtils::AxisOrder::YZX : return "YZX";
        case kinematicUtils::AxisOrder::ZXY : return "ZXY";
        case kinematicUtils::AxisOrder::ZYX : return "ZYX";
        default:
            throw std::runtime_error("Unsupported acclaim axis order");
    }
}

std::size_t DegreeOfFreedom::getChannelIndex(kinematicUtils::Channel channel, const std::vector<DegreeOfFreedom>& dofs) {
    
	auto it = std::find_if(dofs.begin(), dofs.end(), [channel](const DegreeOfFreedom & dof) { return dof.channel == channel; });
	return it != dofs.end() ? std::distance(dofs.begin(), it) : -1;
}

kinematicUtils::AxisOrder::Type Axis::getAxisOrder(const std::string& axis) {
	if (axis == "XYZ") {
		return kinematicUtils::AxisOrder::XYZ;
	}
	else if (axis == "XZY") {
		return kinematicUtils::AxisOrder::XZY;
	}
	else if (axis == "YXZ") {
		return kinematicUtils::AxisOrder::XZY;
	}
	else if (axis == "YZX") {
		return kinematicUtils::AxisOrder::YZX;
	}
	else if (axis == "ZXY") {
		return kinematicUtils::AxisOrder::ZXY;
	}
	else if (axis == "ZYX") {
		return kinematicUtils::AxisOrder::ZYX;
    } else {
		throw std::runtime_error("Unsupported acclaim axis order");
    }
}

DegreeOfFreedom::DegreeOfFreedom(const kinematicUtils::Channel channel) :
	channel(channel),
	minLimit(-std::numeric_limits<double>::infinity()),
	maxLimit(std::numeric_limits<double>::infinity())
{
}

DegreeOfFreedom::DegreeOfFreedom(const kinematicUtils::Channel channel,
	double minLimit, double maxLimit) :
	channel(channel),
	minLimit(minLimit),
	maxLimit(maxLimit)
{
}

DegreeOfFreedom::~DegreeOfFreedom()
{

}

bool DegreeOfFreedom::operator==(const DegreeOfFreedom& b) const
{
	static const double epsilon = 0.0000001f;
	return channel == b.channel &&
		std::abs(minLimit - b.minLimit) < epsilon &&
		std::abs(maxLimit - b.maxLimit) < epsilon;
}

kinematicUtils::Channel DegreeOfFreedom::getChannel( const std::string& channel ) {
	if (channel == "TX") {
		return (kinematicUtils::ChannelType::TX);
	}
	else if (channel == "TY") {
		return (kinematicUtils::ChannelType::TY);
	}
	else if (channel == "TZ") {
		return (kinematicUtils::ChannelType::TZ);
	}
	else if (channel == "RX") {
		return (kinematicUtils::ChannelType::RX);
	}
	else if (channel == "RY") {
		return (kinematicUtils::ChannelType::RY);
	}
	else if (channel == "RZ") {
		return (kinematicUtils::ChannelType::RZ);
	}
	else if (channel == "L") {
        return (DegreeOfFreedom::L);
    } else {
		throw std::runtime_error("Unsupported acclaim data channel");
    }
}

std::string DegreeOfFreedom::getChannelName( kinematicUtils::Channel channel, bool uppercase) {
    switch (channel) {
        case kinematicUtils::ChannelType::TX : return uppercase ? "TX" : "tx";
        case kinematicUtils::ChannelType::TY : return uppercase ? "TY" : "ty";
        case kinematicUtils::ChannelType::TZ : return uppercase ? "TZ" : "tz";
        case kinematicUtils::ChannelType::RX : return uppercase ? "RX" : "rx";
        case kinematicUtils::ChannelType::RY : return uppercase ? "RY" : "ry";
        case kinematicUtils::ChannelType::RZ : return uppercase ? "RZ" : "rz";
        case DegreeOfFreedom::L  : return uppercase ? "L"  : "l";
    }
	throw std::runtime_error("Unsupported acclaim data channel");
}
