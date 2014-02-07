#include "window.h"
#include <QApplication>
#include <QNetworkProxy>
#include "searchengine.h"
#include "design.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    //a.setStyleSheet(Design::instance()->styleSheet(Design::ApplicationStyle));

    // Устанавливаем проксирование для РБК
    //QNetworkProxy proxy(QNetworkProxy::HttpProxy, "cache.rbc.ru", 3128);
    //QNetworkProxy::setApplicationProxy(proxy);

    // Загружаем сведения о валютных инструментах ЦБ РФ
    SearchEngine::instance()->loadInstruments();

    WindowsManager::addWindow();

    return a.exec();
}
