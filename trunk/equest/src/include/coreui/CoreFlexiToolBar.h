/********************************************************************
    created:  2011/12/09
    created:  9:12:2011   14:11
    filename: CoreFlexiToolBar.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_COREFLEXITOOLBAR_H__
#define HEADER_GUARD_COREFLEXITOOLBAR_H__

#include <coreui/Export.h>
#include <map>
#include <QtWidgets/QWidget>

class QMainWindow;
class QToolBar;

namespace coreUI {

	class CoreFlexiToolBarSection;

class COREUI_EXPORT CoreFlexiToolBar : public QWidget
{
    Q_OBJECT;

private:

	typedef std::map<CoreFlexiToolBarSection*, QToolBar*> SectionsToToolbars;

public:

    CoreFlexiToolBar(QWidget * parent = 0, Qt::WindowFlags f = 0);

    virtual ~CoreFlexiToolBar();

	void addSection(CoreFlexiToolBarSection * section);
	void addSectionBreak();
	void insertSection(CoreFlexiToolBarSection * before, CoreFlexiToolBarSection * section);
	void insertSectionBreak(CoreFlexiToolBarSection * before);
	void removeSection(CoreFlexiToolBarSection * section);
	void removeSectionBreak( CoreFlexiToolBarSection * before);
	const bool sectionBreak(CoreFlexiToolBarSection * section) const;

private:

	QMainWindow * innerToolBar_;
	SectionsToToolbars sectionsToToolbars;
};

}

#endif // HEADER_GUARD_COREFLEXITOOLBAR_H__
