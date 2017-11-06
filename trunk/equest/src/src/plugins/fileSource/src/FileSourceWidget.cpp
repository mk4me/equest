#include "FileSourceWidget.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLayout>
#include <QtCore/QTimer>
#include <utils/Debug.h> 
#include <corelib/PluginCommon.h>
#include <loglib/ILog.h>
#include <corelib/IFileDataManager.h>
#include <utils/Filesystem.h>
#include <QtWidgets/QFileDialog>
#include "FileSource.h"

FileSourceWidget::FileSourceWidget( FileSource* source ) :
    fileSource(source)
{
    QLayout* layout = new QVBoxLayout();
    this->setLayout(layout);

    QPushButton* loadFile = new QPushButton();
    loadFile->setText(tr("Load files"));
    layout->addWidget(loadFile);
    connect(loadFile, SIGNAL(clicked()), this, SLOT(onLoadFiles()));

	QPushButton* loadAsfAmcButton = new QPushButton();
	loadAsfAmcButton->setText(tr("ASF AMC"));
	layout->addWidget(loadAsfAmcButton);
	connect(loadAsfAmcButton, SIGNAL(clicked()), this, SLOT(onLoadAsfAmc()));

	QPushButton* addChartStreamButton = new QPushButton();
	addChartStreamButton->setText(tr("addChartStreamButton"));
	layout->addWidget(addChartStreamButton);
	connect(addChartStreamButton, SIGNAL(clicked()), this, SLOT(onAddChartStream()));
}

void FileSourceWidget::onLoadFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file"));
    for (auto it = fileNames.begin(); it != fileNames.end(); ++it) {
        utils::Filesystem::Path path = it->toStdString();
        fileSource->addFile(path);
    }    
}

void FileSourceWidget::onLoadAsfAmc()
{
	fileSource->loadAsfAmc();
}

void FileSourceWidget::onTimer()
{
	fileSource->fillStream();
}

void FileSourceWidget::onAddChartStream()
{
	fileSource->addChartStream();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(100);
}


