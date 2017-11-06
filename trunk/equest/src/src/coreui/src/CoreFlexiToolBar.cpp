#include "CoreUiPCH.h"
#include <coreui/CoreFlexiToolBar.h>
#include <coreui/CoreFlexiToolBarSection.h>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QHBoxLayout>

using namespace coreUI;

CoreFlexiToolBar::CoreFlexiToolBar(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), innerToolBar_(new QMainWindow)
{
	setLayout(new QHBoxLayout);
	layout()->setContentsMargins(0,0,0,0);
	layout()->addWidget(innerToolBar_);
	innerToolBar_->setCentralWidget(nullptr);
	innerToolBar_->layout()->setContentsMargins(0,0,0,0);
}

CoreFlexiToolBar::~CoreFlexiToolBar()
{

}

void CoreFlexiToolBar::addSection(CoreFlexiToolBarSection * section)
{
	QToolBar * toolbar = new QToolBar(innerToolBar_);
	toolbar->addWidget(section);
	toolbar->setFloatable(false);
	toolbar->setMovable(true);
	toolbar->setAllowedAreas(Qt::TopToolBarArea);
	innerToolBar_->addToolBar(toolbar);
	sectionsToToolbars[section] = toolbar;
}

void CoreFlexiToolBar::removeSection(CoreFlexiToolBarSection * section)
{
	auto it = sectionsToToolbars.find(section);
	if(it != sectionsToToolbars.end()){
		innerToolBar_->removeToolBar(it->second);
		delete it->second;
		sectionsToToolbars.erase(it);
	}
}

void CoreFlexiToolBar::addSectionBreak()
{
	if(sectionsToToolbars.empty() == false){
		innerToolBar_->addToolBarBreak();
	}
}

void CoreFlexiToolBar::insertSection(CoreFlexiToolBarSection * before, CoreFlexiToolBarSection * section)
{
	auto it = sectionsToToolbars.find(section);
	if(it != sectionsToToolbars.end()){
		QToolBar * toolbar = new QToolBar;
		toolbar->addWidget(section);
		toolbar->setMovable(true);
		toolbar->setAllowedAreas(Qt::TopToolBarArea);
		toolbar->setParent(innerToolBar_);
		innerToolBar_->insertToolBar(it->second, toolbar);
		sectionsToToolbars[section] = toolbar;
	}
}

void CoreFlexiToolBar::insertSectionBreak(CoreFlexiToolBarSection * before)
{
	auto it = sectionsToToolbars.find(before);
	if(it != sectionsToToolbars.end()){
		innerToolBar_->insertToolBarBreak(it->second);
	}
}

void CoreFlexiToolBar::removeSectionBreak( CoreFlexiToolBarSection * before)
{
	auto it = sectionsToToolbars.find(before);
	if(it != sectionsToToolbars.end()){
		innerToolBar_->removeToolBarBreak(it->second);
	}
}

const bool CoreFlexiToolBar::sectionBreak(CoreFlexiToolBarSection * section) const
{
	bool ret = false;
	auto it = sectionsToToolbars.find(section);
	if(it != sectionsToToolbars.end()){
		innerToolBar_->toolBarBreak(it->second);
	}

	return ret;
}