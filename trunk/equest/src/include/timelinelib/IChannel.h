#ifndef HEADER_GUARD_TIMELINE__ICHANNEL_H__
#define HEADER_GUARD_TIMELINE__ICHANNEL_H__

namespace timeline
{
    /** Klasa bazowa dla kanałów obslugiwanych przez timeline, definiuje ich podstawowe
    * właściwosi i operacje. Klient nie powinien modyfikować wartości swojego kanału po zmianie skali lub offsetu.
    * Zmiany te mają jedynie charakter informacyjny - wszystkim zarządza sam timeline i kanały w które obudowuje timeline kanały dostarczane przez klienta.
    */
    class IChannel
    {
    public:

        virtual ~IChannel() {}

        //! \return Sklonowany kanał
        virtual IChannel * clone() const = 0;

        //! \return długość kanału w sekundach
        virtual double getLength() const = 0;

        //! Czas zawiera się między 0 a getLength()
        //! \param time Aktualny, lokalny czas kanału w sekundach
        virtual void setTime(double time) = 0;

        //! \param newOffset Nowy offset ustawiony kanałowi
        virtual void offsetChanged(double newOffset)
        {

        }

        //! \param newScale Nowa skala ustawiona kanałowi
        virtual void scaleChanged(double newScale)
        {

        }

        //! \param beginMask Czas początku maski
        //! \param endMask Czas trwania maski
        virtual void maskChanged(double beginMask, double lengthMask)
        {

        }

        //! \param active Czy kanał jest aktywny
        virtual void activityChanged(bool active)
        {

        }
    };

} // namespace timeline

#endif  // HEADER_GUARD_TIMELINE__ICHANNEL_H__
