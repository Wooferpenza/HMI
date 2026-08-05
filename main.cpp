#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("HMI");
    QCoreApplication::setApplicationName("HMI");

    MainWindow w;


#ifdef QT_DEBUG
    w.showNormal();
#else
    w.showFullScreen();
#endif

    return a.exec();
}
