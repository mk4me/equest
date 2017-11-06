/********************************************************************
	created:  2014/12/11	08:48:23
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATICUTILS__TYPES_H__
#define __HEADER_GUARD_KINEMATICUTILS__TYPES_H__

#include <array>
#include <string>

namespace kinematicUtils
{
	//! Struktura opisuje kana³ danych dla ruchu
	struct ChannelType
	{
		//! Typ wyliczeniowy kana³u
		enum Type
		{
			TX = 0,	//! Translacja wzd³ó¿ osi X
			TY = 1, //! Translacja wzd³ó¿ osi Y
			TZ = 2, //! Translacja wzd³ó¿ osi Z
			RX = 3, //! Rotacja wokó³ osi X
			RY = 4, //! Rotacja wokó³ osi Y
			RZ = 5, //! Rotacja wokó³ osi Z
			CUSTOM_CHANNEL_BASE = 6 //! Offset dla customowych kana³ów
		};

		//! \param type Typ kana³u
		//! \return Nazwa kana³u w formie napisu
		static std::string toString(const Type type);
		//! \param name Nazwa kana³u w formie napisu
		//! \return Typ kana³u
		static Type fromString(const std::string & name);
	};

	//! Typ okreœlaj¹cy kana³
	using Channel = int;
	//! Typ okreœlaj¹cy 3 kana³y
	using ChannelTriplet = std::array<Channel, 3>;

	//! Struktura opisuj¹ca kolejnoœæ rotacji osi
	struct AxisOrder
	{
		//! Typ wyliczeniowy opisuj¹cy kolejnoœæ rotacji osi
		enum Type
		{
			XYX,
			XYZ,
			XZX,
			XZY,
			YXY,
			YXZ,
			YZX,
			YZY,
			ZXY,
			ZXZ,
			ZYX,
			ZYZ
		};

		//! \param axisOrder Kolejnoœæ rotacji osi
		//! \return Napis okreœlaj¹cy kolejnoœæ osi
		static std::string toString(const AxisOrder::Type axisOrder);
		//! \param axisOrderName Napis okreœlaj¹cy kolejnoœc rotacji osi 
		//! \return Kolejnoœæ rotacji osi
		static Type fromString(const std::string & axisOrderName);
		//! \param channelTriplet Trójka kana³ów
		//! \return Kolejnoœæ rotacji osi
		static Type fromChannelTriplet(const ChannelTriplet & channelTriplet);
		//! \param axisOrder Kolejnoœæ rotacji osi
		//! \return Trójka z kana³ami
		static ChannelTriplet toChannelTriplet(const Type axisOrder);
	};
}

#endif	// __HEADER_GUARD_KINEMATICUTILS__TYPES_H__