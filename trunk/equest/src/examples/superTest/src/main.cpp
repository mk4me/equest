#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <osg/BoundingBox>

int main(int argc, char *argv[])
{

	osg::BoundingBox bb(0,0,0,0,0,0);


	QApplication application(argc, argv);
	QWidget window;
	window.show();
	return application.exec();
}
