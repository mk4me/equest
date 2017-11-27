/********************************************************************
	created:	2013/10/25
	created:	25:10:2013   8:53
	filename: 	AnnotationParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST__AnnotationParser_H__
#define HEADER_GUARD_EQUEST__AnnotationParser_H__


#include <utils/SmartPtr.h>
#include <utils/Filesystem.h>
#include <corelib/IParser.h>

namespace equest {

//! Parser plików png, dostarcza obrazow z warstwami
class AnnotationParser : public plugin::ISourceParser
{
    UNIQUE_ID("{181A95BA-AB96-4DEF-B887-2428511DB734}")
	CLASS_DESCRIPTION("PNG parser", "PNG parser")
	
public:
    AnnotationParser();
    virtual ~AnnotationParser();

public:
    //! Parsowanie pliku c3d
    //! \param path poprawna ścieżka do pliku
    virtual void parse(const std::string & source);
    //! \return pusty obiekt nowego parsera
    virtual plugin::IParser* create() const;
	virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
	virtual void reset();
    //! Zwraca rozszerzenia, które są obsługiwane przez parser (tylko c3d)
    //! \param extensions kolecja z roszerzeniami
    virtual void acceptedExpressions(Expressions & expressions) const;

private:
	utils::ObjectWrapperPtr annotationsOW;
};

}
#endif
