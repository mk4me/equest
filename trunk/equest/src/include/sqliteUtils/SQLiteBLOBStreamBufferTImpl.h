/********************************************************************
	created:  2014/09/24	17:20:37
	filename: SQLiteBLOBStreamBufferTImpl.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/

#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__
	#error "This header should not be included directly without SQLiteBLOBStreamBufferT. This is just implementation"
#endif

#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERTIMPL_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERTIMPL_H__

#include <sqliteUtils/SQLiteDB.h>
#include <sqliteUtils/SQLiteBLOB.h>
#include <sqliteUtils/SQLitePreparedStatement.h>
#include <sqliteUtils/SQLiteBLOBStreamBufferT.h>
#include <streambuf>
#include <iosfwd>
#include <boost/format.hpp>


template<typename Elem, typename Traits>
SQLiteBLOBStreamBufferT<Elem, Traits>::SQLiteBLOBStreamBufferT(BufferPolicyPtr && buff,
	sqlite3 * db, const std::string & table,
	const std::string & column, const sqlite3_int64 rowID,
	const std::string & dbName, std::ios_base::openmode mode) : _Mysb(), buffer(std::move(buff)),
	db(db), blob(SQLiteBLOB::open(db, table, column, rowID, mode & std::ios_base::out, dbName)),
	mode_(mode), dbName(dbName), table(table), column(column),
	rowID(rowID), blobReadBase(0), blobWriteBase(0), blobSize(0),
	blobSeekhigh(0), Mystate(_Getstate(mode)), putBackWrite(false)
{
	if (buffer == nullptr){
		throw std::runtime_error("Uninitialized buffer");
	}
	const auto initialSize = sqlite3_blob_bytes(blob);
	blobWriteBase = ((mode & std::ios_base::app) ? initialSize : 0);
	blobSize = blobSeekhigh = initialSize;
}

template<typename Elem, typename Traits>
SQLiteBLOBStreamBufferT<Elem, Traits>::~SQLiteBLOBStreamBufferT()
{
	sync();
}

template<typename Elem, typename Traits>
typename SQLiteBLOBStreamBufferT<Elem, Traits>::int_type SQLiteBLOBStreamBufferT<Elem, Traits>::pbackfail(typename SQLiteBLOBStreamBufferT<Elem, Traits>::int_type _Meta)
{
	if ((blobReadBase == 0 && (_Mysb::gptr() == nullptr || _Mysb::gptr() == _Mysb::eback()))
		|| (!Traits::eq_int_type(Traits::eof(), _Meta)
		&& !Traits::eq(Traits::to_char_type(_Meta), _Mysb::gptr()[-1])
		&& Mystate & _Constant))
		return (Traits::eof());	// can't put back, fail
	else
	{	// back up one position and store put-back character
		const auto pos = toStreamPos(blobGlobalReadIDX());
		auto newPos = std::max(pos - streamBufferSize(), 0);
		seekpos(newPos, std::ios_base::in);
		if (_Mysb::gptr() == nullptr){
			underflow();
		}
		seekpos(pos-1, std::ios_base::in);
		if (!Traits::eq_int_type(Traits::eof(), _Meta)){
			*_Mysb::gptr() = Traits::to_char_type(_Meta);
			putBackWrite = true;
		}
		return (Traits::not_eof(_Meta));
	}
}

template<typename Elem, typename Traits>
typename SQLiteBLOBStreamBufferT<Elem, Traits>::int_type SQLiteBLOBStreamBufferT<Elem, Traits>::overflow(int_type _Meta)
{
	if ((Mystate & _Constant) || Traits::eq_int_type(Traits::eof(), _Meta) == true ||
		((buffer->begin() == nullptr) && (buffer->updateSize(std::max(toStreamPos(blobSize), (int)DefaultBufferSize)) < 1))){
		return (Traits::eof()); // read only mode or no open blob, or EOF or faile dto create buffer
	}

	//czy cos było pisane do bufora?
	if (_Mysb::pptr() != nullptr){
		// musze zrzucic bufor do strumienia
		//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
		const auto oldBlobSize = blobSize;

		const auto offset = std::min<int>(_Mysb::epptr() - _Mysb::pbase(), toStreamPos(blobSeekhigh - blobWriteBase));

		//zapisuje bufor w stumieniu
		if (flushBuffer(_Mysb::pbase(), offset, blobWriteBase) == false){
			return (Traits::eof());	// zapis sie nie powiodlo, fail
		}

		//czy zmienił się rozmiar?
		if (oldBlobSize < blobSize){
			//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
			buffer->updateSize(toStreamPos(blobSize));
		}
		//rozmiar sie nie zmienił
		//sprawdzam gdzie teraz w strumieniu jestem
		else if (blobWriteBase < blobSize){
			//cały czas piszę wewnątrz bloba, moge odświezyć zawartość bufor ze strumienia
			auto s = refillBuffer(blobWriteBase, buffer->begin());

			//czy cos pobralem albo jakis blad?
			if (s < 1){
				return (Traits::eof()); // nic nie pobralem wiec pewnie koniec
			}
		}

		// jeżeli cos czytałem wcześniej to muszę zapamiętać i wyzerować jeżeli to nie ten sam obszar
		if (_Mysb::gptr() != nullptr){
			blobReadBase += toBlobPos(std::distance(_Mysb::eback(), _Mysb::gptr()));
			_Mysb::setg(buffer->begin(), nullptr, buffer->end());
		}

		blobWriteBase += toBlobPos(offset);

		//ustawiam nowy bufor i jego stan
		_Mysb::setp(buffer->begin(), buffer->end());
	}
	//nie pisałem do bufora zapisu
	else{

		if (putBackWrite == true){
			// musze zrzucic bufor do strumienia
			//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
			const auto oldBlobSize = blobSize;

			//zapisuje bufor w stumieniu
			if (flushBuffer(_Mysb::eback(), std::min<int>(_Mysb::egptr() - _Mysb::eback(), toStreamPos(blobSeekhigh - blobReadBase)), blobReadBase) == false){
				return (Traits::eof());	// zapis sie nie powiodlo, fail
			}

			//czy zmienił się rozmiar?
			if (oldBlobSize < blobSize){
				auto ss = streamBufferSize();
				//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
				if (ss < buffer->updateSize(toStreamPos(blobSize))){
					blobReadBase = blobGlobalReadIDX();
					_Mysb::setg(buffer->begin(), nullptr, buffer->end());
				}
			}
		}

		if (_Mysb::gptr() != nullptr){
			//czytałem coś z bufora a chcę bufor do zapisu
			//sprawdzam czy obszary odczytu/zapisu się nie zazębiają
			if (blobWriteBase < blobReadBase || blobWriteBase >= blobReadMaxIDX()){

				//przeładowuje bufor nowymi danymi, będzie po nich pisać
				auto s = refillBuffer(blobWriteBase, buffer->begin());

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}

				//niestety, obszary są rozłączne, muszę zapamiętac stan bufora do odczytu i zresetować jego obszar
				blobReadBase += toBlobPos(std::distance(_Mysb::eback(), _Mysb::gptr()));
				_Mysb::setg(buffer->begin(), nullptr, buffer->end());

				//ustawiam nowy bufor i jego stan
				_Mysb::setp(buffer->begin(), buffer->end());
			}
			else{
				//obszary pokrywają się - wystarczy poprzestawiać wskaźniki
				auto diff = blobWriteBase - blobReadBase;
				blobWriteBase = blobReadBase;
				//poprawiam obszar do zapisu
				_Mysb::setp(buffer->begin() + diff, buffer->end());
			}
		}
		else{
			//nie było zapisu i odczytu albo oba obszary nieaktyne po seek

			if (blobSize > 0 && blobWriteBase != blobSize){
				//uzupelniam bufor
				auto s = refillBuffer(blobWriteBase, buffer->begin());

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}
			}

			//ustawiam nowy bufor i jego stan
			setp(buffer->begin(), buffer->end());
		}
	}
	//zapisujemy
	*_Mysb::pptr() = Traits::to_char_type(_Meta);
	 _Mysb::pbump(1);
	//aktualizuje koniec strumienia dla poprawnego odczytu całego strumienia podczas zapisu
	updateBlobStreamEnd();

	return (_Meta);
}

template<typename Elem, typename Traits>
typename SQLiteBLOBStreamBufferT<Elem, Traits>::int_type SQLiteBLOBStreamBufferT<Elem, Traits>::underflow()
{
	// get an element from stream, point on it
	if (blobSeekhigh < 1 || (Mystate & _Noread) ||
		((buffer->begin() == nullptr) && (buffer->updateSize(std::max(toStreamPos(blobSize), (int)DefaultBufferSize)) < 1))){
		return (Traits::eof());	// no open blob or no data to read or failed to create buffer, fail
	}
	auto oldSeekhigh = blobSeekhigh;
	updateBlobStreamEnd();

	//czy konieczne jest przeładowanie bufora?
	bool refill = true;

	//sprawdzam czy byly zapisy
	if (_Mysb::pptr() != nullptr){
		//musze sprawdzic czy to gdzie sie chce znalezc wypada poza aktualny obszar - jezeli tak to musze zapisac dane, odswiezyc bufor
		if ((blobReadBase < blobWriteBase) || (blobGlobalWriteIDX() <= blobReadBase)){

			const auto oldBlobSize = blobSize;

			if (flushBuffer(_Mysb::pbase(), std::min<int>(_Mysb::epptr() - _Mysb::pbase(), toStreamPos(blobSeekhigh - blobWriteBase)), blobWriteBase) == false){
				blobSeekhigh = oldSeekhigh;
				return (Traits::eof());	// zapis sie nie powiodlo, fail
			}

			if (oldBlobSize < blobSize){
				//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
				buffer->updateSize(blobSize);
			}

			//wypadam poza
			blobWriteBase = blobGlobalWriteIDX();

			_Mysb::setp(buffer->begin(), buffer->end());
		}
		else{
			//zazębiają się obszary, wystarczy wskaźniki poprawić
			refill = false;
			//maks rozmiar do odczytu
			const auto size = ((blobWriteBase + blobBufferSize()) <= blobSeekhigh) ? blobBufferSize() : blobSeekhigh - blobGlobalWriteIDX();
			//poprawiam obszar do odczytu
			_Mysb::setg(_Mysb::pbase(), _Mysb::pbase() + toStreamPos(blobReadBase - blobWriteBase), buffer->begin() + toStreamPos(size));
			blobReadBase = blobWriteBase;
		}
	}
	else if (putBackWrite == true){
		// musze zrzucic bufor do strumienia
		//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
		const auto oldBlobSize = blobSize;

		//zapisuje bufor w stumieniu
		if (flushBuffer(_Mysb::eback(), std::min<int>(_Mysb::egptr() - _Mysb::eback(), toStreamPos(blobSeekhigh - blobReadBase)), blobReadBase) == false){
			blobSeekhigh = oldSeekhigh;
			return (Traits::eof());	// zapis sie nie powiodlo, fail
		}

		//czy zmienił się rozmiar?
		if (oldBlobSize < blobSize){
			//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
			buffer->updateSize(toStreamPos(blobSize));
		}
	}

	if (refill == true){

		auto readIDX = blobReadBase;

		if (_Mysb::gptr() != nullptr){
			readIDX += toBlobPos(std::distance(_Mysb::eback(), _Mysb::egptr()));
		}

		//uzupelnam bufor
		auto s = refillBuffer(readIDX, buffer->begin());

		//czy cos pobralem albo jakis blad?
		if (s < 1){
			blobSeekhigh = oldSeekhigh;
			return (Traits::eof()); // nic nie pobralem wiec pewnie koniec
		}

		if (readIDX > blobReadBase){
			blobReadBase = readIDX;
		}

		//inicjuje obszar do odczytu
		_Mysb::setg(buffer->begin(), buffer->begin(), buffer->begin() + toStreamPos(s));
	}

	// no codecvt facet, just get it
	Elem _Ch = *(_Mysb::gptr());
	return Traits::to_int_type(_Ch);
}

template<typename Elem, typename Traits>
std::streamsize SQLiteBLOBStreamBufferT<Elem, Traits>::showmanyc()
{
	std::streamsize ret = 0;

	if (blobSeekhigh == 0){
		return -1;
	}

	if (_Mysb::gptr() == nullptr){
		ret = std::min(streamBufferSize(), toStreamPos(blobSeekhigh - blobReadBase));
	}
	else if(_Mysb::gptr() == _Mysb::egptr()){
		if (blobReadBase == blobSize || (blobReadBase + toBlobPos(std::distance(_Mysb::eback(), _Mysb::egptr()))) == blobSeekhigh){
			ret = -1;
		}
		else{
			ret = std::min(streamBufferSize(), std::max(toStreamPos(blobSeekhigh - blobReadBase - blobBufferSize()), 0));
			if (ret == 0){
				ret = -1;
			}
		}
	}
	else{
		ret = std::distance(_Mysb::gptr(), _Mysb::egptr());
	}

	return ret;
}

template<typename Elem, typename Traits>
typename SQLiteBLOBStreamBufferT<Elem, Traits>::pos_type SQLiteBLOBStreamBufferT<Elem, Traits>::seekoff(off_type _Off,
	std::ios_base::seekdir _Way, std::ios_base::openmode _Which)
{	

	SQLiteBLOBStreamBufferT::pos_type ret(_BADOFF);

	if (_Off != _BADOFF){

		switch (_Way)
		{
		case std::ios_base::beg:
			break;

		case std::ios_base::end:

			_Off += toStreamPos(blobSeekhigh);
			break;

		case std::ios_base::cur:

			if ((_Which & std::ios_base::in) && (_Which & std::ios_base::out) &&
				(blobGlobalReadIDX() != blobGlobalWriteIDX())){
				_Off = _BADOFF;
			}
			else{
				_Off += toStreamPos(blobGlobalReadIDX());
			}

			break;
		default:
			break;
		}

		if (_Off != _BADOFF){
			ret = seekpos(_Off, _Which);
		}
	}

	return ret;
}

template<typename Elem, typename Traits>
typename SQLiteBLOBStreamBufferT<Elem, Traits>::pos_type SQLiteBLOBStreamBufferT<Elem, Traits>::seekpos(pos_type _Ptr,
	std::ios_base::openmode _Mode)
{
	// aktualizuje seek high - ciągle mogły być zapisy
	auto oldSeekhigh = blobSeekhigh;
	updateBlobStreamEnd();
	// pozycja bezwgledna
	std::streamoff _StreamPos = (std::streamoff)_Ptr;
	std::streamoff _BlobPos = toBlobPos(_StreamPos);

	//! sprawdzam czy pozycja jest prawidłowa, czy wybrano jakis bufor
	if (_BlobPos == _BADOFF || _BlobPos < 0 || _BlobPos > blobSeekhigh ||
		!(_Mode & std::ios_base::in || _Mode & std::ios_base::out)){
		return _BADOFF;
	}
	else{

		if (_Mode & std::ios_base::out)
		{	// position within write buffer
			if ((_Mysb::pptr() == nullptr) || (blobWriteBase > _BlobPos) || (_BlobPos > blobWriteMaxIDX())){
				if (_Mysb::pptr() != nullptr){
					if (sync() < 0){
						return _BADOFF;
					}
				}
				blobWriteBase = _BlobPos;
				_Mysb::setp(buffer->begin(), buffer->end());
			}
			else{
				_Mysb::pbump((int)(_StreamPos - toStreamPos(blobGlobalWriteIDX())));
			}
		}

		if (_Mode & std::ios_base::in)
		{	// position within read buffer
			if ((_Mysb::gptr() == nullptr) || (blobReadBase > _BlobPos) || (_BlobPos > blobReadMaxIDX())){
				if (_Mysb::gptr() != nullptr){
					if (sync() < 0){
						return _BADOFF;
					}
				}

				blobReadBase = _BlobPos;
				_Mysb::setg(buffer->begin(), nullptr, buffer->end());
			}
			else{
				_Mysb::gbump((int)(_StreamPos - toStreamPos(blobGlobalReadIDX())));
			}
		}
	}

	return pos_type(_StreamPos);
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::sync()
{
	int ret = 0;

	if (!(Mystate & _Constant)){

		if (_Mysb::pptr() != nullptr){
			auto oldBlobSize = blobSize;
			if (flushBuffer(_Mysb::pbase(), std::min<int>(_Mysb::epptr() - _Mysb::pbase(), toStreamPos(blobSeekhigh - blobWriteBase)), blobWriteBase) == true){
				if (oldBlobSize < blobSize){
					auto ss = streamBufferSize();
					if (ss < buffer->updateSize(toStreamPos(blobSize))){
						blobWriteBase = blobGlobalWriteIDX();
						_Mysb::setp(buffer->begin(), buffer->end());
						blobReadBase = blobGlobalReadIDX();
						_Mysb::setg(buffer->begin(), nullptr, buffer->end());
					}
				}
			}
			else{
				ret = -1;
			}
		}
		else if (putBackWrite == true){
			auto oldBlobSize = blobSize;
			if (flushBuffer(_Mysb::eback(), std::min<int>(_Mysb::egptr() - _Mysb::eback(), toStreamPos(blobSeekhigh - blobReadBase)), blobReadBase) == true){
				if (oldBlobSize < blobSize){
					auto ss = streamBufferSize();
					if (ss < buffer->updateSize(toStreamPos(blobSize))){
						blobWriteBase = blobGlobalWriteIDX();
						_Mysb::setp(buffer->begin(), buffer->end());
						blobReadBase = blobGlobalReadIDX();
						_Mysb::setg(buffer->begin(), nullptr, buffer->end());
					}
				}
			}
			else{
				ret = -1;
			}
		}
	}

	return ret;
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::blobBufferSize() const
{
	return toBlobPos(streamBufferSize());
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::streamBufferSize() const
{
	return std::distance(buffer->begin(), buffer->end());
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::blobMaxIDX(const int idx) const
{
	return std::min(idx + blobBufferSize(), blobSeekhigh);
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::blobReadMaxIDX() const
{
	return blobMaxIDX(blobReadBase);
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::blobWriteMaxIDX() const
{
	return blobMaxIDX(blobWriteBase);
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::toStreamPos(int pos)
{
	return pos / sizeof(Elem);
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::toBlobPos(int pos)
{
	return pos * sizeof(Elem);
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::blobGlobalIDX(const int blobBase, const Elem * bufferBase,
	const Elem * bufferPos)
{
	return blobBase + (int)((bufferPos == nullptr) ? 0 : toBlobPos(std::distance(bufferBase, bufferPos)));
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::blobGlobalWriteIDX() const
{
	return blobGlobalIDX(blobWriteBase, _Mysb::pbase(), _Mysb::pptr());
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::blobGlobalReadIDX() const
{
	return blobGlobalIDX(blobReadBase, _Mysb::eback(), _Mysb::gptr());
}

template<typename Elem, typename Traits>
void SQLiteBLOBStreamBufferT<Elem, Traits>::updateBlobStreamEnd()
{
	blobSeekhigh = std::max(blobSeekhigh, blobGlobalWriteIDX());
}

template<typename Elem, typename Traits>
int SQLiteBLOBStreamBufferT<Elem, Traits>::refillBuffer(const int blobIDX, Elem * bufferBase)
{
	const int toRead = std::min(toBlobPos(std::distance(bufferBase, buffer->end())), blobSize - blobIDX);
	using namespace sqliteUtils;
	// jak mam jeszcze cos do czytania
	if (toRead > 0){

		//teraz czytam do mojego bufora
		auto ret = sqlite3_blob_read(blob, bufferBase, toRead, blobIDX);

		// jak sie nie udalo czytac z sqlite
		if (ret != SQLITE_OK){
			return -1;
		}
	}

	return toRead;
}

template<typename Elem, typename Traits>
bool SQLiteBLOBStreamBufferT<Elem, Traits>::flushBuffer(const Elem * base, const int count,
	const int blobIDX)
{
	const unsigned int buffSize = toBlobPos(count);
	const unsigned int newSize = blobIDX + buffSize;
	using namespace sqliteUtils;

	// zapis do bloba
	// czy dane jeszcze sie zmieszcza?
	if (newSize <= blobSize){
		//pisze bezposrednio do bloba
		return sqlite3_blob_write(blob, base, buffSize, blobIDX) == SQLITE_OK;
	}
	// dane sie juz nie mieszcza
	else {

		//przygotowuje update
		static const auto updateQuery = (boost::format("UPDATE %1% SET %2% = ? WHERE iRow = %3%;")
			% updateTableName(dbName, table) % column % rowID).str();

		UniqueWrapperT<sqlite3_stmt> update(SQLitePreparedStatement::prepare(db, updateQuery));

		if (update == nullptr){
			return false;
		}

		//TODO - dodac polityke update oparta na kopiowaniu do plikow przy dużych blobach
		//bufor tymczasowy na nowy blob
		std::string tmpBuffer;

		try{

			//rezerwuje na nowy rozmiar
			tmpBuffer.reserve(newSize);

		}
		catch (...){
			return false; // failed to compose complete data, probably out of memory
		}

		//rozszerzam do aktualnego rozmiaru
		tmpBuffer.resize(blobIDX);
		//czytam calego bloba
		auto ret = sqlite3_blob_read(blob, (void*)tmpBuffer.c_str(), blobIDX, 0);

		if (ret != SQLITE_OK){
			return false;
		}

		//zamykam bloba
		blob.reset();

		//dopisuje to co mam
		tmpBuffer.append(base, buffSize);

		//aktualizuje bloba
		ret = sqlite3_bind_blob(update, 1, tmpBuffer.c_str(), newSize, SQLITE_STATIC);

		if (ret != SQLITE_OK){
			return false; // failed to bind sqlite query
		}

		//odpalam update			
		if (SQLitePreparedStatement::step(update) != SQLITE_DONE){
			return false; // failed to write to sqlite db
		}

		//otwieram ponownie
		blob.reset(SQLiteBLOB::open(db, table.c_str(), column.c_str(), rowID, 1, dbName.c_str()));

		blobSize = newSize;
	}

	putBackWrite = false;

	return true;
}

template<typename Elem, typename Traits>
std::string SQLiteBLOBStreamBufferT<Elem, Traits>::updateTableName(const std::string & dbName,
	const std::string & table)
{
	return (dbName.empty() == true) ? table : dbName + '.' + table;
}

template<typename Elem, typename Traits>
typename SQLiteBLOBStreamBufferT<Elem, Traits>::Strstate SQLiteBLOBStreamBufferT<Elem, Traits>::_Getstate(std::ios_base::openmode _Mode)
{
	Strstate _State = (Strstate)0;
	if (!(_Mode & std::ios_base::in))
		_State |= _Noread;
	if (!(_Mode & std::ios_base::out))
		_State |= _Constant;
	if (_Mode & std::ios_base::app)
		_State |= _Append;
	if (_Mode & std::ios_base::ate)
		_State |= _Atend;
	return _State;
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERTIMPL_H__
