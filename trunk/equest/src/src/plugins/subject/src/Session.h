/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   13:02
	filename: 	Session.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SESSION_H__
#define HEADER_GUARD_SUBJECT__SESSION_H__

#include <plugins/subject/Types.h>
#include <plugins/subject/ISession.h>
#include <vector>
#include <utils/SmartPtr.h>
#include <corelib/BaseDataTypes.h>
#include "BasicDataStorage.h"

class SubjectService;

class Session : public PluginSubject::ISession
{
private:
    const PluginSubject::SubjectID sessionID;

    const PluginSubject::SubjectID localSessionID;

    mutable PluginSubject::SubjectID currentMotionID;
    const core::VariantConstPtr subject;
	const PluginSubject::SubjectConstPtr unpackedSubject;    

    const std::string name;
    const std::string localName;

	static PluginSubject::SubjectID globalID;

	BasicDataStorage dataStorage;
	BasicDataStorage motionStorage;
	
public:

	static PluginSubject::SubjectID nextGlobalID();

	static std::string generateName(PluginSubject::SubjectID sessionID);

	static std::string generateLocalName(PluginSubject::SubjectID localSessionID, const std::string & subjectName);

	Session(const core::VariantConstPtr & subject,
		const PluginSubject::SubjectConstPtr & unpackedSubject,	PluginSubject::SubjectID localSessionID);

	virtual ~Session();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;

    virtual void getMotions(core::ConstVariantsList & motions) const;
	virtual void addMotion(const core::VariantConstPtr & motion);
	virtual void removeMotion(const core::VariantConstPtr & motion);

    virtual const core::VariantConstPtr & getSubject() const;
	virtual const PluginSubject::SubjectConstPtr & getUnpackedSubject() const;

	PluginSubject::SubjectID nextMotionID() const;

	//! \data Dane wchodz¹ce pod kontrolê DM
	virtual void addData(const core::VariantConstPtr & data);
	//! Dane usuwane z DM
	virtual void removeData(const core::VariantConstPtr & data);

	virtual const bool tryAddData(const core::VariantConstPtr & data);

	virtual const bool tryRemoveData(const core::VariantConstPtr & data);

	virtual void getObjects(core::ConstVariantsList & objects) const;

	virtual void getObjects(core::ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const;

	virtual void getObjects(core::VariantsCollection& objects) const;

	virtual const bool isManaged(const core::VariantConstPtr & object) const;

	virtual const bool hasObject(const utils::TypeInfo & type, bool exact) const;
};

#endif
