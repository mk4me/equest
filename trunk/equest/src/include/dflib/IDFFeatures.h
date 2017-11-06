/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:46
    filename: IDFFeatures.h
    author:   Mateusz Janiak
    
    purpose:  Zbiór interfejsów charakterystycznych dla przetwarzania
			  danych. U¿ywamy ich na kilki poziomach przetwarzania.
*********************************************************************/
#ifndef HEADER_GUARD___IDFFEATURES_H__
#define HEADER_GUARD___IDFFEATURES_H__

namespace df{

	// Interfejs obiektu, którego dzia³anie mo¿na wstrzywaæ/wznowiæ
	class IDFPausable
	{
    public:
        virtual ~IDFPausable() {}

		//! Pauzuj obiekt
		virtual void pause() = 0;
		//! Wznów obiekt
		virtual void resume() = 0;
		//! \ret Czy obiekt jest wstrzymany
		virtual const bool paused() const = 0;
	};

	//! Interfejs obiektu do którego mo¿na siê do³¹czyæ czekaj¹c na zakoñczenie jego dzia³¹nia
	class IDFJoinable
	{
    public:
        virtual ~IDFJoinable() {}

		//! Do³¹cz do biektu i czekaj na zakoñczenie jego dzia³¹nia
		virtual void join() = 0;
	};

	//! Interfejs obiektu którego przetwarzanie mo¿na anulowaæ
	class IDFCancelable
	{
    public:
        virtual ~IDFCancelable() {}

		//! Anuluj aktualne dzia³anie obiektu
		virtual void cancel() = 0;
	};

	//! Forward declaration
	class IDFLoggerHelper;

	//! Interfejs obiektu który chce mieæ mozliwoœæ logowania swojego stanu
	class IDFLoggable
	{
	public:        
        virtual ~IDFLoggable() {}

		//! Ustawia obiekt przez któy mo¿na logowaæ
		//! \param logger Obiekt przez który logujemy
		virtual void initLogger(IDFLoggerHelper * logger) = 0;
	};

	//! Interfejs obiektu którego stan mo¿na resetowaæ
	class IDFResetable
	{
	public:
        virtual ~IDFResetable() {}

		//! Resetuj stan obiektu do inicjalnego
		virtual void reset() = 0;
	};

}

#endif	//	HEADER_GUARD___IDFFEATURES_H__
