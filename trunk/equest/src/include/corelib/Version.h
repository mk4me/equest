/********************************************************************
    created:  2013/11/07
    created:  7:11:2013   18:47
    filename: Version.h
    author:   Mateusz Janiak
    
    purpose:  Mechanizm opisuj¹ca wersje API i jego cechy (funkcjonalnoœci)
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VERSION_H__
#define HEADER_GUARD_CORE__VERSION_H__

#include <corelib/Export.h>
#include <string>

#define CORE_API_MAJOR 0	//! G³ówna wersja API, zmiany tej wartoœci oznaczaj¹ brak kompatybilnoœci wstecz dla API i ABI
#define CORE_API_MINOR 7	//! Podrzêna wersja API, zmiany tej wartoœci powinny zachowywaæ kompatybilnoœæ wstecz (+ ABI)
#define CORE_API_PATCH 0	//! Wersja poprawek dla danej wersji i podwersji - zachowuje kompatybilnoœæ wstecz

namespace core
{
	//! Klasa obs³uguj¹ca opis wersji API aplikacji
	class CORELIB_EXPORT Version
	{
	public:

		//! Typ opisuj¹cy numery sk³adowe wersji
		typedef unsigned int VersionNumberType;

	public:
		//! Klasa opisuj¹ca dozwolone wartoœci dla parametrów wersji
		class Traits
		{
		public:
			//! \return Maksymalna wartoœæ dla g³ównego numeru wersji
			static const VersionNumberType maxMajor();
			//! \return Minimalna wartoœæ dla g³ównego numeru wersji
			static const VersionNumberType minMajor();
			//! \param val Wartoœæ g³ównego numeru wersji która weryfikujemy
			//! \return Prawda jeœli g³ówny numer wersji spe³nia majorMin() <= val < majorMax()
			static const bool verifyMajor(const VersionNumberType val);
			//! \return Maksymalna wartoœæ dla podrzêdnego numeru wersji
			static const VersionNumberType maxMinor();
			//! \return Minimalna wartoœæ dla podrzêdnego numeru wersji
			static const VersionNumberType minMinor();
			//! \param val Wartoœæ podrzêdnego numeru wersji która weryfikujemy
			//! \return Prawda jeœli podrzêdny numer wersji spe³nia majorMin() <= val < majorMax()
			static const bool verifyMinor(const VersionNumberType val);
			//! \return Maksymalna wartoœæ dla patcha numeru wersji
			static const VersionNumberType maxPatch();
			//! \return Minimalna wartoœæ dla patcha numeru wersji
			static const VersionNumberType minPatch();
			//! \param val Wartoœæ patcha numeru wersji która weryfikujemy
			//! \return Prawda jeœli patch numer wersji spe³nia majorMin() <= val < majorMax()
			static const bool verifyPatch(const VersionNumberType val);
			//! \param version Wersja któr¹ weryfikujemy
			//! \return Czy wersja ma poprawne wartoœci
			static const bool verify(const Version & version);
		};

		//! \param major Numer g³ównej wersji
		//! \param minor Numer podrzêdny wersji
		//! \param patch Numer poprawaki dla wersji o podwersji
		Version(const VersionNumberType major = CORE_API_MAJOR,
			const VersionNumberType minor = CORE_API_MINOR,
			const VersionNumberType patch = CORE_API_PATCH);

		//! Destruktor
		virtual ~Version();

		//! \return Numer g³ównej wersji
		const VersionNumberType major() const;
		//! \return Numer podrzêdny wersji
		const VersionNumberType minor() const;
		//! \return Numer poprawaki dla wersji o podwersji
		const VersionNumberType patch() const;
		//! \param version Wersja która reprezentujemy jako ci¹g znaków
		//! \param separator Ci¹g znaków oddzielaj¹cy poszczególne sk³adowe wersji
		//! \return Wersja w formie litera³u - sk³adowe wersji oddzielone separatorem
		//! w formacie [major][separator][minor][separator][patch]
		static const std::string formatedVersion(const Version & version = Version(),
			const std::string & separator = std::string("."));
		//! \param version Wersja która sprawdzamy
		//! \return Czy versja spe³nia wymagane parametry
		const bool isAtLeast(const Version & version);
		//! \param version Wersja z któr¹ siê porównujemy
		//! \return Czy dana wersja jest mniejsza od version
		const bool operator<(const Version & version);
		//! \param version Wersja z któr¹ siê porównujemy
		//! \return Czy dana wersja jest równa version
		const bool operator==(const Version & version);
		//! \param version Wersja z któr¹ siê porównujemy
		//! \return Czy dana wersja jest mniejsza lub równa version
		const bool operator<=(const Version & version);
		//! \param version Wersja z któr¹ siê porównujemy
		//! \return Czy dana wersja jest wiêksza od version
		const bool operator>(const Version & version);
		//! \param version Wersja z któr¹ siê porównujemy
		//! \return Czy dana wersja jest wiêksza od version
		const bool operator>=(const Version & version);

	private:
		//! G³ówna wersja
		const VersionNumberType major_;
		//! Podrzêdna wersja
		const VersionNumberType minor_;
		//! Pacz:)
		const VersionNumberType patch_;
	};

	//! Klasa reprezentuj¹ca wewrsjê wraz z mo¿liwoœciami jakie ona ze sob¹ niesie
	class CORELIB_EXPORT ExtendedVersion : public Version
	{
	public:
		//! \param major Numer g³ównej wersji
		//! \param minor Numer podrzêdny wersji
		//! \param patch Numer poprawaki dla wersji o podwersji
		ExtendedVersion(const VersionNumberType major = 0,
			const VersionNumberType minor = 0,
			const VersionNumberType patch = 0);

		//! Destruktor
		virtual ~ExtendedVersion();

		//! \param featureName Nazwa mo¿liwoœci o które pytamy
		//! \return Prawda jeœli dana mo¿liwoœæ jest wspierana
		virtual const bool hasFeature(const std::string & featureName) const;
	};
}

#endif	//	HEADER_GUARD_CORE__VERSION_H__
