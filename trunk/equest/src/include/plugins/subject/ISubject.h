/********************************************************************
    created:  2011/11/03
    created:  3:11:2011   10:06
    filename: ISubject.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISUBJECT_H__
#define HEADER_GUARD___ISUBJECT_H__

#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <plugins/subject/Types.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/IDataManager.h>

namespace PluginSubject {

//! Abstrakcyjna klasa reprezentująca obiekt wykonujący ruch - człowiek, zwierzę, avatar

class ISubject : public core::IDataManagerReader::IOperations
{
public:

    virtual ~ISubject() {}

    //! \return Unikalny identyfikator obiektu. Po usunięciu i ponownym dodaniu zostanie przyznany nowy!!
    virtual SubjectID getID() const = 0;
    //! \return "Przyjazna" lub "naturalna" nazwa obiektu.
    virtual const std::string & getName() const = 0;
    //! \param sessions Lista sesji związanych z tym obiektem
    virtual void getSessions(core::ConstVariantsList & sessions) const = 0;

	virtual void addSession(const core::VariantConstPtr & session) = 0;
	virtual void removeSession(const core::VariantConstPtr & session) = 0;

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

DEFINE_WRAPPER(PluginSubject::ISubject, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);

#endif //   HEADER_GUARD___ISUBJECT_H__

