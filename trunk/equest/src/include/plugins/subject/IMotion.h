/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:02
    filename: IMotion.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___IMOTION_H__
#define HEADER_GUARD___IMOTION_H__

#include <stdexcept>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <plugins/subject/Types.h>
#include <corelib/BaseDataTypes.h>
#include <plugins/subject/ISession.h>
#include <corelib/IDataManager.h>

namespace PluginSubject{

//! Abstrakcyjna klasa reprezentująca pojedynczy ruch obiektu w ramach sesji
class IMotion : public core::IDataManagerReader::IOperations
{
public:

    virtual ~IMotion() {}

    //! \return Globalny, unikalny ID ruchu
    virtual SubjectID getID() const = 0;
    //! \return Lokalny, unikalny ID w ramach sesji
    virtual SubjectID getLocalID() const = 0;
    //! \return Sesja z którą związany jest ten ruch
    virtual const core::VariantConstPtr & getSession() const = 0;
	virtual const SessionConstPtr & getUnpackedSession() const = 0;

    //! \return Globalna nazwa ruchu
    virtual const std::string & getName() const = 0;
    //! \return Lokalna nazwa ruchu w ramach sesji
    virtual const std::string & getLocalName() const = 0;

	//! \data Dane wchodzące pod kontrolę DM
	virtual void addData(const core::VariantConstPtr & data) = 0;
	//! Dane usuwane z DM
	virtual void removeData(const core::VariantConstPtr & data) = 0;

	virtual const bool tryAddData(const core::VariantConstPtr & data) = 0;

	virtual const bool tryRemoveData(const core::VariantConstPtr & data) = 0;

	virtual void getObjects(core::ConstVariantsList & objects) const = 0;

	virtual void getObjects(core::ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const = 0;

	virtual void getObjects(core::VariantsCollection& objects) const = 0;

	virtual const bool isManaged(const core::VariantConstPtr & object) const = 0;

	virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const = 0;
};

}

DEFINE_WRAPPER(PluginSubject::IMotion, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___IMOTION_H__
