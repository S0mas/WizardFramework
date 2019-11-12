#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStringListModel>
#include <QQmlContext>
#include "../6716/include/ProductionTestWizardData_6716_SignalInterface.h"
#include <QThread>
#include "../Common/include/Clipboard.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
	QmlClipboardAdapter clipboard;
	std::unique_ptr<ProductionTestWizardData_6716> data = std::make_unique<ProductionTestWizardData_6716>();
	data->initialize();

	auto dataInterface = std::make_unique<ProductionTestWizardData_6716_SignalInterface>(data);

	engine.rootContext()->setContextProperty("dataInterface", dataInterface.get());
	engine.rootContext()->setContextProperty("testsModel", data->testsModel());
	engine.rootContext()->setContextProperty("preTestsModel", data->preTestsModel());
    engine.rootContext()->setContextProperty("usersNamesModel", data->userListModel());
	engine.rootContext()->setContextProperty("clipboard", &clipboard);

    const QUrl url(QStringLiteral("qrc:/6716/Components/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
