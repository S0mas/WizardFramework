#include "..\include\TestSuit.h"
#include <QThread>

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
		if (test->getShouldBeRun() && !stopped){
			emit testRunned(test->getName());
			test->run();
			emit testFinished(test->getName(), test->getResult());
		}
	});
	emit testsDone();
}