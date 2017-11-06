/********************************************************************
	created:  2014/11/19	10:49:53
	filename: ProtocolReadBufferHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__PROTOCOLREADBUFFERHELPER_H__
#define __HEADER_GUARD_IMU_COSTUME__PROTOCOLREADBUFFERHELPER_H__

#include <imucostumelib/Export.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CANopenFrame.h>
#include <utils/Debug.h>
#include <vector>

namespace imuCostume
{
	//! Klasa obsługująca dane z protokołu komunikacji
	class IMUCOSTUME_EXPORT ProtocolReadBufferHelper
	{
	public:

		//! Identyfikator wiadomości
		struct HeaderProxy
		{
			//! Cały identyfikator
			const uint8_t * base;
			//! \return długość danych
			inline uint8_t length() const
			{
				const auto length = base[1] >> 4;
				UTILS_ASSERT(length <= CANopenFrame::SizeLimits::MaxSize);
				return length;
			};
			//! Identyfikator węzła
			inline CANopenFrame::COBID cobID() const { return CANopenFrame::COBID{ uint16_t(((uint8_t)base[1] & 0x0F) << 8) | (uint16_t)base[0] }; };
		};

		//! Klasa obsługująca interpretację ramki can open z bufora
		class IMUCOSTUME_EXPORT CANopenFrameProxy
		{
		public:
			//! \param base Wskaźnik rozpoczęcia ramki
			//! \param buffer Bufor w którym ulokowana jest ramka
			CANopenFrameProxy(const uint8_t * base = nullptr);
			//! Destruktor
			~CANopenFrameProxy();

			//! \return Adres początku ramki
			const uint8_t * base() const;
			//! \return Nagłówek ramki
			HeaderProxy header() const;
			//! \return Wskaźnik bloku danych ramki
			const uint8_t * data() const;
			//! \return Czy można interpretować dane ramki
			bool empty() const;

		private:
			//! Wskaźnik poczatku bloku ramki
			const uint8_t * base_;
		};

	private:

		//! \param frameBasis Wskaźniki początków ramek CANopen w danych z kostiumu
		ProtocolReadBufferHelper(const std::vector<const uint8_t*> & frameBasis);

	public:

		//! Konstruktor kopiujący
		ProtocolReadBufferHelper();
		//! \param Other Kopiowany obiekt
		ProtocolReadBufferHelper(ProtocolReadBufferHelper & Other);
		//! \param Other Obiekt przejmowany
		ProtocolReadBufferHelper(ProtocolReadBufferHelper && Other);

		//! \param buffer Odczytany bufor z sieci
		//! \param length Długość danych w buforze
		static ProtocolReadBufferHelper create(const void * buffer, const uint16_t length);

		//! \return Ilość ramek w buforze
		std::vector<const uint8_t*>::size_type size() const;
		//! \return Czy są ramki w buforze
		bool empty() const;
		//! \param idx Indeks ramki o która pytamu
		//! \return Obiekt interpretujący nasza ramkę
		CANopenFrameProxy operator[](const uint16_t idx) const;

		//! \tparam Container Typ kontenera do którego dopisujemy
		//! \param container [out] Kontener do którego dopisujemy
		template<class Container>
		void appendContainer(Container & container)
		{
			for (const auto & b : frameBasis){
				container.push_back(CANopenFrame(b));
			}
		}

		//! \tparam Container Typ kontenera który tworzymy
		//! \return Kontener z ramkami CANopen
		template<class Container>
		Container createContainer()
		{
			Container container;
			for (const auto & b : frameBasis){
				container.push_back(CANopenFrame(b));
			}
			return container;
		}

	private:
		//! Wektor indeksów początków ramek w buforze
		std::vector<const uint8_t*> frameBasis;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__PROTOCOLREADBUFFERHELPER_H__
