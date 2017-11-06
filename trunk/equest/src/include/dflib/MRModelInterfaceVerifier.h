/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   8:14
    filename: MRModelInterfaceVerifier.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MRMODELINTERFACEVERIFIER_H__
#define HEADER_GUARD___MRMODELINTERFACEVERIFIER_H__

#include <algorithm>
#include <list>

//! Forward declarations 
namespace df{

	class IPin;
	class INode;

	class IInputPin;
	class IOutputPin;

	class ISourceNode;
	class ISinkNode;
	class IProcessingNode;

	class IDFInput;
	class IDFOutput;

	class IDFSource;
	class IDFSink;
	class IDFProcessor;

	class IModelReader;
}

//! Klasa weryfikuj¹ca interfejsy elementów modelu pod k¹tem przetwarzania danych.
//! Realizacja jest oparta o rzutowanie dynamic_castem elementów na konkretne interfejsy.
//! W wyniku mozemy dostaæ wyniki rzutowania aby przy dalszym przetwarzaniu nie by³o trzeba
//! tej procedury powtarzaæ. Veryfikacja obejmuje jedynie pod³¹czone elementy
class MRModelInterfaceVerifier
{
public:
	//! Typ opisuj¹cy interfejs wejscia pod k¹tem przetwarzania danych
	typedef std::pair<df::IInputPin*, df::IDFInput*> InputVerificationData;
	//! Typ agreguj¹cy opisy wejœæ pod k¹tem przetwarzania danych
	typedef std::list<InputVerificationData> InputVerification;

	//! Typ opisuj¹cy interfejs wyjscia pod k¹tem przetwarzania danych
	typedef std::pair<df::IOutputPin*, df::IDFOutput*> OutputVerificationData;
	//! Typ agreguj¹cy opisy wyjœæ pod k¹tem przetwarzania danych
	typedef std::list<OutputVerificationData> OutputVerification;

	//! Typ opisuj¹cy interfejs Ÿród³¹ pod k¹tem przetwarzania danych
	struct SourceVerificationData
	{
		//! Wêze³ Ÿród³owy
		df::ISourceNode * node;
		//! Interfejs Ÿród³a do produkowania danych na potrzeby przetwarzania
		df::IDFSource * source;
		//! Weryfikacja pinów wyjœciowych
		OutputVerification outputVerication;
	};

	//! Typ opisuj¹cy interfejs wêz³a terminuj¹cego pod k¹tem przetwarzania danych
	struct SinkVerificationData
	{
		//! Wêze³ terminuj¹cy
		df::ISinkNode * node;
		//! Interfejs wêz³a terminuj¹cego do konsumowania danych
		df::IDFSink * sink;
		//! Weryfikacja pinów wejœciowych
		InputVerification inputVerication;
	};

	//! Typ opisuj¹cy interfejs wêz³a przetwarzaj¹cego pod k¹tem przetwarzania danych
	struct ProcessorVerificationData
	{
		//! Wêze³ przetwarzaj¹cy
		df::IProcessingNode * node;
		//! Interfejs wêz³a przetwarzaj¹cego na potrzeby przetwarzania danych
		df::IDFProcessor * processor;
		//! Weryfikacja pinów wejsciowych
		InputVerification inputVerication;
		//! Weryfikacja pinów wyjsciowych
		OutputVerification outputVerication;
	};

	//! Typ agreguj¹cy weryfikacje Ÿróde³
	typedef std::list<SourceVerificationData> SourceVerification;
	//! Typ agreguj¹cy weryfikacje sinków
	typedef std::list<SinkVerificationData> SinkVerification;
	//! Typ agreguj¹cy weryfikacje procesorów
	typedef std::list<ProcessorVerificationData> ProcessorVerification;

	//! Typ agreguj¹cy piny nie implementuj¹ce wymaganych interfejsów na potrzeby przetwarzania
	typedef std::list<df::IPin *> IncompatiblePins;
	//! Typ agreguj¹cy wêz³y nie implementuj¹ce wymaganych interfejsów na potrzeby przetwarzania
	typedef std::list<df::INode *> IncompatibleNodes;

	//! Typ opisuj¹cy model pod k¹tem mozliwoœci przetwarzania danych
	struct ModelVerificationData
	{
		//! Weryfikacja Ÿróde³
		SourceVerification sourceVerification;
		//! Weryfikacja sinków
		SinkVerification sinkVerification;
		//! Weryfikacja procesorów
		ProcessorVerification processorVerification;

		//! Piny nienadaj¹ce siê do przetwarzania
		IncompatiblePins incompatiblePins;
		//! Wêz³y nienadaj¹ce siê do przetwarzania
		IncompatibleNodes incompatibleNodes;
	};

public:
	//! \param model Model do weryfikacji
	//! \param modelVerification Opis veryfikacji modelu
	//! \return Czy modle nadaje siê do przetwarzania
	static const bool verifyModelInterface(df::IModelReader * model, ModelVerificationData & modelVerification);

private:
	//! \param node Wêze³ sink do weryfikacji
	//! \param inputVeryfication Opis weryfikacji wejœæ wêz³a
	//! \param incompatiblePins Zbiór pinów nienadaj¹cych sie do przetwarzania
	static void verifyInputPinsInterface(df::ISinkNode * node, InputVerification & inputVerification, IncompatiblePins & incompatiblePins);
	//! \param node Wêze³ Ÿród³o do weryfikacji
	//! \param outputVeryfication Opis weryfikacji wyjœæ wêz³a
	//! \param incompatiblePins Zbiór pinów nienadaj¹cych sie do przetwarzania
	static void verifyOutputPinsInterface(df::ISourceNode * node, OutputVerification & outputVerification, IncompatiblePins & incompatiblePins);
};

#endif	//	HEADER_GUARD___MRMODELVERIFIER_H__
