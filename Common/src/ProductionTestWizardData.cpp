#include "..\include\ProductionTestWizardData.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QDebug>

void ProductionTestWizardData::loadUsersFromDataBase() noexcept {
	dbConnection->executeSqlQuery("SELECT uname FROM users");
	auto query = dbConnection->getLastQuery();
	QStringList usersList;
	while (query.next())
		usersList.append(query.value(0).toString());
	usersList.append("TEST");

	usersNamesModel = std::make_unique<QStringListModel>();
	usersNamesModel->setStringList(usersList);
}

void ProductionTestWizardData::addTest(std::unique_ptr<AbstractTest>&& test) {
	connect(test.get(), &AbstractTest::log, printer.get(), &PrintInterface::addLog);
	testsListModel->getTestsList().push_back(std::move(test));
}

void ProductionTestWizardData::addPreTest(std::unique_ptr<AbstractTest>&& test) {
	connect(test.get(), &AbstractTest::log, printer.get(), &PrintInterface::addLog);
	preTestsListModel->getTestsList().push_back(std::move(test));
}
ProductionTestWizardData::ProductionTestWizardData() {
	//prepare testListModel
	testsListModel = std::make_unique<TestListModel>();
	preTestsListModel = std::make_unique<TestListModel>();
	connect(this, &ProductionTestWizardData::startTests, &testsListModel->getTestsSuit(), &TestSuit::runTests);
	connect(this, &ProductionTestWizardData::startPreTests, &preTestsListModel->getTestsSuit(), &TestSuit::runTests);
	connect(this, &ProductionTestWizardData::stopTests, &testsListModel->getTestsSuit(), &TestSuit::stopTests);
	connect(this, &ProductionTestWizardData::stopTests, &preTestsListModel->getTestsSuit(), &TestSuit::stopTests);
	connect(&testsListModel->getTestsSuit(), &TestSuit::testsDone, this, &ProductionTestWizardData::testsDone);
	connect(&preTestsListModel->getTestsSuit(), &TestSuit::testsDone, this, &ProductionTestWizardData::testsDone);
	testsListModel->getTestsSuit().moveToThread(&testingThread);
	preTestsListModel->getTestsSuit().moveToThread(&preTestingThread);
	testingThread.start();
	preTestingThread.start();
	//connect to database
	dbConnection = std::make_unique<QMySqlDataBaseConnector>("192.168.168.222", 3306, "prodtest", "operator", "EG1878-Nd");
	if (!dbConnection->isConnectionOpen())
		qDebug() << "Error while connecting Database! :" << dbConnection->getLastError().text();
	loadUsersFromDataBase();

	printer = std::make_unique<PrintInterface>();
	connect(&printerThread, &QThread::started, printer.get(), &PrintInterface::printingProcess);
	connect(printer.get(), &PrintInterface::printLog, this, &ProductionTestWizardData::logMsg);
	printer->moveToThread(&printerThread);
	printerThread.start();
}

ProductionTestWizardData::~ProductionTestWizardData() {
	emit stopTests();
	printerThread.requestInterruption();

	preTestingThread.quit();
	preTestingThread.wait();

	testingThread.quit();
	testingThread.wait();

	printerThread.quit();
	printerThread.wait();
}

Q_INVOKABLE int ProductionTestWizardData::getActiveUser() const noexcept {
	return activeUser;
}

Q_INVOKABLE QString ProductionTestWizardData::getSubtype() const noexcept {
	return subtype;
}

Q_INVOKABLE QString ProductionTestWizardData::getSerialNumber() const noexcept {
	return serialNumber;
}

Q_INVOKABLE QString ProductionTestWizardData::getDriverRevision() const noexcept {
	return driverRevision;
}

Q_INVOKABLE QString ProductionTestWizardData::getFirmwareRevision() const noexcept {
	return firmwareRevision;
}

Q_INVOKABLE bool ProductionTestWizardData::getMdaTestPassed() const noexcept {
	return mdaTestPassed;
}

Q_INVOKABLE void ProductionTestWizardData::setActiveUser(const int index) noexcept {
	activeUser = index;
}

Q_INVOKABLE void ProductionTestWizardData::setSubtype(const QString& str) noexcept {
	saveSubtype(str);
	loadSubtype();
}

Q_INVOKABLE void ProductionTestWizardData::setSerialNumber(const QString& str) noexcept {
	saveSerialNumber(str);
	loadSerialNumber();
}

Q_INVOKABLE void ProductionTestWizardData::setFirmwareRevision(const QString& str) noexcept {
	saveFirmwareRevision(str);
	loadFirmwareRevision();
}

Q_INVOKABLE void ProductionTestWizardData::setMdaTestPassed(const bool value) noexcept {
	mdaTestPassed = value;
}

Q_INVOKABLE TestListModel* ProductionTestWizardData::getTestsModel() const noexcept {
	return testsListModel.get();
}

Q_INVOKABLE TestListModel* ProductionTestWizardData::getPreTestsModel() const noexcept {
	return preTestsListModel.get();
}

Q_INVOKABLE QStringListModel* ProductionTestWizardData::getUserListModel() const noexcept {
	return usersNamesModel.get();
}
