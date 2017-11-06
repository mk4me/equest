BEGIN_INSTALLER("EDRUtils" "EDRUtils" "EDRUtilsInstaller.cmake" "dev")

SET_INSTALLER_VENDOR("PJWSTK")
SET_INSTALLER_VERSION("0.0.1")

ADD_INSTALLER_GROUP_COMPONENT(utils "utils" "General purpose classes")
ADD_INSTALLER_GROUP_COMPONENT(vidlib "vidlib" "Video decoding library")
ADD_INSTALLER_GROUP_COMPONENT(kinematiclib "kinematiclib" "Library supporting kinematic chains")
ADD_INSTALLER_GROUP_COMPONENT(osgui "osgui" "Library extending OSG capabilities")
ADD_INSTALLER_GROUP_COMPONENT(dflib "dflib" "Library supporting data flow processing")
ADD_INSTALLER_GROUP_COMPONENT(timelinelib "timelinelib" "Library offering timeline functionality")
ADD_INSTALLER_GROUP_COMPONENT(c3dlib "c3dlib" "Library offering c3d format support")
ADD_INSTALLER_GROUP_COMPONENT(webserviceslib "webserviceslib" "Library offering hml motion database access and functionality")
ADD_INSTALLER_GROUP_COMPONENT(qthtmlgenlib "qthtmlgenlib" "Library generating html documents for Qt controls")
#ADD_INSTALLER_GROUP_COMPONENT(qtutils "qtutils" "Library providing additional functionality for Qt")

END_INSTALLER()