BEGIN_INSTALLER("MEDUSA" "Medusa" "MedusaInstaller.cmake" "product")

SET_INSTALLER_VENDOR("PJATK")
SET_INSTALLER_VERSION("0.0.2")
SET_INSTALLER_ADDITIONAL_INFO("http://hm.pjwstk.edu.pl" "http://hmkb.pjwstk.edu.pl" "Marek.Kulbacki@pjwstk.edu.pl")

ADD_INSTALLATION_TYPE(Full "Complete installation")
ADD_INSTALLATION_TYPE(Min "Minimal installation")

BEGIN_INSTALLER_GROUP(core "Core" "Core application elements")
ADD_INSTALLER_GROUP_COMPONENT(corelib "corelib" "General purpose library" "REQUIRED" "Full Min")
ADD_INSTALLER_GROUP_COMPONENT(coreui "coreui" "UI support library" "REQUIRED" "Full Min")
ADD_INSTALLER_GROUP_COMPONENT(medusa_view "medusa_view" "Medusa view" "HIDDEN" "Full Min")

END_INSTALLER_GROUP()

BEGIN_INSTALLER_GROUP(plugins "Plugins" "Built-in application plugins")

ADD_INSTALLER_GROUP_COMPONENT(plugin_c3d "c3d" "Plugin supporting reading C3D files data" "" "Full Min")
ADD_INSTALLER_GROUP_COMPONENT(plugin_newTimeline "newTimeline" "Plugin offering time managment and synchronization" "" "Full Min")
ADD_INSTALLER_GROUP_COMPONENT(plugin_kinematic "kinematic" "Plugin offering skeleton and kinematic data managment and visualisation" "" "Full Min")
ADD_INSTALLER_GROUP_COMPONENT(plugin_newCommunication "newCommunication" "Plugin offering motion database and services access" "" "Full Min")
ADD_INSTALLER_GROUP_COMPONENT(plugin_subject "subject" "Plugin supporting motion data organisation" "" "Full Min")
ADD_INSTALLER_GROUP_COMPONENT(plugin_dicom "dicom" "Plugin delivering dicom images support" "" "Full")
ADD_INSTALLER_GROUP_COMPONENT(plugin_medusaExporter "MedusaExporter" "Plugin adds data exporting operations" "" "Full")

END_INSTALLER_GROUP()

SET_INSTALLER_FINISH_RUN_APP(medusa_view)

END_INSTALLER()