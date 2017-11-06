#include <plugins/hmdbCommunication/ContextConfigurationSettingsFile.h>
#include <QtCore/QSettings>
#include "corelib/PluginCommon.h"
#include <networkUtils/EncryptionUtils.h>

using namespace hmdbCommunication;


//! Metoda pr�buje uaktualni� podan� warto�� parametru
//! \param s Ustawienia, z kt�rych podejmowana jest pr�ba aktualizacji parametru
//! \param propertyName Nazwa parametru do aktualizacji
//! \param param Aktualizowany parametr, zmiena we/wy
//! \param shadow Czy parametr jest szyfrowany
static void getSetting(const QSettings& s, const QString& propertyName, QString& param, bool shadow = false)
{
	param = s.value(propertyName, param).toString();
	if (shadow && s.contains(propertyName)) {
		param = networkUtils::EncryptionUtils::decrypt(param);
	}
}

IHMDBSourceViewManager::ContextConfiguration ContextConfigurationSettingsFile::read(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& defaultConf)
{
	IHMDBSourceViewManager::ContextConfiguration ccfg = defaultConf;
	QSettings settings(path, QSettings::IniFormat);
	bool shadow = settings.value("EncryptedPasswords", false).toBool();
	getSetting(settings, "ConfigName", ccfg.name);

	getSetting(settings, "StorageConfigurationPath", ccfg.storageConfiguration.path);
	getSetting(settings, "StoragePassword", ccfg.storageConfiguration.password, shadow);

	//#ifdef _DEBUG 
	getSetting(settings, "MotionConfigurationUser", ccfg.motionServicesConfiguration.userConfiguration.user);
	getSetting(settings, "MotionConfigurationPassword", ccfg.motionServicesConfiguration.userConfiguration.password, shadow);
	//#endif
	getSetting(settings, "MotionConfigurationURL", ccfg.motionServicesConfiguration.serviceConfiguration.url);
	getSetting(settings, "MotionConfigurationCertificatePath", ccfg.motionServicesConfiguration.serviceConfiguration.caPath);
	
	getSetting(settings, "MotionDataConfigurationURL", ccfg.motionDataConfiguration.serviceConfiguration.url);
	getSetting(settings, "MotionDataConfigurationCertificatePath", ccfg.motionDataConfiguration.serviceConfiguration.caPath);
	getSetting(settings, "MotionDataConfigurationUser", ccfg.motionDataConfiguration.userConfiguration.user);
	getSetting(settings, "MotionDataConfigurationPassword", ccfg.motionDataConfiguration.userConfiguration.password, shadow);
	return ccfg;
}

void ContextConfigurationSettingsFile::write(const QString& path, const IHMDBSourceViewManager::ContextConfiguration& ccfg)
{
	QSettings settings(path, QSettings::IniFormat);
	settings.setValue("ConfigName", ccfg.name);
	settings.setValue("StorageConfigurationPath", ccfg.storageConfiguration.path);
	settings.setValue("StoragePassword", ccfg.storageConfiguration.password);
	settings.setValue("MotionConfigurationUser", ccfg.motionServicesConfiguration.userConfiguration.user);
	settings.setValue("MotionConfigurationPassword", ccfg.motionServicesConfiguration.userConfiguration.password);
	settings.setValue("MotionConfigurationURL", ccfg.motionServicesConfiguration.serviceConfiguration.url);
	settings.setValue("MotionConfigurationCertificatePath", ccfg.motionServicesConfiguration.serviceConfiguration.caPath);
	settings.setValue("MotionDataConfigurationURL", ccfg.motionDataConfiguration.serviceConfiguration.url);
	settings.setValue("MotionDataConfigurationCertificatePath", ccfg.motionDataConfiguration.serviceConfiguration.caPath);
	settings.setValue("MotionDataConfigurationUser", ccfg.motionDataConfiguration.userConfiguration.user);
	settings.setValue("MotionDataConfigurationPassword", ccfg.motionDataConfiguration.userConfiguration.password);

	settings.sync();
}

IHMDBSourceViewManager::ContextConfiguration hmdbCommunication::ContextConfigurationSettingsFile::defaultConfig()
{
	hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration ccfg;
	ccfg.name = QObject::tr("Default PJATK MDE data connection");
	ccfg.storageConfiguration.path = QString::fromStdString((plugin::getPaths()->getUserApplicationDataPath() / "db" / "localStorage.db").string());
	ccfg.storageConfiguration.password = "P,j.W/s<T>k2:0\"1;2";

	#ifdef _DEBUG 
	ccfg.motionServicesConfiguration.userConfiguration.user = "test_PJWSTK";
	ccfg.motionServicesConfiguration.userConfiguration.password = "PJtestP@ss";
	#endif
	ccfg.motionServicesConfiguration.serviceConfiguration.url = "";//https://v21.pjwstk.edu.pl/HMDB";
	ccfg.motionServicesConfiguration.serviceConfiguration.caPath = "";//QString::fromStdString((plugin::getPaths()->getResourcesPath() / "v21.pjwstk.edu.pl.crt").string());

	ccfg.motionDataConfiguration.serviceConfiguration.url = "";//"ftps://v21.pjwstk.edu.pl";
	ccfg.motionDataConfiguration.serviceConfiguration.caPath = "";
	#ifdef _DEBUG 
	ccfg.motionDataConfiguration.userConfiguration.user = "testUser";
	ccfg.motionDataConfiguration.userConfiguration.password = "testUser";
	#endif
	return ccfg;
}
