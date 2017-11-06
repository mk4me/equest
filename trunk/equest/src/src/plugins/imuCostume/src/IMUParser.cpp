#include "PCH.h"
#include "IMUParser.h"
#include "IMUDatParser.h"
#include "IMUCFGParser.h"
#include <utils/Filesystem.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>

using namespace IMU;

//void IMUParser::parse(const std::string & source)
//{
//    accWrapper = utils::ObjectWrapper::create<VectorChannelCollection>();
//    magWrapper = utils::ObjectWrapper::create<VectorChannelCollection>();
//    gyrWrapper = utils::ObjectWrapper::create<VectorChannelCollection>();
//    //quatWrapper = utils::ObjectWrapper::create<kinematic::JointAngleChannel>();
//
//    utils::Filesystem::Path path(source);
//    utils::Filesystem::Path dir = path.parent_path();
//    //auto cfgFiles = utils::Filesystem::listFiles(dir, false, ".cfg");
//    IMUConfigPtr config = utils::make_shared<IMUConfig>();
//    if (cfgFiles.size() == 1) {
//        config = IMUCFGParser::parse(cfgFiles[0].string());
//    } else {
//        //throw loglib::runtime_error("No config file");
//    }
//    VectorChannelCollectionPtr accelerations = utils::make_shared<VectorChannelCollection>();
//    VectorChannelCollectionPtr magnetometers = utils::make_shared<VectorChannelCollection>();
//    VectorChannelCollectionPtr gyroscopes = utils::make_shared<VectorChannelCollection>();
//
//    int hz = 50;
//	IMU::Frames frames;
//	if (config->imusCount != -1) {
//		frames = IMU::IMUDatParser::parse(source, config->imusCount);
//	}
//	else {
//		auto res = IMU::IMUDatParser::parse(source);
//		frames = res.first;
//		config->imusCount = res.second;
//	}
//
//    
//
//    for (int i = 0; i < config->imusCount; ++i) {
//        accelerations->addChannel(createChannel(hz, *config, i));
//
//        magnetometers->addChannel(createChannel(hz, *config, i));
//        gyroscopes->addChannel(createChannel(hz, *config, i));
//    }
//    
//    for (auto itFrame = frames.begin(); itFrame != frames.end(); ++itFrame) {
//        const IMU::Frame& f = *itFrame;
//        for (int i = 0; i < config->imusCount; ++i) {
//            accelerations->getChannel(i)->addPoint(osg::Vec3(f[i].raw.acc_x, f[i].raw.acc_y, f[i].raw.acc_z));
//            magnetometers->getChannel(i)->addPoint(osg::Vec3(f[i].raw.mag_x, f[i].raw.mag_y, f[i].raw.mag_z));
//            gyroscopes->getChannel(i)->addPoint(osg::Vec3(f[i].raw.rate_x, f[i].raw.rate_y, f[i].raw.rate_z));
//        }
//    }
//    
//    accWrapper->set(accelerations);
//    magWrapper->set(magnetometers);
//    gyrWrapper->set(gyroscopes);
//}
//
//void IMUParser::reset()
//{
//    accWrapper.reset();
//    magWrapper.reset();
//    gyrWrapper.reset();
//    quatWrapper.reset();
//}
//
//plugin::IParser* IMUParser::create() const
//{
//    return new IMUParser();
//}
//
//void IMUParser::acceptedExpressions(Expressions & expressions) const
//{
//    ExpressionDescription expDesc;
//    expDesc.description = "Imu dat format";
//
//    expDesc.objectsTypes.push_back(typeid(VectorChannelCollection));
//    expDesc.objectsTypes.push_back(typeid(VectorChannelCollection));
//    expDesc.objectsTypes.push_back(typeid(VectorChannelCollection));
//    //expDesc.objectsTypes.push_back(typeid(kinematic::JointAngleChannel));
//    expressions.insert(Expressions::value_type(".*\\.dat", expDesc));
//}
//
//void IMUParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
//{
//    switch (idx) {
//    case 0: object.set(accWrapper); break;
//    case 1: object.set(magWrapper); break;
//    case 2: object.set(gyrWrapper); break;
//    case 3: object.set(quatWrapper); break;
//    }
//}
//
//VectorChannelPtr IMU::IMUParser::createChannel(int hz, const IMUConfig& config, int i)
//{
//    auto name = [&](const IMU::IMUConfig& conf, int idx) -> std::string {
//		auto size = conf.joints.size();
//        for (int i = 0; i < size; ++i) {
//            if (conf.joints[i].idx == idx) {
//                return conf.joints[i].name;
//            }
//        }
//		QString unknown = QObject::translate("Sensor %1").arg(idx);
//        return unknown.toStdString();
//    };
//
//    auto c = utils::make_shared<VectorChannel>(hz);
//    // + 1 , bo numeracja w plikach *.cfg zaczyna się od 1
//    c->setName(name(config, i + 1));
//
//    return c;
//}
//

