#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    bool triggered = false;

    QApplication a(argc, argv);
    MainWindow w(&triggered);
    w.show();
    a.exec();

    if(triggered)
    {
        //qDebug() << "Triggered!";
        system("C:\\WINDOWS\\System32\\shutdown /s");
    }

    return 0;
}
