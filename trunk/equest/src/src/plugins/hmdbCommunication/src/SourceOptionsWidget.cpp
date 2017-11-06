#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/SourceOptionsWidget.h>
#include "ui_SourceOptionsWidget.h"
#include <plugins/hmdbCommunication/ServerStatusWidget.h>
#include <plugins/hmdbCommunication/IHMDBService.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <coreui/CorePopup.h>
#include <coreui/CoreCursorChanger.h>
#include <corelib/IServiceManager.h>
#include <corelib/ISourceManager.h>
#include <corelib/PluginCommon.h>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QFileDialog>
#include "SQLCipherStorage.h"
#include "ShallowCopyUtils.h"
#include <coreui/CoreDockWidget.h>
#include "HMDBServicesVerificationHelper.h"
#include <QtWidgets/QMessageBox>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <hmdbserviceslib/IAuthorizationWS.h>


#ifdef DEMO_MODE
SourceOptionsWidget* SourceOptionsWidget::instance = nullptr;
#endif // DEMO_MODE

const bool verifyFileExistance(const QString & filePath)
{
	bool ret = true;

	if (filePath.isEmpty() == false){
		auto p = utils::Filesystem::Path(filePath.toStdString());
		if (utils::Filesystem::pathExists(p) == false || utils::Filesystem::isRegularFile(p) == false){
			ret = false;
		}
	}

	return ret;
}

void verify(const QString & text, const QString & message, QStringList & errors)
{
	if (text.isEmpty() == true){
		errors.push_back(message);
	}
}

using namespace hmdbCommunication;

SourceOptionsWidget::SourceOptionsWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::SourceOptionsWidget),
	serverStatusWidget(new ServerStatusWidget), sourcesPlaceholder_(nullptr)
{
	ui->setupUi(this);

	auto l = new QHBoxLayout;
	l->addWidget(serverStatusWidget);

	ui->statusPlaceholderGroupBox->setLayout(l);
	onSourceModeChange(Qt::Unchecked);

	ui->loginTab->setEnabled(false);

	onRefreshViews();
	onRefreshConfigurations();
	setLoginAdvanceConfiguration(false);

#ifdef DEMO_MODE
#ifdef DEMO_MODE_ONLINE
	ui->onlineModeCheckBox->setCheckState(Qt::Checked);
	ui->onlineModeCheckBox->setEnabled(true);
#else
	ui->onlineModeCheckBox->setCheckState(Qt::Unchecked);
	ui->onlineModeCheckBox->setEnabled(false);
#endif
	//ui->loginLineEdit->setText("motion_admin");
	ui->loginLineEdit->setEnabled(false);
	//ui->passwordLineEdit->setText("HMDBP@ss");
	ui->passwordLineEdit->setEnabled(false);
	this->setVisible(false);
	this->instance = this;
#endif // DEMO_MODE
}

SourceOptionsWidget::~SourceOptionsWidget()
{

}

void SourceOptionsWidget::setSourcesPlaceholder(QMainWindow * placeholder)
{
	sourcesPlaceholder_ = placeholder;
}

const QMainWindow * SourceOptionsWidget::sourcesPlaceholder() const
{
	return sourcesPlaceholder_;
}

void SourceOptionsWidget::onRefreshConfigurations()
{
	if (ui->viewComboBox->currentIndex() < 0){
		return;
	}

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){
			
			auto vm = hmdbSource->viewManager();
			if (vm->viewPrototypesCount() > 0){

				ui->profileComboBox->blockSignals(true);

				auto configName = ui->profileComboBox->currentText();

				auto view = vm->viewPrototype(ui->viewComboBox->currentText());

				//uzupe³niam widoki
				ui->profileComboBox->clear();
				ui->profileComboBox->addItem(tr("Custom connection profile"));
				QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->profileComboBox->model());
				QModelIndex firstIndex = model->index(0, ui->profileComboBox->modelColumn(),
					ui->profileComboBox->rootModelIndex());
				QStandardItem* firstItem = model->itemFromIndex(firstIndex);
				firstItem->setSelectable(false);

				const auto as = vm->configurationsCount();

				if (as > 0){

					for (auto i = 0; i < as; ++i){
						ui->profileComboBox->addItem(vm->configuration(i).name);
					}
				}

				const auto s = vm->configurationsCount(view->name());

				if (as > 0 && s > 0){
					ui->profileComboBox->insertSeparator(as);
				}

				for (auto i = 0; i < s; ++i){
					ui->profileComboBox->addItem(vm->configuration(i, view->name()).name);
				}

				auto f = ui->profileComboBox->findText(configName);

				bool refresh = false;

				if (f < 0){
					f = 1;
					refresh = true;
				}

				ui->profileComboBox->setCurrentIndex(f);

				ui->profileComboBox->blockSignals(false);

				if (refresh == true){
					onProfileChange(f);
				}
			}
		}
	}
}

void SourceOptionsWidget::onRefreshViews()
{
	ui->loginTab->setEnabled(false);

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){
			auto vm = hmdbSource->viewManager();
			
			if (vm->viewPrototypesCount() > 0){

				ui->loginTab->setEnabled(true);
				auto currentViewIndex = ui->viewComboBox->currentIndex();

				//uzupe³niam widoki
				ui->viewComboBox->clear();

				for (auto i = 0; i < vm->viewPrototypesCount(); ++i){
					auto viewProto = vm->viewPrototype(i);
					auto icon = viewProto->icon();
					if (icon.isNull() == false){
						ui->viewComboBox->addItem(icon, viewProto->name());
					}
					else{
						ui->viewComboBox->addItem(viewProto->name());
					}
				}

				if (currentViewIndex > -1 && currentViewIndex < vm->viewPrototypesCount()){
					ui->viewComboBox->setCurrentIndex(currentViewIndex);
				}
				else{
					ui->viewComboBox->setCurrentIndex(0);
				}
			}
		}
	}
}

void SourceOptionsWidget::onAddProfile()
{

}

void SourceOptionsWidget::onRemoveProfile()
{

}

void SourceOptionsWidget::setLoginAdvanceConfiguration(bool show)
{	
	ui->line_16->setVisible(show);
	ui->loginStorageGroupBox->setVisible(show);
	ui->line_17->setVisible(show);
	ui->loginSourceModeGroupBox->setVisible(show);
	ui->line_18->setVisible(show);
	ui->loginConnectionsGroupBox->setVisible(show);
	ui->advanceToolButton->setText(show == true ? ">> " + tr("Simple") : "<< " + tr("Advance"));
}

void SourceOptionsWidget::onViewChange(int idx)
{
	if (idx >= 0){
		auto sm = plugin::getSourceManager();
		if (sm != nullptr){
			auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
			if (hmdbSource != nullptr){
				
				auto vm = hmdbSource->viewManager();
				if (idx < vm->viewPrototypesCount()){
					auto viewProto = vm->viewPrototype(ui->viewComboBox->currentText());
					if (viewProto != nullptr){
						if (viewProto->requiresRemoteContext() == true){
							ui->onlineModeCheckBox->setChecked(true);
							//ui->onlineModeCheckBox->setEnabled(false);
							return;
						}
					}
				}
			}
		}
	}

	//ui->onlineModeCheckBox->setEnabled(true);
}

void SourceOptionsWidget::onProfileChange(int idx)
{
	const auto viewIDX = ui->viewComboBox->currentIndex();

	if (idx >= 1 && viewIDX >= 0){
		--idx;
		auto sm = plugin::getSourceManager();
		if (sm != nullptr){
			auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
			if (hmdbSource != nullptr){
				auto vm = hmdbSource->viewManager();
				if (vm->viewPrototypesCount() > 0
					&& viewIDX < vm->viewPrototypesCount()){

					hmdbCommunication::IHMDBSourceViewManager::IHMDBSourceContextView * view = nullptr;

					const auto commonConfigsCount = vm->configurationsCount();

					if (idx >= commonConfigsCount){
						view = vm->viewPrototype(ui->viewComboBox->currentText());
						idx -= commonConfigsCount;

						if (commonConfigsCount > 0){
							--idx;
						}
					}

					const auto cfg = vm->configuration(idx, view->name());

					setConnectionProfile(cfg);
				}
			}
		}
	}
}

void SourceOptionsWidget::setConnectionProfile(const hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration & connectionProfile)
{
	this->currentConnectionProfile = connectionProfile;
	ui->loginLineEdit->blockSignals(true);
	ui->passwordLineEdit->blockSignals(true);
	ui->storagePathLineEdit->blockSignals(true);
	ui->storagePasswordLineEdit->blockSignals(true);
	ui->motionServiceURLLineEdit->blockSignals(true);
	ui->motionDataURLLineEdit->blockSignals(true);
	ui->medicaServicelURLLineEdit->blockSignals(true);
	ui->medicalDataURLLineEdit->blockSignals(true);
	ui->servicesCertificatePathLineEdit->blockSignals(true);
	ui->dataUserLineEdit->blockSignals(true);
	ui->dataPasswordLineEdit->blockSignals(true);

	if (connectionProfile.motionServicesConfiguration.userConfiguration.user.isEmpty() == false){
		ui->loginLineEdit->setText(connectionProfile.motionServicesConfiguration.userConfiguration.user);
		ui->passwordLineEdit->setText(connectionProfile.motionServicesConfiguration.userConfiguration.password);
	}
	else{
		ui->loginLineEdit->setText(QString());
		ui->passwordLineEdit->setText(QString());
	}

	ui->storagePathLineEdit->setText(connectionProfile.storageConfiguration.path);
	ui->storagePasswordLineEdit->setText(connectionProfile.storageConfiguration.password);

	if (connectionProfile.motionDataConfiguration.userConfiguration.user.isEmpty() == false
		&& connectionProfile.motionDataConfiguration.userConfiguration.password.isEmpty() == false){

		bool online = false;
		
		if (connectionProfile.motionServicesConfiguration.serviceConfiguration.url.isEmpty() == false
			&& connectionProfile.motionDataConfiguration.serviceConfiguration.url.isEmpty() == false){
			ui->motionServiceURLLineEdit->setText(connectionProfile.motionServicesConfiguration.serviceConfiguration.url);
			ui->motionDataURLLineEdit->setText(connectionProfile.motionDataConfiguration.serviceConfiguration.url);
			online = true;
		}

		if (connectionProfile.medicalServicesConfiguration.serviceConfiguration.url.isEmpty() == false
			&& connectionProfile.medicalDataConfiguration.serviceConfiguration.url.isEmpty() == false){

			ui->medicaServicelURLLineEdit->setText(connectionProfile.medicalServicesConfiguration.serviceConfiguration.url);
			ui->medicalDataURLLineEdit->setText(connectionProfile.medicalDataConfiguration.serviceConfiguration.url);
			online = true;
		}

		if (online == true){
			ui->servicesCertificatePathLineEdit->setText(connectionProfile.motionServicesConfiguration.serviceConfiguration.caPath);
			ui->dataUserLineEdit->setText(connectionProfile.motionDataConfiguration.userConfiguration.user);
			ui->dataPasswordLineEdit->setText(connectionProfile.motionDataConfiguration.userConfiguration.password);
			ui->onlineModeCheckBox->setChecked(true);
		}
	}

	ui->loginLineEdit->blockSignals(false);
	ui->passwordLineEdit->blockSignals(false);
	ui->storagePathLineEdit->blockSignals(false);
	ui->storagePasswordLineEdit->blockSignals(false);
	ui->motionServiceURLLineEdit->blockSignals(false);
	ui->motionDataURLLineEdit->blockSignals(false);
	ui->medicaServicelURLLineEdit->blockSignals(false);
	ui->medicalDataURLLineEdit->blockSignals(false);
	ui->servicesCertificatePathLineEdit->blockSignals(false);
	ui->dataUserLineEdit->blockSignals(false);
	ui->dataPasswordLineEdit->blockSignals(false);
}

void SourceOptionsWidget::onConnectionProfileEdit()
{
	ui->profileComboBox->blockSignals(true);
	ui->profileComboBox->setCurrentIndex(0);
	ui->profileComboBox->blockSignals(false);
}

void SourceOptionsWidget::onStoragePathBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Open storage"), ui->storagePathLineEdit->text().isEmpty() == false ? ui->storagePathLineEdit->text() : QString(), tr("Storage File (*.dat)"));

	if (sf.isEmpty() == false){
		ui->storagePathLineEdit->setText(sf);
	}
}

void SourceOptionsWidget::onServicesSSLCertificateBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Chose service certificate"), ui->servicesCertificatePathLineEdit->text().isEmpty() == false ? ui->servicesCertificatePathLineEdit->text() : QString(), tr("Certificate File (*.crt)"));

	if (sf.isEmpty() == false){
		ui->servicesCertificatePathLineEdit->setText(sf);
	}
}

const QString formatSourceWidgetTitle(const QString & viewName, const QString & user)
{
	return viewName + "@" + user;
}

const QString formatErrorMessage(const QStringList & errors)
{
	QString ret = QObject::tr("UNKNOWN error");

	if (errors.empty() == false){		
		if (errors.size() == 1){
			ret = errors.front();
		}
		else{
			int i = 1;
			auto it = errors.begin();
			ret = QString("%1: ").arg(i++) + *it;
			while (++it != errors.end()){
				ret += QString("\n%1: ").arg(i++) + *it;
			}
		}
	}

	return QObject::tr("Error(s)") + ":\n" + ret;
}

void SourceOptionsWidget::onLogin()
{
	coreUI::CoreCursorChanger ccc;

	QStringList errors;
	
	auto ok = verify(errors);

	if (ok == false){		
		coreUI::CorePopup::showMessage(tr("Login failed"), formatErrorMessage(errors));
		return;
	}	
	auto sm = plugin::getServiceManager();	
	auto hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);	
	auto srcm = plugin::getSourceManager();	
	auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(srcm);	

	QString message;

	try{

		utils::shared_ptr<hmdbCommunication::SQLCipherStorage> storage(new hmdbCommunication::SQLCipherStorage);

		storage->open(ui->storagePathLineEdit->text().toStdString(),
			ui->storagePasswordLineEdit->text().toStdString());

		hmdbCommunication::IHMDBSessionPtr session;

		if (ui->onlineModeCheckBox->isChecked() == true){

			session = hmdbService->createSession(ui->motionServiceURLLineEdit->text().toStdString(),
				ui->medicalDataURLLineEdit->text().toStdString(),
				ui->loginLineEdit->text().toStdString(),
				ui->passwordLineEdit->text().toStdString(),
				ui->motionDataURLLineEdit->text().toStdString(),
				ui->medicalDataURLLineEdit->text().toStdString(),
				ui->servicesCertificatePathLineEdit->text().toStdString(),
				networkUtils::HVMatch);
		}

		auto sc = hmdbSource->createSourceContext(storage, ui->loginLineEdit->text().toStdString(),
			ui->passwordLineEdit->text().toStdString(), session);
		auto scc = hmdbSource->createShallowCopyContext(sc);		
		//utworzenie widgeta aktualnie wybranego widoku z nowym kontekstem		
		auto vm = hmdbSource->viewManager();
		auto scvp = vm->viewPrototype(ui->viewComboBox->currentText());
		if (scvp != nullptr){
			auto view = scvp->createView(scc, vm);
			if (view != nullptr){
				auto dw = coreUI::CoreDockWidget::embeddWidget(view,
					formatSourceWidgetTitle(scvp->name(), ui->loginLineEdit->text()),
					Qt::AllDockWidgetAreas, true);

				if (sourcesPlaceholder_ != nullptr){
					sourcesPlaceholder_->addDockWidget(Qt::LeftDockWidgetArea, dw);
				}
				else{
					dw->setFloating(true);
					dw->setVisible(true);
				}

				message = tr("Logged successfully");
				hmdbSource->registerShallowContext(scc);
			}
		}
	}
	catch (std::exception & e){
		message = QString::fromStdString(e.what());
	}
	catch (...){
		message = tr("UNKNOWN login error");
	}

	coreUI::CorePopup::showMessage(tr("Login status"), message);
}

void SourceOptionsWidget::onConnectionChanged(int idx)
{

}

void SourceOptionsWidget::onAccountOperationServiceSSLCertificateBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Chose account service certificate"), ui->opServiceCertificatePathLineEdit->text().isEmpty() == false ? ui->opServiceCertificatePathLineEdit->text() : QString(), tr("Certificate File (*.crt)"));

	if (sf.isEmpty() == false){
		ui->opServiceCertificatePathLineEdit->setText(sf);
	}
}

void SourceOptionsWidget::onRegister()
{
	coreUI::CoreCursorChanger ccc;

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){
			QString message;
			try{
				hmdbSource->registerAccount(ui->opServiceURLLineEdit->text().toStdString(),
					ui->regUserLineEdit->text().toStdString(),
					ui->regPasswordLineEdit->text().toStdString(),
					ui->regNameLineEdit->text().toStdString(),
					ui->regSurnameLineEdit->text().toStdString(),
					ui->regEmailLineEdit->text().toStdString(),
					ui->opPropagateCheckBox->isChecked(),
					ui->opServiceCertificatePathLineEdit->text().toStdString());

				ccc.restore();
				message = tr("Registration successful");
			}
			catch (std::exception & e){
				message = tr("Registration failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Registration failed with UNKNOWN error");
			}

			coreUI::CorePopup::showMessage(tr("Registration status"), message);
		}
		else{
			PLUGIN_LOG_ERROR("Could not find HMDBSource");
		}
	}
	else{
		PLUGIN_LOG_ERROR("Uninitialized source manager");
	}
}

void SourceOptionsWidget::onActivate()
{
	coreUI::CoreCursorChanger ccc;	

	QString message(tr("Account %1 activation failed for service %2 with UNKNOWN error"));

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){			
			try{
				hmdbSource->activateAccount(ui->opServiceURLLineEdit->text().toStdString(),
					ui->activateUserLineEdit->text().toStdString(), ui->activateActivationCodeLineEdit->text().toStdString(),
					ui->opPropagateCheckBox->isChecked(), ui->opServiceCertificatePathLineEdit->text().toStdString());

				message = tr("Activation successful for account %1 within service %2");
			}
			catch (std::exception & e){
				message = tr("Account %1 activation failed for service %2 with error: ") + QString::fromStdString(e.what());
			}
			catch (...){

			}			
		}
		else{
			message = tr("Account %1 activation failed for service %2 with error: Could not find HMDBService");						
		}
	}
	else{
		message = tr("Account %1 activation failed for service %2 with error: Uninitialized service manager");		
	}

	coreUI::CorePopup::showMessage(tr("Account activation status"), message.arg(ui->activateUserLineEdit->text()).arg(ui->opServiceURLLineEdit->text()));
}

void SourceOptionsWidget::onPasswordReset()
{
	coreUI::CoreCursorChanger ccc;	

	QString message(tr("Password reset for email %1 failed within service %2 with UNKNOWN error"));

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){
			try{
				hmdbSource->resetPassword(ui->opServiceURLLineEdit->text().toStdString(),
					ui->resetEmailLineEdit->text().toStdString(), ui->opPropagateCheckBox->isChecked(),
					ui->opServiceCertificatePathLineEdit->text().toStdString());

				message = tr("Password reset successful for email %1 within service %2");
			}
			catch (std::exception & e){
				message = tr("Password reset for email %1 failed for service %2 with error: ") + QString::fromStdString(e.what());
			}
			catch (...){

			}
		}
		else{
			message = tr("Password reset for email %1 failed for service %2 with error: Could not find HMDBService");
		}
	}
	else{
		message = tr("Password reset for email %1 failed for service %2 with error: Uninitialized service manager");
	}

	coreUI::CorePopup::showMessage(tr("Password reset status"), message.arg(ui->resetEmailLineEdit->text()).arg(ui->opServiceURLLineEdit->text()));
}

void SourceOptionsWidget::onServerChange(int idx)
{

}

void SourceOptionsWidget::onServerSSLCertificateBrowse()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Chose server certificate"), ui->statusServerCertificatePathLineEdit->text().isEmpty() == false ? ui->statusServerCertificatePathLineEdit->text() : QString(), tr("Certificate File (*.crt)"));

	if (sf.isEmpty() == false){
		ui->statusServerCertificatePathLineEdit->setText(sf);
	}
}

void SourceOptionsWidget::onStatusCheck()
{
	coreUI::CoreCursorChanger ccc;

	char ret = ServerStatusWidget::UnknownStatus;

	auto sm = plugin::getServiceManager();
	if (sm != nullptr){
		auto hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);
		if (hmdbService != nullptr){
			QString message;
			try{
				const auto response = hmdbService->serverOnline(ui->statusServerURLLineEdit->text().toStdString(),
					ui->statusTimeoutSpinBox->value(), ui->statusServerCertificatePathLineEdit->text().toStdString());

				ret = ((response == true) ? ServerStatusWidget::OnlineStatus : ServerStatusWidget::OfflineStatus);

				ccc.restore();

				message = tr("Server status: ") + ((response == true) ? tr("On line") : tr("Off line"));

				serverStatusWidget->setServerStatus(ret);
			}
			catch (std::exception & e){
				message = tr("Server status check failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Server status check failed with UNKNOWN error");
			}

			coreUI::CorePopup::showMessage(tr("Server status check"), message);
		}
		else{
			PLUGIN_LOG_ERROR("Could not find HMDBService");
		}
	}
	else{
		PLUGIN_LOG_ERROR("Uninitialized service manager");
	}
}

void SourceOptionsWidget::onSourceModeChange(int mode)
{
	if (mode == Qt::Unchecked){
		ui->line_18->setVisible(false);
		ui->loginConnectionsGroupBox->setVisible(false);
	}
	else if (mode == Qt::Checked){
		ui->line_18->setVisible(true);
		ui->loginConnectionsGroupBox->setVisible(true);
	}
}

const bool SourceOptionsWidget::verify(QStringList & messages)
{
	auto sm = plugin::getServiceManager();
	if (sm == nullptr){
		messages.push_back(tr("Uninitialized service manager"));
	}

	auto srcm = plugin::getSourceManager();
	if (srcm == nullptr){
		messages.push_back(tr("Uninitialized source manager"));
	}

	utils::shared_ptr<hmdbCommunication::IHMDBService> hmdbService;
	if (sm != nullptr){
		hmdbService = core::queryService<hmdbCommunication::IHMDBService>(sm);
		if (hmdbService == nullptr){
			messages.push_back(tr("HMDBService not found"));
		}
	}

	utils::shared_ptr<hmdbCommunication::IHMDBSource> hmdbSource;
	if (srcm != nullptr){
		hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(srcm);
		if (hmdbSource == nullptr){
			messages.push_back(tr("HMDBSource not found"));
		}
	}

	bool storageCreated = false;
	utils::Filesystem::Path storagePath(ui->storagePathLineEdit->text().toStdString());
	if (storagePath.empty() == true){
		messages.push_back(tr("Storage file was not not chosen."));
	}
	else if (storagePath.is_absolute() == false){
		messages.push_back(tr("Storage file path is not the absolute path."));
	}
	else if (storagePath.has_filename() == false
		|| storagePath.has_extension() == false){
		messages.push_back(tr("Storage file path is not a file path. Both: file name and extension are required."));
	}
	else if (utils::Filesystem::pathExists(storagePath) == false){

		auto ret = QMessageBox::question(this, tr("Storage not exists"), tr("Given storage does not exist. Would You like to create storage with the given parameters?"));

		if (ret == QMessageBox::Yes){

			auto parentDir = storagePath.parent_path();

			if (utils::Filesystem::pathExists(parentDir) == false){

				try{
					utils::Filesystem::createDirectory(parentDir);
				}
				catch (...){

				}
			}

			if (utils::Filesystem::pathExists(parentDir) == false){
				messages.push_back(tr("Failed to create storage in the given directory - do You have required permissions?"));
			}
			else if (hmdbCommunication::SQLCipherStorage::create(storagePath, ui->storagePasswordLineEdit->text().toStdString()) == true){
				coreUI::CorePopup::showMessage(tr("Info"), tr("Storage successfully created"));
				storageCreated = true;
			}
			else{
				coreUI::CorePopup::showMessage(tr("Info"), tr("Failed to create storage"));
				messages.push_back(tr("Failed to create storage"));
			}
		}
		else{
			messages.push_back(tr("Storage file not exists."));
		}
	}

	if (storageCreated == false && utils::Filesystem::pathExists(storagePath) == true) {
		std::unique_ptr<hmdbCommunication::SQLCipherStorage> storage(new hmdbCommunication::SQLCipherStorage);

		storage->open(ui->storagePathLineEdit->text().toStdString(),
			ui->storagePasswordLineEdit->text().toStdString());

		if (storage->isOpened() == false){
			messages.push_back(tr("Could not open storage. Probably incorrect password"));
		}
		else{
			storage->close();
		}
	}

	if (ui->onlineModeCheckBox->isChecked() == true){

		QStringList remoteErrors;

		::verify(ui->loginLineEdit->text(), tr("Empty login"), remoteErrors);
		::verify(ui->passwordLineEdit->text(), tr("Empty password"), remoteErrors);

		if (ui->motionServiceURLLineEdit->text().isEmpty() == true && ui->medicaServicelURLLineEdit->text().isEmpty() == true){
			remoteErrors.push_back(tr("Empty services URLs"));
		}

		if (remoteErrors.empty() == true){			

			if (ui->motionServiceURLLineEdit->text().isEmpty() == false || ui->motionDataURLLineEdit->text().isEmpty() == false){
				::verify(ui->motionServiceURLLineEdit->text(), tr("Empty motion service URL"), messages);
				::verify(ui->motionDataURLLineEdit->text(), tr("Empty motion data URL"), messages);
			}

			if (ui->medicaServicelURLLineEdit->text().isEmpty() == false || ui->medicalDataURLLineEdit->text().isEmpty() == false){
				::verify(ui->medicaServicelURLLineEdit->text(), tr("Empty medical service URL"), messages);
				::verify(ui->medicalDataURLLineEdit->text(), tr("Empty medical data URL"), messages);
			}
		}		

		messages.append(remoteErrors);

		if (messages.empty() == true){			
			auto session = hmdbService->createSession(ui->motionServiceURLLineEdit->text().toStdString(),
					ui->medicalDataURLLineEdit->text().toStdString(),
					ui->loginLineEdit->text().toStdString(),
					ui->passwordLineEdit->text().toStdString(),
					ui->motionDataURLLineEdit->text().toStdString(),
					ui->medicalDataURLLineEdit->text().toStdString(),
					ui->servicesCertificatePathLineEdit->text().toStdString(),
					networkUtils::HVMatch);			

			try{

				if (session->authorization()->checkMyLogin() == false){					
					messages.push_back(tr("Incorrect login or password"));
				}

			}
			catch (std::exception & e){
				messages.push_back(tr("Error while verifying user: ") + QString::fromStdString(e.what()));
			}
			catch (...){
				messages.push_back(tr("UNKNOWN rrror while verifying user"));
			}			
		}
	}

	return messages.empty();
}

template<typename T>
static void _verify(T * service, const QString & errorMessage, QStringList & errors)
{
	if (HMDBServicesVerificationHelper::verify(service) == false){
		errors.push_back(errorMessage);
	}
}

void SourceOptionsWidget::verify(hmdbServices::IFileStoremanWS * fileStoreman,
	hmdbCommunication::IHMDBFtp * ftp, const QString & errorMessage,
	QStringList & errors)
{
	if (HMDBServicesVerificationHelper::verify(fileStoreman, ftp) == false){
		errors.push_back(errorMessage);
	}
}

void SourceOptionsWidget::verify(hmdbServices::ISingleAccountFactoryWS * singleAccountFactory, QStringList & errors)
{
	_verify(singleAccountFactory, tr("Single-account factory service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IMultiAccountFactoryWS * multiAccountFactory, QStringList & errors)
{
	_verify(multiAccountFactory, tr("Multi-account factory service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IAdministrationWS * administration, QStringList & errors)
{
	_verify(administration, tr("Administration service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IAuthorizationWS * authorization, QStringList & errors)
{
	_verify(authorization, tr("Authorization service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IGeneralBasicQueriesWS * generalQueries, QStringList & errors)
{
	_verify(generalQueries, tr("General queries service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IMotionBasicQueriesWS * motionQueries, QStringList & errors)
{
	_verify(motionQueries, tr("Motion queries service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IMotionBasicUpdatesWS * motionUpdate, QStringList & errors)
{
	_verify(motionUpdate, tr("Motion update service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IMedicalBasicUpdatesWS * medicalUpdate, QStringList & errors)
{
	_verify(medicalUpdate, tr("Medical update service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IMotionFileStoremanWS * motionFilestoreman, QStringList & errors)
{
	_verify(motionFilestoreman, tr("Motion filestoreman service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IMedicalFileStoremanWS * medicalFilestoreman, QStringList & errors)
{
	_verify(medicalFilestoreman, tr("Medical filestoreman service failed"), errors);
}

void SourceOptionsWidget::verify(hmdbServices::IUserPersonalSpaceWS * userPersonalSpace, QStringList & errors)
{
	_verify(userPersonalSpace, tr("User personal space service failed"), errors);
}

void SourceOptionsWidget::onVerifyLogin()
{
	QStringList errors;
	bool ok = false;
	{
		coreUI::CoreCursorChanger ccc;
		ok = verify(errors);
	}

	if (ok == false){
		QMessageBox::critical(this, tr("Configuration error(s)"), formatErrorMessage(errors));		
	}
	else{
		QMessageBox::information(this, tr("Configuration status"), tr("Correct configuration"));
	}
}

const hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration& SourceOptionsWidget::getConnectionProfile() const
{
	return this->currentConnectionProfile;
}

