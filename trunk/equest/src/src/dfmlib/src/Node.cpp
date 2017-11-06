#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>
#include <utils/Debug.h>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Node::Node(const std::string & name) : name(name), initialized(false)
{
}


Node::~Node(void)
{
}

void Node::configure()
{

}

void Node::setName(const std::string & name)
{
    this->name = name;
}

const std::string & Node::getName() const{
	return name;
}

bool Node::isInitialized() const
{
    return initialized;
}

void Node::initialize()
{
    if(initialized == true){
        throw std::runtime_error("Node already initialized!");
    }

    pinsAdder.reset(new PinsAdder(this));

    doInitialization(pinsAdder);

    initialized = true;
}

void Node::doInitialization(const Node::PinsAdderPtr & pinsAdder)
{

}

MPtr Node::getModel() const
{
    /*if(model.expired() == true){
        throw std::runtime_error("Model of this node not exist any more!");
    }*/

    return model.lock();
}

//void Node::setNodeName(const std::string & nodeName){
//	this->name = nodeName;
//}

void Node::addInPin(const PinPtr & pin )
{
    UTILS_ASSERT((pin != nullptr), "Nieprawidlowy pin");
    if(pin->parentNode.lock() != nullptr){
        throw std::runtime_error("Pin is already connected with node!");
    }
	
    pin->parentNode = shared_from_this();
    pin->pinIndexFunc = std::bind(&Node::indexOfInPin, this, std::placeholders::_1);
    inPins.push_back(pin);
	pin->setType(Pin::IN);
}

void Node::addOutPin(const PinPtr & pin )
{
    UTILS_ASSERT((pin != nullptr), "Nieprawidlowy pin");
    if(pin->parentNode.lock() != nullptr){
        throw std::runtime_error("Pin is already connected with node!");
    }
	
    pin->parentNode = shared_from_this();
	pin->pinIndexFunc = std::bind(&Node::indexOfOutPin, this, std::placeholders::_1);
    outPins.push_back(pin);
	pin->setType(Pin::OUT);
}

//const Node::Pins & Node::getInPins() const{
//	return inPins;
//}
//
//const Node::Pins & Node::getOutPins() const{
//	return outPins;
//}
//
//bool Node::pinExists(const Pins & pins, const PinPtr & pin){
//	bool ret = false;
//
//	if(pins.find(pin) != pins.end()){
//		ret = true;
//	}
//
//	return ret;
//}

//bool Node::pinExists(const CNPtr & node, const PinPtr & pin) {
//	return pinExists(node->getInPins(),pin) || pinExists(node->getOutPins(),pin);
//}



Node::iterator Node::beginIn() const
{
    return inPins.begin();
}

Node::iterator Node::endIn() const
{
    return inPins.end();
}

const PinPtr & Node::getInPin(int idx) const
{
    UTILS_ASSERT((idx >=0 && idx < static_cast<int>(inPins.size())), "Błędny indeks pinu wejściowego");
    return inPins[idx];
}

int Node::indexOfInPin(const PinPtr & pin) const
{
    int ret = -1;
    auto it = std::find(inPins.begin(), inPins.end(), pin);
    if(it != inPins.end()){
        ret = std::distance(inPins.begin(), it);
    }

    return ret;
}

Node::size_type Node::sizeIn() const
{
    return inPins.size();
}

Node::iterator Node::beginOut() const
{
    return outPins.begin();
}

Node::iterator Node::endOut() const
{
    return outPins.end();
}

const PinPtr & Node::getOutPin(int idx) const
{
    UTILS_ASSERT((idx >=0 && idx < static_cast<int>(outPins.size())), "Błędny indeks pinu wyjściowego");
    return outPins[idx];
}

int Node::indexOfOutPin(const PinPtr & pin) const
{
    int ret = -1;
    auto it = std::find(outPins.begin(), outPins.end(), pin);
    if(it != outPins.end()){
        ret = std::distance(outPins.begin(), it);
    }

    return ret;
}

Node::size_type Node::sizeOut() const
{
    return outPins.size();
}

bool Node::validInPinsConnection(const CNPtr & node) {
	bool ret = true;
	bool empty = true;
	bool required = false;

	for(auto it = node->beginIn(); it != node->endIn(); ++it){
		if((*it)->isRequired() == true){
			required = true;
			if((*it)->empty()){
				ret = false;
				break;
			}
		}else{
			empty &= (*it)->empty();
		}
	}

	if(required == false && empty == true){
		ret = false;
	}

	return ret;
}

bool Node::validOutPinsConnection(const CNPtr & node) {
	bool ret = true;

	for(auto it = node->beginOut(); it != node->endOut(); ++it){
		if((*it)->isComplete() == false){
			ret = false;
			break;
		}
	}

	return ret;
}

bool Node::validConnection(const CNPtr & node){
	return validInPinsConnection(node) && validOutPinsConnection(node);
}

bool Node::anyInPinConnected(const CNPtr & node){
	return anyPinConnected(node->inPins);
}

bool Node::anyOutPinConnected(const CNPtr & node){
	return anyPinConnected(node->outPins);
}

bool Node::anyPinConnected(const Pins & pins){
	bool ret = false;

	for(auto it = pins.begin(); it != pins.end(); ++it){
		if((*it)->empty() == false){
			ret = true;
			break;
		}
	}

	return ret;
}

bool Node::anyPinConnected(const CNPtr & node){
	return anyInPinConnected(node) && anyOutPinConnected(node);
}
}
