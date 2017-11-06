/********************************************************************
	created:  2012/12/17
	created:  17:12:2012   8:30
	filename: DFModelRunner.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___STDFMODELRUNNER_H__
#define HEADER_GUARD___STDFMODELRUNNER_H__

#include <dflib/IDFFeatures.h>
#include <stdexcept>


////////////////////////////////////////////////////////////////////////////////
namespace df{
	////////////////////////////////////////////////////////////////////////////////
	//! Forward declarations
	class IModelReader;
	class IDFLogger;
	class INode;


	//! Klasa realizująca wielowątkowe, potokowe przetwarzanie danych. Każdy węzeł modelu jest wrapowany przez
	//! dedykowaną klasę wraz z obsługującycm ją wątkiem. Przetwarzanie odbywa się od źródeł do sinków lub procesorów
	//! z niepodpiętymi wyjsciami. Tylko źródła są synchronizowane - czekają aż wszystkie razem skończć produkować dane
	//! do modelu, aby sprawdzić czy są w stanie wszystkie dostarczyć nowych danych. jeżeli tak ponownie moga produkowac dane,
	//! w przeciwnym wypadku czekamy na przetworzenie danych już znajdujących się w modelu.
	class STDFModelRunner 
	{
	public:
		//! Domyślny konstruktor
		STDFModelRunner();
		//! Destruktor
		~STDFModelRunner();

		//! Metoda startująca model, nieblokująca -> należy użyć join by czekać na koniec modelu
		//! \param model Model który będziemy przetwarzać
		//! \param logger Obiekt logujący stan przetwarzania
		//! \param tFactory Fabryka wątków - gdy pusta używamy domyslnej wynikającej z implementacji
		void start(IModelReader * model, IDFLogger * logger);

		//! Natychmiastowe przerwania przetwarzania
		//! Metoda blokująca - czeka na zakończenie wszystkich wątków
		void stop();


		//! \param reader Model który potencjalnie ma być uruchomiony
		//! \return Czy model można uruchomić za pomoca tego runnera
		static const bool verifyModel(const IModelReader * reader);

	private:
		//! Forward declaration
		class STDFModelRunnerImpl;

	private:
		//! Hidden implementation
		STDFModelRunnerImpl * privImpl;
	};
}

#endif	//	HEADER_GUARD___DFMODELRUNNER_H__
