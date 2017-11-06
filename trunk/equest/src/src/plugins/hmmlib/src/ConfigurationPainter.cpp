#include <plugins/hmmlib/ConfigurationPainter.h>
#include <QtGui/QCloseEvent>
#include <stdexcept>
#include <utils/Debug.h>
#include <loglib/Exceptions.h>

ConfigurationPainter::ConfigurationPainter( QWidget* parent ) :
QWidget(parent),
    scale(1.0f)
{
    setMouseTracking(true);
    installEventFilter(this);
}


void ConfigurationPainter::paintEvent( QPaintEvent * )
{
    QPainter painter(this);
    if (background) {
        painter.drawImage(QRect(0, 0, scale * background->width(), scale * background->height()), *background);
    }

    for (auto it = areas.begin(); it != areas.end(); ++it) {
        (*it)->draw(painter, (*it) == currentArea);
    }
    painter.end();
}

void ConfigurationPainter::addArea( const IAreaPtr & data )
{
    for (auto it = areas.begin(); it != areas.end(); ++it) {
        if ((*it)->getName() == data->getName()) {
            throw loglib::runtime_error("Picture already added!");
        }
    }
    data->setScale(scale);
    areas.push_back(data);
}

void ConfigurationPainter::removeArea( const QString& name )
{
    for (auto it = areas.begin(); it != areas.end(); ++it) {
        if ((*it)->getName() == name) {
            areas.erase(it);
            return;
        }
    }

    throw loglib::runtime_error("Picture not found!");
}

void ConfigurationPainter::mouseMoveEvent( QMouseEvent * event )
{
	QPointF p = event->localPos();
    auto picture = getArea(p.x(), p.y());
    if (picture != currentArea) {
        if (currentArea) {
            Q_EMIT elementHovered(currentArea->getName(), false);
        }
        if (picture) {
            Q_EMIT elementHovered(picture->getName(), true);
        }
        currentArea = picture;
        repaint();
    }
}

void ConfigurationPainter::mousePressEvent( QMouseEvent * event )
{
}

void ConfigurationPainter::mouseReleaseEvent( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton && currentArea) {
        bool active = !currentArea->isActive();
        currentArea->setActive(active);
        //currentPicture->setGlobalAlpha(255);
        Q_EMIT elementSelected(currentArea->getName(), active);
        repaint();
    }
}

IAreaPtr ConfigurationPainter::getArea( int x, int y )
{
    typedef std::pair<IAreaPtr, int> pair;
    std::vector<pair> clickedAreas;
    for (auto it = areas.rbegin(); it != areas.rend(); ++it) {
        if ((*it)->isInside(x, y)) {
            int w2 = (*it)->getWidth() / 2;
            int h2 = (*it)->getHeight() / 2;
            int distX = x - ((*it)->getX() + w2);
            int distY = y - ((*it)->getY() + h2);
            // obsługa przypadku, gdy kliknięto wiecej niz jeden obszar
            // wykorzystywana jest odległość kursora od srodka (metryka kwadratowa bo jest szybsza)
            clickedAreas.push_back(std::make_pair(*it, distX * distX + distY * distY));
        }
    }

    if (clickedAreas.empty() == false) {
        // jeśli są jakieś obszary, na które kliknięto, to zwracany jest ten,
        // którego środek jest bliżej kursora
        auto lambda_sort = [&](const pair& left, const pair& right)
        {
            return left.second < right.second;
        };
        std::sort(clickedAreas.begin(), clickedAreas.end(), lambda_sort);

        return clickedAreas[0].first;
    }

    return IAreaPtr();
}

bool ConfigurationPainter::eventFilter( QObject *obj, QEvent *event )
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        this->mouseMoveEvent(mouseEvent);
    }
    return false;
}

void ConfigurationPainter::trySetActive( const QString& name, bool selected )
{
    for (auto it = areas.begin(); it != areas.end(); ++it) {
        if ((*it)->getName() == name) {
            (*it)->setActive(selected);
            return;
        }
    }
}

void ConfigurationPainter::intersectNames( const  NamesDictionary& names )
{
    std::list<AreasList::iterator> toErase;
    for (AreasList::iterator it = areas.begin(); it != areas.end(); ++it) {
        const QString& areaName = (*it)->getName();
        bool found = false;
        for (auto it2 = names.begin(); it2 != names.end(); it2++) {
            QString name = it2->first;
            if (name == areaName) {
                found = true;
                break;
            }
        }
        if (!found) {
            toErase.push_back(it);
        }
    }

    for (auto it = toErase.begin(); it != toErase.end(); ++it) {
        areas.erase(*it);
    }
}

void ConfigurationPainter::setScale( float val )
{
    UTILS_ASSERT(val > 0.0f);
    scale = val;
    for (AreasList::iterator it = areas.begin(); it != areas.end(); ++it) {
        (*it)->setScale(val);
    }
}
