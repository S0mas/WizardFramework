#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStringListModel>
#include <QQmlContext>
#include "../Common/include/DatabaseHandler.h"
#include "../Common/include/Clipboard.h"
#include "../Common/include/AppicationSettings.h"

#include "include/ProductionTestWizardContentMock.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
	QCoreApplication::setOrganizationName("Bustec");
	QCoreApplication::setOrganizationDomain("Bustec");
	QCoreApplication::setApplicationName("ProductionTest Mock");  // -------------------> CHANGE HERE

	/// DATA INTERFACE -------------------> CHANGE HERE
	std::unique_ptr<AbstractProductionTestWizardContent> data = std::make_unique<ProductionTestWizardContentMock>();
	engine.rootContext()->setContextProperty("dataInterface", data.get());
	///////////////////////////////////////////////////////////////////////

	/// APP SETTINGS
	ApplicationSettings appSettings;
	engine.rootContext()->setContextProperty("appSettings", &appSettings);
	///////////////////////////////////////////////////////////////////////

	/// CLIPBOARD
	QmlClipboardAdapter clipboard;
	engine.rootContext()->setContextProperty("clipboard", &clipboard);
	///////////////////////////////////////////////////////////////////////

	/// USERS NAMES MODEL
	DatabaseHandler dbHandler;
	QStringListModel usersNamesModel;
	usersNamesModel.setStringList(dbHandler.users());
	engine.rootContext()->setContextProperty("usersNamesModel", &usersNamesModel);
	///////////////////////////////////////////////////////////////////////

	/// USER ACTION REQUEST CONTROLLER
	engine.rootContext()->setContextProperty("userActionRequestController", &data->userActionRequestController_);
	///////////////////////////////////////////////////////////////////////

	/// DEVICES CONNECTION CONTROLLER
	engine.rootContext()->setContextProperty("devicesController", &data->devicesManagerController_);
	///////////////////////////////////////////////////////////////////////

	/// PRETESTS CONTROLLER
	engine.rootContext()->setContextProperty("preTestsController", &data->preTestsRunnerController_);
	///////////////////////////////////////////////////////////////////////

	/// MAIN TESTS CONTROLLER
	engine.rootContext()->setContextProperty("mainTestsController", &data->mainTestsRunnerController_);
	///////////////////////////////////////////////////////////////////////

	/// TESTS SELECTION MODEL
	TestsSelectionModel testsSelectionModel(data->mainTests());
	engine.rootContext()->setContextProperty("testsModel", &testsSelectionModel);
	///////////////////////////////////////////////////////////////////////

	/// RESOURCES
	engine.rootContext()->setContextProperty("addressesIdentificationPageContent", &data->addressesIdentificationPageContent);
	///////////////////////////////////////////////////////////////////////

	/// RESOURCES
	engine.rootContext()->setContextProperty("uutIdentificationPageContent", &data->uutIdentificationPageContent);
	///////////////////////////////////////////////////////////////////////

	/// RESOURCES
	engine.rootContext()->setContextProperty("testEquipmentIdentificationPageContent", &data->testEquipmentIdentificationPageContent);
	///////////////////////////////////////////////////////////////////////

	/// CHANNELS COUNT
	engine.rootContext()->setContextProperty("channelsCount", data->channelsCount());
	///////////////////////////////////////////////////////////////////////

    const QUrl url(QStringLiteral("qrc:/CommonComponents/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
