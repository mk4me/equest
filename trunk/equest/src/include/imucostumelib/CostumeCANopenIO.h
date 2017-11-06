/********************************************************************
	created:  2014/09/03	18:39:36
	filename: CostumeCANopenIO.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__COSTUMECANOPENIO_H__
#define __HEADER_GUARD_IMU_COSTUME__COSTUMECANOPENIO_H__

#include <imucostumelib/Export.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CANopenFrame.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <list>

namespace imuCostume
{
	//! Klasa obs³uguj¹ca protoku³ kostiumu (odczyt danych z CANopen i zapis danych na CANopen)
	struct IMUCOSTUME_EXPORT CostumeCANopenIO
	{
	public:

		//! Struktura opisuj¹ca nag³ówek protoko³u
		struct HeaderStructure
		{
			//! Typ ramki
			uint8_t type;
			//! Numer sekwencyjny ramki
			uint8_t sequenceNumber;
		};

		//! Unia realizuj¹ca ró¿ne reprezentacje nag³ówka
		union Header
		{
			//! Forma bufora
			std::array<uint8_t, 2> buffer;
			//! Forma poszczególnych pól
			HeaderStructure structure;
			//! Zwiêz³a reprezentacja
			uint16_t value;
		};

		//! Struktura opisuj¹ca strukturê ca³ej ramki
		struct FrameStructure
		{
			//! Nag³ówek
			Header header;
			//! Sekcja danych
			std::array<uint8_t, CostumeRawIO::MaxDataSize - sizeof(Header)> data;
		};

		//! Unia realizuj¹ca ró¿ne reprezentacje ramki
		union Frame
		{
			//! Forma bufora
			CostumeRawIO::Buffer buffer;
			//! Forma poszczególnych pól
			FrameStructure structure;
		};

		//! Typ identyfikatora czasu ramki
		using Timestamp = uint32_t;

		//! Struktura opisuj¹ca wypakowane dane z protoko³u
		struct IMUCOSTUME_EXPORT Data
		{
			//! Znacnik czasowy
			Timestamp timestamp;
			//! Ramki CANopen
			std::list<CANopenFrame> CANopenFrames;
		};

	public:

		//! \param seqNumber Numer zawarty w komunikacie, powinien byæ równie¿ zawarty w odpowiadaj¹cej wiadomoœci zwrotnej
		//! \param data Dane do wys³ania - ramka powinna byæ poprawn¹ zgodnie ze specyfikacj¹ protoko³u komunikacyjnego
		//! \param length Rozmiar ramki CANopen [B]
		//! \param timeout Timeout dla wysy³ania danych
		//! \param costume Kostium dla którego wysy³amy dane na CANopen
		//! \return Czy transmisja siê powiod³a (tylko transfer danych do kostiumu a nie na CANopen)
		static bool send(const uint8_t seqNumber, const void * data,
			const uint16_t length, const uint16_t timeout, CostumeRawIO & costume);

		//! \param seqNumber Numer zawarty w komunikacie, powinien byæ równie¿ zawarty w odpowiadaj¹cej wiadomoœci zwrotnej
		//! \param data Dane do wys³ania - ramka powinna byæ poprawn¹ zgodnie ze specyfikacj¹ protoko³u komunikacyjnego
		//! \param length Rozmiar ramki CANopen [B]
		//! \return Czy odebrany komunikat jest potwierdzeniem odbioru
		static bool verifyReceiveConfirmation(const uint8_t seqNumber,
			const void * data, const uint16_t length);

		//! \param data Dane wyci¹gniête z naszego prtoko³u
		//! \param length D³ugoœæ danych [B]
		//! \return Wypakowane dane
		static Data extractData(const void * data, const uint16_t length);

		//! Metoda przygotowuje kostium do dzia³ania (w³¹cza wszystkie sensory w tryb operational przez NMT)
		//! \param sendBuffer Bufor w którym umieszczamy sformatowane ramki CANopen zgodnie z protoko³em
		static void prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer);
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__COSTUMECANOPENIO_H__