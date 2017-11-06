/********************************************************************
	created:  2014/12/29	18:52:19
	filename: CoreQuaternionValidator.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__COREQUATERNIONVALIDATOR_H__
#define __HEADER_GUARD_COREUI__COREQUATERNIONVALIDATOR_H__

#include <QtCore/QObject>
#include <osg/Quat>
#include <coreui/CoreQtMetaTypes.h>

namespace coreUI
{
	class CoreQuaternionValidator : public QObject
	{
		Q_OBJECT

	public:

		explicit CoreQuaternionValidator(QObject * parent = nullptr);
		virtual ~CoreQuaternionValidator();

		virtual bool fixup(osg::Quat & input) const;
		virtual bool validate(const osg::Quat & input) const = 0;
	};
}

#endif	// __HEADER_GUARD_COREUI__COREQUATERNIONVALIDATOR_H__