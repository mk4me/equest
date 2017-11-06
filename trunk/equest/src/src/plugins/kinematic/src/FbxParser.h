/********************************************************************
	created:	2014/07/13
	created:	10:45
	filename: 	FbxParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__FbxParser_H__
#define HEADER_GUARD_KINEMATIC__FbxParser_H__


#include <utils/SmartPtr.h>
#include <utils/Filesystem.h>
#include <corelib/IParser.h>

//! Parser plików fbx, wymaga wtyczki z osg
class FbxParser : public plugin::ISourceParser
{
	UNIQUE_ID("0007615B-14C8-41A3-A521-268361B9B3BA")
	CLASS_DESCRIPTION("FBX parser", "FBX parser")

public:
	FbxParser();
	virtual ~FbxParser();

public:
	//! Parsowanie pliku 
	//! \param path poprawna œcie¿ka do pliku
	virtual void parse(const std::string & source);
	//! \return pusty obiekt nowego parsera
	virtual plugin::IParser* create() const;
	virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
	virtual void reset();
	//! Zwraca rozszerzenia, które s¹ obs³ugiwane przez parser (tylko fbx)
	//! \param extensions kolecja z roszerzeniami
	virtual void acceptedExpressions(Expressions & expressions) const;

private:
	utils::ObjectWrapperPtr fbxWrapper;
};
DEFINE_SMART_POINTERS(FbxParser);

#endif // HEADER_GUARD
