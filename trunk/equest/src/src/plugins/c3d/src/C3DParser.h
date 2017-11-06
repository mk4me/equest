/********************************************************************
	created:	2011/05/25
	created:	25:5:2011   10:14
	filename: 	C3DParser.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PLUGINC3D__C3DPARSER_H__
#define HEADER_GUARD_PLUGINC3D__C3DPARSER_H__

#include <utils/SmartPtr.h>
#include <utils/Filesystem.h>
#include <corelib/IParser.h>
#include <c3dlib/C3DParser.h>


//! Klasa wykorzystuje c3dlib do zasilenia systemu w obiekty domenowe pochodzące z plików C3D
class C3DParser : public plugin::ISourceParser
{
    UNIQUE_ID("{D7801231-BACA-42C6-9A8E-706F561A563F}")
	CLASS_DESCRIPTION("C3D parser", "C3D parser")
private:
	//! Dane parsera po parsowaniu
	utils::ObjectsVector data;
	std::string path;

private:

	static void initObjects(utils::ObjectsVector & objects);

public:
    C3DParser();
    virtual ~C3DParser();

public:
    //! Parsowanie pliku c3d
    //! \param path poprawna ścieżka do pliku
    virtual void parse(const std::string & source);
    //! \return pusty obiekt nowego parsera
    virtual plugin::IParser* create() const;
	virtual void getObject(core::Variant & object, const core::VariantsVector::size_type idx) const;
    //! Zwraca rozszerzenia, które są obsługiwane przez parser (tylko c3d)
    //! \param extensions kolecja z roszerzeniami
    virtual void acceptedExpressions(Expressions & expressions) const;
	virtual void reset();
};


#endif
