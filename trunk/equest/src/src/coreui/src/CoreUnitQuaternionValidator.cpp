#include "CoreUiPCH.h"
#include <coreui/CoreUnitQuaternionValidator.h>

using namespace coreUI;

static const double DOUBLE_EPSILON2 = 0.0001;

CoreUnitQuaternionValidator::CoreUnitQuaternionValidator(QObject * parent)
	: CoreQuaternionValidator(parent)
{

}

CoreUnitQuaternionValidator::~CoreUnitQuaternionValidator()
{

}

bool CoreUnitQuaternionValidator::validate(const osg::Quat & input) const
{
	const auto l2 = input.length2();
	return (std::fabs(1.0 - l2) <= DOUBLE_EPSILON2);
}