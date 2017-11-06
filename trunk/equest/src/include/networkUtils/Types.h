/********************************************************************
	created:  2014/05/14	20:41:14
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_NETWORKUTILS__TYPES_H__
#define __HEADER_GUARD_NETWORKUTILS__TYPES_H__

namespace networkUtils
{
	//! Mechanizm weryfikacji hosta dla bezpiecznych po³¹czeñ przez SSL za pomoca curla i openssl
	enum SSLHostVerification {
		HVNone = 0,     //! Brak weryfikacji
		HVExist = 1,   //! Wpis o CN przynajmniej istnieje
		HVMatch = 2,    //! Wpis o CN istnieje i jest zgodny
	};
}

#endif	// __HEADER_GUARD_NETWORKUTILS__TYPES_H__