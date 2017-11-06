#include "RotationConverterTest.h"
#include <kinematicUtils/RotationConverter.h>
#include <osg/Vec3>

void RotationConverterTest::testConversion()
{

}

void RotationConverterTest::testIdentity()
{
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertXYX(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertXYZ(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertXZX(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertXZY(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertYXY(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertYXZ(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertYZX(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertYZY(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertZXY(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertZXZ(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertZYX(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Quat() == kinematicUtils::convertZYZ(osg::Vec3d()));

	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertXYX(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertXYZ(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertXZX(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertXZY(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertYXY(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertYXZ(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertYZX(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertYZY(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertZXY(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertZXZ(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertZYX(osg::Quat()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::convertZYZ(osg::Quat()));
}

void RotationConverterTest::testDeg2Rad()
{
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::toRadians(osg::Vec3d()));
	CPPUNIT_ASSERT(osg::Vec3d() == kinematicUtils::toDegrees(osg::Vec3d()));

	osg::Vec3d deg = kinematicUtils::toDegrees(osg::Vec3d(0.1, 1.5, 3));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.x(), 5.7295779513100005, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.y(), 85.94366926965, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.z(), 171.8873385393, 0.000001);

	deg = kinematicUtils::toDegrees(osg::Vec3d(osg::PI_4, osg::PI_2, osg::PI));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.x(), 45.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.y(), 90.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.z(), 180.0, 0.000001);

	deg = kinematicUtils::toDegrees(osg::Vec3d(-osg::PI_4, -osg::PI_2, -osg::PI));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.x(), -45.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.y(), -90.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(deg.z(), -180.0, 0.000001);

	osg::Vec3d rad = kinematicUtils::toRadians(osg::Vec3d(0.1, 1.5, 3));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.x(), 0.0017453292519900002, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.y(), 0.02617993877985, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.z(), 0.0523598775597, 0.000001);

	rad = kinematicUtils::toRadians(osg::Vec3d(45.0, 90.0, 180.0));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.x(), osg::PI_4, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.y(), osg::PI_2, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.z(), osg::PI,   0.000001);

	rad = kinematicUtils::toRadians(osg::Vec3d(-45.0, -90.0, -180.0));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.x(), -osg::PI_4, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.y(), -osg::PI_2, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(rad.z(), -osg::PI, 0.000001);
}


void RotationConverterTest::testEulerQuatEuler()
{
	typedef kinematicUtils::AxisOrder AO;
	std::vector<AO::Type> orders{/* AO::XYX,*/ AO::XYZ/*, AO::XZX, AO::XZY, AO::YXY, AO::YXZ, AO::YZX, AO::YZY, AO::ZXY, AO::ZXZ, AO::ZYX, AO::ZYZ*/ };
	for (int x = 0; x < 360; x += 10) {
		for (int y = 0; y < 360; y += 10) {
			for (int z = 0; z < 360; z += 10) {
				osg::Vec3 rad(kinematicUtils::toRadians(osg::Vec3(x, y, z)));
				rad.normalize();
				for (auto o : orders) {
					osg::Quat q = kinematicUtils::convert(rad, o);
					osg::Vec3 r2 = kinematicUtils::convert(q, o);
					//r2.normalize();
					// obrot mo¿e byæ ten sam, ale k¹ty ró¿ne, dlatego badamy iloczyn skalarny utworzonych wektorów (znormalizowanych dla uproszczenia)
					//CPPUNIT_ASSERT(abs(r2*rad - 1.0) < 0.0001 || (r2.length() < 0.00001 && rad.length() < 0.00001));
					CPPUNIT_ASSERT(kinematicUtils::isSameDirection(rad, r2));
				}
				
			}
		}
	}
}

void RotationConverterTest::testNormalizeAngle()
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Rad>(0.0), 0.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Rad>(-osg::PI), osg::PI, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Rad>(kinematicUtils::_2PI+ 0.00000001), 0.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Rad>(-0.1), kinematicUtils::_2PI - 0.1, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Rad>(10 * kinematicUtils::_2PI + 0.1), 0.1, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Rad>(-10 * kinematicUtils::_2PI + 0.1), 0.1, 0.000001);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Deg>(0.0), 0.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Deg>(-180.0), 180.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Deg>(360.0 + 0.000001), 0.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Deg>(-1.0), 359.0, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Deg>(10 * 360.0 + 0.1), 0.1, 0.000001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(kinematicUtils::normalizeAngle<kinematicUtils::Deg>(-10 * 360.0 + 0.1), 0.1, 0.000001);
	
}

void RotationConverterTest::testCompareAngle()
{
	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Rad>(0.0, 0.0, 0.00001));
	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Rad>(-osg::PI, osg::PI, 0.00001));
	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Rad>(0.0, kinematicUtils::_2PI, 0.00001));
	CPPUNIT_ASSERT(!kinematicUtils::compareAngle<kinematicUtils::Rad>(0.0, osg::PI, 0.00001));

	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Deg>(0.0, 0.0, 0.00001));
	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Deg>(-180.0, 180.0, 0.00001));
	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Deg>(0.0, 360.0, 0.00001));
	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Deg>(10.0, 10* 360.0 + 10.0, 0.00001));
	CPPUNIT_ASSERT(kinematicUtils::compareAngle<kinematicUtils::Deg>(10.0, -350.0, 0.00001));
	CPPUNIT_ASSERT(!kinematicUtils::compareAngle<kinematicUtils::Deg>(0.0, 10.0, 0.00001));
}

void RotationConverterTest::testSameDirection()
{
	CPPUNIT_ASSERT(kinematicUtils::isSameDirection(osg::Vec3(0, 1, 0), osg::Vec3(0, 1, 0)));
	CPPUNIT_ASSERT(kinematicUtils::isSameDirection(osg::Vec3(0, 10, 0), osg::Vec3(0, 1, 0)));
	CPPUNIT_ASSERT(kinematicUtils::isSameDirection(osg::Vec3(-10, 10, 0), osg::Vec3(-1, 1, 0)));
	CPPUNIT_ASSERT(kinematicUtils::isSameDirection(osg::Vec3(), osg::Vec3()));
	CPPUNIT_ASSERT(!kinematicUtils::isSameDirection(osg::Vec3(-10, 10, 0), osg::Vec3(-1, 10, 0)));
	CPPUNIT_ASSERT(!kinematicUtils::isSameDirection(osg::Vec3(-1, 10.5, 0), osg::Vec3(-1, 10, 0)));
}

