#include "DicomPCH.h"
#include "PointsLayer.h"
#include <memory>
#include <QtWidgets/QGraphicsItemGroup>
#include <QtWidgets/QGraphicsPathItem>
#include <QtGui/QPen>
#include "spline.h"
#include "boost/serialization/export.hpp"


dicom::PointsLayer::PointsLayer(const int annotationIDX) :
	ILayerGraphicItem(annotationIDX),
    group (new QGraphicsItemGroup()),
    pathItem(new QGraphicsPathItem()),
    name("VPoints")
{
    pathItem->setParentItem(group.get());
    group->setHandlesChildEvents(false);
}

dicom::PointsLayer::~PointsLayer()
{
}

QGraphicsItem* dicom::PointsLayer::getItem()
{
    if (pathItem->path().isEmpty()) {
        refresh();
    }
    return group.get();
}

QString dicom::PointsLayer::getName() const
{
    return name;
}


QSize dicom::PointsLayer::getSize() const
{
    QSize s(0, 0);
    for (auto it = points.begin(); it != points.end(); ++it) {
        QPointF pos = (*it)->scenePos();
        int w = pos.x();
        int h = pos.y();
        if (w > s.width()) {
            s.setWidth(w);
        }

        if (h > s.height()) {
            s.setHeight(h);
        }
    }

    return s;
}

void dicom::PointsLayer::addPoint( const QPointF& p, int idx )
{
    QGraphicsItem* rect = pointsDrawer->createPoint();
    
    rect->setPos(p.x(), p.y());
    rect->setZValue(1.0);
    rect->setParentItem(group.get());
    rect->setFlag(QGraphicsItem::ItemIsMovable);
    rect->setFlag(QGraphicsItem::ItemIsSelectable, false);

    addPoint(rect, idx);
    setSelected(false);
}

void dicom::PointsLayer::addPoint( const QPointF& p)
{
    addPoint(p, points.size());
}

void dicom::PointsLayer::addPoint( QGraphicsItem* itm )
{
    addPoint(itm, points.size());
}

void dicom::PointsLayer::addPoint( QGraphicsItem* itm, int idx )
{
    points.insert(idx, itm);
    setSelected(false);
}



int dicom::PointsLayer::getNumPoint() const
{
    return points.size();
}

QGraphicsItem* dicom::PointsLayer::removePoint( int idx )
{
    QGraphicsItem* toRemove = points[idx];
    group->removeFromGroup(toRemove);
    points.erase(points.begin() + idx);
    return toRemove;

}

void dicom::PointsLayer::setName( const QString& name )
{
    this->name = name;
}


void dicom::PointsLayer::refresh()
{
    pathItem->setPath(pointsDrawer->createPath(points));
}

bool dicom::PointsLayer::hasPoint( QGraphicsItem* itm )
{
    return points.contains(itm);
}

QGraphicsItem* dicom::PointsLayer::getPoint( int idx ) const
{
    return points[idx];
}

dicom::IPointsDrawerPtr dicom::PointsLayer::getPointsDrawer() const
{
    return pointsDrawer;
}

void dicom::PointsLayer::setPointsDrawer( dicom::IPointsDrawerPtr val )
{
    pointsDrawer = val;

    pathItem->setPen(pointsDrawer->getLinePen(getEditable()));
    pathItem->setBrush(pointsDrawer->getLineBrush(getEditable()));
    pathItem->setPath(pointsDrawer->createPath(points));

    // TODO: Podmiana punktow
}

bool dicom::PointsLayer::getEditable() const
{
    return !group->handlesChildEvents();
}

void dicom::PointsLayer::setEditable( bool val )
{
    /*if (val) {
        setSelected(false);
        for (auto it = points.begin(); it != points.end(); ++it) {
            (*it)->setSelected(false);
            (*it)->setFlag(QGraphicsItem::ItemIsSelectable, !val);
        }

        pathItem->setSelected(false);

        pathItem->setFlag(QGraphicsItem::ItemIsMovable, !val);
        pathItem->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    }
    group->setHandlesChildEvents(!val);
    group->setFlag(QGraphicsItem::ItemIsMovable, !val);
    group->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    pathItem->setPen(pointsDrawer->getLinePen(val));*/

    setSelected(false);
    for (auto it = points.begin(); it != points.end(); ++it) {
        (*it)->setSelected(false);
        (*it)->setFlag(QGraphicsItem::ItemIsSelectable, true);
        (*it)->setFlag(QGraphicsItem::ItemIsMovable, val);
		(*it)->setVisible(val);
    }
    group->setHandlesChildEvents(!val);
    group->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    group->setFlag(QGraphicsItem::ItemIsMovable, !val);
    pathItem->setFlag(QGraphicsItem::ItemIsSelectable, !val);
    pathItem->setFlag(QGraphicsItem::ItemIsMovable, false);
    if (pointsDrawer) {
        pathItem->setPen(pointsDrawer->getLinePen(val));
        pathItem->setBrush(pointsDrawer->getLineBrush(val));
    }
}

bool dicom::PointsLayer::getSelected() const
{
    return group->isSelected();
}

void dicom::PointsLayer::setSelected( bool val )
{
    group->setSelected(val);
    auto childs = group->childItems();
    for (auto it2 = childs.begin(); it2 != childs.end(); ++it2) {
        (*it2)->setSelected(val);
    }
}

int dicom::PointsLayer::getPointIdx( QGraphicsItem* itm )
{
    return points.indexOf(itm);
}

dicom::PointsLayer* dicom::PointsLayer::clone() const
{
    std::unique_ptr<PointsLayer> pl(new PointsLayer(getAdnotationIdx()));    
    pl->setName(this->name);
    pl->setPointsDrawer(IPointsDrawerPtr(pointsDrawer->clone()));

    for (auto it = points.begin(); it != points.end(); ++it) {
        pl->addPoint((*it)->pos());
    }
    pl->setEditable(getEditable());

    return pl.release();
}

std::vector<QPointF> dicom::PointsLayer::getPointsCloud(int density , int normalizeLength) const
{
    UTILS_ASSERT(density >= 0);
    std::vector<QPointF> res;
    if (DrawersBuilder::isCurved((annotations::annotationsIdx)getAdnotationIdx()) || normalizeLength > 0) {
		QPainterPath path = pointsDrawer->createPath(points); 
        int newCount = normalizeLength > 0 ? normalizeLength : points.size() * (density + 1);
		for (int i = 0; i < newCount; ++i) {
            float percent = float(i) / newCount;
			auto e = path.pointAtPercent(percent);
			res.push_back(QPointF(e.x(), e.y()));
		}
	} else {
		for (auto it = points.begin(); it != points.end(); ++it) {
			res.push_back((*it)->pos());
		}
	}
	return res;
}

std::vector<QPointF> dicom::PointsLayer::getPixelPointsCloud() const
{
	auto point = [](const QPointF& p) { return QPoint(p.x(), p.y()); };
	auto isSame = [](const QPoint& p1, const QPoint& p2) { return p1.x() == p2.x() && p1.y() == p2.y();  };
	auto isDiffAdjacent = [&](const QPoint& p) {
		if (p.isNull()) return false;
		auto x2 = p.x() * p.x();
		auto y2 = p.y() * p.y();
		return (x2 + y2) <= 2 && (x2 == 1 || y2 == 1);
	};

	std::vector<QPointF> res;
	QPainterPath path = pointsDrawer->createPath(points);
	int count = path.elementCount();
	double delta = 1.0 / count;
	double progress = 0.0;
	while (progress <= 1.0) {
		auto raw = path.pointAtPercent(progress);
		auto e = point(raw);
		res.push_back(e);
		auto next = point(path.pointAtPercent(progress + delta));
		bool adj = isDiffAdjacent(e - next);
		bool same = isSame(e, next);
		while (!adj && !same) {
			delta *= 0.5;
			next = point(path.pointAtPercent(progress + delta));
			adj = isDiffAdjacent(e - next);
			same = isSame(e, next);
		}
		progress += delta;
		while (!adj && progress <= 1) {
			progress += delta;
			next = point(path.pointAtPercent(progress));
			adj = isDiffAdjacent(e - next);
		}
	}
	return res;
}





QGraphicsItem* dicom::PolyDrawer::createPoint()
{
    const int size = 5;
    auto rect = new QGraphicsRectItem(-size, -size, size * 2, size * 2);
    rect->setPen(getLinePen(true));
    return rect;
}

QString dicom::PolyDrawer::methodName()
{
    return "Polygon";
}

QPainterPath dicom::PolyDrawer::createPath(const QVector<QGraphicsItem*>& points)
{
    int count = points.size();
    if (count >= 2) {
        QPolygonF poly;
        for (auto it = points.begin(); it != points.end(); ++it) {
            poly << (*it)->pos();
        }
        if (!openLine) {
            poly << (*points.begin())->pos();
        }
        QPainterPath path;
        path.addPolygon(poly);
        return path;
    }

    return QPainterPath();
}

QBrush dicom::PolyDrawer::getLineBrush( bool editable )
{

    return openLine ? QBrush() : QBrush(editable ? 
        QColor(colorEdit.red(), colorEdit.green(), colorEdit.blue(), 7) : 
        QColor(color.red(), color.green(), color.blue(), 7));
}

QPen dicom::PolyDrawer::getLinePen(bool editable)
{
    QPen pen(editable ? colorEdit : color);
    pen.setWidth(1);
    return pen;
}

dicom::IPointsDrawer* dicom::PolyDrawer::clone() const
{
    //return QBrush(editable ? QColor(0, 100, 255, 30) : QColor(128, 212, 220, 30));
    return new PolyDrawer(openLine, color, colorEdit);
}

dicom::PolyDrawer::PolyDrawer( bool openLine, const QColor& color, const QColor& colorEdit) :
    color(color),
    colorEdit(colorEdit),
    openLine(openLine)
{

}


QGraphicsItem* dicom::CurveDrawer::createPoint()
{
    int size = 5;
    auto itm = new QGraphicsEllipseItem(-size, -size, 2 * size, 2 * size);
    itm->setPen(getLinePen(true));
    return itm;
}

QString dicom::CurveDrawer::methodName()
{
    return "Curve";
}

QPainterPath dicom::CurveDrawer::createPath(const QVector<QGraphicsItem*>& points)
{
    int count = points.size();
    if (count > 2) {
        QPolygonF poly;
        auto beginPos = (*points.begin())->pos();
        auto endPos = (*(points.end() - 1))->pos();
        for (auto it = points.begin(); it != points.end(); ++it) {
            poly << (*it)->pos();
        }
        if (openLine == false && poly.isClosed() == false) {
            poly << beginPos;
        }

        SplineCurveFitter fitter;
        poly = fitter.fitCurve(poly);
        QPainterPath path;
        path.addPolygon(poly);
        return path;
    }

    return QPainterPath();
}

QBrush dicom::CurveDrawer::getLineBrush( bool editable )
{
    return openLine ? QBrush() : QBrush(editable ? 
        QColor(colorEdit.red(), colorEdit.green(), colorEdit.blue(), 7) : 
        QColor(color.red(), color.green(), color.blue(), 7));
}

QPen dicom::CurveDrawer::getLinePen(bool editable)
{
    //QPen pen(editable ? QColor(255, 0, 0) : QColor(220, 128, 128));
    QPen pen(editable ? colorEdit : color);
    pen.setWidth(1);
    return pen;
}


dicom::IPointsDrawer* dicom::CurveDrawer::clone() const
{
    return new CurveDrawer(openLine, color, colorEdit);
}

dicom::CurveDrawer::CurveDrawer(bool openLine, const QColor& color, const QColor& colorEdit) :
    color(color),
    colorEdit(colorEdit),
    openLine(openLine)
{

}

std::pair<QColor, QColor> dicom::DrawersBuilder::getColors( annotations::annotationsIdx annotationIdx )
{
    switch(annotationIdx) {
        case annotations::unknownAnnotation:
            return std::make_pair(QColor(50, 50, 50), QColor(0, 0, 0));
        case annotations::otherAnnotation:                           
            return std::make_pair(QColor(150, 75, 50), QColor(100, 50, 0));
        case annotations::skin:                            
            return std::make_pair(QColor(220, 175, 128), QColor(255, 100, 0));
        case annotations::bone:                            
            return std::make_pair(QColor(128, 220, 128), QColor(0, 255, 0));
        case annotations::tendon:                          
            return std::make_pair(QColor(200, 200, 128), QColor(255, 255, 0));
        case annotations::joint: 
            return std::make_pair(QColor(128, 212, 220), QColor(0, 100, 255));
        case annotations::inflammatory:
            return std::make_pair(QColor(220, 128, 128), QColor(255, 0, 0));
        case annotations::intensity:
            return std::make_pair(QColor(50, 75, 150), QColor(0, 50, 100));
        case annotations::noise:                           
            return std::make_pair(QColor(200, 200, 200), QColor(150, 150, 150));
        default:
            throw loglib::runtime_error("Unknown annotation");
    }
}

dicom::IPointsDrawerPtr dicom::DrawersBuilder::createDrawer( annotations::annotationsIdx annotationIdx )
{
    auto color = getColors(annotationIdx);
    auto openLine = isOpenLine(annotationIdx);
    if (isCurved(annotationIdx)) {
        return utils::make_shared<CurveDrawer>(openLine, color.first, color.second);
    } else {
        return utils::make_shared<PolyDrawer>(openLine, color.first, color.second);
    }
}

bool dicom::DrawersBuilder::isOpenLine( annotations::annotationsIdx annotationIdx )
{
    switch(annotationIdx) {
    case annotations::unknownAnnotation:
        return false;
    case annotations::otherAnnotation:                           
        return false;
    case annotations::skin:                            
        return true;
    case annotations::bone:                            
        return true;
    case annotations::tendon:                          
        return true;
    case annotations::joint: 
        return false;
    case annotations::inflammatory:
        return false;
    case annotations::intensity:
        return false;
    case annotations::noise:                           
        return false;
    default:
        throw loglib::runtime_error("Unknown annotation");
    }
}

bool dicom::DrawersBuilder::isCurved( annotations::annotationsIdx annotationIdx )
{
    switch(annotationIdx) {
    case annotations::unknownAnnotation:
        return false;
    case annotations::otherAnnotation:                           
        return false;
    case annotations::skin:                            
        return true;
    case annotations::bone:                            
        return false;
    case annotations::tendon:                          
        return false;
    case annotations::joint: 
        return false;
    case annotations::inflammatory:
        return true;
    case annotations::intensity:
        return false;
    case annotations::noise:                           
        return false;
    default:
        throw loglib::runtime_error("Unknown annotation");
    }
}

QPixmap dicom::DrawersBuilder::getColorPixmap( annotations::annotationsIdx annotationIdx )
{
    static std::map<annotations::annotationsIdx, QPixmap> pixmaps;
    if (pixmaps.empty()) {
        auto makePix = [&](annotations::annotationsIdx idx) -> QPixmap {
            QPixmap pix(8, 8);
            pix.fill(getColors(idx).second);
            return pix;
        };
        pixmaps[annotations::unknownAnnotation     ] = makePix(annotations::unknownAnnotation     );
        pixmaps[annotations::otherAnnotation       ] = makePix(annotations::otherAnnotation       );
        pixmaps[annotations::skin        ] = makePix(annotations::skin        );
        pixmaps[annotations::bone        ] = makePix(annotations::bone        );
        pixmaps[annotations::tendon      ] = makePix(annotations::tendon      );
        pixmaps[annotations::joint       ] = makePix(annotations::joint       );
        pixmaps[annotations::inflammatory] = makePix(annotations::inflammatory);
        pixmaps[annotations::intensity   ] = makePix(annotations::intensity   );
        pixmaps[annotations::noise       ] = makePix(annotations::noise       );
    }
    return pixmaps[annotationIdx];
}
