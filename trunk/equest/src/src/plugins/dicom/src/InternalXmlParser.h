/********************************************************************
	created:	2013/12/20
	created:	20:12:2013   12:01
	filename: 	InternalXmlParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__INTERNALXMLPARSER_H__
#define HEADER_GUARD_DICOM__INTERNALXMLPARSER_H__

#include <utils/SmartPtr.h>
#include <utils/Filesystem.h>
#include <corelib/IParser.h>

namespace dicom {

//! Parser plików png, dostarcza obrazow z warstwami
class InternalXmlParser : public plugin::ISourceParser
{
    UNIQUE_ID("{10F620FF-98EC-46A3-BE64-6269EFB16784}")
	CLASS_DESCRIPTION("Internal Xml Parser", "Internal Xml Parser")
	
public:
    InternalXmlParser();
    virtual ~InternalXmlParser();

public:
    //! Parsowanie pliku 
    //! \param path poprawna ścieżka do pliku
    virtual void parse(const std::string & source);
    //! \return pusty obiekt nowego parsera
    virtual plugin::IParser* create() const;
	virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
	virtual void reset();
    //! Zwraca rozszerzenia, które są obsługiwane przez parser
    //! \param extensions kolecja z roszerzeniami
    virtual void acceptedExpressions(Expressions & expressions) const;

private:
    utils::ObjectWrapperPtr sessionInternal;
};

}
#endif
