/********************************************************************
	created:  2012/12/17
	created:  17:12:2012   8:30
	filename: DFModelRunner.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___DFMODELRUNNER_H__
#define HEADER_GUARD___DFMODELRUNNER_H__

#include <dflib/IDFFeatures.h>
#include <stdexcept>

//! Forward declaration
namespace threadingUtils
{
	class IThreadPool;
}

////////////////////////////////////////////////////////////////////////////////
namespace df{
	////////////////////////////////////////////////////////////////////////////////
	//! Forward declarations
	class IModelReader;
	class IDFLogger;
	class INode;

	//! wyjątki

	//! Wyjątek weryfikacji modelu pod kątem możliwości jego przetwarzania
	class ModelVerificationException : public std::runtime_error
	{
	public:
		ModelVerificationException(const char * e) : std::runtime_error(e) {}
	};

	//! Wyjątek inicjalizacji przetwarzania -> mapowania obiektów modelu na obiekty przetwarzające
	class ModelRunInitializationException : public std::runtime_error
	{
	public:
		ModelRunInitializationException(const char * e) : std::runtime_error(e) {}
	};

	//! Wyjątek startu przetwarzania -> przygotowanie i uruchomienie wątków obsługujących poszczególne obiekty modelu
	class ModelStartException : public std::runtime_error
	{
	public:
		ModelStartException(const char * e) : std::runtime_error(e) {}
	};

	//! Wyjątek przetwarzania modelu -> coś poszło nie tak w dataflow
	class ModelRunningException : public std::runtime_error
	{
	public:
		ModelRunningException(const char * e) : std::runtime_error(e) {}
	};

//	//! Klasa realizująca wielowątkowe, potokowe przetwarzanie danych. Każdy węzeł modelu jest wrapowany przez
//	//! dedykowaną klasę wraz z obsługującycm ją wątkiem. Przetwarzanie odbywa się od źródeł do sinków lub procesorów
//	//! z niepodpiętymi wyjsciami. Tylko źródła są synchronizowane - czekają aż wszystkie razem skończą produkować dane
//	//! do modelu, aby sprawdzić czy są w stanie wszystkie dostarczyć nowych danych. jeżeli tak ponownie moga produkowac dane,
//	//! w przeciwnym wypadku czekamy na przetworzenie danych już znajdujących się w modelu.
//	class DFModelRunner : public IDFPausable, public IDFJoinable
//	{
//	public:
//		//! Domyślny konstruktor
//		DFModelRunner();
//		//! Destruktor
//		~DFModelRunner();
//
//		//! Metoda startująca model, nieblokująca -> należy użyć join by czekać na koniec modelu
//		//! \param model Model który będziemy przetwarzać
//		//! \param logger Obiekt logujący stan przetwarzania
//		//! \param tFactory Fabryka wątków - gdy pusta używamy domyslnej wynikającej z implementacji
//		void start(IModelReader * model, IDFLogger * logger, threadingUtils::IThreadPool * tPool);
//
//		//! Natychmiastowe przerwania przetwarzania
//		//! Metoda blokująca - czeka na zakończenie wszystkich wątków
//		void stop();
//
//		//! \param node węzeł który chcemy pauzować
//		virtual void pause(INode * node);
//		//! \param node węzeł który chcemy przywrócić do pracy
//		virtual void resume(INode * node);
//
//		//! Pauzowanie przetwrarzania
//		virtual void pause();
//		//! Przywracanie przetwarzania
//		virtual void resume();
//		//! Czy przetwarzanie wstrzymane
//		virtual const bool paused() const;
//
//		//! Metoda blokująca, czeka aż model zakończy przetwarzanie w jakikolwiek sposób,
//		//! poprawnie bądź z błędem - wtedy rzuca wyjątkiem i zwalnia blokadę.
//		virtual void join();
//
//		//! \param reader Model który potencjalnie ma być uruchomiony
//		//! \return Czy model można uruchomić za pomoca tego runnera
//		static const bool verifyModel(const IModelReader * reader);
//
//	private:
//		//! Forward declaration
//		class DFModelRunnerImpl;
//
//	private:
//		//! Hidden implementation
//		DFModelRunnerImpl * privImpl;
//	};
}

#endif	//	HEADER_GUARD___DFMODELRUNNER_H__
