#include "..\include\AbstractTest.h"
#include <QThread>
AbstractTest::AbstractTest(const QString& name) : name(name) {}

QString AbstractTest::getName() const noexcept {
	return name;
}

void AbstractTest::run() const {
	log(QString("-----------> %1 <-----------\n").arg(name));
	try {
		wasRunned = true;
		preTestSetUp();
		result = test();
		postTestCleanUp();
	}
	catch (const std::runtime_error & exc) {
		result = false;
		log(QString(exc.what()));
	}
	catch (...) {
		log("Exception thrown...\n");
	}
	log(QString("-----------> %1 <-----------\n").arg(result ? "PASSED" : "FAILED"));
}

bool AbstractTest::getShouldBeRun() const noexcept {
	return m_shouldBeRun;
}

bool AbstractTest::getResult() const noexcept {
	return result;
}

bool AbstractTest::getRunned() const noexcept {
	return wasRunned;
}

void AbstractTest::setShouldBeRun(const bool value) noexcept {
	m_shouldBeRun = value;
	emit shouldBeRunChanged(m_shouldBeRun);
}
