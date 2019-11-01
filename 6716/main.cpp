#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStringListModel>
#include <QQmlContext>
#include "include/ProductionTestWizardData_6716.h"
#include "../Common/include/Clipboard.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	qDebug() << "MAIN:" <<QThread::currentThreadId();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
	QmlClipboardAdapter clipboard;
	std::unique_ptr<ProductionTestWizardData_6716> data = std::make_unique<ProductionTestWizardData_6716>();
	qmlRegisterUncreatableType<AbstractTest>("abstractTest", 1, 0, "AbstractTests", "creation unavailable"); // MyQMLType will be usable with: import com.yourcompany.xyz 1.0

    engine.rootContext()->setContextProperty("dataObject", data.get());
	engine.rootContext()->setContextProperty("testsModel", data->getTestsModel());
	engine.rootContext()->setContextProperty("preTestsModel", data->getPreTestsModel());
    engine.rootContext()->setContextProperty("usersNamesModel", data->getUserListModel());
	engine.rootContext()->setContextProperty("clipboard", &clipboard);

    const QUrl url(QStringLiteral("qrc:/6716/Components/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
	data->initialize();
    return app.exec();
}
