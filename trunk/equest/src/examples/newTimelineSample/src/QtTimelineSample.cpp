#include "SampleWindow.h"
#include "TimeChannelWidget.h"
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QCheckBox>
#include <boost/shared_ptr.hpp>

#include "TestClientChannel.h"
#include "TimelineWidget.h"
#include <timelinelib/Model.h>


int main(int argc, char *argv[])
{
    timeline::ModelPtr model(new timeline::Model("Timeline"));

    model->addChannel("Channel A/Channel AA", timeline::IChannelPtr(new timeline::TestClientChannel(150)));
    model->addChannel("Channel A/Channel AB", timeline::IChannelPtr(new timeline::TestClientChannel(50)));

    model->addChannel("Channel B/Channel AA", timeline::IChannelPtr(new timeline::TestClientChannel(100)));
    model->addChannel("Channel B/Channel AB", timeline::IChannelPtr(new timeline::TestClientChannel(200)));

    model->setChannelLocalOffset("Channel A/Channel AA", -30);
    model->setChannelLocalTimeScale("Channel A", 2);

    model->setChannelLocalOffset("Channel B/Channel AA", 70);
    model->setChannelLocalTimeScale("Channel B", 0.5);

    QApplication application(argc, argv);
    SampleWindow window;

    QDockWidget qDocW("Test QDockWidget with TimeChannelWidget", &window);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(new TimelineWidget(model));

    QWidget * helper = new QWidget();
    helper->setLayout(hbox);

    qDocW.setWidget(helper);
    window.setCentralWidget(&qDocW);

    window.show();

    return application.exec();
}
