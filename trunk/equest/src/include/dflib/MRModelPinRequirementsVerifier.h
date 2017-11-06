/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   8:48
    filename: MRModelPinRequirementsVerifier.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MRMODELPINREQUIREMENTSVERIFIER_H__
#define HEADER_GUARD___MRMODELPINREQUIREMENTSVERIFIER_H__

#include <list>

//! Forward declarations
namespace df{

	class IExtendedInputPin;
	class IExtendedOutputPin;

	class ISinkNode;
	class ISourceNode;

	class IModelReader;
}

//! Klasa realizuj¹ca weryfikacjê opisu przejœæ wêz³ów. Operacja ta obejmuje
//! sprawdzenie czy piny s¹ opisane rozszerzonymi interfejsami i czy te wymagania
//! s¹ spe³nione.
class MRModelPinRequirementsVerifier
{
public:
	//! Typ opisuj¹cy agregat pinów wejœciowych z niespe³nionymi zale¿noœciami (wymagane a niepodpiête)
	typedef std::list<df::IExtendedInputPin*> UnsatisfiedInputs;
	//! Typ opisuj¹cy agregat pinów wyjœciowych z niespe³nionymi zale¿noœciami (niepidpiête zale¿noœci a pin podpiêty)
	typedef std::list<df::IExtendedOutputPin*> UnsatisfiedOutputs;

	//! Typ opisuj¹cy weryfikacjê opisu przejœæ
	struct UnsatisfiedPins
	{
		//! Piny wejœciowe z niespe³nionymi zale¿noœciami
		UnsatisfiedInputs unsatisfiedInputs;
		//! Piny wyjœciowe z niespe³nionymi zale¿noœciami
		UnsatisfiedOutputs unsatisfiedOutputs;
	};

public:

	//! \param model Model do weryfikacji
	//! \param unsatisfiedPins [out] Stan weryfikacji opisu przejsæ modelu
	//! \return Czy model jest poprawny pod k¹tem opisu przejœæ
	static const bool verifyModelPinRequirements(df::IModelReader * model, UnsatisfiedPins & unsatisfiedPins);

private:
	//! \param node Wêze³ sink do weryfikacji
	//! \param unsatisfiedInputs [out] Wejœcia z niespe³nonymi wymaganiami
	static void verifyInputPinRequirements(df::ISinkNode * node, UnsatisfiedInputs & unsatisfiedInputs);
	//! \param node Wêze³ Ÿród³owy do weryfikacji
	//! \param unsatisfiedOutputs [out] Wyjœcia z niespe³nonymi wymaganiami
	static void verifyOutputPinRequirements(df::ISourceNode * node, UnsatisfiedOutputs & unsatisfiedOutputs);
};

#endif	//	HEADER_GUARD___MRMODELPINREQUIREMENTSVERIFIER_H__
