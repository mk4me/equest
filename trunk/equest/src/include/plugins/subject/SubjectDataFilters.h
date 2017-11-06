/********************************************************************
	created:	2011/10/04
	created:	4:10:2011   8:07
	filename: 	SubjectDataFilters.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__IDATAFILTER_H__
#define HEADER_GUARD_SUBJECT__IDATAFILTER_H__

#include <plugins/subject/Export.h>
#include <corelib/Variant.h>
#include <plugins/subject/Types.h>

class ISubjectHierarchyFilter;
typedef utils::shared_ptr<ISubjectHierarchyFilter> SubjectHierarchyFilterPtr;
typedef utils::shared_ptr<const ISubjectHierarchyFilter> SubjectHierarchyFilterConstPtr;

class ISubjectHierarchyFilter
{
public:
	virtual ~ISubjectHierarchyFilter() {}

	virtual void filterSubjects(const core::ConstVariantsList & inputSubjects, core::ConstVariantsList & outputSubjects) const = 0;
	virtual void filterSubjectData(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectData) const = 0;
	virtual void filterSubjectSessions(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectSessions) const = 0;

	virtual void filterSessions(const core::ConstVariantsList & inputSessions, core::ConstVariantsList & outputSessions) const = 0;
	virtual void filterSessionData(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionData) const = 0;
	virtual void filterSessionMotions(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionMotions) const = 0;

	virtual void filterMotions(const core::ConstVariantsList & inputMotion, core::ConstVariantsList & outputMotions) const = 0;
	virtual void filterMotionData(const core::VariantConstPtr & motion, core::ConstVariantsList & outputMotionData) const = 0;	
};

class PLUGINSUBJECT_EXPORT SubjectHierarchyEmptyFilter : public ISubjectHierarchyFilter
{
public:

	SubjectHierarchyEmptyFilter();

	virtual ~SubjectHierarchyEmptyFilter();

	virtual void filterSubjects(const core::ConstVariantsList & inputSubjects, core::ConstVariantsList & outputSubjects) const;

	virtual void filterSubjectData(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectData) const;

	virtual void filterSubjectSessions(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectSessions) const;

	virtual void filterSessions(const core::ConstVariantsList & inputSessions, core::ConstVariantsList & outputSessions) const;

	virtual void filterSessionData(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionData) const;

	virtual void filterSessionMotions(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionMotions) const;

	virtual void filterMotions(const core::ConstVariantsList & inputMotion, core::ConstVariantsList & outputMotions) const;

	virtual void filterMotionData(const core::VariantConstPtr & motion, core::ConstVariantsList & outputMotionData) const;
};

class PLUGINSUBJECT_EXPORT SubjectHierarchyCompoundFilter : public ISubjectHierarchyFilter
{
public:
	typedef std::list<SubjectHierarchyFilterPtr> Filters;

public:

	SubjectHierarchyCompoundFilter(const Filters & filters);

	virtual ~SubjectHierarchyCompoundFilter();

	virtual void filterSubjects(const core::ConstVariantsList & inputSubjects, core::ConstVariantsList & outputSubjects) const;

	virtual void filterSubjectData(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectData) const;

	virtual void filterSubjectSessions(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectSessions) const;

	virtual void filterSessions(const core::ConstVariantsList & inputSessions, core::ConstVariantsList & outputSessions) const;

	virtual void filterSessionData(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionData) const;

	virtual void filterSessionMotions(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionMotions) const;

	virtual void filterMotions(const core::ConstVariantsList & inputMotion, core::ConstVariantsList & outputMotions) const;

	virtual void filterMotionData(const core::VariantConstPtr & motion, core::ConstVariantsList & outputMotionData) const;

private:

	Filters filters;
};

class PLUGINSUBJECT_EXPORT SubjectHierarchyTypeFilter : public SubjectHierarchyEmptyFilter
{
public:

	SubjectHierarchyTypeFilter(const utils::TypeInfo & type);
	SubjectHierarchyTypeFilter(const utils::TypeInfoList & types);

	virtual ~SubjectHierarchyTypeFilter();

	virtual void filterSubjects(const core::ConstVariantsList & inputSubjects, core::ConstVariantsList & outputSubjects) const;

	virtual void filterSessions(const core::ConstVariantsList & inputSessions, core::ConstVariantsList & outputSessions) const;

	virtual void filterMotions(const core::ConstVariantsList & inputMotion, core::ConstVariantsList & outputMotions) const;

	virtual void filterMotionData(const core::VariantConstPtr & motion, core::ConstVariantsList & outputMotionData) const;

private:

	utils::TypeInfoSet types;
};

#endif
