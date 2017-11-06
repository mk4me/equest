/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   9:35
    filename: MRModelLogicVerifier.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MRMODELLOGICVERIFIER_H__
#define HEADER_GUARD___MRMODELLOGICVERIFIER_H__

#include <list>

//! Forward declarations
namespace df {

	class IModelReader;
	class INode;
	class IProcessingNode;
}

//! Klasa realizuj¹ca weryfikacjê logiki modelu na potrzeby przetwarzania.
//! Model sprawdzany jest pod k¹tem pod³¹czonych wêz³ów (ka¿dy wêze³ powinien
//! byæ podpiêty, elementy przetwarzaj¹ce musz¹ mieæ podpiête wejœcie)
class MRModelLogicVerifier
{
public:
	//! Typ opisuj¹cy weryfikacje logiki modelu
	struct ModelVerificationData
	{
		//! Lista wêz³ów niepod³¹czonych
		std::list<df::INode*> unconnectedNodes;
		//! Lista elementów przetwarzaj¹cych z niepodpiêtymi wejœciami
		std::list<df::IProcessingNode*> artificialSources;
	};

public:

	//! \param mode Model do weryfikacji logiki
	//! \patam logicVerification [out] Stan weryfikacji logiki
	//! \return Czy logika modelu zakoñczona sukcesem
	static const bool verifyModelLogic(df::IModelReader * model, ModelVerificationData & logicVerification);
};

#endif	//	HEADER_GUARD___MRMODELLOGICVERIFIER_H__
