/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   11:03
    filename: IPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPIN_H__
#define HEADER_GUARD___IPIN_H__

#include <dflib/INode.h>
////////////////////////////////////////////////////////////////////////////////
namespace df {
////////////////////////////////////////////////////////////////////////////////

//! Forward declaration
class IConnection;
class ModelBase;

//! Interfejs bazowy pinu. Opisuje jego typ, po³¹czenia, wêze³ do którego nale¿y.
class IPin
{
	friend class ModelBase;

public:

	//! Typ pinu
	enum PinType {
		INPUT_PIN,	//! pin wejsciowy
		OUTPUT_PIN	//! pin wyjœciowy
	};

	//! Typ iloœci po³¹czeñ
	typedef std::size_t size_type;

public:
	//! \return Typ pinu
	virtual const PinType type() const = 0;

	//! \return Wêze³ do którego nalezy pin
	virtual INode * node() = 0;
	//! \return Wêze³ do którego nalezy pin
	virtual const INode * node() const = 0;

	//! \return Czy pin jest pod³¹czony
	virtual const bool connected() const = 0;
	//! \return Iloœæ po³¹czeñ pinu
	virtual const size_type connectionsSize() const = 0;
	//! \param idx Indeks po³¹czenia które chcemy pobraæ
	//! \return Po³¹czenie o zadanym indeksie
	virtual const IConnection * connection(size_type idx) const = 0;
	//! \param idx Indeks po³¹czenia które chcemy pobraæ
	//! \return Po³¹czenie o zadanym indeksie
	virtual IConnection * connection(size_type idx) = 0;

private:
	//! \param connection Nowe po³¹czenie którego czêsci¹ jest dany pin
	virtual void addConnection(IConnection * connection) = 0;
	//! \param connection Po³¹czenie którego czêsci¹ by³ dany pin - usuwamy
	virtual void removeConnection(IConnection * connection) = 0;
};

//! Forward declaration
class IOutputPin;

//! Interfejs pinu wejsciowego. Dodatkowo opisuje mo¿liwoœci ³¹czenia tego pinu
//! z pinami wyjœciowymi
class IInputPin : public IPin
{
public:	
	//! \return Typ pinu
	virtual const PinType type() const { return IPin::INPUT_PIN; }

	//! \return Wêze³ do którego nalezy pin
	virtual ISinkNode * node() = 0;
	//! \return Wêze³ do którego nalezy pin
	virtual const ISinkNode * node() const = 0;

	//! \param pin Pin wyjœciowy, który testujemy pod k¹tem kompatybilnoœci ³¹czenia
	//! \return Czy mo¿na tego typu  piny po³¹czyæ
	virtual const bool pinCompatible(const IOutputPin * pin) const = 0;
	//! \param pin Pin wyjœciowy, który testujemy pod k¹tem mozliwoœci ³¹czenia
	//! \return Czy mo¿na te piny po³¹czyæ
	virtual const bool canConnect(const IOutputPin * pin) const = 0;
};

//! Interfejs pinu wyjœciowego. 
class IOutputPin : public IPin
{
public:
	//! \return Typ pinu
	virtual const PinType type() const { return IPin::OUTPUT_PIN; }
	//! \return Wêze³ do którego nalezy pin
	virtual ISourceNode * node() = 0;
	//! \return Wêze³ do którego nalezy pin
	virtual const ISourceNode * node() const = 0;
};

//! Interfejs rozszerzonego piny wejsciowego. Dodaje informacje o koniecznoœci
//! pod³¹czenia danego pinu oraz ewentualnych pinach uzale¿nionych od tego pinu
class IExtendedInputPin : public IInputPin
{
public:
	//! \return Czy pin musi byæ pod³¹czony
	virtual const bool required() const = 0;

	//! \return Iloœæ pinów zale¿nych od pinu
	virtual const size_type dependentSize() const = 0;
	//! \return Czy s¹ jakieœ piny zale¿ne
	virtual const bool dependentEmpty() const = 0;
	//! \param idx Indeks pinu wyjœciowego zale¿nego od pinu
	//! \return Pin wyjœciowy zale¿ny od pinu
	virtual IOutputPin * dependent(size_type idx) = 0;
	//! \param idx Indeks pinu wyjœciowego zale¿nego od pinu
	//! \return Pin wyjœciowy zale¿ny od pinu
	virtual const IOutputPin * dependent(size_type idx) const = 0;

	//! \param pin Pin wyjœciowy który testujemy pod k¹tem zale¿noœci od pinu
	//! \return Czy zadany pin wyjœciowy jest zalezny od pinu
	virtual const bool isDependcyFor(const IOutputPin * pin) const = 0;
};

//! Interfejs rozszerzonego pinu wyjœciowego. Dodaje informacjê na temat
//! zale¿noœci tego pinu od pinów wejœciowych.
class IExtendedOutputPin : public IOutputPin
{
public:
	//! \return Iloœæ pinów od kórych zale¿ny jest pin
	virtual const size_type dependencySize() const = 0;
	//! \return Czy pin jest uzale¿niony od jakichœ pinów wejœciowych
	virtual const bool dependencyEmpty() const = 0;

	//! \param idx Indeks pinu od którego jest uzale¿niony ten pin
	//! \return Pin wejœciowy od którego jest uzalezniony ten pin
	virtual IInputPin * dependency(size_type idx) = 0;
	//! \param idx Indeks pinu od którego jest uzale¿niony ten pin
	//! \return Pin wejœciowy od którego jest uzalezniony ten pin
	virtual const IInputPin * dependency(size_type idx) const = 0;
	//! \param pin Pin wejœciowy testowany pod k¹tem zale¿noœci
	//! \return Czy pin jest uzale¿niony od danego pinu
	virtual const bool dependsOn(const IInputPin * pin) const = 0;
	//! \return Czy zale¿noœci pinu s¹ spe³nione
	virtual const bool dependenciesSatisfied() const = 0;
};

}

#endif	//	HEADER_GUARD___IPIN_H__
