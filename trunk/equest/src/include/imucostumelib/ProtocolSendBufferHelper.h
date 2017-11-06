/********************************************************************
	created:  2014/11/19	10:52:11
	filename: ProtocolSendBufferHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__PROTOCOLSENDBUFFERHELPER_H__
#define __HEADER_GUARD_IMU_COSTUME__PROTOCOLSENDBUFFERHELPER_H__

#include <imucostumelib/Export.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CANopenFrame.h>
#include <vector>

namespace imuCostume
{
	//! Klasa pomocnicza przy tworzniu bufora do wys³ania na kostium
	class IMUCOSTUME_EXPORT ProtocolSendBufferHelper
	{
	public:

		//! Struktura opisuj¹ca dynamiczny bufor danych
		struct IMUCOSTUME_EXPORT Buffer
		{
			//! \param bufor Dynamicznie zaalokowane dane
			utils::shared_array<uint8_t> buffer;
			//! \param length Rozmiar bufora danych
			uint16_t length;

			Buffer(Buffer && Other);
			Buffer() = default;
			Buffer(const Buffer& other) = default;

			Buffer& operator=(Buffer&& Other);
			Buffer& operator=(const Buffer& other) = default;
		};

	public:

		//! Konstruktor domyœlny
		ProtocolSendBufferHelper();
		//! Konstruktor kopiuj¹cy
		//! \param Other Obiekt kopiowany
		ProtocolSendBufferHelper(const ProtocolSendBufferHelper & Other);
		//! Konstruktor przenosz¹cy w³asnoœæ
		//! \param Other Obiekt przejmowany
		ProtocolSendBufferHelper(ProtocolSendBufferHelper && Other);

		//! \tpara Typ iteratora pozwalaj¹cego wy³uskaæ CANopenFrame
		//! \param first Iterator pierwszego elementu
		//! \param last Iterator za ostatnim elementem
		template<class Iter>
		ProtocolSendBufferHelper(Iter first, Iter last)
		{
			while (first != last){
				add(*first);
				++first;
			}
		}

		//! \param frame Ramka do spakowania przed wys³aniem
		void add(const CANopenFrame & frame);
		//! \param frame Ramka do spakowania przed wys³aniem
		//! \param length D³ugoœc ramki
		void add(const CANopenFrame & frame, const uint8_t length);
		//! \return Czy zbiór ramek pusty
		bool empty() const;
		//! \return Upakowane ramki CANopen, gotowe do wsy³ania w jednym pakiecie,
		//! powoduje ich usuniêcie z lokalnej listy
		Buffer createBuffer();
		//! \param frame [out] Ramka danych rozszerzana o d³ugoœ pola danych
		//! \param length D³ugoœc ca³ej ramki [B]
		static void updateFrameLength(CANopenFrame & frame, const uint8_t length = 10);

	private:
		//! Lista ramek do spakowania przed wys³aniem
		std::list<CANopenFrame> frames;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__PROTOCOLSENDBUFFERHELPER_H__
