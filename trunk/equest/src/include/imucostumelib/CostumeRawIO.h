/********************************************************************
	created:  2014/09/03	09:09:11
	filename: CostumeRawIO.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__COSTUMERAWIO_H__
#define __HEADER_GUARD_IMU_COSTUME__COSTUMERAWIO_H__

#include <imucostumelib/Export.h>
#include <cstdint>
#include <string>
#include <list>
#include <utils/SmartPtr.h>
#include <array>

namespace imuCostume
{
	//! Klasa realizuj¹ca surow¹ komunikacjê z kostiumem po UDP,
	//! pozwalaj¹ca wykryæ dostêpne kostiumu i skonfigurowaæ ich
	//! czêstotliwoœæ próbkowania danych
	class IMUCOSTUME_EXPORT CostumeRawIO
	{
	private:
		//! Forward declaration realizacji
		class CostumeRawIOImpl;	

	public:

		//! Rozmiar bufora na dane z kostiumu
		static const uint16_t MaxDataSize = 1024;

		//! Typ surowej ramki danych wysy³anej przez kostium
		using Buffer = std::array<uint8_t, MaxDataSize>;

		//! Struktura identyfikuj¹ca kostium w sieci
		struct CostumeAddress
		{
			std::string ip;		//! Adres IP
			unsigned int port;	//! Port

			bool operator<(const imuCostume::CostumeRawIO::CostumeAddress & b) const
			{
				if (ip < b.ip){
					return true;
				}
				else if (ip == b.ip && port < b.port){
					return true;
				}

				return false;
			}

			bool operator==(const imuCostume::CostumeRawIO::CostumeAddress & b) const
			{
				return ((port == b.port) && (ip == b.ip));
			}
		};	

	public:
		//! \param ip Adres kostiumu
		//! \param port Port na którym kostium s³ucha
		CostumeRawIO(const std::string & ip, const unsigned int port = 1234);
		//! \param Other Kopiowany kostium
		CostumeRawIO(const CostumeRawIO & Other);
		//! Destruktor
		~CostumeRawIO();

		//! \return Adres kostiumu
		const std::string & ip() const;
		//! \return Port na którym mamy kostium
		unsigned int port() const;

		//! \param buffer [out] Miejsce na odebrane dane
		//! \param length [out] D³ugoœæ odebranych danych [B]
		//! \param timeout Dopuszczalny czas oczekiwania na dane [ms] - 0 oznacza nieskoñczonoœæ
		//! \return Prawda jeœli odebrano poprawn¹ ramke danych
		bool receive(Buffer & buffer, uint16_t & length, const uint16_t timeout = 0);

		//! \param data Dane do wys³ania
		//! \param length Ile danych wys³aæ z bufora [B]
		//! \param timeout Dopuszczalny czas oczekiwania na dane [ms] - 0 oznacza nieskoñczonoœæ
		//! \return Prawda jeœli wys³ano dane
		bool send(const void * data, const uint16_t length, const uint16_t timeout = 0);

		//! \return Lista dostêpnych kostiumów
		static std::list<CostumeAddress> listAvailableCostumes();

		//! \param delay Czas przerwy pomiedzy probkami [ms]
		//! \param timeout Czas na wys³anie [ms]
		void setSamplingDelay(const uint32_t delay, const uint16_t timeout = 0);
		//! \return Czas przerwy pomiedzy probkami [ms]
		uint32_t samplingDelay() const;

	private:
		//! Obiekt implementujacy funkcjonalnoœæ kostiumu
		utils::shared_ptr<CostumeRawIOImpl> impl;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__COSTUMERAWIO_H__