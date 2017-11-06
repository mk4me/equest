/********************************************************************
	created:  2015/01/11	17:30:37
	filename: IMUCostumeProfileConfigurationWizard.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMEPROFILECONFIGURATIONWIZARD_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMEPROFILECONFIGURATIONWIZARD_H__

#include <plugins/imuCostume/CostumeProfile.h>

class QWizard;
class QWidget;

namespace IMU
{
	class IMUCostumeProfileConfigurationWizard
	{
	public:

		static QWizard * create(const CostumeProfile & profile,
			QWidget * parent = nullptr);
	};
}

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMEPROFILECONFIGURATIONWIZARD_H__