#include "window.h"
#include <QApplication>
#include "design.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    //a.setStyleSheet(Design::instance()->styleSheet(Design::ApplicationStyle));
    WindowsManager::addWindow();

    return a.exec();
}
