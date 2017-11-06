/********************************************************************
    created:  2011/12/12
    created:  12:12:2011   9:58
    filename: CoreFlexiToolBarSection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_FLEXITOOLBARSECTION_H__
#define HEADER_GUARD_FLEXITOOLBARSECTION_H__

#include <coreui/Export.h>
#include <QtWidgets/QFrame>

class QAction;

namespace Ui {
	class CoreFlexiToolBarSection;
}

namespace coreUI {

class COREUI_EXPORT CoreFlexiToolBarSection : public QFrame
{
    Q_OBJECT;

public:
    CoreFlexiToolBarSection(const QString & sectionName = QString(), const QPixmap & icon = QPixmap(),
		QAction * configAction = 0, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~CoreFlexiToolBarSection();

    void setName(const QString & sectionName);
	const QString name() const;
    void setConfigurationAction(QAction * configAction);
	QAction * configurationAction() const;
    void setIcon(const QPixmap & icon);
	const QPixmap icon() const;
    void setInnerWidget(QWidget * widget);
	QWidget * innerWidget() const;

private:
    QWidget * innerWidget_;
	Ui::CoreFlexiToolBarSection * ui;
};

}

#endif // HEADER_GUARD_FLEXITOOLBARSECTION_H__

