#ifndef HEADER_GUARD__TIMELINESTREAM_H__
#define HEADER_GUARD__TIMELINESTREAM_H__

#include <timelinelib/Types.h>
#include <timelinelib/IChannel.h>
#include <utils/NamedTree.h>
#include <utils/ObserverPattern.h>

namespace timeline {

class Model;

/** Klasa do obudowania kanałów, wykorzystywana wewnętrznie przez timeline, definiuje 
* dodatkowe właściwości kanałów na potrzeby timeline
*/
class Channel
{
    friend class Model;

public:

    typedef ConstTags::const_iterator tag_const_iterator;
    typedef ConstTags::size_type tag_size_type;

    //! początek maski i jej długość
    typedef std::pair<double, double> Mask;

protected:

    typedef Tags::iterator tag_iterator;

private:

    //! długość kanału
    double length;

    //! Maska czasu dla danego kanału
    Mask mask;
    
    //! Offset czasu względem początku rodzica
    double localOffset;

    //! Offset czasu względem 0
    double globalOffset;

    //! Skala z jaka przetwarzamy kanał, lokalna, trzaba uwzględnić lokalną skalę rodzica
    //! 1 - oryginalny czas, > 1 szybciej, < 1 wolniej
    double localScale;

    //! Skala z jaka przetwarzamy kanał, globalna po uwzględnieniu rodzica
    //! 1 - oryginalny czas, > 1 szybciej, < 1 wolniej
    double globalScale;

    //! Czas kanału uwzględniający skale
    double time;

    //! Czy kanał jest aktywny podczas przetwarzania
    bool active;

    //! Tagi wystepujące w timeline
    Tags tags;

    //! Tagi wystepujące w timeline
    ConstTags constTags;

    //! kanał dostarczony przez klienta
    IChannelPtr innerChannel;

    //! kanał dostarczony przez klienta
    IChannelConstPtr constInnerChannel;

public:

    //! Konstruktor zerujący uzywany w celu stworzenia virtualnych kanałów
    //! tutaj tworzymy root
    //! \name nazwa kanału
    //! \param channel kanał do opakowania jako root
    Channel(const IChannelPtr & channel = IChannelPtr());

    Channel(const Channel & channel, bool deep = true);

    virtual ~Channel();


    // ------------ Obsluga dodatkowych cech strumienia danych-------------------------------------

    //! \return Czy globalny czas jest w zakresie obslugiwanym przez ten kanał
    //! \param time Globalny czas
    bool timeInChannel(double time) const;

    //! \param time Czas który chcemy ustawić w kanale
    //! \return Poprawny czas w kanale - ograniczony do zakresu kanału wraz z maska
    double makeTimeInChannel(double time) const;

    //! \param channel kanał zdefiniowany przez użytkownika i opakowany przez nasza klase
    void setInnerChannel(const IChannelPtr & channel);

    //! \return kanał zdefinowany przez klienta
    const IChannelConstPtr & getInnerChannel() const;

    //! \param mask Maska czasu realizowana wewnętrznie przez nasz kanał
    void setMask(const Mask & mask);

    //! \param maskBegin początek maski - wiekszy lub rowny 0
    void setMaskBegin(double maskBegin);

    //! \param maskLength długość maski - maskBegin + maskLength <= channel length
    void setMaskLength(double maskLength);

    //! \param maskEnd Koniec maski - mniejszy lub rowny length
    void setMaskEnd(double maskEnd);

    //! \param offset Czas względem początku czasu rodzica
    void setLocalOffset(double offset);

    //! \param offset Czas względem 0
    void setGlobalOffset(double offset);

    //! \param Aktualny czas kanału uwzględniając skale rodzica
    void setTime(double time);

    //! \param scale Skala kanału, jest składana ze skala rodzica
    void setLocalTimeScale(double scale);

    //! \param scale Skala kanału, bezwzględna wartość skali
    void setGlobalTimeScale(double scale);

    //! \param active Czy strumień jest aktywny podczas operacji oczasowych i odtwarzania timeline
    void setActive(bool active);    

    //! \return Wewnętrzna maska czasu kanału
    const Mask & getMask() const;
    
    //! \return początek maski
    double getMaskBegin() const;

    //! \return długość maski
    double getMaskLength() const;

    //! \return Koniec maski
    double getMaskEnd() const;

    //! \return Offset czasu - przesunięcie względem początku rodzica
    double getLocalOffset() const;

    //! \return Offset czasu - przesunięcie względem 0
    double getGlobalOffset() const;

    //! \return długość kanału w milisekundach, uwzględniając skale
    double getLength() const;

    //! \param długość kanału
    void setLength(double length);

    //! \return Aktualny czas kanału
    double getTime() const;

    //! \return Skala czasu kanału względem skali rodzica
    double getLocalTimeScale() const;

    //! \return Skala czasu kanału
    double getGlobalTimeScale() const;

    //! maska dla podkanałów
    //! \return Czy kanał jest aktywny podczas odtwarzania timeline i operacji czasowych
    bool isActive() const;

    //------- Obsluga tagów ---------------------

    //! \return Pierwszy tag
    tag_const_iterator beginTags() const;

    //! \return Koniec tagow
    tag_const_iterator endTags() const;

    //! \param name Nazwa szukanego taga
    //! \return Iterator znalezionego taga lub selectionsEnd() jeśli nie znaleziono
    tag_const_iterator findTag(const std::string & name) const;

    //! \param idx Indeks tagu
    //! \return Tag
    const TagConstPtr & getTag(tag_size_type idx) const;

    //! \param name Nazwa tagu
    //! \return Tag
    const TagConstPtr & getTag(const std::string & name) const;

    //! \return Ilość tagów danego kanału
    tag_size_type getNumTags() const;

protected:

    //! \return kanał zdefinowany przez klienta
    const IChannelPtr & getInnerChannel();

    //! Czysci liste tagów związana z tym kanałem
    void clearTags();

    //! \param tag Tag do wstawienia dla danego kanału
    void addTag(const TagPtr & tag);

    //! \param tag Tag do usuniecvia dla danego kanału
    void removeTag(const TagPtr & tag);

    //! \param idx Indeks tagu
    //! \return Tag
    const TagPtr & getTag(tag_size_type idx);

    //! \param name Nazwa tagu
    //! \return Tag
    const TagPtr & getTag(const std::string & name);

    //! \param name Nazwa szukanego taga
    //! \return Iterator znalezionego taga lub selectionsEnd() jeśli nie znaleziono
    tag_iterator findTag(const std::string & name);

    //! \return Pierwszy tag
    tag_iterator beginTags();

    //! \return Koniec tagow
    tag_iterator endTags();

    //! \param begin Pierwszy iterator do przeszukiwanego zakresu
    //! \param end Koniec zakresu
    //! \param name Szukana nazwa elementu
    //! \return Pozycja znalezionego elementu lub koniec zakresu
    template<class Iter>
    static Iter findByName(Iter begin, Iter end, const std::string & name)
    {
        while(begin != end) {
            if((*begin)->getName() == name) {
                break;
            }
            ++begin;
        }

        return begin;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINESTREAM_H__
