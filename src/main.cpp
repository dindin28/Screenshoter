#include "mainwindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QScreen* screen = QApplication::primaryScreen();
    QPixmap pixmap = screen->grabWindow(0);
    pixmap.save("screenshot.png");
    w.show();
    return a.exec();
}
