/********************************************************************
	created:  2015/07/28	14:58:20
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__TYPES_H__
#define __HEADER_GUARD_DATAACCESSOR__TYPES_H__

namespace dataaccessor
{
	//! Typ cech kana³u
	enum FeatureType
	{
		Description = 0,	//! Opis zawartoœci kana³u, argumentów, wartoœci i ich jednostek
		Statistics,			//! Statystyki kana³u
		Function,			//! Opis czy dane reprezentuj¹ funkcjê
		FunctionDescription,//! Opis funkcji - parzystoœæ, nieparzystoœæ,  monotonicznoœæ, ...
		ArgumentBounded,	//! Opis zakresu argumentów
		ValueBounded,		//! Opis zakresu wartoœci
		UniformArguments,	//! Opis argumentów równooodalonych
		Periodic,			//! Opis okresu
		CustomBase = 100	//! Indeks dla w³asnych cech
	};

	//! Typ monotonicznoœci
	enum MonotonyType
	{
		NonMonotonic,	//! Niemonotonoczna
		NonDecreasing,	//! Niemalej¹ca
		Decreasing,		//! Œciœle malej¹ca
		Growing,		//! Œciœle rosn¹ca
		NonGrowing,		//! Nierosn¹ca
		Constant		//! Sta³a
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__TYPES_H__