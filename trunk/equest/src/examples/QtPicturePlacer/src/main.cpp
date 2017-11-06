#include "PicturePlacerWindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    PicturePlacerWindow window;
    window.show();
    return application.exec();
}
