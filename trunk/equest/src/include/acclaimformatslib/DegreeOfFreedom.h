#ifndef HEADER_GUARD_acclaimMODEL__DEGREEOFFREEDOM_H__
#define HEADER_GUARD_acclaimMODEL__DEGREEOFFREEDOM_H__

#include <kinematicUtils/Types.h>
#include <vector>
#include <cmath>
#include <limits>

namespace acclaim
{
     /*! 
     * \brief Struktura pełni funkcje przestrzeni nazw dla 
              wyliczenia Order i metod z tym związanych
     */
    struct Axis {
        
        /// \brief  Zwraca kolejność rotacji na podstawie nazwy. 
        /// \param  axis Napis określający kolejność rotacji. 
        /// \return Kolejność rotacji. 
        static kinematicUtils::AxisOrder::Type getAxisOrder(const std::string& axis);

        /// \brief  Zwraca nazwę określająca kolejność rotacji. 
        /// \param  order kolejność rotacji 
        /// \return The axis order name. 
		static std::string getAxisOrderName(kinematicUtils::AxisOrder::Type order);
    };

    /// \brief  Struktura przechowuje pojedynczy stopien swobody dla kości.
    struct DegreeOfFreedom
    {
		//! Dodatkowy kanał danych gdzie długość kości może ulegać zmianie
		static const int L = kinematicUtils::ChannelType::CUSTOM_CHANNEL_BASE + 1;

		kinematicUtils::Channel channel; //<! kanał, dla którego definiowany jest stopien swobody
        double minLimit; //<! minimalna wartość dla kanału (domyślnie -nieskonczoność)
		double maxLimit; //<! maksymalna wartość dla kanału (domyślnie nieskonczoność)

        // \param channel Kanał danych opisywany ograniczeniami
		DegreeOfFreedom(const kinematicUtils::Channel channel);

		// \param channel Kanał danych opisywany ograniczeniami
		//! \param minLimit Najmniejsza możliwa wartość dla danych kanału
		//! \param maxLimit Największa możliwa wartość dla danych kanału
		DegreeOfFreedom(const kinematicUtils::Channel channel, double minLimit, double maxLimit);

		//! Destruktor
		~DegreeOfFreedom();

		//! Operator porównania
		bool operator ==(const DegreeOfFreedom& b) const;

        /// \brief  Zwraca indeks (w tablicy wartości kanałów dla pojedynczej klatki kości)
        /// \param  Kanał dla którego powinnien być zwrócony indeks 
        /// \param  dofs lista ze stopniami swobody (na jej podstawie ustalany jest indeks)
        /// \return Indeks kanału lub -1 jeśli kanału nie znaleziono
		static std::size_t getChannelIndex(kinematicUtils::Channel channel, const std::vector<DegreeOfFreedom>& dofs);
        /// \brief  Zwraca kanał na podstawie nazwy.
        /// 		jeśli kanał nie zostanie znaleziony to rzucany jest wyjątek 
        /// \param  channel nazwa kanału. 
        /// \return otrzymany kanał. 
		static kinematicUtils::Channel getChannel(const std::string& channel);
        /// \brief  Zwraca nazwę kanału na postawie wyliczenia. 
        /// \param  channel kanał dla którego będzie zwrócona nazwa. 
        /// \param  uppercase określa czy zwracana nazwa ma mieć duże, czy male litery. 
        /// \return Nazwa kanału. 
		static std::string getChannelName(kinematicUtils::Channel channel, bool uppercase);
    };
	
}

#endif
