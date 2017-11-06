#include <plugins/hmmlib/ConfigurationDialog.h>
#include <corelib/PluginCommon.h>
#include <QtCore/QFileInfo>
#include <QtWidgets/QListWidgetItem>
#include <utils/Debug.h>
#include <tinyxml2.h>
#include <loglib/Exceptions.h>
#include "ui_FilterConfiguratorWidget.h"

const float SCALE = 0.6f;

ConfigurationWidget::ConfigurationWidget(QWidget* parent) :
    QWidget(parent),
    painterFront(this),
    painterBack(this),
    isFront(true),
    ui(new Ui::FilterConfiguratorWidget())
{
    painterFront.setScale(SCALE);
    painterBack.setScale(SCALE);
    ui->setupUi(this);
    scrollLayout.setMargin(0);
    scrollLayout.setContentsMargins(0, 0, 0, 0);
    ui->scrollAreaWidget->setLayout(&scrollLayout);
    scrollLayout.addWidget(&painterFront);
    scrollLayout.addWidget(&painterBack);
    painterBack.hide();
    setWindowFlags(Qt::Tool);
    connect(&painterFront, SIGNAL(elementSelected(const QString&, bool)), this, SLOT(onItemSelected(const QString&, bool)));
    connect(&painterBack, SIGNAL(elementSelected(const QString&, bool)), this, SLOT(onItemSelected(const QString&, bool)));
    connect(&painterFront, SIGNAL(elementHovered(const QString&, bool)), this, SLOT(onElementHovered(const QString&, bool)));
    connect(&painterBack, SIGNAL(elementHovered(const QString&, bool)), this, SLOT(onElementHovered(const QString&, bool)));
}

void ConfigurationWidget::setBackground(ConfigurationPainter& painter, const QString& name, QImageConstPtr pixmap )
{
    int w = static_cast<int>(SCALE * pixmap->width());
    int h = static_cast<int>(SCALE * pixmap->height());
    ui->scrollArea->setMaximumSize(w, h);
    ui->scrollArea->setMinimumSize(w, h);
    ui->scrollAreaWidget->setGeometry(QRect(0, 0, w, h));
    painter.setBackground(name, pixmap);
}

void ConfigurationWidget::loadPicture(ConfigurationPainter& painter, const QString& name, int x, int y, bool alwaysVisible)
{
    SinglePicturePtr data(new SinglePicture(name, x, y, alwaysVisible));
    data->setActive(true);
    painter.addArea(data);
}

void ConfigurationWidget::loadPictures(ConfigurationPainter& painter, const QStringList &files )
{
    int count = files.size();
    for (int i = 0; i < count; ++i) {
        const QString& name = files.at(i);
        loadPicture(painter, name, 0, 0);
    }

    painter.repaint(painter.rect());
}

void ConfigurationWidget::createMarker( ConfigurationPainter &painter, const QString& name, int x, int y)
{
    SingleMarkerPtr marker(new SingleMarker(name, x, y));
    marker->setActive(true);
    painter.addArea(marker);
}


void ConfigurationWidget::loadConfigurations(const QString& frontXml, const QString& backXml, const  std::map<QString, std::pair<QString, QString>>& names)
{
    loadXml(painterFront, frontXml);
    loadXml(painterBack, backXml);

    painterFront.intersectNames(names);
    painterBack.intersectNames(names);

    //loadPicture(painterFront, QString::fromStdString(plugin::getResourcePath("images/przod.png").string()), 0, 0, true);
    //loadPicture(painterBack, QString::fromStdString(plugin::getResourcePath("images/tyl.png").string()), 0, 0, true);
    QImagePtr imgFront = utils::make_shared<QImage>(QString::fromUtf8(":/hmm/icons/przod.png"));
    SinglePicturePtr front(new SinglePicture("__front", imgFront , 0, 0, true));
    front->setActive(true);
    painterFront.addArea(front);

    QImagePtr imgBack = utils::make_shared<QImage>(QString::fromUtf8(":/hmm/icons/tyl.png"));
    SinglePicturePtr back(new SinglePicture("__back", imgBack , 0, 0, true));
    back->setActive(true);
    painterBack.addArea(back);
}



void ConfigurationWidget::loadXml(ConfigurationPainter& painter, const QString &filename )
{
    QFileInfo fileInfo(filename);
    QString path = fileInfo.absolutePath();
    tinyxml2::XMLDocument document;
	if (document.LoadFile(filename.toStdString().c_str()) != tinyxml2::XML_SUCCESS) {
        throw loglib::runtime_error("Unable to load file");
    }
    tinyxml2::XMLHandle hDocument(&document);
    tinyxml2::XMLElement* root = hDocument.FirstChildElement().ToElement();
    if (!root) {
        throw loglib::runtime_error("Unable to load file");
    }

    QString background(root->Attribute("BackgroundFilename"));
    background = path + "\\" + background;
    QImagePtr img = utils::make_shared<QImage>(background);
    setBackground(painter, background, img);

    tinyxml2::XMLElement* element = root->FirstChildElement();
    while (element) {
        int x, y;
        element->QueryIntAttribute("X", &x);
		element->QueryIntAttribute("Y", &y);
        std::string val = element->Value();
        if (val == "Picture") {
            QString filename(element->Attribute("Filename"));
            filename = path + "\\" + filename;
            loadPicture(painter, filename, x, y);
        } else if (val == "Marker") {
            QString name(element->Attribute("Name"));
            createMarker(painter, name, x, y);

        }
        element = element->NextSiblingElement();
    }
}

void ConfigurationWidget::onElementHovered( const QString& name, bool selected )
{
    Q_EMIT elementHovered(name, selected);

}

void ConfigurationWidget::setText( const QString& text )
{
    ui->label->setText(text);
}

void ConfigurationWidget::setVisibles( const std::map<QString, bool>& visibles )
{
    for (auto it = painterFront.begin(); it != painterFront.end(); ++it) {
        auto found = visibles.find((*it)->getName());
        if (found == visibles.end()) {
            (*it)->setActive(true);
        } else {
            (*it)->setActive(found->second);
        }
    }
    for (auto it = painterBack.begin(); it != painterBack.end(); ++it) {
        auto found = visibles.find((*it)->getName());
        if (found == visibles.end()) {
            (*it)->setActive(true);
        } else {
            (*it)->setActive(found->second);
        }
    }
}

void ConfigurationWidget::onItemSelected( const QString& name, bool selected )
{
    if (name == "__front") {
        showBack();
    } else if (name == "__back") {
        showFront();
    } else {
        Q_EMIT itemSelected(name, selected);
        this->painterFront.trySetActive(name, selected);
        this->painterBack.trySetActive(name, selected);
    }
}


void ConfigurationWidget::showFront()
{
    UTILS_ASSERT(!isFront);
    this->painterFront.show();
    this->painterBack.hide();
    this->currentPainter = &painterFront;
    isFront = true;
}

void ConfigurationWidget::showBack()
{
    UTILS_ASSERT(isFront);
    this->painterFront.hide();
    this->painterBack.show();
    this->currentPainter = &painterBack;
    isFront = false;
}

