/********************************************************************
	created:  2015/01/21	21:51:10
	filename: RecordingWizard.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__RECORDINGWIZARD_H__
#define __HEADER_GUARD_IMU__RECORDINGWIZARD_H__

#include <set>
#include <QtWidgets/QWizard>
#include <plugins/imuCostume/IIMUDataSource.h>

class QListWidgetItem;

namespace Ui
{
	class RecordingWizard;
}

class RecordingWizard : public QWizard
{
	Q_OBJECT

public:

	typedef std::map<imuCostume::CostumeRawIO::CostumeAddress, imuCostume::Costume::SensorIDsSet> CostumesToRecord;

public:

	explicit RecordingWizard(const CostumesToRecord & costumes,
		QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~RecordingWizard();

	QString outputPath() const;
	CostumesToRecord costumes() const;

private slots:
	void pageChanged(int idx);
	void onCostumeSelectionChange(QListWidgetItem* item);
	void onSensorSelectionChanged(QListWidgetItem* item);
	void onChoose();
	void onOutputFilePathEdit(const QString & path);
	void onCostumeChange(QListWidgetItem * current, QListWidgetItem * previous);

private:
	imuCostume::CostumeRawIO::CostumeAddress currentCostume;
	CostumesToRecord baseCostumesToRecord_;
	CostumesToRecord costumesToRecord_;
	Ui::RecordingWizard * ui;
};

#endif	// __HEADER_GUARD_IMU__RECORDINGWIZARD_H__