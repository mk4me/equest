/********************************************************************
	created:  2014/09/24	09:04:05
	filename: SQLitePreparedStatement.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEPREPAREDSTATEMENT_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEPREPAREDSTATEMENT_H__

#include <sqliteUtils/Export.h>
#include <sqliteUtils/WrapperT.h>
#include <sqlite3.h>
#include <string>

namespace sqliteUtils
{
	class SQLITEUTILS_EXPORT SQLitePreparedStatement
	{
	public:

		class SQLITEUTILS_EXPORT Finalizer
		{
		public:
			Finalizer(const unsigned int retriesCount = 0,
				const unsigned int stepWaitTime = 150);

			bool operator()(sqlite3_stmt * statment);

		private:
			const unsigned int retriesCount;
			const unsigned int stepWaitTime;
		};		

		//! \param db Uchwyt bazy danych
		//! \param sql Zapytanie
		//! \return Uchwyt zapytania lub nullptr
		static sqlite3_stmt * prepare(sqlite3 * db, const std::string & sql);		

		//! \param statement Uchwyt zapytania	
		//! \return Czy uda³o siê zmakn¹æ uchwyt
		static bool finalize(sqlite3_stmt * statement);		

		//! \param statement Uchwyt zapytania
		//! \return Kod operacji wykonania
		static int step(sqlite3_stmt * statement);
	};
}

namespace std
{

template<>
struct default_delete<sqlite3_stmt>
{
	default_delete() = default;

	~default_delete() = default;

	template<class U>	
	default_delete(const default_delete<U>&) throw()
	{	// construct from another default_delete
		//static_assert(is_convertible<_Ty2 *, _Ty *>::value, "Type U must be convertible to sqlite3_stmt");
	}

	void operator()(sqlite3_stmt * statement) const throw()
	{	// delete a pointer
		sqliteUtils::SQLitePreparedStatement::finalize(statement);

	}
};

}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEPREPAREDSTATEMENT_H__
