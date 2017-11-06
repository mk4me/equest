#include <limits>
#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <osg/Vec3d>
#include <osg/Quat>
#include <boost/shared_ptr.hpp>
#include <list>
#include <biovisionformatslib/BvhParser.h>
#include "BvhParserTest.h"
#include <boost/filesystem.hpp>


using namespace std;
using namespace boost::filesystem;

BvhParserTest::BvhParserTest(void)
{
}

BvhParserTest::~BvhParserTest(void)
{

}


void BvhParserTest::testLoad()
{
    biovision::BvhParser bvh;
    //kinematic::SkeletalModelPtr model(new kinematic::SkeletalModel);
	//kinematic::SkeletalDataPtr data(new kinematic::SkeletalData);
    //// TODO zmienic ktorys z plikow CMakeLists bo na razie ten test nie ma sensu
    //bvh.parse(model, data, "kinematic/biovision/walks/backwards_walk.bvh");
}

void BvhParserTest::fullTest() {
    /*vector<string> files = fill("kinematic");
    for (unsigned int i = 0; i < files.size(); i++) {
        string s = files[i];
        LOGGER(Logger::Info, "Test : " + s);
        SkeletalModelPtr model1(new SkeletalModel);
        SkeletalModelPtr model2(new SkeletalModel);
		SkeletalDataPtr data1(new SkeletalData);
		SkeletalDataPtr data2(new SkeletalData);
        BvhParser b1, b2;
        b1.parse(model1, data1, s);
        b1.save(model1, data1, "temp.bvh");
        b2.parse(model2, data2, "temp.bvh");
        
        SkeletalModel::JointMap::iterator it1, it2;
        SkeletalModel::JointMap map1 = model1->getJointMap();
        SkeletalModel::JointMap map2 = model2->getJointMap();
        CPPUNIT_ASSERT(map1.size() == map2.size());
        for (it1 = map1.begin(), it2 = map2.begin(); it1 != map1.end() && it2 != map2.end(); it1++, it2++) {
            CPPUNIT_ASSERT(MiscTest::testJointContentEquality(it1->second, it2->second, false, false));
        }
    }*/
}

std::vector<std::string> BvhParserTest::fill( const std::string& rootPath ) {
    typedef std::vector<std::string> v;
    v temp;
    /*if (exists(rootPath) && is_directory(rootPath)) {

        for (directory_iterator it = directory_iterator(rootPath); it != directory_iterator(); it++) {
            if (is_directory(*it)) {
                v res = fill(it->path().string());
                temp.insert(temp.end(), res.begin(), res.end());
            } else if (is_regular_file(*it) && extension(*it) == ".bvh") {
                temp.push_back(it->path().string());
            }
        }
    } else {
        LOGGER(Logger::Info, "Zla sciezka: " + rootPath);
    }*/
    return temp;
}



