/********************************************************************
	created:  2014/09/24	16:30:03
	filename: SQLiteBLOBStreamBufferT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__

#include <string>
#include <streambuf>
#include <iosfwd>
#include <sqliteUtils/BufferPolicyT.h>

namespace sqliteUtils
{
	//! Wzorzec klasy obslugujacej sturmieniowe IO do blobow bazy sqlite
	//! \tparam _Elem Elementy sk³adowane w blobach
	//! \tparam _Traits Cechy obiektu typu _Elem
	//! \tparam _BufferPolicy Polityka zarzadzania buforem danych
	template<typename Elem = uint8_t,
		typename Traits = std::char_traits<Elem>>
	class SQLiteBLOBStreamBufferT : public std::basic_streambuf<Elem, Traits>
	{
	public:

		using BufferPolicy = IBufferPolicyT<Elem>;
		using BufferPolicyPtr = utils::unique_ptr<BufferPolicy>;

	private:

		enum
		{	// constants for bits in stream state		
			_Constant = 2,	// set if character array nonmutable
			_Noread = 4,	// set if character array cannot be read
			_Append = 8,	// set if all writes are appends
			_Atend = 16		// set if initial writes are appends
		};

		//! Internal state
		using Strstate = int;

		//! Typ inta
		using int_type = typename Traits::int_type;
		//! Typ pozycji
		using pos_type = typename Traits::pos_type;
		//! Typ offsetu
		using off_type = typename Traits::off_type;

	public:
		//! Typ mojego bufora
		using _Myt = SQLiteBLOBStreamBufferT<Elem, Traits>;
		//! Typ bufora strumienia
		using _Mysb = std::basic_streambuf<Elem, Traits>;
		//! Typ mojego stanu
		using _Myst = typename Traits::state_type;

		static const pos_type _BADOFF; //za cppreference, -1 jest uzywane do okreslania wadliwych offsetow;

	public:
		//! Konstruktor
		//! \param _Mode W jakim trybie otwarty jest strumieñ
		//! \param db Baza na której bêdziemy dzia³aæ
		//! \param dbName Nazwa bazy na której dzia³amy (https://sqlite.org/c3ref/blob_open.html)
		//! \param table Nazwa tabeli
		//! \param column Nazwa kolumny
		//! \param rowID Unikalny identyfikator wiersza w zadanej tabeli		
		SQLiteBLOBStreamBufferT(BufferPolicyPtr && buffer, sqlite3 * db, const std::string & table,
			const std::string & column, const sqlite3_int64 rowID, const std::string & dbName = "main",
			std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

		//! Destruktor wirtualny
		virtual ~SQLiteBLOBStreamBufferT();

	protected:

		virtual std::streamsize showmanyc();

		virtual int_type pbackfail(int_type = Traits::eof());

		virtual int_type overflow(int_type _Meta = Traits::eof());

		virtual int_type underflow();

		virtual pos_type seekoff(off_type _Off,
			std::ios_base::seekdir _Way,
			std::ios_base::openmode _Which = std::ios_base::in | std::ios_base::out);

		virtual pos_type seekpos(pos_type _Ptr,
			std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out);

		virtual int sync();

	private:

		//! \return Rozmiar bufora
		int streamBufferSize() const;
		//! \return Rozmiar bufora
		int blobBufferSize() const;

		//! \param idx Globalny index
		//! \return Maksymalny index bufora uwzglêdniaj¹cy zakres strumienia i rozmiar bufora
		int blobMaxIDX(const int idx) const;

		//! \return Maksymalny index globalny bufora do odczytu
		int blobReadMaxIDX() const;

		//! \return Maksymalny index globalny bufora do zapisu
		int blobWriteMaxIDX() const;

		//! \param blobBase Globalna baza
		//! \param bufferBase Baza lokalna bufora
		//! \param bufferPos Lokalna pozycja bufora
		//! \return Globalna pozycja strumienia
		static int blobGlobalIDX(const int blobBase, const Elem * bufferBase,
			const Elem * bufferPos);

		//! \return Globalna pozycja dla zapisu
		int blobGlobalWriteIDX() const;

		//! \return Globalna pozycja dla odczytu
		int blobGlobalReadIDX() const;

		//! \param pos Pozycja we wspolrzednych bloba [B]
		//! \return Pozycja we wspolrzednych strumienia
		static int toStreamPos(int pos);
		//! \param pos Pozycja we wspolrzednych strumienia [_Elem]
		//! \return Pozycja we wspolrzednych bloba
		static int toBlobPos(int pos);

		//! Aktualizuje globalny indeks ostatniego elementu strumienia
		void updateBlobStreamEnd();

		//! Metoda wypelnia bufor zawartoscia bloba
		//! \param buffer Wskaznik poczatku bufora gdzie moge czytac dane z bloba
		//! \param bufferSize Ile maksymalnie moge wczytac do bufora
		//! \param blob Blob z ktorego czytam
		//! \param blobIDX Wskaznik w obrebie bloba od ktorego zaczynam czytac
		//! \param blobSize Rozmiar bloba
		//! \return Ilosc znakow odczytanych z bloba
		int refillBuffer(const int blobIDX, Elem * bufferBase);

		//! Metoda wypycha lokalne zmiany do bloba - synchronizuje
		//! \return Czy udalo sie dane zapisac do bloba
		bool flushBuffer(const Elem * base, const int count,
			const int blobIDX);

		//! Metoda generuje nazwe tabeli dla zapytaia aktualizujacego bloba
		//! \param dbName Nazwa bazy danych
		//! \param table Nazwa tabeli
		//! \return Nazwa tabeli dla zapytania update
		static std::string updateTableName(const std::string & dbName,
			const std::string & table);

		//! \param _Mode tryb otwarcia strumienia
		//! \return Wewnêtrzny tryb otwarcia strumienia
		static Strstate _Getstate(std::ios_base::openmode _Mode);

	private:

		//! ------------ OPIS DANYCH W SQLITE ------------------
		const std::ios_base::openmode mode_;
		//! Uchwyt bazy danych
		sqlite3 * db;
		//! Uchwyt bloba
		UniqueWrapperT<sqlite3_blob> blob;
		//! Nazwa bazy
		std::string dbName;
		//! Nazwa tabeli
		std::string table;
		//! Nazwa kolumny
		std::string column;
		//! Identyfikator wiersza
		sqlite3_int64 rowID;
		//! Aktualny rozmiar bloba
		int blobSize;

		//! ----------------- STAN BUFORÓW --------------------
		//! Bufor
		BufferPolicyPtr buffer;
		//! Aktualny indeks wewn¹trz bloba dla bufora odczytu
		int blobReadBase;
		//! Aktualny indeks wewn¹trz bloba dla bufora zapisu
		int blobWriteBase;
		//! Czy by³ zapis przez putback
		bool putBackWrite;

		//! -------------- STAN STRUMIENIA ---------------------
		Strstate Mystate;	// Stan strumienia
		int blobSeekhigh;		// Faktyczny rozmiar strumienia - uwzglêdnia rozmiar bloba i stan zape³nienia bufora
	};
	template<typename Elem, typename Traits>	
	const typename SQLiteBLOBStreamBufferT<Elem, Traits>::pos_type SQLiteBLOBStreamBufferT<Elem, Traits>::_BADOFF = typename SQLiteBLOBStreamBufferT<Elem, Traits>::pos_type(-1); //za cppreference, -1 jest uzywane do okreslania wadliwych offsetow

	#include <sqliteUtils/SQLiteBLOBStreamBufferTImpl.h>
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__
