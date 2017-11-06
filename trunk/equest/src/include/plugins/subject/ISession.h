/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:04
    filename: ISession.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___ISESSION_H__
#define HEADER_GUARD___ISESSION_H__

#include <stdexcept>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <plugins/subject/Types.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/IDataManager.h>

namespace PluginSubject {

//! Abstrakcyjna klasa reprezentująca zbiór ruchów w ramach jednego celu/badania/grupy
class ISession : public core::IDataManagerReader::IOperations
{
public:
    virtual ~ISession() {}

    //! \return Unikalne, globalne ID sesji. Po usunięci i dodaniu może być inne!
    virtual SubjectID getID() const = 0;
    //! \return Unikalne, lokalne ID sesji w obrębie sesji tego samego obiektu wykonującego ruch. Podobnie jak globalne - po usunięci i ponownym dodaniu ID może ulec zmianie!
    virtual SubjectID getLocalID() const = 0;
    //! \param motions Zbiór wypełniany konkretnymi ruchami danego obiektu w ramach aktualnej sesji
    virtual void getMotions(core::ConstVariantsList & motions) const = 0;

	virtual void addMotion(const core::VariantConstPtr & motion) = 0;
	virtual void removeMotion(const core::VariantConstPtr & motion) = 0;

    //! \return Obiekt z którym związana jest ta sesja
    virtual const core::VariantConstPtr & getSubject() const = 0;
	virtual const SubjectConstPtr & getUnpackedSubject() const = 0;

    //! \return Globalna nazwa sesji
    virtual const std::string & getName() const = 0;
    //! \return Lokalna nazwa sesji
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

DEFINE_WRAPPER(PluginSubject::ISession, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISESSION_H__
