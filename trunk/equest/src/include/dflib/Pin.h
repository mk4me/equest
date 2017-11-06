#ifndef HEADER_GUARD__PIN_H__
#define HEADER_GUARD__PIN_H__

#include <dflib/IPin.h>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace df {

	class IConnection;

	//! Wzorzec klasy realizujacej przechowywanie połączeń w pinie
	template<class Node, class Connection>
	class PinImpl
	{
	private:

		typedef std::vector<Connection*> Connections;

	public:

		typedef typename Connections::size_type size_type;

	public:

		PinImpl(Node * node) : node_(node) {}

		Node * node()
		{
			return node_;
		}

		const Node * node() const
		{
			return node_;
		}

		const bool empty() const
		{
			return connections_.empty();
		}

		const size_type size() const
		{
			return connections_.size();
		}

		const Connection * connection(size_type idx) const
		{
			return connections_[idx];
		}

		Connection * connection(size_type idx)
		{
			return connections_[idx];
		}

		void addConnection(Connection * connection)
		{
			auto it = find(connection);
			if(it != connections_.end())
			{
				throw std::runtime_error("Connection already managed by the pin");
			}

			//innerAddConnection(connection, boost::is_same<IConnection, Connection>::value);
			connections_.push_back(connection);
		}

		void removeConnection(Connection * connection)
		{
			auto it = find(connection);
			if(it == connections_.end())
			{
				throw std::runtime_error("Connection not managed by the pin");
			}

			connections_.erase(it);
		}

	private:

		typename Connections::iterator find(Connection * connection)
		{
			return std::find(connections_.begin(), connections_.end(), connection);
		}

	private:

		Node * node_;
		Connections connections_;
	};

	//! Wzorzec klasy realizującej przechowywanie połączeń w pinie i jego dodatkowy opis zależności
	template<class Node, class Connection, class Pin>
	class ExtPinImpl : public PinImpl<Node, Connection>
	{
	public:
		typedef typename PinImpl<Node, Connection>::size_type size_type;
		typedef std::vector<Pin*> Dependencies;

	public:

		ExtPinImpl(Node * node, const Dependencies & dep = Dependencies()) : PinImpl<Node, Connection>(node), dependency_(dep) {}

		const bool dependsOn(const Pin * pin) const
		{
			return (findDepenency(pin) != dependency_.end());
		}

		const size_type dependencySize() const
		{
			return dependency_.size();
		}

		const bool dependencyEmpty() const
		{
			return dependency_.empty();
		}

		Pin * dependency(size_type idx)
		{
			return dependency_[idx];
		}

		const Pin * dependency(size_type idx) const
		{
			return dependency_[idx];
		}

		const bool dependencyOK() const
		{
			bool ret = true;
			for(auto it = dependency_.begin(); it != dependency_.end(); ++it)
			{
				if((*it)->empty() == true){
					ret = false;
				}
			}

			return ret;
		}

		void addDependency(Pin * pin)
		{
			auto it = findDepenency(pin);
			if(it != dependency_.end())
			{
				throw std::runtime_error("Dependency already exists");
			}

			dependency_.push_back(pin);
		}

		void removeDependency(Pin * pin)
		{
			auto it = findDepenency(pin);
			if(it == dependency_.end())
			{
				throw std::runtime_error("Dependency not exists");
			}

			dependency_.erase(it);
		}

	private:

		typename Dependencies::iterator findDepenency(Pin * pin)
		{
			return std::find(dependency_.begin(), dependency_.end(), pin);
		}

		typename Dependencies::const_iterator findDepenency(const Pin * pin) const
		{
			return std::find(dependency_.begin(), dependency_.end(), pin);
		}

	private:
		Dependencies dependency_;
	};

//! Implementacja pina wejściowego
class InputPin : public IInputPin
{
public:

	InputPin(ISinkNode * node);

	virtual ~InputPin();

	virtual ISinkNode * node();

	virtual const ISinkNode * node() const;

	virtual const bool connected() const;

	virtual const size_type connectionsSize() const;

	virtual const IConnection * connection(size_type idx) const;

	virtual IConnection * connection(size_type idx);

	virtual void addConnection(IConnection * connection);

	virtual void removeConnection(IConnection * connection);

	virtual const bool pinCompatible(const IOutputPin * pin) const;

	virtual const bool canConnect(const IOutputPin * pin) const;

protected:

	PinImpl<ISinkNode, IConnection> pinImpl;
};

//! Implementacja pina wyjściowego
class OutputPin : public IOutputPin
{
public:

	OutputPin(ISourceNode * node);

	virtual ~OutputPin();

	virtual ISourceNode * node();

	virtual const ISourceNode * node() const;

	virtual const bool connected() const;

	virtual const size_type connectionsSize() const;

	virtual const IConnection * connection(size_type idx) const;

	virtual IConnection * connection(size_type idx);

	virtual void addConnection(IConnection * connection);

	virtual void removeConnection(IConnection * connection);

protected:

	PinImpl<ISourceNode, IConnection> pinImpl;
};

//! Implementacja rozszerzonego pina wejściowego
class ExtInputPin : public IExtendedInputPin
{
public:

	ExtInputPin(ISinkNode * node, bool required);

	virtual ~ExtInputPin();

	virtual ISinkNode * node();

	virtual const ISinkNode * node() const;

	virtual const bool connected() const;

	virtual const size_type connectionsSize() const;

	virtual const IConnection * connection(size_type idx) const;

	virtual IConnection * connection(size_type idx);

	virtual void addConnection(IConnection * connection);

	virtual void removeConnection(IConnection * connection);

	virtual const bool isDependencyFor(const IOutputPin * pin) const;

	virtual const size_type dependentSize() const;

	virtual const bool dependentEmpty() const;

	virtual IOutputPin * dependent(size_type idx);

	virtual const IOutputPin * dependent(size_type idx) const;

	virtual const bool required() const;

	virtual void addDependent(IOutputPin * pin);

	virtual void removeDependent(IOutputPin * pin);

	virtual const bool pinCompatible(const IOutputPin * pin) const;

	virtual const bool canConnect(const IOutputPin * pin) const;

private:
	bool required_;
	ExtPinImpl<ISinkNode, IConnection, IOutputPin> pinImpl;
};

//! Implementacja rozszerzonego pina wyjściowego
class ExtOutputPin : public IExtendedOutputPin
{
public:

	typedef ExtPinImpl<ISourceNode, IConnection, IInputPin>::Dependencies Dependencies;
	typedef ExtPinImpl<ISourceNode, IConnection, IInputPin>::size_type size_type;

public:

	ExtOutputPin(ISourceNode * node, const Dependencies & dep);

	virtual ~ExtOutputPin();

	virtual ISourceNode * node();

	virtual const ISourceNode * node() const;

	virtual const bool connected() const;

	virtual const size_type connectionsSize() const;

	virtual const IConnection * connection(size_type idx) const;

	virtual IConnection * connection(size_type idx);

	virtual void addConnection(IConnection * connection);

	virtual void removeConnection(IConnection * connection);

	virtual const bool dependsOn(const IInputPin * pin) const;	

	virtual const size_type dependencySize() const;

	virtual const bool dependencyEmpty() const;

	virtual IInputPin * dependency(size_type idx);

	virtual const IInputPin * dependency(size_type idx) const;

private:
	ExtPinImpl<ISourceNode, IConnection, IInputPin> pinImpl;
};

}

#endif
