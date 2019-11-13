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

	usersNamesModel_ = std::make_unique<QStringListModel>();
	usersNamesModel_->setStringList(usersList);
}

void ProductionTestWizardData::addTest(std::unique_ptr<AbstractTest>&& test) {
	connect(test.get(), &AbstractTest::log, printer.get(), &PrintInterface::addLog);
	connect(test.get(), &AbstractTest::askUserAction, this, &ProductionTestWizardData::askUserAction);
	connect(test.get(), &AbstractTest::testSummary, this, &ProductionTestWizardData::testSummary);
	connect(this, &ProductionTestWizardData::continueAction, test.get(), &AbstractTest::continueTest);
	connect(this, &ProductionTestWizardData::reportProblem, test.get(), &AbstractTest::reportProblem);
	testsListModel_->getTestsList().push_back(std::move(test));
}

void ProductionTestWizardData::addPreTest(std::unique_ptr<AbstractTest>&& test) {
	connect(test.get(), &AbstractTest::log, printer.get(), &PrintInterface::addLog);
	connect(test.get(), &AbstractTest::askUserAction, this, &ProductionTestWizardData::askUserAction);
	connect(this, &ProductionTestWizardData::continueAction, test.get(), &AbstractTest::continueTest);
	connect(this, &ProductionTestWizardData::reportProblem, test.get(), &AbstractTest::reportProblem);
	preTestsListModel_->getTestsList().push_back(std::move(test));
}
ProductionTestWizardData::ProductionTestWizardData() {
	//prepare testListModel
	testsListModel_ = std::make_unique<TestListModel>();
	preTestsListModel_ = std::make_unique<TestListModel>();
	connect(this, &ProductionTestWizardData::startTests, &testsListModel_->getTestsSuit(), &TestSuit::runTests);
	connect(this, &ProductionTestWizardData::startPreTests, &preTestsListModel_->getTestsSuit(), &TestSuit::runTests);
	connect(this, &ProductionTestWizardData::stopTests, &testsListModel_->getTestsSuit(), &TestSuit::stopTests);
	connect(this, &ProductionTestWizardData::stopTests, &preTestsListModel_->getTestsSuit(), &TestSuit::stopTests);
	connect(&testsListModel_->getTestsSuit(), &TestSuit::testsDone, this, &ProductionTestWizardData::testsDone);
	connect(&preTestsListModel_->getTestsSuit(), &TestSuit::testsDone, this, &ProductionTestWizardData::testsDone);
	testsListModel_->getTestsSuit().moveToThread(&testingThread);
	preTestsListModel_->getTestsSuit().moveToThread(&preTestingThread);
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

	dataThread.quit();
	dataThread.wait();
}

void ProductionTestWizardData::setActiveUser(const QString& user) {
	activeUser_ = user;
}

void ProductionTestWizardData::setSubtype(const QString& str) {
	saveSubtype(str);
	loadSubtype();
}

void ProductionTestWizardData::setSerialNumber(const QString& str) {
	saveSerialNumber(str);
	loadSerialNumber();
}

void ProductionTestWizardData::setFirmwareRevision(const QString& str) {
	saveFirmwareRevision(str);
	loadFirmwareRevision();
}

void ProductionTestWizardData::setMdaTestPassed(const bool value) {
	mdaTestPassed_ = value;
}

void ProductionTestWizardData::setShouldStoreCalibrationDataToEeprom(const bool value) {
	AbstractTest::setStoreCalibrationDataToEeprom(value);
}

TestListModel* ProductionTestWizardData::testsModel() const noexcept {
	return testsListModel_.get();
}

TestListModel* ProductionTestWizardData::preTestsModel() const noexcept {
	return preTestsListModel_.get();
}

QStringListModel* ProductionTestWizardData::userListModel() const noexcept {
	return usersNamesModel_.get();
}
