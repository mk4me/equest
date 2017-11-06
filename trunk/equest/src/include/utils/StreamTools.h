/********************************************************************
	created:  2016/01/20
	filename: StreamTools.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__STREAMTOOLS_H__
#define __HEADER_GUARD_UTILS__STREAMTOOLS_H__

#include <sstream>
#include <utils/array_traits.h>

namespace utils
{
	struct StreamTools
	{
		//! \param stream Strumieñ którego rozmiar pobieramy
		//! \return Rozmiar strumienia
		inline static std::streamsize size(std::istream & stream)
		{
			const auto pos = stream.tellg();
			stream.seekg(0, std::ios::beg);
			const auto start = stream.tellg();
			stream.seekg(0, std::ios::end);
			const auto end = stream.tellg();
			stream.seekg(pos);

			return std::streamsize(end - start);
		}

		//------------------------------------------------------------------------------

		//! \param stream Strumieñ do czytania
		//! \param out Bufor do któego czytamy strumieñ
		//! \param bytesCount Iloœæ bajtów do przeczytania ze strumienia
		//! \return Faktyczna iloœc bajtów przeczytana ze strumienia
		inline static std::streamsize forceReadSome(std::istream & stream,
			char * out, const std::streamsize bytesCount)
		{
			const auto state = stream.rdstate();
			stream.read(out, bytesCount);
			const std::streamsize bytes = stream.gcount();
			stream.clear(state | (stream.eof() == true ? std::ios_base::eofbit : std::ios_base::goodbit));
			return bytes;
		}

		//------------------------------------------------------------------------------

		//! \param stream Strumieñ do skopiowania
		//! \return Strumieñ zapisany w stringu
		inline static std::string read(std::istream & stream, void * buffer,
			const std::size_t bufferSize)
		{
			std::string ret;

			std::streamsize read = 0;
			while ((read = forceReadSome(stream, static_cast<char*>(buffer), bufferSize)) > 0) { ret.append(static_cast<char*>(buffer), static_cast<unsigned int>(read)); }
			return ret;
		}

		//------------------------------------------------------------------------------

		//! \param stream Strumieñ do skopiowania
		//! \return Strumieñ zapisany w stringu
		inline static std::string read(std::istream & stream)
		{
			char buffer[1024 * 512] = { 0 };
			//unsigned int BufferSize = 1024 * 512;
			//std::unique_ptr<char[]> buffer(new char[BufferSize] {0});
			//return read(stream, buffer.get(), BufferSize);
			return read(stream, buffer);
		}

		template<typename T, unsigned int Size>
		//! \param stream Strumieñ do skopiowania
		//! \return Strumieñ zapisany w stringu
		inline static std::string read(std::istream & stream, T(&fixedArray)[Size])
		{
			return read(stream, (void*)fixedArray, utils::size(fixedArray));
		}
	};

	//------------------------------------------------------------------------------
	//! Klasa pozwalaj¹ca tymczasowo przejmowaæ operacje wejœcia dla strumienia
	class StringStreamBufferGrabber : public std::stringstream
	{
	public:
		//! \param os Strumieñ któego operacje wejscia przejmujemy
		StringStreamBufferGrabber(std::ostream & os) : old(nullptr), os(os)
		{
			old = os.rdbuf(rdbuf());
		}
		//! Destruktor wirtualny
		virtual ~StringStreamBufferGrabber()
		{
			os.rdbuf(old);
		}

	private:
		//! Poprzedni bufor strumienia
		std::streambuf* old;
		//! Strumieñ
		std::ostream & os;
	};
}

#endif  // __HEADER_GUARD_UTILS__STREAMTOOLS_H__