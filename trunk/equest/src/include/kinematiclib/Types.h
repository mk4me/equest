/********************************************************************
	created:  2015/05/04	12:38:57
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__TYPES_H__
#define __HEADER_GUARD_KINEMATIC__TYPES_H__

namespace kinematic
{
	//! Typ danych
	enum DataType
	{
		Local = 0,	//! Lokalne
		Global = 1	//! Globalne
	};	

	//! \tparam T Typ bazowy, wrapowany
	//! \tparam Type Wartość reprezentująca typ
	template<typename T, int Type>
	//! Klasa wrapująca dane ruchu (stan, zmianę) dodająca informacje o charakterze tych danych - lokalne|globalne
	class Data
	{
		static_assert(Type == Local || Type == Global, "Type must be Local or Global");

	public:
		//! Konstruktor domyslny
		Data() {}
		//! Konstruktor kopiujący
		//! \param data Kopiowane dane
		Data(const Data & data) : data_(data.data_) {}
		//! Konstruktor przenoszący
		//! \param data Przenoszone dane
		Data(Data && data) : data_(std::move(data.data_)) {}

		Data& operator=(const Data& d)
		{
			data_ = d.data_;
			return *this;;
		}
		//! Konstruktor
		//! \param data Kopiowane dane
		explicit Data(const T & data) : data_(data) {}
		//! Konstruktor
		//! \param data Przenoszone dane
		explicit Data(T && data) : data_(std::move(data)) {}
		//! Destruktor wirtualny
		virtual ~Data() {}

		explicit inline operator T& () { return data_; }

		explicit inline operator const T& () const { return data_; }

		inline T& data() { return data_; }

		inline const T& data() const { return data_; }

	public:

		enum {
			type = Type	//! Typ danych
		};

	private:
		//! Datne
		T data_;
	};

	//! \tparam Base Typ bazowy
	template<typename Base>
	//! Wrapper dla reprezentacji danych lokalnych
	using LocalData = Data < Base, Local > ;

	//! \tparam Base Typ bazowy
	template<typename Base>
	//! Wrapper dla reprezentacji danych globalnych
	using GlobalData = Data < Base, Global > ;
}

#endif	// __HEADER_GUARD_KINEMATIC__TYPES_H__
