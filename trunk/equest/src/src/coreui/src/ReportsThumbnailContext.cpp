#include "CoreUiPCH.h"
#include <coreui/ReportsThumbnailContext.h>
#include <corelib/PluginCommon.h>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTabWidget>
#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

using namespace coreUI;

ReportsThumbnailContext::ReportsThumbnailContext( QTabWidget * flexiTabWidget, QWidget* reportsTab) :
    flexiTabWidget(flexiTabWidget),
    flexiSection(nullptr),
    groupID(-1),
    projectName(nullptr),
    projectTemplate(nullptr),
    cssCombo(nullptr),
    reportsTab(reportsTab)
{

}

void ReportsThumbnailContext::activateContext( QWidget * contextWidget )
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest ju¿ zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    groupID = flexiTabWidget->addTab(flexiSection, QObject::tr("Raports Tab"));
    flexiSection->setVisible(true);
    flexiTabWidget->setCurrentIndex(groupID);
}

void ReportsThumbnailContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(groupID != -1){
        flexiTabWidget->removeTab(groupID);
        flexiSection->setVisible(false);
        groupID = -1;
    }
}

void ReportsThumbnailContext::onRegisterContextWidget( QWidget * contextWidget )
{
    flexiSection = new QWidget(flexiTabWidget);
    flexiSection->setVisible(false);
    QVBoxLayout* l = new QVBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    QToolButton* button = new QToolButton();
    connect(button, SIGNAL(clicked()), this, SLOT(createRaport()));
    button->setText(tr("Create raport"));

    projectName = new QPlainTextEdit();
    projectName->setMaximumSize(100, 20);
    projectName->setPlainText(tr("Simple Raport"));
    projectName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QString dirPath = QString::fromStdString(plugin::getResourcePath("templates").string());
    QStringList filters;
    filters << "*.htm" << "*.html";
    QDir templateDir(dirPath);
    templateDir.setNameFilters(filters);

    projectTemplate = new QComboBox();
    projectTemplate->addItem(tr("Empty"));
    projectTemplate->addItems(templateDir.entryList());
    projectTemplate->setMaximumSize(100, 20);
    projectTemplate->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QDir cssDir(dirPath, "*.css");
    cssCombo = new QComboBox();
    cssCombo->addItem(tr("Empty"));
    cssCombo->addItems(cssDir.entryList());
    cssCombo->setMaximumSize(100, 20);
    cssCombo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QWidget* upper = new QWidget();
    upper->setLayout(new QHBoxLayout());
    QLabel* label = new QLabel("Raport name:");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    upper->layout()->addWidget(label);
    upper->layout()->addWidget(projectName);
    upper->layout()->setContentsMargins(1, 0, 1, 1);

    QWidget* lower = new QWidget();
    lower->setLayout(new QHBoxLayout());
    label = new QLabel(tr("Raport template:"));
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lower->layout()->addWidget(label);
    lower->layout()->addWidget(projectTemplate);
    lower->layout()->addWidget(cssCombo);
    lower->layout()->setContentsMargins(1, 1, 1, 0);

    l->addWidget(upper);
    l->addWidget(lower);
    l->addWidget(button);
    flexiSection->setLayout(l);
}

void ReportsThumbnailContext::onUnregisterContextWidget( QWidget * contextWidget )
{

}

QString ReportsThumbnailContext::createRaport(const QWidget* thumbs, const QString& projectName, const QString& templateReport, const QString& css)
{
    const QWidget* thumbParent = thumbs; //this->reportsTab;

    QObjectList children = thumbParent->children();
    QDateTime time = QDateTime::currentDateTime();
    QString images = "<table>";
    int counter = 0;
    for (auto it = children.begin(); it != children.end(); ++it) {
        QLabel* l = qobject_cast<QLabel*>(*it);
        if (l) {
            const QPixmap* pixmap = l->pixmap();
            if (pixmap) {
                QBuffer buffer;
                pixmap->save(&buffer, "PNG");
                int w = pixmap->width();
                int h = pixmap->height();
                int maxWidth = 800;
                if (w > maxWidth) {
                    h *= static_cast<double>(maxWidth) / w;
                    w = maxWidth;
                }
                QString base64 = buffer.buffer().toBase64();
                images += tr("<tr><td>Screenshot %1 </td></tr>").arg(counter++);
                images += tr("<tr><td><IMG SRC=\"data:image/png;base64,%1\" ALIGN=BOTTOM WIDTH=%2 HEIGHT=%3 BORDER=0></P> </td></tr>").arg(base64).arg(w).arg(h);
            }

        }
    }
    images += "</table>";

    QString html;
    QString cssStyle = css;
    if (templateReport.isEmpty()) {
        if (css.length()) {
            cssStyle = QString(
                "<HEAD>                    "
                "<style type=\"text/css\"> "
                "%1                        "
                "</style>                  "
                "</HEAD> ").arg(css);
        }
        html = QString(
            "<HTML> "
            "%1     "
            "<BODY> "
            "<P><FONT SIZE=8> %3</FONT></P>"
            "%2     "
            "</BODY>"
            "</HTML>").arg(cssStyle).arg(images).arg(projectName);
    } else {
      html = templateReport.arg(projectName).arg(time.toString()).arg(images).arg(css);
    }
    
    return html;
}