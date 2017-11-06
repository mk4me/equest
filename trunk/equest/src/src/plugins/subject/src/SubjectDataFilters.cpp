#include "SubjectPCH.h"
#include <plugins/subject/SubjectDataFilters.h>
#include <plugins/subject/IMotion.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/ISubject.h>

SubjectHierarchyEmptyFilter::SubjectHierarchyEmptyFilter()
{

}

SubjectHierarchyEmptyFilter::~SubjectHierarchyEmptyFilter()
{

}

void SubjectHierarchyEmptyFilter::filterSubjects(const core::ConstVariantsList & inputSubjects, core::ConstVariantsList & outputSubjects) const
{
	outputSubjects.insert(outputSubjects.end(), inputSubjects.begin(), inputSubjects.end());
}

void SubjectHierarchyEmptyFilter::filterSubjectData(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectData) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	s->getObjects(outputSubjectData);
}

void SubjectHierarchyEmptyFilter::filterSubjectSessions(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectSessions) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	core::ConstVariantsList sessions;
	s->getSessions(sessions);
	filterSessions(sessions, outputSubjectSessions);
}

void SubjectHierarchyEmptyFilter::filterSessions(const core::ConstVariantsList & inputSessions, core::ConstVariantsList & outputSessions) const
{
	outputSessions.insert(outputSessions.end(), inputSessions.begin(), inputSessions.end());
}

void SubjectHierarchyEmptyFilter::filterSessionData(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionData) const
{
	PluginSubject::SessionConstPtr s = session->get();
	s->getObjects(outputSessionData);
}

void SubjectHierarchyEmptyFilter::filterSessionMotions(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionMotions) const
{
	PluginSubject::SessionConstPtr s = session->get();
	core::ConstVariantsList motions;
	s->getMotions(motions);
	filterMotions(motions, outputSessionMotions);
}

void SubjectHierarchyEmptyFilter::filterMotions(const core::ConstVariantsList & inputMotion, core::ConstVariantsList & outputMotions) const
{
	outputMotions.insert(outputMotions.end(), inputMotion.begin(), inputMotion.end());
}

void SubjectHierarchyEmptyFilter::filterMotionData(const core::VariantConstPtr & motion, core::ConstVariantsList & outputMotionData) const
{
	PluginSubject::MotionConstPtr s = motion->get();
	s->getObjects(outputMotionData);	
}

SubjectHierarchyCompoundFilter::SubjectHierarchyCompoundFilter(const Filters & filters) : filters(filters)
{

}

SubjectHierarchyCompoundFilter::~SubjectHierarchyCompoundFilter()
{

}

void SubjectHierarchyCompoundFilter::filterSubjects(const core::ConstVariantsList & inputSubjects, core::ConstVariantsList & outputSubjects) const
{
	core::ConstVariantsList localInput(inputSubjects);

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterSubjects(localInput, localOutput);
		localInput.swap(localOutput);			
	}

	outputSubjects.splice(outputSubjects.end(), localInput);
}

void SubjectHierarchyCompoundFilter::filterSubjectData(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectData) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	core::ConstVariantsSet localInputSet;
	{
		core::ConstVariantsList localInput;
		s->getObjects(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterSubjectData(subject, localOutput);
		core::ConstVariantsSet localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstVariantsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstVariantsSet(localIntersection.begin(), endIT);
	}

	outputSubjectData.insert(outputSubjectData.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterSubjectSessions(const core::VariantConstPtr & subject, core::ConstVariantsList & outputSubjectSessions) const
{
	PluginSubject::SubjectConstPtr s = subject->get();
	core::ConstVariantsSet localInputSet;
	{
		core::ConstVariantsList localInput;
		s->getSessions(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterSubjectSessions(subject, localOutput);
		core::ConstVariantsSet localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstVariantsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstVariantsSet(localIntersection.begin(), endIT);
	}

	outputSubjectSessions.insert(outputSubjectSessions.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterSessions(const core::ConstVariantsList & inputSessions, core::ConstVariantsList & outputSessions) const
{
	core::ConstVariantsList localInput(inputSessions);

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterSessions(localInput, localOutput);
		localInput.swap(localOutput);
	}

	outputSessions.splice(outputSessions.end(), localInput);
}

void SubjectHierarchyCompoundFilter::filterSessionData(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionData) const
{
	PluginSubject::SessionConstPtr s = session->get();
	core::ConstVariantsSet localInputSet;
	{
		core::ConstVariantsList localInput;
		s->getObjects(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterSessionData(session, localOutput);
		core::ConstVariantsSet localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstVariantsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstVariantsSet(localIntersection.begin(), endIT);
	}

	outputSessionData.insert(outputSessionData.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterSessionMotions(const core::VariantConstPtr & session, core::ConstVariantsList & outputSessionMotions) const
{
	PluginSubject::SessionConstPtr s = session->get();
	core::ConstVariantsSet localInputSet;
	{
		core::ConstVariantsList localInput;
		s->getMotions(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterSessionMotions(session, localOutput);
		core::ConstVariantsSet localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstVariantsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstVariantsSet(localIntersection.begin(), endIT);
	}

	outputSessionMotions.insert(outputSessionMotions.end(), localInputSet.begin(), localInputSet.end());
}

void SubjectHierarchyCompoundFilter::filterMotions(const core::ConstVariantsList & inputMotion, core::ConstVariantsList & outputMotions) const
{
	core::ConstVariantsList localInput(inputMotion);

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterSessions(localInput, localOutput);
		localInput.swap(localOutput);
	}

	outputMotions.splice(outputMotions.end(), localInput);
}

void SubjectHierarchyCompoundFilter::filterMotionData(const core::VariantConstPtr & motion, core::ConstVariantsList & outputMotionData) const
{
	PluginSubject::MotionConstPtr m = motion->get();
	core::ConstVariantsSet localInputSet;
	{
		core::ConstVariantsList localInput;
		m->getObjects(localInput);
		localInputSet.insert(localInput.begin(), localInput.end());
	}		

	for(auto it = filters.begin(); it != filters.end(); ++it){
		core::ConstVariantsList localOutput;
		(*it)->filterMotionData(motion, localOutput);
		core::ConstVariantsSet localOutputSet(localOutput.begin(), localOutput.end());
		core::ConstVariantsList localIntersection(std::min(localOutputSet.size(), localInputSet.size()));
		auto endIT = std::set_intersection(localInputSet.begin(), localInputSet.end(), localOutputSet.begin(), localOutputSet.end(), localIntersection.begin());
		localInputSet = core::ConstVariantsSet(localIntersection.begin(), endIT);
	}

	outputMotionData.insert(outputMotionData.end(), localInputSet.begin(), localInputSet.end());
}

SubjectHierarchyTypeFilter::SubjectHierarchyTypeFilter(const utils::TypeInfo & type)
{
	types.insert(type);
}

SubjectHierarchyTypeFilter::SubjectHierarchyTypeFilter(const utils::TypeInfoList & types) : types(types.begin(), types.end())
{

}

SubjectHierarchyTypeFilter::~SubjectHierarchyTypeFilter()
{

}

void SubjectHierarchyTypeFilter::filterSubjects(const core::ConstVariantsList & inputSubjects, core::ConstVariantsList & outputSubjects) const
{
	for(auto subjectIT = inputSubjects.begin(); subjectIT != inputSubjects.end(); ++subjectIT){
		PluginSubject::SubjectConstPtr s = (*subjectIT)->get();
		core::ConstVariantsList sessions;
		bool found = false;
		for(auto sessionIT = sessions.begin(); sessionIT != sessions.end(); ++sessionIT){
			PluginSubject::SessionConstPtr s = (*sessionIT)->get();
			core::ConstVariantsList motions;
			s->getMotions(motions);
			for(auto motionIT = motions.begin(); motionIT != motions.end(); ++motionIT){
				PluginSubject::MotionConstPtr m = (*motionIT)->get();
				for(auto it = types.begin(); it != types.end(); ++it){
					if(m->hasObject(*it, false) == true){
						outputSubjects.push_back(*subjectIT);
						found = true;
						break;
					}
				}

				if(found == true){
					break;
				}
			}

			if(found == true){
				break;
			}
		}
	}
}

void SubjectHierarchyTypeFilter::filterSessions(const core::ConstVariantsList & inputSessions, core::ConstVariantsList & outputSessions) const
{
	for(auto sessionIT = inputSessions.begin(); sessionIT != inputSessions.end(); ++sessionIT){
		PluginSubject::SessionConstPtr s = (*sessionIT)->get();
		core::ConstVariantsList motions;
		s->getMotions(motions);
		bool found = false;
		for(auto motionIT = motions.begin(); motionIT != motions.end(); ++motionIT){
			PluginSubject::MotionConstPtr m = (*motionIT)->get();
			for(auto it = types.begin(); it != types.end(); ++it){
				if(m->hasObject(*it, false) == true){
					outputSessions.push_back(*sessionIT);
					found = true;
					break;
				}
			}

			if(found == true){
				break;
			}
		}
	}
}

void SubjectHierarchyTypeFilter::filterMotions(const core::ConstVariantsList & inputMotion, core::ConstVariantsList & outputMotions) const
{
	for(auto motionIT = inputMotion.begin(); motionIT != inputMotion.end(); ++motionIT){
		PluginSubject::MotionConstPtr m = (*motionIT)->get();
		for(auto it = types.begin(); it != types.end(); ++it){
			if(m->hasObject(*it, false) == true){
				outputMotions.push_back(*motionIT);
				break;
			}
		}
	}
}

void SubjectHierarchyTypeFilter::filterMotionData(const core::VariantConstPtr & motion, core::ConstVariantsList & outputMotionData) const
{
	core::ConstVariantsList data;
	PluginSubject::MotionConstPtr m = motion->get();
	for(auto it = types.begin(); it != types.end(); ++it){
		m->getObjects(data, *it, false);
	}

	core::ConstVariantsSet dataSet(data.begin(), data.end());
	outputMotionData.insert(outputMotionData.end(), dataSet.begin(), dataSet.end());
}