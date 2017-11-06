/********************************************************************
	created:  2014/12/29	19:53:17
	filename: CoreUnitQuaternionValidator.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__COREUNITQUATERNIONVALIDATOR_H__
#define __HEADER_GUARD_COREUI__COREUNITQUATERNIONVALIDATOR_H__

#include <coreui/CoreQuaternionValidator.h>

namespace coreUI
{
	class CoreUnitQuaternionValidator : public CoreQuaternionValidator
	{
		Q_OBJECT

	public:

		explicit CoreUnitQuaternionValidator(QObject * parent = nullptr);
		virtual ~CoreUnitQuaternionValidator();

		virtual bool validate(const osg::Quat & input) const override;
	};
}

#endif	// __HEADER_GUARD_COREUI__COREUNITQUATERNIONVALIDATOR_H__