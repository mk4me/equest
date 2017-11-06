#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <corelib/IParser.h>
#include <corelib/IDataManagerReader.h>

//! klasa wykorzystuje bibliotekę kinematiclib w celu wczytania danych animacji szkieletowej
class KinematicParser : public plugin::ISourceParser
{
	UNIQUE_ID("{AE673002-AE76-4E79-983E-ABDB15713E74}")
	CLASS_DESCRIPTION("Kinematic parser", "Kinematic parser")
private:
    //! wrapper, do którego trafią wczytane dane
    utils::ObjectWrapperPtr skeletonData;

public:
    KinematicParser();
    virtual ~KinematicParser();

public:
    //! metoda odpowiada za wczytanie pliku z animacją szkieletową
    //! \param path ścieżka do pliku 
    virtual void parse(const std::string & source);
    //! tworzy pusty obiekt parsera
    virtual plugin::IParser* create() const;
    //! zwraca wspierane rozszerzenia plików (amc, bvh)
    //! \param extensions kolecja, do której trafią wspierane rozszerzenia
    virtual void acceptedExpressions(Expressions & expressions) const;
	virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
	virtual void reset();
	
};

#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
