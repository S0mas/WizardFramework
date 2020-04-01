#pragma once
#include "../include/TestsRunnerController.h"
#include <algorithm>

 void TestsRunnerController::startTests() const noexcept {
	emit start();
}

void TestsRunnerController::pauseTests() const noexcept {
	emit pause();
}

void TestsRunnerController::abortTests() const noexcept {
	emit abort();
}

void TestsRunnerController::resumeTests() const noexcept {
	emit resume();
}

void TestsRunnerController::setChannelsStateOption(const std::vector<bool>& value) const noexcept {
	emit setChannelsStates(value);
}

TestsRunnerController::TestsRunnerController(QObject* parent) noexcept : LoggingObject(parent) {}

void TestsRunnerController::connectTestsRunner(const TestsRunner* runner) noexcept {
	connect(this, &TestsRunnerController::start, runner, &TestsRunner::startTests);
	connect(this, &TestsRunnerController::pause, runner, &TestsRunner::pauseTests);
	connect(this, &TestsRunnerController::resume, runner, &TestsRunner::resumeTests);
	connect(this, &TestsRunnerController::abort, runner, &TestsRunner::abortTests);
	connect(this, &TestsRunnerController::setChannelsStates, runner, &TestsRunner::setChannelsStates);
	connect(this, &TestsRunnerController::reportError, runner, &TestsRunner::reportError);

	//output
	connect(runner, &TestsRunner::testsAborted, this, &TestsRunnerController::aborted);
	connect(runner, &TestsRunner::testsCompleted, this, &TestsRunnerController::completed);
	connect(runner, &TestsRunner::testFinished, this, &TestsRunnerController::oneFinished);
	connect(runner, &TestsRunner::testRunned, this, &TestsRunnerController::oneRunned);
	connect(runner, &TestsRunner::testSummary, this, &TestsRunnerController::oneSummary);
	connect(runner, &TestsRunner::testsPaused, this, &TestsRunnerController::paused);
	connect(runner, &TestsRunner::testsResumed, this, &TestsRunnerController::resumed);

	// connect to printer
	connect(runner, &TestsRunner::logMsg, &printer, &PrintInterface::addLog);
}
