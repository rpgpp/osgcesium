#include "webuihandler.h"

#include <QApplication>
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QThread>
#include <QSplashScreen>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>
#include <QtWidgets/QApplication>
#include <qbitmap>
#include "LabViewer.h"
#include<stdlib.h>

void SplashScreenScroll(QSplashScreen* mypSplash)
{
    mypSplash->showMessage(QString(u8"系统正在初始化..."), Qt::AlignHCenter | Qt::AlignBottom, Qt::gray);
    qApp->processEvents();
}

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("Weathertoy");
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app(argc, argv);
    bool simpleMode = argc > 1 && std::string(argv[1]) == "simple";
    LabViewer viewer(simpleMode);
    QSplashScreen* mypSplash = new QSplashScreen(&viewer,QPixmap(":/res/splash2.png"));
    mypSplash->show();
    viewer.mSplash = mypSplash;
    SplashScreenScroll(mypSplash);
    viewer.show();

    return app.exec();
}
