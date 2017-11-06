#include "CoreUiPCH.h"
#include <coreui/CoreQuaternionValidator.h>

using namespace coreUI;

CoreQuaternionValidator::CoreQuaternionValidator(QObject * parent)
	: QObject(parent)
{

}

CoreQuaternionValidator::~CoreQuaternionValidator()
{

}

bool CoreQuaternionValidator::fixup(osg::Quat & input) const
{
	return false;
}