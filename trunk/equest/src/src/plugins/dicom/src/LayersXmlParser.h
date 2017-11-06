/********************************************************************
	created:	2013/12/20
	created:	20:12:2013   11:54
	filename: 	LayersXmlParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYERSXMLPARSER_H__
#define HEADER_GUARD_DICOM__LAYERSXMLPARSER_H__

#include <utils/SmartPtr.h>
#include <utils/Filesystem.h>
#include <corelib/IParser.h>

namespace dicom {

//! Parser plików xml z warstwami
class LayersXmlParser : public plugin::ISourceParser
{
    UNIQUE_ID("{D572DC2F-934B-43CE-9460-12ABAFFB20E0}")
	CLASS_DESCRIPTION("Layers parser", "Layers parser")
	
public:
    LayersXmlParser();
    virtual ~LayersXmlParser();

public:
    //! Parsowanie pliku z warstwami
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
    utils::ObjectWrapperPtr layers;
};

}
#endif
