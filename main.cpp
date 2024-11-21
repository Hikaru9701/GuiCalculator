#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include <QRect>

int main(int argc, char* argv[])
{
	// 高解像度対策
	qputenv("QT_SCALE_FACTOR", "1.25");
	//QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QApplication a(argc, argv);
	MainWindow w;

	// アプリはモニターの外にある時、中間に修正
	QList<QScreen*> screens = QGuiApplication::screens();					//全てのモニターを獲得
	QRect virtualScreenRect;
	for (QScreen* screen : screens) {
		virtualScreenRect = virtualScreenRect.united(screen->geometry());
	}
	if (!virtualScreenRect.contains(w.geometry().center())) {
		QRect screenGeometry = screens[0]->geometry();
		int x = (screenGeometry.width() - w.width()) / 2;
		int y = (screenGeometry.height() - w.height()) / 2;
		w.move(x, y);
	}

	w.show();
	
	return a.exec();
}
