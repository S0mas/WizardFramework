#include "..\include\AbstractTest.h"
#include <QThread>
#include <thread>
#include <chrono>

AbstractTest::AbstractTest(const QString& name) : name(name) {}

const QString& AbstractTest::getName() const noexcept {
	return name;
}

void AbstractTest::run() const {
	log(QString("%1 -----------> STARTED").arg(name));
	try {
		wasRunned = true;
		problemReportedByUser = false;
		preTestSetUp();
		result = test();
		postTestCleanUp();
	}
	catch (const std::runtime_error & exc) {
		result = false;
		log(QString(exc.what()));
	}
	catch (...) {
		log("Exception thrown...");
	}
	log(QString("%1 -----------> %2").arg(name).arg(result ? "PASSED" : "FAILED"));
	log(QString("****** ***** ***** ***** ***** *****"));
}

bool AbstractTest::getShouldBeRun() const noexcept {
	return shouldBeRun;
}

bool AbstractTest::getResult() const noexcept {
	return result;
}

bool AbstractTest::getRunned() const noexcept {
	return wasRunned;
}

void AbstractTest::setShouldBeRun(const bool value) noexcept {
	shouldBeRun = value;
	emit shouldBeRunChanged(shouldBeRun);
}

void AbstractTest::setStoreCalibrationDataToEeprom(const bool value) noexcept {
	storeCalibrationDataToEeprom = value;
}

bool AbstractTest::getStoreCalibrationDataToEeprom() noexcept {
	return storeCalibrationDataToEeprom;
}

void AbstractTest::waitForUserAction(const QString& msg, const UserActionType actionType) const noexcept {
	emit askUserAction(msg, static_cast<int>(actionType));
	while (!continueTestClicked) { std::this_thread::sleep_for(std::chrono::milliseconds(1000));}
	continueTestClicked = false;
}

void AbstractTest::continueTest() {
	continueTestClicked = true;
}

void AbstractTest::reportProblem() {
	problemReportedByUser = true;
}
