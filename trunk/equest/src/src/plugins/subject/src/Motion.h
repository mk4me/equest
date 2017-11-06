/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   10:52
	filename: 	Motion.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__MOTION_TESTETESTEH__
#define HEADER_GUARD_SUBJECT__MOTION_TESTETESTEH__

#include <plugins/subject/IMotion.h>
#include <corelib/BaseDataTypes.h>
#include "BasicDataStorage.h"

class SubjectService;

//! Klasa reprezentuje pojedyncza próbę pomiarowa w ramach konkretnej sesji.
class Motion : public PluginSubject::IMotion
{
public:

	Motion(const core::VariantConstPtr & session, const PluginSubject::SessionConstPtr & unpackedSession, PluginSubject::SubjectID localMotionID);

	virtual ~Motion();

	static std::string generateName(PluginSubject::SubjectID motionID);
	static std::string generateLocalName(PluginSubject::SubjectID motionLocalID, const std::string & sessionLocalName);

	static PluginSubject::SubjectID nextGlobalID();

public:

    virtual const std::string & getName() const;
    virtual const std::string & getLocalName() const;

    virtual PluginSubject::SubjectID getID() const;
    virtual PluginSubject::SubjectID getLocalID() const;

    virtual const core::VariantConstPtr & getSession() const;
	virtual const PluginSubject::SessionConstPtr & getUnpackedSession() const;

	//! \data Dane wchodzące pod kontrolę DM
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

private:

	BasicDataStorage storage;

    const PluginSubject::SubjectID motionID;
    const PluginSubject::SubjectID localMotionID;

    const core::VariantConstPtr session;
	const PluginSubject::SessionConstPtr unpackedSession;

    const std::string name;
    const std::string localName;

	static PluginSubject::SubjectID globalID;
};

#endif
