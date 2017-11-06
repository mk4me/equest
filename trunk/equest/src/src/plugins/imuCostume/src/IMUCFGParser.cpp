#include "PCH.h"
#include "IMUCFGParser.h"
#include <fstream>
#include <utils/Debug.h>
#include <kinematiclib/Skeleton.h>
#include <corelib/IParser.h>
//#include <objectwrapperlib/ObjectWrapper.h>
#include <kinematicUtils/RotationConverter.h>

IMU::IMUConfigPtr IMU::IMUCFGParser::rawParse(const std::string& path)
{
    IMUConfig cfg;
    std::ifstream file;
    file.open(path);
    file >> cfg.imusCount >> cfg.imuID >> cfg.param1 >> cfg.param2;
    
    for (int i = 0; i < cfg.imusCount; ++i) {
        IMUConfig::Joint joint;
        int offsetsCount;
        file >> joint.idx >> joint.parentIdx >> joint.name >> offsetsCount;
        
        for (int j = 0; j < offsetsCount; ++j) {
            char c;
            file >> c;
            if (c == 'q') {
                float r, x, y, z;
                file >> r >> x >> y >> z;
                joint.offsets.push_back(osg::Quat(r, x, y, z));
            } else if (c == 'e') {
                float x, y, z;
                file >> x >> y >> z;
                //joint.offsets.push_back(kinematic::SkeletonUtils::getQuatFromEuler(x, y, z));
				//TODO - do weryfikacji czy dobra konwencja k¹tów eulera do konwersji kwaternionów
				joint.offsets.push_back(kinematicUtils::convertXYX(osg::Vec3(x, y, z)));
            }
        }

        cfg.joints.push_back(joint);
    }

    file.close();

    return utils::make_shared<IMUConfig>(cfg);
}

void IMU::IMUCFGParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "IMU cfg format";
	expDesc.objectsTypes.push_back(typeid(IMUConfig));
	expressions.insert(Expressions::value_type(".*\\.cfg$", expDesc));
}

plugin::IParser* IMU::IMUCFGParser::create() const
{
	return new IMUCFGParser();
}

void IMU::IMUCFGParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	object.set(cfgWrapper);
}

void IMU::IMUCFGParser::reset()
{
	cfgWrapper.reset();
}

void IMU::IMUCFGParser::parse(const std::string& path)
{
	cfgWrapper = utils::ObjectWrapper::create<IMUConfig>();
	cfgWrapper->set(rawParse(path));
}
