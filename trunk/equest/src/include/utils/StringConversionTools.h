/********************************************************************
	created:  2011/02/08
	created:  8:2:2011   11:51
	filename: StringConversionTools.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__STRINGCONVERSIONTOOLS_H__
#define __HEADER_GUARD_UTILS__STRINGCONVERSIONTOOLS_H__

#include <QtCore/QString>
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	struct is_text : public std::integral_constant<bool, std::is_same<T, std::string>::value || std::is_same<T, QString>::value> {};

	struct Convert
	{

		static inline QString toQString(const boost::filesystem::path& path)
		{
			return toQString(path.string());
		}

		static inline QString toQString(const char* ascii, int size = -1)
		{
			return QString::fromLatin1(ascii, size);
		}

		static inline QString toQString(const std::string& str)
		{
			return toQString(str.c_str(), static_cast<int>(str.size()));
		}

		template<typename T>
		static inline QString toQString(const T & val)
		{
			return QString("%1").arg(val);
		}

		static inline std::string toStdString(const char* ascii, int size = -1)
		{
			if ( size < 0 ) {
				return std::string(ascii);
			} else {
				return std::string(ascii, ascii+size);
			}
		}

		static inline std::string toStdString(const QString& str)
		{
			const QByteArray asc = str.toLatin1(); 
			return toStdString(asc.constData(), asc.length()); 
		}

		static inline std::string toStdString(const boost::filesystem::path& path)
		{
			return path.string();
		}

		template<typename T>
		static inline std::string toStdString(const T & val)
		{
			return boost::lexical_cast<std::string>(val);
		}

		//! Przechowywanie instancji tego typu spowoduje błędy przy wyłuskiwaniu wartości.
		struct toString_t
		{
			//! Forma pośrednia, potrzebna gdy korzystamy z l-wartości.
			std::string strval;
			//! Gdy korzystamy z r-wartości.
			const std::string& strref;

			//! Konstruktor dla r-wartości
			toString_t(const std::string& str) : strval(), strref(str)
			{}
			//! Konstruktor dla l-wartości.
			toString_t(std::string && str) : strval(str), strref(strval)
			{}

			template<typename T>
			toString_t(const T & val) : toString_t(toStdString<T>(val))
			{}

			operator const std::string&()
			{
				return strref;
			}
			operator QString()
			{
				return toQString(strref);
			}
			operator boost::filesystem::path()
			{
				return boost::filesystem::path(strref);
			}
		};

		static inline toString_t toString(const QString& str)
		{
			return toString_t(str.toStdString());
		}

		static inline toString_t toString(const std::string& str)
		{
			return toString_t(str);
		}

		static inline toString_t toString(std::string&& str)
		{
			return toString_t(str);
		}

		static inline toString_t toString(const boost::filesystem::path& path)
		{
			return toString_t(toStdString(path));
		}

		template<typename T>
		static inline const T & toString(const T & val)
		{
			return val;
		}

		template<typename T>
		static inline toString_t toString(const T & val)
		{
			return toString_t(val);
		}

	};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_UTILS__STRINGCONVERSIONTOOLS_H__
