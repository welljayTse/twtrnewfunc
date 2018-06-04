#include <QtGui>
#define GO 1024

int main(int argc,char * argv[])
{
	QApplication app(argc,argv);
	QLabel label("hello world");
	label.show();
	return app.exec();
 }

