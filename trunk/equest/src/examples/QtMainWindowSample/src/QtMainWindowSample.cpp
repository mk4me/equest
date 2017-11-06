#include "SampleWindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    SampleWindow window;
    window.show();
    return application.exec();
}
