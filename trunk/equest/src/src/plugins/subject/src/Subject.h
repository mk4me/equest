/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   9:34
	filename: 	Subject.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SUBJECT_H__
#define HEADER_GUARD_SUBJECT__SUBJECT_H__

#include <plugins/subject/ISubject.h>
#include "BasicDataStorage.h"

class SubjectService;

//! Reprezentacja subjecta. Z jednym subjectem związane są sytemy (np. miesniowy, szkieletowy) oraz jeden ruch (motion)
class Subject : public PluginSubject::ISubject
{
    friend class SubjectService;

private:
    PluginSubject::SubjectID subjectID;
    std::string name;
    mutable PluginSubject::SubjectID currentSessionID;
	static PluginSubject::SubjectID globalID;

	BasicDataStorage dataStorage;
	BasicDataStorage sessionStorage;

public:

	static void generateName(std::string & name, PluginSubject::SubjectID subjectID);

	static PluginSubject::SubjectID nextGlobalID();

public:
	Subject();

	virtual ~Subject();

    virtual const std::string & getName() const;

    virtual PluginSubject::SubjectID getID() const;

    virtual void getSessions(core::ConstVariantsList & sessions) const;
	virtual void addSession(const core::VariantConstPtr & session);
	virtual void removeSession(const core::VariantConstPtr & session);

	PluginSubject::SubjectID nextSessionID() const;

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
};

#endif
