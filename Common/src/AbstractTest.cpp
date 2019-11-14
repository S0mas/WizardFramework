#include "..\include\AbstractTest.h"
#include <QThread>
#include <thread>
#include <chrono>

AbstractTest::AbstractTest(const QString& name, const bool summaryOn) : name(name), summaryOn(summaryOn) {}

const QString& AbstractTest::getName() const noexcept {
	return name;
}

void AbstractTest::run() const {
	log(QString("%1 -----------> STARTED").arg(name));
	isRunning = true;
	wasRunned = true;
	problemReportedByUser = false;
	try {
		preTestSetUp();
		result = test();
		postTestCleanUp();
	}
	catch (const std::runtime_error & exc) {
		result = false;
		log(QString(exc.what()));
	}
	catch (...) {
		result = false;
		log("Exception thrown...");
	}
	isRunning = false;
	log(QString("%1 -----------> %2").arg(name).arg(result ? "PASSED" : "FAILED"));
	if(summaryOn)
		logSummary();
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
	if (isRunning) {
		emit askUserAction(msg, static_cast<int>(actionType));
		while (!continueTestClicked) { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }
		continueTestClicked = false;
	}
}

void AbstractTest::continueTest() {
	if (isRunning) {
		continueTestClicked = true;
	}
}

void AbstractTest::inputUserDecision(const bool decision) {
	if (isRunning) {
		userDecision = decision;
	}
}


void AbstractTest::reportError(const QString& errorData, const int errorType) {
	if (isRunning) {
		problemReportedByUser = true;
		if (static_cast<UserErrorType>(errorType) == UserErrorType::CHANNELS_ERROR) {
			auto channelMask = errorData.toInt(nullptr, 2);
			channelsErrorsMask |= channelMask;
			log(QString("Error reported by user with channels: %1").arg(channelMask));
		}
	}
}

void AbstractTest::logSummary() const noexcept {
	QString summary;
	summary += QString("Test result summary: %1\n").arg(getName());
	for (int i = 0; i < 16; i++)
		summary += QString("Channel %1 --- %2\n").arg(i + 1).arg(channelsErrorsMask & (1 << i) ? "FAILED" : "PASSED");
	summary += QString("Result: %1\n").arg(getResult() ? "PASSED" : "FAILED");
	log(summary);
	emit testSummary(summary);
}