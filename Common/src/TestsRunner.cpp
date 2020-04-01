#include "..\include\TestsRunner.h"
#include"..\include\Exceptions.h"
#include <QCoreApplication>
#include <algorithm>

void TestsRunner::connectTest(AbstractTest& test) const noexcept {
	connect(&test, &AbstractTest::log, this, &TestsRunner::logMsg);
	connect(&test, &AbstractTest::testSummary, this, &TestsRunner::testSummary);
}

void TestsRunner::addTest(AbstractTest* test) {
	test->setParent(this);
	connectTest(*test);
	tests_.push_back(test);
}

std::vector<AbstractTest*>& TestsRunner::tests() noexcept {
	return tests_;
}

void TestsRunner::pauseTests() noexcept {
	paused = true;
	emit testsPaused();
	while (paused && !aborted)
		QCoreApplication::processEvents();
}

void TestsRunner::resumeTests() noexcept {
	paused = false;
	emit testsResumed();
}

void TestsRunner::abortTests() noexcept {
	aborted = true;
	emit logMsg(QString("User choose to abort the testrun."));
	emit testsAborted();
}

void TestsRunner::setChannelsStates(const std::vector<bool>& value) noexcept {
	for (auto& test : tests_) {
		auto ptr = dynamic_cast<AbstractChannelsTest*>(test);
		if(ptr)
			ptr->setChannelsStates(value);
	}	
}

void TestsRunner::startTests() noexcept {
	for (const auto& test : tests_)
		test->reset();
	for(const auto& test : tests_) {
		QCoreApplication::processEvents();
		if (aborted) 
			return;
		if (test->shouldBeRun()) {
			try {
				emit testRunned(test->name());
				test->run();
				emit testFinished(test->name(), test->result().toInt());
			}
			catch (const StopTestRunException & ex) {
				emit logMsg(QString("User choose to stop the test run after error occured. Error msg: %1").arg(ex.what()));
				emit testsAborted();
				return;
			}
			catch (...) {
				emit logMsg("Unexpected exception thrown.. test failed.");
			}
		}
	}
	auto testsRunPassed = std::find_if(tests_.begin(), tests_.end(), [](auto const& test) { return test->wasRunned() && !test->passed(); }) == tests_.end();
	emit testsCompleted(testsRunPassed);
}
