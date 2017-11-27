/********************************************************************
    created:  2017/10/24 19:35
    filename: EquestAnnotation.h
    author:   Wojciech Knieæ
    
    purpose: 
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_EQUEST__EquestAnnotation_h__
#define HEADER_GUARD_PLUGIN_EQUEST__EquestAnnotation_h__

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "qstring_serialization.h"
#include "utils/Filesystem.h"

namespace equest {

	class EquestAnnotations
	{
	private:
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive & ar, const unsigned int version) {
			ar & BOOST_SERIALIZATION_NVP(annotations);
		}
	public:
		struct Entry
		{
			friend class boost::serialization::access;
			template<class Archive> void serialize(Archive & ar, const unsigned int version) {
				ar & BOOST_SERIALIZATION_NVP(startTime);
				ar & BOOST_SERIALIZATION_NVP(endTime);
				ar & BOOST_SERIALIZATION_NVP(text);
			}
			double startTime;
			double endTime;
			QString text;
		};
	public:
		std::vector<Entry> annotations;

		static void save(const utils::Filesystem::Path& path, const EquestAnnotations& annotations);
		static EquestAnnotations load(const utils::Filesystem::Path& path);
	};
	DEFINE_SMART_POINTERS(EquestAnnotations);
}

#endif