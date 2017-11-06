#include <dfmlib/Pin.h>
#include <dfmlib/Connection.h>
#include <dfmlib/Node.h>
#include <dfmlib/Model.h>
#include <utils/Debug.h>
#include <typeinfo>
#include <boost/assert.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Pin::Pin(const std::string & pinName, bool required,
	const Pin::ReqPinsSet & requiredPins)
	: utils::enable_shared_from_this<Pin>(), pinType(UNSET),
    pinRequired(required),requiredPinsDependency(requiredPins),
    name(pinName)
{

}

Pin::~Pin(void){

}

void Pin::setName(const std::string & name)
{
    this->name = name;
}

NPtr Pin::getParent() const{
	if(parentNode.expired() == true){
        throw std::runtime_error("Pin referring to expired Node!");
    }

    return parentNode.lock();
}
//const Pin::Connections & Pin::getConnections() const{
//	return connections;
//}

Pin::iterator Pin::begin() const{
    return connections.begin();
}

Pin::iterator Pin::end() const{
    return connections.end();
}

int Pin::getIndexOfConnection(const ConnPtr & connection) const
{
    int ret = -1;
    auto it = std::find(begin(), end(), connection);
    if(it != end()){
        ret = std::distance(begin(), it);
    }

    return ret;
}

const ConnPtr & Pin::getConnection(int idx) const
{
    UTILS_ASSERT((idx >= 0 && idx < static_cast<int>(connections.size())), "Błędny indeks połączenia");
    return connections[idx];
}

bool Pin::empty() const
{
    return connections.empty();
}

Pin::size_type Pin::size() const
{
    return connections.size();
}

const Pin::ReqPinsSet & Pin::getDependantPins() const{
	return requiredPinsDependency;
}

Pin::PinType Pin::getType() const {
	return pinType;
}

void Pin::setType(PinType type){
	pinType = type;
}

bool Pin::isCompatible(const CPinPtr & pin) const{
	return typeid(*this) == typeid(*pin);
}

void Pin::addConnection(const ConnPtr & connection)
{
	if(std::find(begin(), end(), connection) != end()){
        throw std::runtime_error("Connection already added to pin!");
    }

    connections.push_back(connection);
}

void Pin::removeConnection(const ConnPtr & connection)
{
    // rev - czemu nie dzialalo w takiej formie? jakas konwersja?
    //auto it = std::find(begin(), end(), connection);
    //if(it == end()){
    auto it = std::find(connections.begin(), connections.end(), connection);
    if(it == connections.end()){
        throw std::runtime_error("Connection not found in pin!");
    }

    connections.erase(it);
}

void Pin::clearConnections(){
	Connections().swap(connections);
}

const std::string & Pin::getName() const{
	return name;
}

int Pin::getPinIndex() const
{
    if(parentNode.expired() == true){
        return -1;
    }

	return pinIndexFunc(utils::const_pointer_cast<Pin>(shared_from_this()));
}

//void Pin::setPinName(const std::string & pinName){
//	this->name = pinName;
//}

bool Pin::dependsOnPin(const PinPtr & pin) const{
	if(requiredPinsDependency.find(pin) != requiredPinsDependency.end()){
		return true;
	}

	return false;
}

bool Pin::isRequired() const{
	return pinRequired;
}

bool Pin::isComplete() const{
	bool ret = true;
	for(auto it = requiredPinsDependency.begin(); it != requiredPinsDependency.end(); ++it){
        if((*it).lock()->empty() == true){
			ret = false;
			break;
		}
	}

	return ret;
}

Pin::StaticStatus Pin::getStaticStatus() const{
	StaticStatus ret = OK;

	switch(pinType){

		case IN:
			if(empty() == false){
				ret = CONNECTED;
			}else if(pinRequired == true){
				ret = REQUIRED;
			}
			break;

		case OUT:
			if(isComplete() == false){
				ret = INCOMPLETE;
			}else if(empty() == false){
				ret = CONNECTED;
			}
			break;
	}

	return ret;
}

Pin::DynamicStatus Pin::getDynamicStatus(const PinPtr & refPin, const MPtr & model) const{
	DynamicStatus ret = COMPATIBLE;

	if(pinType == refPin->pinType || (pinType == IN && isCompatible(refPin) == false) ||
		(pinType == OUT && refPin->isCompatible(shared_from_this()) == false)){
		ret = INCOMPATIBLE;
	}else if(pinType == IN && model->createCycle(refPin, shared_from_this()) == true){
		ret = COMPATIBLE_CYCLE;
	}else if(pinType == OUT && model->createCycle(shared_from_this(), refPin) == true){
		ret = COMPATIBLE_CYCLE;
	}

	return ret;
}

}
