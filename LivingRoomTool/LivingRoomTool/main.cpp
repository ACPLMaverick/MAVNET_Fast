#include "LivingRoomTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LivingRoomTool w;
	w.show();
	w.PostInit();
	return a.exec();
}
