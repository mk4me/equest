#include "PCH.h"
#include "CostumeSkeletonMotionHelper.h"
#include <chrono>
#include <thread>
#include <loglib/ILog.h>
#include <corelib/IPlugin.h>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressDialog>

static double calculateDelta(const imuCostume::CostumeCANopenIO::Timestamp current,
	const imuCostume::CostumeCANopenIO::Timestamp previous)
{
	double deltaT = (current >= previous) ? (current - previous) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - previous + current);
	deltaT /= 1000.0;

	if (deltaT >= 1000)
	{
		PLUGIN_LOG_DEBUG("Probably wrong delta calculation -> current: " << current << " previous: " << previous);
	}

	return deltaT;
}

void CostumeSkeletonMotionHelper::estimate(IMU::SensorsStreamData & streamData)
{
	for (auto & m : algorithmsProgress)
	{
		auto it = streamData.sensorsData.find(m.first);
		if (it != streamData.sensorsData.end()){
			double deltaTime = calculateDelta(streamData.timestamp, m.second.lastTime);

			if (m.second.counter == 0 && m.second.lastTime == 0){
				deltaTime = 0.0;
			}

			m.second.lastTime = streamData.timestamp;
			++(m.second.counter);
			it->second.orientation = m.second.algo->estimate(it->second.accelerometer, it->second.gyroscope, it->second.magnetometer, deltaTime, it->second.orientation);
		}
	}
}

CostumeSkeletonMotionHelper::CostumeSkeletonMotionHelper(IMU::SensorsStreamPtr sensorsStream,
IMU::CostumeProfilePtr costumeProfile,
const unsigned int maxSamples,
const unsigned int calibratinStageChangeValue, QWidget * parent)
: QObject(parent), sensorsStream(sensorsStream), costumeProfile(costumeProfile),
observer(new threadingUtils::ResetableStreamStatusObserver),
calibratinStageChangeValue(calibratinStageChangeValue), previousTime(0),
first(true), complete(false), dialog(new QDialog(parent)),
cw(nullptr)
{
	pd = new QProgressDialog(tr("Orientation estimation algorithms"), tr("Cancel"), 0, maxSamples, dialog);
	pd->setWindowFlags(Qt::Widget);
	auto l = new QVBoxLayout;	
	l->addWidget(pd);

	for (const auto & sa : costumeProfile->sensorsDescriptions)
	{
		AlgoProgress ap;
		ap.counter = 0;
		ap.lastTime = 0;
		ap.algo = sa.second.orientationEstimationAlgorithm;

		algorithmsProgress.insert({ sa.first, ap });
	}	

	sensorsStream->attach(observer);		
	pd->setWindowTitle(tr("Costume initialization"));
	dialog->setWindowTitle(tr("Costume initialization"));
	pd->setValue(0);	

	cw = costumeProfile->calibrationAlgorithm->calibrationWidget();
	if (cw != nullptr){		
		cw->setParent(dialog);
		l->addWidget(cw);		
		cw->setVisible(false);
	}

	dialog->setLayout(l);

	//connect(pd, SIGNAL(reset()), pd, SLOT(cancel()));
	//connect(pd, SIGNAL(canceled()), pd, SLOT(cancel()));

	connect(pd, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(pd, SIGNAL(finished(int)), dialog, SLOT(done(int)));
	connect(pd, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
	connect(pd, SIGNAL(canceled()), dialog, SLOT(reject()));

	
	connect(&timer, SIGNAL(timeout()), this, SLOT(perform()));
}

CostumeSkeletonMotionHelper::~CostumeSkeletonMotionHelper()
{
	sensorsStream->detach(observer);
}

int CostumeSkeletonMotionHelper::exec()
{
	complete = false;
	timer.start(0);
	dialog->exec();

	return pd->wasCanceled() == true ? QDialog::Rejected : QDialog::Accepted;
}

bool CostumeSkeletonMotionHelper::isComplete() const
{
	return complete;
}

void CostumeSkeletonMotionHelper::resetCounters()
{
	for (auto & val : algorithmsProgress)
	{
		val.second.counter = 0;
	}
}

unsigned int CostumeSkeletonMotionHelper::minCounter() const
{
	unsigned int ret = std::numeric_limits<unsigned int>::max();

	for (const auto & val : algorithmsProgress)
	{
		ret = std::min(ret, val.second.counter);
	}

	return ret;
}

void CostumeSkeletonMotionHelper::perform()
{
	//PLUGIN_LOG_DEBUG("Performing data check");
	if (observer->modified() == true){
		//PLUGIN_LOG_DEBUG("New data processing");
		IMU::SensorsStreamData data;
		sensorsStream->data(data);
		double deltaTime = calculateDelta(data.timestamp, previousTime);		
		if (first == true){
			first = false;
			deltaTime = 0.0;
		}
		
		if (pd->value() == calibratinStageChangeValue) {
			pd->setLabelText(tr("Calibrating"));

			resetCounters();
		}

		estimate(data);
		if (pd->value() >= calibratinStageChangeValue){
			if (cw != nullptr){
				if (cw->isVisible() == false){
					cw->setVisible(true);					
				}				
			}
			bool ret = costumeProfile->calibrationAlgorithm->calibrate(data.sensorsData, deltaTime);
			if (ret == true){
				complete = true;				
				pd->setValue(pd->maximum());				
			}
		}

		if (complete == true){
			cancel();
			dialog->accept();
		}
		else {
			previousTime = data.timestamp;
			auto val = minCounter();

			if (val >= pd->maximum()){
				cancel();
				dialog->reject();
			}
			else {

				if (pd->value() >= calibratinStageChangeValue){
					val += calibratinStageChangeValue;
				}

				pd->setValue(val);
			}
		}
	}
}

void CostumeSkeletonMotionHelper::cancel()
{
	disconnect(&timer, SIGNAL(timeout()), this, SLOT(perform()));
	timer.stop();
}