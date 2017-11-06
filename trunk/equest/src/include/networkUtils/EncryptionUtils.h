/********************************************************************
    created:  2015/12/07 19:00
    filename: EncryptionUtils.h
    author:   Wojciech Knieæ
    
    purpose: 
*********************************************************************/

#ifndef HEADER_GUARD_NETWORKUTILS__EncryptionUtils_h__
#define HEADER_GUARD_NETWORKUTILS__EncryptionUtils_h__

#include <networkUtils/Export.h>
#include <QtCore/QString>
#include <array>


namespace networkUtils {
	namespace EncryptionUtils 
	{
		//! D³ugoœæ klucza szyfruj¹cego
		static const unsigned int EncryptionKeyLength = 16;
		//! Wymusza 16 bajtowy klucz (+ 0 na koñcu)
		using EncryptionKey = std::array<unsigned char, EncryptionKeyLength + 1> ;
		//! Szyfruje tekst metod¹ aes
		//! \param txt tekst do szyfrowania
		//! \version 0.9.1
		//! \return zwraca zaszyfrowany teskt
		NETWORKUTILS_EXPORT QString encrypt(const QString& txt);
		//! Szyfruje tekst metod¹ aes
		//! \version 0.9.1
		//! \param txt tekst do szyfrowania
		//! \param key1 16 bajtowy klucz szyfrujacy np. {"aaaabbbbccccdddd"}
		//! \param key2 16 bajtowy klucz szyfrujacy
		//! \return zwraca zaszyfrowany teskt
		NETWORKUTILS_EXPORT QString encrypt(const QString& txt, const EncryptionKey& key1, const EncryptionKey& key2);
		//! Deszyfruje tekst metod¹ aes
		//! \version 0.9.1
		//! \param txt tekst do rozszyfrowania
		//! \return zwraca rozszyfrowany teskt
		NETWORKUTILS_EXPORT QString decrypt(const QString& txt);	  
		//! Deszyfruje tekst metod¹ aes
		//! \version 0.9.1
		//! \param txt tekst do rozszyfrowania
		//! \param key1 16 bajtowy klucz deszyfrujacy np. {"aaaabbbbccccdddd"}
		//! \param key2 16 bajtowy klucz deszyfrujacy
		//! \return zwraca zaszyfrowany teskt
		NETWORKUTILS_EXPORT QString decrypt(const QString& txt, const EncryptionKey& key1, const EncryptionKey& key2);
		//! Zamienia bufor na postaæ base64
		//! \version 0.9.1
		//! \param buffer bufor do konwersji
		//! \return String w postacji base64
		NETWORKUTILS_EXPORT QString toBase64(const QByteArray& buffer);
		//! Zamienia tekst w postaci base64 na odpowiednik w postacji bufora
		//! \version 0.9.1
		//! \param buffer teskt do zamiany
		//! \return bufor powsta³y z base64
		NETWORKUTILS_EXPORT QByteArray fromBase64(const QString& buffer);
		//! Zamienia QString na typ EncryptionKey
		//! \version 0.9.1
		//! \param key Klucz do konwersji, musi mieæ odpowiedni¹ d³ugoœæ (16 znaków)
		//! \return Klucz w postaci EncryptionKey (std::array<char, 16 + 1>)
		NETWORKUTILS_EXPORT EncryptionKey convertEncryptionKey(const QString& key);
	};
}

#endif


