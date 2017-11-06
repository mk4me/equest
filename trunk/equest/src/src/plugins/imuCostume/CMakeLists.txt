BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Export.h
	Streams.h
	CostumeIMUExtractor.h	
	IIMUDataSource.h
	IIMUOrientationEstimationAlgorithm.h
	IMUCostumeCalibrationAlgorithm.h
	IMUCostumeMotionEstimationAlgorithm.h
	Types.h
	CostumeProfile.h
)

SET_PRIVATE_HEADERS (
	CostumeSkeletonMotionHelper.h
	OrientationDelegate.h
	IMUCostumeListWidget.h
	DataSource.h
	IMUDatParser.h
	IMUParser.h
	IMUCFGParser.h
	IMUPerspective.h
	PCH.h	
	IMUCostumeProfileEditionWizard.h
	IMUCostumeProfileConfigurationWizard.h
	RecordingWizard.h
	CostumeParser.h
	OrientationEstimationAlgorithmDelegate.h
	SensorIDDelegate.h
	SkeletonJointsDelegate.h
	Vector3DDelegate.h
	ConnectCostumeInputDialog.h
	ModelConfigurationWizardPage.h
	ModelPhysicalDimensionsWizardPage.h
	MotionEstimationConfigurationWizardPage.h
	SensorsConfigurationWizardPage.h
)

SET_SOURCE_FILES(
	CostumeIMUExtractor.cpp
	CostumeSkeletonMotionHelper.cpp
	OrientationDelegate.cpp
	IMUCostumeListWidget.cpp
	DataSource.cpp
	IMUDatParser.cpp
	IMUParser.cpp
	IMUPerspective.cpp
	IMUCFGParser.cpp
	PCH.cpp
	Plugin.cpp	
	IMUCostumeProfileEditionWizard.cpp
	IMUCostumeProfileConfigurationWizard.cpp
	RecordingWizard.cpp
	CostumeParser.cpp
	CostumeProfile.cpp
	OrientationEstimationAlgorithmDelegate.cpp
	SensorIDDelegate.cpp
	SkeletonJointsDelegate.cpp
	Vector3DDelegate.cpp
	ConnectCostumeInputDialog.cpp
	ModelConfigurationWizardPage.cpp
	ModelPhysicalDimensionsWizardPage.cpp
	MotionEstimationConfigurationWizardPage.cpp
	SensorsConfigurationWizardPage.cpp
)

SET_MOC_FILES(
	CostumeSkeletonMotionHelper.h
	OrientationDelegate.h
	IMUCostumeListWidget.h	
	IMUCostumeProfileEditionWizard.h
	RecordingWizard.h
	SkeletonJointsDelegate.h
	OrientationEstimationAlgorithmDelegate.h
	SensorIDDelegate.h
	ConnectCostumeInputDialog.h
	ModelConfigurationWizardPage.h
	ModelPhysicalDimensionsWizardPage.h
	MotionEstimationConfigurationWizardPage.h
	SensorsConfigurationWizardPage.h
)

SET_UI_FILES(
	IMUCostumeListWidget.ui
	ModelConfigurationWizardPage.ui
	ModelPhysicalDimensionsWizardPage.ui
	MotionEstimationConfigurationWizardPage.ui
	SensorsConfigurationWizardPage.ui
	RecordingWizard.ui
	ConnectCostumeInputDialog.ui
)

SET_RC_FILES(
	imuCostumeIcons.qrc
)

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( PCH.h PCH.cpp )

END_PROJECT()