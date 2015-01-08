#include <QApplication>
#include "mainwindow.h"

int main ( int argc, char *argv[] ) {
    QApplication app ( argc, argv );
    MainWindow * mw = new MainWindow();
    mw->show();
    int result = app.exec();
    delete mw;
    return result;
}
