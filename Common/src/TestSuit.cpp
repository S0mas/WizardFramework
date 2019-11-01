#include "..\include\TestSuit.h"
#include <QThread>
#include <QDebug>

const std::vector<std::unique_ptr<AbstractTest>>& TestSuit::getTestsList() const noexcept {
	return testsList;
}

std::vector<std::unique_ptr<AbstractTest>>& TestSuit::getTestsList() noexcept {
	return testsList;
}

void TestSuit::stopTests() {
	stopped = true;
}

void TestSuit::runTests() const {
	std::for_each(testsList.begin(), testsList.end(), [&](const auto& test) {
		QCoreApplication::processEvents();
		qDebug() << QThread::currentThreadId();
		if (test->getShouldBeRun() && !stopped)
			emit testRunned(test->getName());
			test->run();
		});
	emit testsDone();
}