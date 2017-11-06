/********************************************************************
	created:	2013/11/05
	created:	5:11:2013   17:49
	filename: 	Serializers.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__SERIALIZERS_H__
#define HEADER_GUARD_DICOM__SERIALIZERS_H__

#include <QtCore/QPointF>
#include <string>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <loglib/Exceptions.h>

namespace boost {
    namespace serialization {

        template<class Archive>
        inline void save( Archive& ar, const QPointF& p, const unsigned int /*version*/ )
        {
            using boost::serialization::make_nvp;
            float x = p.x();
            float y = p.y();
            ar << make_nvp("PointX", x);
            ar << make_nvp("PointY", y);
        }

        template<class Archive>
        inline void load( Archive& ar, QPointF& p, const unsigned int /*version*/ )
        {
            using boost::serialization::make_nvp;

            float x = 0, y = 0;
            ar >> make_nvp("PointX", x);
            ar >> make_nvp("PointY", y);
            p = QPointF(x, y);
        }

        template<class Archive>
        inline void serialize( Archive& ar, QPointF& s, const unsigned int file_version )
        {
            boost::serialization::split_free(ar, s, file_version);
        }

        // ---------------------------------------------------------------------------------------------

        template<class Archive>
        inline void save( Archive& ar, const QVector<QPointF>& v, const unsigned int /*version*/ )
        {
            using boost::serialization::make_nvp;
            std::string res;
            for (auto it = v.begin(); it != v.end(); ++it) {
                res += boost::lexical_cast<std::string>(it->x()) + " ";
                res += boost::lexical_cast<std::string>(it->y()) + " ";
            }

            ar << make_nvp("PointsXY", res);
        }

        template<class Archive>
        inline void load( Archive& ar, QVector<QPointF>& v, const unsigned int /*version*/ )
        {
            using boost::serialization::make_nvp;
            std::string res;
            ar >> make_nvp("PointsXY", res);
            std::vector<float> numbers;
            boost::char_separator<char> sep(" ");
            boost::tokenizer< boost::char_separator<char> > t(res, sep);

            std::transform( t.begin(), t.end(), std::back_inserter(numbers), &boost::lexical_cast<float,std::string> );

            int count = numbers.size();
            if ((count % 2) == 0) {
                for (int i = 0; i < count; i += 2) {
                    v.push_back(QPointF(numbers[i], numbers[i + 1]));
                }
            } else {
                throw loglib::runtime_error("Wrong points number in xml file");
            }
        }

        template<class Archive>
        inline void serialize( Archive& ar, QVector<QPointF>& v, const unsigned int file_version )
        {
            boost::serialization::split_free(ar, v, file_version);
        }

    } // namespace serialization
} // namespace boost

#endif
