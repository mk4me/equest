/********************************************************************
	created:	2013/12/19
	created:	19:12:2013   19:15
	filename: 	EquestPerspective.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST__EquestPerspective_H__
#define HEADER_GUARD_EQUEST__EquestPerspective_H__

#include <plugins/hmdbCommunication/IHierarchyPerspective.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/Visualizer.h>
#include <corelib/HierarchyDataItem.h>
#include "plugins/equest/Export.h"

namespace equest {

//! Klasa pomocnicza przy tworzeniu wizualizatorów
class PLUGIN_EQUEST_EXPORT EquestHelper : public core::WrappedItemHelper
{
public:
    EquestHelper(const core::VariantConstPtr & wrapper, const core::ConstVariantsList& layers,
        const std::string& xmlFilename, const std::string& trialName);
    virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series );

public:
	const std::string& getTrialName() { return trialName;  }
private:
    core::ConstVariantsList layers;
    std::string xmlFilename; 
    std::string trialName;
};
DEFINE_SMART_POINTERS(EquestHelper);

//! Klasa pomocnicza przy tworzeniu wizualizatorów
class PLUGIN_EQUEST_EXPORT EquestMultiHelper : public core::HierarchyHelper
{
public:
	//EquestMultiHelper(const std::vector<EquestHelperPtr>& helpers);
	EquestMultiHelper(core::IHierarchyItemWeakPtr sessionItem);

protected:
	virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series);

	std::vector<EquestHelperPtr> getHelpers(core::IHierarchyItem* item) const;

public:
	virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
	virtual std::vector<utils::TypeInfo> getTypeInfos() const;

private:
	//std::vector<EquestHelperPtr> helpers;
	core::IHierarchyItemWeakPtr sessionItem;
};
DEFINE_SMART_POINTERS(EquestMultiHelper);

//! Klasa realizuj¹ca perspektywe danych
class PLUGIN_EQUEST_EXPORT EquestPerspective : public hmdbCommunication::IHierarchyPerspective
{
public:
	virtual ~EquestPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );

    core::HierarchyDataItemPtr tryGetHierarchyItem(const std::string& filename) const;

private:
    std::map<std::string, core::HierarchyDataItemWeakPtr> name2hierarchy;
};
DEFINE_SMART_POINTERS(EquestPerspective);

}
#endif
