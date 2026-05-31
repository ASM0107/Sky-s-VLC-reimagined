#include <QApplication>
#include <QIcon>
#include "../ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("Sky's VLC Reimagined");
    app.setOrganizationName("SkyDev");
    app.setOrganizationDomain("skydev.com");
    app.setWindowIcon(QIcon(":/assets/logo.png"));
    
    MainWindow w;
    w.show();
    
    return app.exec();
}
