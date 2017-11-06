#include "CommunicationPCH.h"
#include "HMDBSourceViewManager.h"
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>
#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <QtCore/QSettings>

using namespace hmdbCommunication;

HMDBSourceViewManager::HMDBSourceViewManager()
{

}

HMDBSourceViewManager::~HMDBSourceViewManager()
{

}

const bool HMDBSourceViewManager::registerViewPrototype(IHMDBSourceContextView * viewPrototype)
{
	if (viewPrototype == nullptr){
		return false;
	}

	if (viewPrototype->name().isEmpty() == true){
		return false;
	}

	auto it = viewDescriptions.find(viewPrototype->name());
	if (it != viewDescriptions.end()){
		return false;
	}	
	
	ViewDescription vd;
	vd.viewPrototype.reset(viewPrototype);
	viewDescriptions.insert(ViewDescriptions::value_type(viewPrototype->name(), vd));
	return true;	
}

const unsigned int HMDBSourceViewManager::viewPrototypesCount() const
{
	return viewDescriptions.size();
}

HMDBSourceViewManager::IHMDBSourceContextView * HMDBSourceViewManager::viewPrototype(const QString & prototypeName)
{
	if (prototypeName.isEmpty() == true){
		return nullptr;
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.viewPrototype.get();
	}

	return nullptr;
}

const HMDBSourceViewManager::IHMDBSourceContextView * HMDBSourceViewManager::viewPrototype(const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return nullptr;
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.viewPrototype.get();
	}

	return nullptr;
}

HMDBSourceViewManager::IHMDBSourceContextView * HMDBSourceViewManager::viewPrototype(const unsigned int idx)
{
	auto it = viewDescriptions.begin();
	std::advance(it, idx);
	return it->second.viewPrototype.get();
}

const HMDBSourceViewManager::IHMDBSourceContextView * HMDBSourceViewManager::viewPrototype(const unsigned int idx) const
{
	auto it = viewDescriptions.begin();
	std::advance(it, idx);
	return it->second.viewPrototype.get();
}

const bool HMDBSourceViewManager::registerConfiguration(const ContextConfiguration & config, const QString & prototypeName)
{
	ViewDescription * vd = find(prototypeName);

	if (vd == nullptr){
		return false;
	}

	auto cIT = std::find_if(vd->configurations.begin(), vd->configurations.end(), [=](ContextConfiguration & cc)
	{
		if (cc.name == config.name){
			return true;
		}

		return false;
	});

	if (cIT != vd->configurations.end()){
		return false;
	}

	vd->configurations.push_back(config);
	return true;
}

const unsigned int HMDBSourceViewManager::configurationsCount(const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.configurations.size();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.configurations.size();
	}

	return 0;
}

const HMDBSourceViewManager::ContextConfiguration HMDBSourceViewManager::configuration(const unsigned int idx, const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.configurations[idx];
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.configurations[idx];
	}

	return ContextConfiguration();
}

const bool HMDBSourceViewManager::registerPerspective(IDataSourcePerspective * perspective, const QString & prototypeName)
{
	ViewDescription * vd = find(prototypeName);

	if (vd == nullptr){
		return false;
	}

	auto pIT = std::find_if(vd->perspectives.begin(), vd->perspectives.end(), [=](utils::shared_ptr<IDataSourcePerspective> p)
	{
		if (p->name() == perspective->name()){
			return true;
		}

		return false;
	});

	if (pIT != vd->perspectives.end()){
		return false;
	}

	vd->perspectives.push_back(utils::shared_ptr<IDataSourcePerspective>(perspective));
	return true;
}

const unsigned int HMDBSourceViewManager::perspectivesCount(const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.perspectives.size();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.perspectives.size();
	}

	return 0;
}

IDataSourcePerspective * HMDBSourceViewManager::perspective(const unsigned int idx, const QString & prototypeName)
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.perspectives[idx].get();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.perspectives[idx].get();
	}

	return nullptr;
}

const IDataSourcePerspective * HMDBSourceViewManager::perspective(const unsigned int idx, const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.perspectives[idx].get();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.perspectives[idx].get();
	}

	return nullptr;
}

const bool HMDBSourceViewManager::registerContent(IDataSourceContent * content, const QString & prototypeName)
{
	ViewDescription * vd = find(prototypeName);

	if (vd == nullptr){
		return false;
	}

	auto pIT = std::find_if(vd->contents.begin(), vd->contents.end(), [=](utils::shared_ptr<IDataSourceContent> p)
	{
		if (p->name() == content->name()){
			return true;
		}

		return false;
	});

	if (pIT != vd->contents.end()){
		return false;
	}

	vd->contents.push_back(utils::shared_ptr<IDataSourceContent>(content));
	return true;
}

const unsigned int HMDBSourceViewManager::contentsCount(const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.contents.size();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.contents.size();
	}

	return 0;
}

IDataSourceContent * HMDBSourceViewManager::content(const unsigned int idx, const QString & prototypeName)
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.contents[idx].get();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.contents[idx].get();
	}

	return nullptr;
}

const IDataSourceContent * HMDBSourceViewManager::content(const unsigned int idx, const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.contents[idx].get();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.contents[idx].get();
	}

	return nullptr;
}

const bool HMDBSourceViewManager::registerFilter(ShallowCopyFilter * filter, const QString & prototypeName)
{
	ViewDescription * vd = find(prototypeName);

	if (vd == nullptr){
		return false;
	}

	auto pIT = std::find_if(vd->filters.begin(), vd->filters.end(), [=](utils::shared_ptr<ShallowCopyFilter> p)
	{
		if (p->name() == filter->name()){
			return true;
		}

		return false;
	});

	if (pIT != vd->filters.end()){
		return false;
	}

	vd->filters.push_back(utils::shared_ptr<ShallowCopyFilter>(filter));
	return true;
}

const unsigned int HMDBSourceViewManager::filtersCount(const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.filters.size();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.filters.size();
	}

	return 0;
}

ShallowCopyFilter * HMDBSourceViewManager::filter(const unsigned int idx, const QString & prototypeName)
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.filters[idx].get();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.filters[idx].get();
	}

	return nullptr;
}

const ShallowCopyFilter * HMDBSourceViewManager::filter(const unsigned int idx, const QString & prototypeName) const
{
	if (prototypeName.isEmpty() == true){
		return generalViewElements.filters[idx].get();
	}

	auto it = viewDescriptions.find(prototypeName);
	if (it != viewDescriptions.end()){
		return it->second.filters[idx].get();
	}

	return nullptr;
}

const bool HMDBSourceViewManager::addHierarchyPerspective(IHierarchyPerspectivePtr hierarchyPerspective)
{
	if (hierarchyPerspective == nullptr){
		return false;
	}

	auto it = std::find(hierarchyPerspectives.begin(), hierarchyPerspectives.end(), hierarchyPerspective);
	if (it == hierarchyPerspectives.end()){
		hierarchyPerspectives.push_back(hierarchyPerspective);
		return true;
	}

	return false;
}

const unsigned int HMDBSourceViewManager::hierarchyPerspectivesCount() const
{
	return hierarchyPerspectives.size();
}

const IHierarchyPerspectivePtr HMDBSourceViewManager::hierarchyPerspective(const unsigned int idx)
{
	return hierarchyPerspectives[idx];
}

const IHierarchyPerspectiveConstPtr HMDBSourceViewManager::hierarchyPerspective(const unsigned int idx) const
{
	return hierarchyPerspectives[idx];
}

HMDBSourceViewManager::ViewDescription * HMDBSourceViewManager::find(const QString & name)
{
	ViewDescription * ret = nullptr;
	if (name.isEmpty() == true){
		ret = &generalViewElements;
	}
	else{
		auto it = viewDescriptions.find(name);
		if (it != viewDescriptions.end()){
			ret = &(it->second);
		}
	}

	return ret;
}

const HMDBSourceViewManager::ViewDescription * HMDBSourceViewManager::find(const QString & name) const
{
	const ViewDescription * ret = nullptr;
	if (name.isEmpty() == true){
		ret = &generalViewElements;
	}
	else{
		auto it = viewDescriptions.find(name);
		if (it != viewDescriptions.end()){
			ret = &(it->second);
		}
	}

	return ret;
}

