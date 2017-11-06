#ifndef HEADER_GUARD_acclaimMODEL__AMCPARSER_H__
#define HEADER_GUARD_acclaimMODEL__AMCPARSER_H__

#include <acclaimformatslib/MotionData.h>

namespace acclaim
{
    /// \brief Klasa realizuje zapis i odczyt plików *.amc
    class AmcParser
    {
    public:
        /// \brief  Parsuje podany plik. 
        /// \param  model reprezentacja wewnętrzna animacji
        /// \param  filename  Nazwa pliku do parsowania. 
		static MotionData::FramesData parse(std::istream& stream);

		/// \brief  Inicjalizuje plik *.amc do zapisu danych ruchu
		//! \param stream Strumień który inicjalizujemy do zapisu
		//! \param angleType Typ jednostek zapisanych kątów
		static void initSerialize(std::ostream& stream,
			const kinematicUtils::AngleUnitType angleType);

		/// \brief  Zapisuje dane z modelu do pliku *.amc
		//! \param frame Ramka danych do zapisania
		//! \param stream Strumień do którego zapisujemy
		static void serialize(const MotionData::FrameData & frame,			
			std::ostream& stream);

		/// \brief  Zapisuje dane z modelu do pliku *.amc
		/// \param  data Dane ruchu szkieletu do zpaisu
		//! \param angleType Typ jednostek zapisanych kątów
		//! \param stream Strumień do którego zapisujemy
		static void serialize(const MotionData & data,
			const kinematicUtils::AngleUnitType angleType,
			std::ostream& stream);
    };
}

#endif // HEADER_GUARD_acclaimMODEL__AMCPARSER_H__
