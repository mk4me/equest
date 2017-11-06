/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   11:25
    filename: ModelBase.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MODELBASE_H__
#define HEADER_GUARD___MODELBASE_H__

#include <dflib/INode.h>
#include <dflib/IPin.h>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

	//! Forward declaration
	class INode;

	//! Implementacja bazy modelu
	class ModelBase
	{
	public:
		//! Publiczny virtualny destruktor
		virtual ~ModelBase();

	protected:
		//! Chroniony domyœlny konstruktor
		ModelBase();

		//! \param node Wêze³ dodawany do modelu
		void attachNode(INode * node);

		//! \param node Wêze³ usuwany z modelu
		void detachNode(INode * node);

		//! \param pin Pin do którego dodajemy po³¹czenie
		//! \param connection Po³¹czenie dodawane do pinu
		static void addConnection(IPin * pin, IConnection * connection);

		//! \param pin Pin z którego usuwamy po³¹czenie
		//! \param connection Po³¹czenie usuwane z pinu
		static void removeConnection(IPin * pin, IConnection * connection);

		//! \param node Wêze³ sink którego pin wejœciowy jest ³¹czony
		//! \param pin Pin wejœciowy który jest ³¹czony
		static void connectInput(ISinkNode * node, IInputPin * pin);

		//! \param node Wêze³ sink którego pin wejœciowy jest roz³anczany
		//! \param pin Pin wejœciowy roz³anczany
		static void disconnectInput(ISinkNode * node, IInputPin * pin);

		//! \param node Wêze³ Ÿród³owy którego pin wyjœciowy jest ³¹czony
		//! \param pin Pin wyjœciowy który jest ³¹czony
		static void connectOutput(ISourceNode * node, IOutputPin * pin);

		//! \param node Wêze³ Ÿród³owy którego pin wyjœciowy jest roz³anczany
		//! \param pin Pin wyjœciowy który jest roz³anczany
		static void disconnectOutput(ISourceNode * node, IOutputPin * pin);
	};

}

#endif	//	HEADER_GUARD___MODELBASE_H__
