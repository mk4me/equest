#include "SampleWindow.h"
#include "QtWidgets/QMessageBox"


SampleWindow::SampleWindow()
{
    setupUi(this);
}

void SampleWindow::showHelloWorld()
{
    QMessageBox::information( this, "Sample", "Hello world!" );
}
