#include "PicturePlacerWindow.h"
#include <string>
#include <boost/lexical_cast.hpp>
#include <tinyxml.h>
#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>


PicturePlacerWindow::PicturePlacerWindow() :
    painter(this),
    scrollLayout(this)
{
    setupUi(this);
    scrollLayout.setMargin(0);
    scrollLayout.setContentsMargins(0,0,0,0);
    scrollAreaWidget->setLayout(&scrollLayout);
    scrollLayout.addWidget(&painter);
    //scrollAreaWidget->layout()->addWidget(&painter);
    connect(loadBackgroundButton, SIGNAL(clicked()), this, SLOT(loadBackground()));
    connect(this->loadImagesButton, SIGNAL(clicked()), this, SLOT(loadPictures()));
    connect(this->powerButton, SIGNAL(clicked()), this, SLOT(power()));
    connect(this->actionOpen, SIGNAL(triggered()), this, SLOT(loadProject()));
    connect(this->actionSave, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(this->actionMarkersMode, SIGNAL(triggered()), this, SLOT(setMarkers()));
    connect(this->actionNormalMode, SIGNAL(triggered()), this, SLOT(setNormal()));
    connect(&this->painter, SIGNAL(markerAdded(SingleMarkerPtr)), this, SLOT(markerAdded(SingleMarkerPtr)));
    connect(this->markersList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(markerNameChanged(QListWidgetItem *)));
    connect(this->markersList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(currentItemChanged(QListWidgetItem*, QListWidgetItem*)));
    connect(this->imagesList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(currentItemChanged(QListWidgetItem*, QListWidgetItem*)));
}

void PicturePlacerWindow::showAbout()
{
    QMessageBox::information( this, "Sample", "Hello world!" );
}

void PicturePlacerWindow::loadBackground()
{
    const QString filename = QFileDialog::getOpenFileName(this, tr("Load background file"), "C:\\programming\\HMEdr\\src\\edr\\resources\\images", "*.png");
    if (!filename.isEmpty()) {
        QPixmapPtr pixmap(new QPixmap(filename));
        setBackground(filename, pixmap);
    }
 }

void PicturePlacerWindow::setBackground(const QString& name, QPixmapConstPtr pixmap )
{ 
    scrollAreaWidget->setFixedSize(pixmap->width(), pixmap->height());
    painter.setBackground(name, pixmap);
}

void PicturePlacerWindow::loadPictures()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Load image files"), "C:\\programming\\HMEdr\\src\\edr\\resources\\images", "*.png");
    loadPictures(files);
}

void PicturePlacerWindow::loadPictures( const QStringList &files )
{
    int count = files.size();
    for (int i = 0; i < count; i++) {
        const QString& name = files.at(i);
        loadPicture(name, 0, 0);
    }

    painter.repaint(painter.rect());
}

void PicturePlacerWindow::saveProject()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Load image files"), "C:\\programming\\HMEdr\\src\\edr\\resources\\images", "*.xml");
    if (!name.isEmpty()) {
        saveXml(name, painter);
    }
}

void PicturePlacerWindow::loadProject()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load background file"), "C:\\programming\\HMEdr\\src\\edr\\resources\\images", "*.xml");
    if (!filename.isEmpty()) {
        loadXml(filename);
    }
}

void PicturePlacerWindow::power()
{
    QString filename = "C:\\programming\\HMEdr\\src\\edr\\src\\hmm_view\\ui\\skeleton.png";
    QPixmapPtr pixmap(new QPixmap(filename));
    setBackground(filename, pixmap);
    QStringList list;
    list << 
    "C:\\programming\\HMEdr\\src\\edr\\src\\hmm_view\\ui\\close.png" <<
    "C:\\programming\\HMEdr\\src\\edr\\src\\hmm_view\\ui\\head.png" <<
    "C:\\programming\\HMEdr\\src\\edr\\src\\hmm_view\\ui\\logo.png" <<
    "C:\\programming\\HMEdr\\src\\edr\\src\\hmm_view\\ui\\leg.png" <<
    "C:\\programming\\HMEdr\\src\\edr\\src\\hmm_view\\ui\\logo2.png";
    loadPictures(list);
}

void PicturePlacerWindow::saveXml( const QString& filename, const Painter& painter )
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    TiXmlElement * root = new TiXmlElement( "Project" );
    QFileInfo fi(painter.getName());
    root->SetAttribute("version", "0.01");
    root->SetAttribute("BackgroundFilename", fi.fileName().toStdString());
    
    for(auto it = painter.getBegin(); it != painter.getEnd(); it++) {
        SinglePictureConstPtr picture = boost::dynamic_pointer_cast<const SinglePicture>(*it);
        if (picture) {
            QFileInfo fi((*it)->getName());
            TiXmlElement * element = new TiXmlElement( "Picture" );
            element->SetAttribute("Filename", fi.fileName().toStdString());
            element->SetAttribute("X", (*it)->getX());
            element->SetAttribute("Y", (*it)->getY());
            root->LinkEndChild(element);
        }

        SingleMarkerConstPtr marker = boost::dynamic_pointer_cast<const SingleMarker>(*it);
        if (marker) {
            TiXmlElement * element = new TiXmlElement( "Marker" );
            element->SetAttribute("Name", marker->getName().toStdString());
            int x = marker->getX() + marker->getWidth() / 2;
            int y = marker->getY() + marker->getHeight() / 2;
            element->SetAttribute("X", x);
            element->SetAttribute("Y", y);
            root->LinkEndChild(element);
        }
        
    }
    doc.LinkEndChild( decl );
    doc.LinkEndChild( root );
    doc.SaveFile( filename.toStdString() );
}

void PicturePlacerWindow::loadXml( const QString& filename )
{
    QFileInfo fileInfo(filename);
    QString path = fileInfo.absolutePath();
    TiXmlDocument document(filename.toStdString());
    if(!document.LoadFile()) {
        throw std::runtime_error("Unable to load file");
    }
    TiXmlHandle hDocument(&document);
    TiXmlElement* root = hDocument.FirstChildElement().Element();
    if (!root) {
        throw std::runtime_error("Unable to load file");
    }

    QString background(root->Attribute("BackgroundFilename"));
    background = path + "\\" + background;
    QPixmapPtr pixmap(new QPixmap(background));
    setBackground(background, pixmap);

    TiXmlElement* element = root->FirstChildElement();
    while (element) {
        int x, y;
        element->Attribute("X", &x);
        element->Attribute("Y", &y);
        std::string val = element->Value();
        if (val == "Picture") {
            QString filename(element->Attribute("Filename"));
            filename = path + "\\" + filename;
            loadPicture(filename, x, y);
        } else if (val == "Marker") {
            QString name(element->Attribute("Name"));
            SingleMarkerPtr marker(new SingleMarker(name, x, y));
            
            addMarker(marker);

        }
        
        element = element->NextSiblingElement();
    }
}

void PicturePlacerWindow::loadPicture( const QString& name, int x, int y )
{
    SinglePicturePtr data(new SinglePicture(name, x, y));
    painter.addArea(data);
    QFileInfo info(name);
    QListWidgetItem* item = new QListWidgetItem(info.baseName());
    imagesList->addItem(item);
    item2Area[item] = data;
}

void PicturePlacerWindow::undo()
{

}

void PicturePlacerWindow::redo()
{

}

void PicturePlacerWindow::setNormal()
{
    painter.setState(Painter::Normal);
}

void PicturePlacerWindow::setMarkers()
{
    painter.setState(Painter::Markers);
}

void PicturePlacerWindow::markerAdded(SingleMarkerPtr marker)
{
    QListWidgetItem* item = new QListWidgetItem(marker->getName());
    item2Area[item] = marker;
    item->setFlags (item->flags () | Qt::ItemIsEditable);
    markersList->addItem(item);
}

void PicturePlacerWindow::markerNameChanged( QListWidgetItem * current)
{
    SingleMarkerPtr marker = boost::dynamic_pointer_cast<SingleMarker>(item2Area[current]);
    if (marker) {
        marker->setName(current->text());
    }
}

void PicturePlacerWindow::currentItemChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
    if (current) {
        painter.setActiveArea(item2Area[current]);
    } else {
        painter.setActiveArea(IAreaPtr());
    }
}

void PicturePlacerWindow::addMarker( SingleMarkerPtr marker )
{
    painter.addArea(marker);
    QListWidgetItem* item = new QListWidgetItem(marker->getName());
    item2Area[item] = marker;
    item->setFlags (item->flags () | Qt::ItemIsEditable);
    markersList->addItem(item);
}

void Painter::paintEvent( QPaintEvent * )
{
    QPainter painter(this);
    if (background) {
        painter.drawPixmap(0, 0, rect().right(), rect().bottom(), *background);
    }

    for (auto it = areas.begin(); it != areas.end(); it++) {
        (*it)->draw(painter);
    }

    if (activeArea) {
        QPen pen(QColor(255,100,10));
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawRect(activeArea->getX() - 2, activeArea->getY() - 2, activeArea->getWidth() + 2, activeArea->getHeight() + 2);
    }
}

void Painter::addArea( IAreaPtr data )
{
    for (auto it = areas.begin(); it != areas.end(); it++) {
        if ((*it)->getName() == data->getName()) {
            throw std::runtime_error("Area with that name already added!");
        }
    }

    areas.push_back(data);
}

void Painter::removeArea( const QString& name )
{
    for (auto it = areas.begin(); it != areas.end(); it++) {
        if ((*it)->getName() == name) {
            areas.erase(it);
            return;
        }
    }

    throw std::runtime_error("Picture not found!");
}

void Painter::mouseMoveEvent( QMouseEvent * event )
{
    if (currentArea) {
        QPointF p = event->posF();
        int gX = p.x();
        int gY = p.y();
        currentArea->setX(currentArea->getX() + (gX - pressedX));
        currentArea->setY(currentArea->getY() + (gY - pressedY));
        pressedX = gX;
        pressedY = gY;
        repaint(rect());
    }
    
}

void Painter::mousePressEvent( QMouseEvent * event )
{
    QPointF p = event->posF();
    if (state == Normal) {
        if (event->button() == Qt::LeftButton) {
            currentArea = getArea(p.x(), p.y());
            pressedX = p.x();
            pressedY = p.y();
            repaint(rect());
        }
    } else if (state == Markers) {
        QString name = QString("Marker_%1").arg(markerCounter++);
        SingleMarkerPtr marker(new SingleMarker(name, p.x(), p.y()));
        addArea(marker);
        repaint(rect());
        emit markerAdded(marker);
        /*QListWidgetItem* item = new QListWidgetItem(name);
        markersList->addItem(item);*/
    }
}

void Painter::mouseReleaseEvent( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton) {
        currentArea.reset();
    }
}

IAreaPtr Painter::getArea( int x, int y )
{
    for (auto it = areas.rbegin(); it != areas.rend(); it++) {
        int x1 = (*it)->getX();
        int x2 = x1 + (*it)->getWidth();
        int y1 = (*it)->getY();
        int y2 = y1 + (*it)->getHeight();
        if (x1 <= x && x2 >= x && y1 <= y && y2 >= y) {
            return *it;
        }
    }

    return SinglePicturePtr();
}
