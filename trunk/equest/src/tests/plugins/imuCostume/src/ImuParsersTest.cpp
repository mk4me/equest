#include "ImuParsersTest.h"
#include "..\src\plugins\imuCostume\src\IMUCFGParser.h"
#include "..\src\plugins\imuCostume\src\IMUDatParser.h"
#include "boost\lexical_cast.hpp"
#include "boost\format.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ImuParsersTest);


void ImuParsersTest::setUp()
{
}

void ImuParsersTest::testSplit()
{
    IMU::IMUConfig conf;
    conf.imusCount = 18;
    IMU::Frames frames = IMU::IMUDatParser::parse("E:/IMU/nagranie.dat", conf.imusCount);
    std::vector<std::pair<int, int>> splits;
    splits.push_back(std::make_pair(0, 2100));
    splits.push_back(std::make_pair(2101, 3670));
    splits.push_back(std::make_pair(3671, 5250));
    splits.push_back(std::make_pair(5251, 6850));
    splits.push_back(std::make_pair(6851, 8520));
    splits.push_back(std::make_pair(8521, 10200));
    splits.push_back(std::make_pair(10201, 11800));
    splits.push_back(std::make_pair(11801, 13480));
    splits.push_back(std::make_pair(13481, 15180));
    splits.push_back(std::make_pair(15181, 16880));
    splits.push_back(std::make_pair(16881, 18500));
    splits.push_back(std::make_pair(18501, 20200));
    splits.push_back(std::make_pair(20201, 22000));
    splits.push_back(std::make_pair(25401, 27400));
    splits.push_back(std::make_pair(27401, 29100));
    splits.push_back(std::make_pair(29101, 30700));
    splits.push_back(std::make_pair(30701, 32400));
    splits.push_back(std::make_pair(32401, 35000));
    splits.push_back(std::make_pair(35001, 37500));
    splits.push_back(std::make_pair(37501, 39100));
    splits.push_back(std::make_pair(39101, 40700));
    splits.push_back(std::make_pair(40701, 42400));
    splits.push_back(std::make_pair(42401, 44100));
    splits.push_back(std::make_pair(44101, 45800));
    splits.push_back(std::make_pair(45801, 47400));
    splits.push_back(std::make_pair(47401, 49100));
    splits.push_back(std::make_pair(49101, 50800));
    splits.push_back(std::make_pair(50801, 52500));
    splits.push_back(std::make_pair(52501, 54300));
    splits.push_back(std::make_pair(54301, 56000));
    splits.push_back(std::make_pair(56001, 57800));
    splits.push_back(std::make_pair(57801, 59500));
    splits.push_back(std::make_pair(59501, 61300));
    splits.push_back(std::make_pair(61301, 63100));
    splits.push_back(std::make_pair(63101, 64800));
    splits.push_back(std::make_pair(64801, 66500));
    splits.push_back(std::make_pair(66501, 68350));
    splits.push_back(std::make_pair(68351, 70000));
    splits.push_back(std::make_pair(70001, 71700));
    splits.push_back(std::make_pair(71701, 73400));
    splits.push_back(std::make_pair(73401, 75100));
    splits.push_back(std::make_pair(75101, 76900));
    splits.push_back(std::make_pair(76901, 78600));
    splits.push_back(std::make_pair(78601, 80200));
    splits.push_back(std::make_pair(80201, 81900));
    splits.push_back(std::make_pair(81901, 83500));
    splits.push_back(std::make_pair(83501, 85300));
    splits.push_back(std::make_pair(85301, 87000));
    splits.push_back(std::make_pair(87001, 88700));
    splits.push_back(std::make_pair(88701, 90500));
    splits.push_back(std::make_pair(90501, 92300));
    splits.push_back(std::make_pair(92301, 94000));
    splits.push_back(std::make_pair(94001, 95800));
    splits.push_back(std::make_pair(95801, 97600));
    splits.push_back(std::make_pair(97601, 99400));
    splits.push_back(std::make_pair(99401, 101100));
    splits.push_back(std::make_pair(101101, 103000));
    splits.push_back(std::make_pair(103001, 104700));
    splits.push_back(std::make_pair(104701, 106400));
    splits.push_back(std::make_pair(106401, 108200));
    splits.push_back(std::make_pair(108201, 110000));
    splits.push_back(std::make_pair(110001, 111800));
    splits.push_back(std::make_pair(111801, 113500));
    splits.push_back(std::make_pair(113501, 115400));
    splits.push_back(std::make_pair(115401, 117300));
    splits.push_back(std::make_pair(117301, 119000));
    splits.push_back(std::make_pair(119001, 120800));
    splits.push_back(std::make_pair(120801, 122600));
    splits.push_back(std::make_pair(122601, 124400));
    splits.push_back(std::make_pair(124401, 126300));
    splits.push_back(std::make_pair(126301, 128000));
    splits.push_back(std::make_pair(128001, 129600));
    splits.push_back(std::make_pair(129601, 131700));
    splits.push_back(std::make_pair(131701, 132300));
    splits.push_back(std::make_pair(132301, 133000));
    splits.push_back(std::make_pair(133001, 133500));
    splits.push_back(std::make_pair(133501, 134191));
    splits.push_back(std::make_pair(134192, 135341));
    auto framesVector = IMU::IMUDatSplitter::split(frames, splits);

    int count = framesVector.size();
    for (int i = 0; i < count; ++i) {
        boost::format fmt("E:/IMU/2014-06-16-B9100-S0001-T%04d.dat");
        fmt % (i + 1);
        IMU::IMUDatParser::save(fmt.str(), framesVector[i]);
    }
}
