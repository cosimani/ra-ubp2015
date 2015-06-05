#include <QApplication>
#include <QStyleFactory>
#include <QNetworkProxyFactory>
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    qApp->setStyle( QStyleFactory::create( "Fusion" ) );

    MainWindow *augmentedReality = new MainWindow;
    augmentedReality->showMaximized();

    return app.exec();
}
