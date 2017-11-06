
/********************************************************************
	created:	2013/11/20
	created:	20:11:2013   18:49
	filename: 	PointsLayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__POINTSLAYER_H__
#define HEADER_GUARD_DICOM__POINTSLAYER_H__

#include <plugins/dicom/ILayer.h>
#include <QtGui/QPainterPath>
#include <QtWidgets/QGraphicsPathItem>
#include <qwt_spline.h>
#include <plugins/dicom/ValueLayer.h>
#include <QtGui/QPen>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include "qstring_serialization.h"
#include "Serializers.h"
#include <plugins/dicom/Annotations.h>
#include "boost/lexical_cast.hpp"

namespace dicom {

    

    class IPointsDrawer
    {
    public:
        virtual ~IPointsDrawer() {}
        virtual QPen getLinePen(bool editable) = 0;
        virtual QBrush getLineBrush(bool editable) = 0;
        //virtual QPen getPointPen() = 0;
        virtual QString methodName() = 0;
        virtual QPainterPath createPath(const QVector<QGraphicsItem*>& points) = 0;
        virtual QGraphicsItem* createPoint() = 0;
        virtual IPointsDrawer* clone() const = 0;
    };
    DEFINE_SMART_POINTERS(IPointsDrawer);

    class DrawersBuilder 
    {
    public:
        static IPointsDrawerPtr createDrawer(annotations::annotationsIdx annotationIdx);
        static std::pair<QColor, QColor> getColors(annotations::annotationsIdx annotationIdx);
        static bool isOpenLine(annotations::annotationsIdx annotationIdx);
        static bool isCurved(annotations::annotationsIdx annotationIdx);
        static QPixmap getColorPixmap(annotations::annotationsIdx annotationIdx);
    };

    class CurveDrawer : public IPointsDrawer 
    {
    public:
        CurveDrawer(bool openLine, const QColor& color, const QColor& colorEdit);
        QPen getLinePen(bool editable);
        QString methodName();
		QPainterPath createPath(const QVector<QGraphicsItem*>& points);
        QGraphicsItem* createPoint();

        virtual QBrush getLineBrush( bool editable );
        virtual IPointsDrawer* clone() const;
    private:
        QColor color;
        QColor colorEdit;
        bool openLine;
    };

    class PolyDrawer : public IPointsDrawer
    {
    public:
        PolyDrawer(bool openLine, const QColor& color, const QColor& colorEdit);
        QPen getLinePen(bool editable);
        QString methodName();
        QPainterPath createPath(const QVector<QGraphicsItem*>& points);
        QGraphicsItem* createPoint();

        virtual QBrush getLineBrush( bool editable );
        virtual IPointsDrawer* clone() const;

    private:
        QColor color;
        QColor colorEdit;
        bool openLine;
    };

    class PointsLayer : public ILayerGraphicItem
    {
    public:
        PointsLayer(const int annotationIDX = -1);
	    virtual ~PointsLayer();

        virtual QGraphicsItem* getItem();
        virtual QString getName() const;
        virtual void setName(const QString& name);
        virtual QSize getSize() const;
    public:
        void addPoint(const QPointF& p);
        void addPoint(const QPointF& p, int idx);
        void addPoint(QGraphicsItem* itm, int idx);
        void addPoint(QGraphicsItem* itm);
        int getNumPoint() const;
        QGraphicsItem* getPoint(int idx) const;
        QGraphicsItem* removePoint(int idx);
        void refresh();

        bool hasPoint(QGraphicsItem* itm);
        int getPointIdx(QGraphicsItem* itm);

        dicom::IPointsDrawerPtr getPointsDrawer() const;
        void setPointsDrawer(dicom::IPointsDrawerPtr val);
        bool getEditable() const;
        void setEditable(bool val);


        virtual bool getSelected() const;

        virtual void setSelected( bool val );

        virtual PointsLayer* clone() const;
        
    private:
        utils::unique_ptr<QGraphicsItemGroup> group;
        utils::unique_ptr<QGraphicsPathItem> pathItem; 
        QVector<QGraphicsItem*> points;
        QString name;
        IPointsDrawerPtr pointsDrawer;

    private:
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            QVector<QPointF> rawPoints;
            for (auto it = points.begin(); it != points.end(); ++it) {
                rawPoints.push_back((*it)->scenePos());
            }
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ILayerItem);
            ar & BOOST_SERIALIZATION_NVP(rawPoints);
            ar & BOOST_SERIALIZATION_NVP(name);
            QString method = pointsDrawer->methodName();
            ar & BOOST_SERIALIZATION_NVP(method);
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            QVector<QPointF> rawPoints;

            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ILayerItem);
            ar & BOOST_SERIALIZATION_NVP(rawPoints);
            ar & BOOST_SERIALIZATION_NVP(name);
            QString method;
            ar & BOOST_SERIALIZATION_NVP(method);
            pointsDrawer = DrawersBuilder::createDrawer(static_cast<annotations::annotationsIdx>(getAdnotationIdx()));
            pathItem->setPen(pointsDrawer->getLinePen(false));

            for (auto it = rawPoints.begin(); it != rawPoints.end(); ++it) {
                addPoint(*it);
            }
            setEditable(false);
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER();

		virtual std::vector<QPointF> getPointsCloud(int density = 0, int normalizeLength = -1) const;

		virtual std::vector<QPointF> getPixelPointsCloud() const;


    };
	typedef boost::shared_ptr<PointsLayer> PointsLayerPtr;
	typedef boost::shared_ptr<const PointsLayer> PointsLayerConstPtr;


	template<typename T>
	class ValueLayer : public IValueLayer
	{
	public:
		ValueLayer(const int annotationIDX = -1) : IValueLayer(annotationIDX), value_(T()), selected(false) {}
		ValueLayer(const int annotationIDX, const T & val) : IValueLayer(annotationIDX), value_(val), selected(false) {}
		ValueLayer(const ValueLayer & other)
			: IValueLayer(other.getAdnotationIdx()),
			value_(other.value_), name(other.getName()),
			selected(false) {}

	    virtual ~ValueLayer() {}

		virtual QString getName() const { return name; }
		virtual void setName(const QString& name) { this->name = name; }

		virtual ValueLayer<T>* clone() const
		{
			return new ValueLayer<T>(*this);
		}

		const T & value() const { return value_; }

		void setValue(const T & val) { value_ = val; }
        
    private:
        QString name;
		T value_;
		bool selected;

    private:
        friend class boost::serialization::access;

        template<class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ILayerItem);
            ar & BOOST_SERIALIZATION_NVP(value_);
            ar & BOOST_SERIALIZATION_NVP(name);
        }

        template<class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ILayerItem);
            ar & BOOST_SERIALIZATION_NVP(value_);
            ar & BOOST_SERIALIZATION_NVP(name);
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER();

		virtual std::string valueAsString() const
		{
			return annotations::annotationValueAsString<T>(value_).toStdString();
		}

	};

	typedef ValueLayer<dicom::annotations::bloodLevelDescriptor> BloodLevelLayer;
	typedef ValueLayer<dicom::annotations::inflammatoryLevelDescriptor> InflammatoryLevelLayer;
	typedef ValueLayer<dicom::annotations::fingerTypeDescriptor> FingerTypeLayer;
	typedef ValueLayer<dicom::annotations::jointTypeDescriptor> JointTypeLayer;
	typedef ValueLayer<dicom::annotations::imageTypeDescriptor> ImageQualityLayer;

	DEFINE_SMART_POINTERS(BloodLevelLayer);
	DEFINE_SMART_POINTERS(InflammatoryLevelLayer);
	DEFINE_SMART_POINTERS(FingerTypeLayer);
	DEFINE_SMART_POINTERS(JointTypeLayer);
	DEFINE_SMART_POINTERS(ImageQualityLayer);
}

#endif
