#include <dflib/Pin.h>

namespace df {

	InputPin::InputPin(ISinkNode * node) : pinImpl(node)
	{

	}

	InputPin::~InputPin() {}

	ISinkNode * InputPin::node()
	{
		return pinImpl.node();
	}

	const ISinkNode * InputPin::node() const
	{
		return pinImpl.node();
	}

	const bool InputPin::connected() const
	{
		return !pinImpl.empty();
	}

	const InputPin::size_type InputPin::connectionsSize() const
	{
		return pinImpl.size();
	}

	const IConnection * InputPin::connection(size_type idx) const
	{
		return pinImpl.connection(idx);
	}

	IConnection * InputPin::connection(size_type idx)
	{
		return pinImpl.connection(idx);
	}

	void InputPin::addConnection(IConnection * connection)
	{
		pinImpl.addConnection(connection);
	}

	void InputPin::removeConnection(IConnection * connection)
	{
		pinImpl.removeConnection(connection);
	}

	const bool InputPin::pinCompatible(const IOutputPin * pin) const
	{
		return true;
	}

	const bool InputPin::canConnect(const IOutputPin * pin) const
	{
		return pinImpl.empty();
	}

	OutputPin::OutputPin(ISourceNode * node) : pinImpl(node)
	{
	}

	OutputPin::~OutputPin() {}

	ISourceNode * OutputPin::node()
	{
		return pinImpl.node();
	}

	const ISourceNode * OutputPin::node() const
	{
		return pinImpl.node();
	}

	const bool OutputPin::connected() const
	{
		return !pinImpl.empty();
	}

	const OutputPin::size_type OutputPin::connectionsSize() const
	{
		return pinImpl.size();
	}

	const IConnection * OutputPin::connection(size_type idx) const
	{
		return pinImpl.connection(idx);
	}

	IConnection * OutputPin::connection(size_type idx)
	{
		return pinImpl.connection(idx);
	}

	void OutputPin::addConnection(IConnection * connection)
	{
		pinImpl.addConnection(connection);
	}

	void OutputPin::removeConnection(IConnection * connection)
	{
		pinImpl.removeConnection(connection);
	}

	ExtInputPin::ExtInputPin(ISinkNode * node, bool required) : pinImpl(node), required_(required_)
	{
	}

	ExtInputPin::~ExtInputPin() {}

	ISinkNode * ExtInputPin::node()
	{
		return pinImpl.node();
	}

	const ISinkNode * ExtInputPin::node() const
	{
		return pinImpl.node();
	}

	const bool ExtInputPin::connected() const
	{
		return !pinImpl.empty();
	}

	const ExtInputPin::size_type ExtInputPin::connectionsSize() const
	{
		return pinImpl.size();
	}

	const IConnection * ExtInputPin::connection(size_type idx) const
	{
		return pinImpl.connection(idx);
	}

	IConnection * ExtInputPin::connection(size_type idx)
	{
		return pinImpl.connection(idx);
	}

	void ExtInputPin::addConnection(IConnection * connection)
	{
		pinImpl.addConnection(connection);
	}

	void ExtInputPin::removeConnection(IConnection * connection)
	{
		pinImpl.removeConnection(connection);
	}

	const bool ExtInputPin::isDependencyFor(const IOutputPin * pin) const
	{
		return pinImpl.dependsOn(pin);
	}

	const ExtInputPin::size_type ExtInputPin::dependentSize() const
	{
		return pinImpl.dependencySize();
	}

	const bool ExtInputPin::dependentEmpty() const
	{
		return pinImpl.dependencyEmpty();
	}

	IOutputPin * ExtInputPin::dependent(size_type idx)
	{
		return pinImpl.dependency(idx);
	}

	const IOutputPin * ExtInputPin::dependent(size_type idx) const
	{
		return pinImpl.dependency(idx);
	}

	const bool ExtInputPin::required() const
	{
		return required_;
	}

	void ExtInputPin::addDependent(IOutputPin * pin)
	{
		pinImpl.addDependency(pin);
	}

	void ExtInputPin::removeDependent(IOutputPin * pin)
	{
		pinImpl.removeDependency(pin);
	}

	const bool ExtInputPin::pinCompatible(const IOutputPin * pin) const
	{
		return true;
	}

	const bool ExtInputPin::canConnect(const IOutputPin * pin) const
	{
		return pinImpl.empty();
	}

	ExtOutputPin::ExtOutputPin(ISourceNode * node, const Dependencies & dep) : pinImpl(node, dep)
	{
	}

	ExtOutputPin::~ExtOutputPin() {}

	ISourceNode * ExtOutputPin::node()
	{
		return pinImpl.node();
	}

	const ISourceNode * ExtOutputPin::node() const
	{
		return pinImpl.node();
	}

	const bool ExtOutputPin::connected() const
	{
		return !pinImpl.empty();
	}

	const ExtOutputPin::size_type ExtOutputPin::connectionsSize() const
	{
		return pinImpl.size();
	}

	const IConnection * ExtOutputPin::connection(size_type idx) const
	{
		return pinImpl.connection(idx);
	}

	IConnection * ExtOutputPin::connection(size_type idx)
	{
		return pinImpl.connection(idx);
	}

	void ExtOutputPin::addConnection(IConnection * connection)
	{
		pinImpl.addConnection(connection);
	}

	void ExtOutputPin::removeConnection(IConnection * connection)
	{
		pinImpl.removeConnection(connection);
	}

	const bool ExtOutputPin::dependsOn(const IInputPin * pin) const
	{
		return pinImpl.dependsOn(pin);
	}

	const ExtOutputPin::size_type ExtOutputPin::dependencySize() const
	{
		return pinImpl.dependencySize();
	}

	const bool ExtOutputPin::dependencyEmpty() const
	{
		return pinImpl.dependencyEmpty();
	}

	IInputPin * ExtOutputPin::dependency(size_type idx)
	{
		return pinImpl.dependency(idx);
	}

	const IInputPin * ExtOutputPin::dependency(size_type idx) const
	{
		return pinImpl.dependency(idx);
	}
}