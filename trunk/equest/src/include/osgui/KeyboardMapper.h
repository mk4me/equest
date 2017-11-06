#ifndef HEADER_GUARD__KEYBOARDMAPPER_H__
#define HEADER_GUARD__KEYBOARDMAPPER_H__

#include <set>
#include <boost/range.hpp>
#include <osgWidget/ViewerEventHandlers>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class KeyboardMapper
{
    friend class KeyboardMapperHandler;

public:
    //! Zestaw klawiszy.
	typedef std::set<osgGA::GUIEventAdapter::KeySymbol> Keys;
    typedef Keys::const_iterator const_iterator;
    typedef const_iterator iterator;

private:
    //! Zestaw klawiszy.
    Keys keysPressed;

public:
    //! Konstruktor zerujący
	KeyboardMapper();

protected:
    //! Chroniony wirtualny destruktor - niszczenie tylko polimorficzne!
    virtual ~KeyboardMapper();

public:

    //! \return Liczba wciśniętych klawiszy.
    int getNumPressedKeys() const
    {
        return keysPressed.size();
    }

    //! \return Czy zdany klaiwsz jest naciśnięty?
	bool isKeyPressed(osgGA::GUIEventAdapter::KeySymbol key) const
    {        
        return keysPressed.find(key) != keysPressed.end();
    }

    //! \return Czy któryś z zadanych klawiszy jest naciśnięty?
    template <class Range>
    bool areAnyKeysPressed(const Range& range) const
    {
        return areAnyKeysPressed(boost::begin(range), boost::end(range));
    }

    //! \return Czy któryś z zadanych klawiszy jest naciśnięty?
    template <class FwdIter>
    bool areAnyKeysPressed(FwdIter first, FwdIter last) const
    {
        while ( first != last && !isKeyPressed(*first) ) {
            ++first;
        }
        return first != last;
    }

    //! \param range Zakres.
    //! \param sorted Czy zakres jest posortowany?
    //! \return Czy wszystkie zadane klawisze są naciśnięte?
    template <class Range>
    bool areKeysPressed(const Range& range, bool sorted = true) const
    {
        return areKeysPressed(boost::begin(range), boost::end(range), sorted);
    }

    //! \param first Początek zakresu.
    //! \param last Koniec zakresu.
    //! \param sorted Czy zakres jest posortowany?
    //! \return Czy wszystkie zadane klawisze są naciśnięte?
    template <class FwdIter>
    bool areKeysPressed(FwdIter first, FwdIter last, bool sorted = true) const
    {
        // TODO: rozważyć, czy wariant z includes w ogóle jest przydatny
        if (sorted) {
            return std::includes(begin(), end(), first, last);
        } else {
            while ( first != last && isKeyPressed(*first) ) {
                ++first;
            }
            return first == last;
        }
    }

    //! \return Wszystkie naciśnięte klawisze.
	const Keys & getPressedKeys() const;

    //! \return Początek wyliczenia klawiszy.
    const_iterator begin() const
    {
        return keysPressed.begin();
    }
    //! \return Koniec wyliczenia klawiszy.
    const_iterator end() const
    {
        return keysPressed.end();
    }

protected:
	void keyPressed(osgGA::GUIEventAdapter::KeySymbol key);
	void keyReleased(osgGA::GUIEventAdapter::KeySymbol key);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////

#endif
