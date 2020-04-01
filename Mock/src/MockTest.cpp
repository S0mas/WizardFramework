#include "../include/MockTest.h"
#include <chrono>
#include <thread>
#include <QString>

Result MockTest::test() const {
	for (int i = 0; i < 3; ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		log(QString("step_%1...").arg(i));
	}
	return mockResult ? Result::VALUE::PASSED : Result::VALUE::FAILED;
}

void MockTest::preTestSetUp() const {
	log("PRECONDITIONS..");
}

void MockTest::postTestCleanUp() const {
	log("POSTCONDITIONS..");
}

void MockTest::setChannelsStates(const std::vector<bool>& states) noexcept {
	device1->setChannelsStates(states);
}

MockTest::MockTest(const bool mockResult, QObject* parent) noexcept : AbstractChannelsTest(QString("MockTest %1").arg(id++), 32, parent), mockResult(mockResult) {}

Result MockTestWithConfirmationAction::test() const {
	for (int i = 0; i < 2; ++i) {
		sender_->waitForConfirmation("Test msg");
		log("Confirmed");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		log(QString("step_%1...").arg(i));
	}
	return mockResult ? Result::VALUE::PASSED : Result::VALUE::FAILED;
}

MockTestWithConfirmationAction::MockTestWithConfirmationAction(const bool mockResult, QObject* parent) noexcept : MockTest(mockResult, parent) {}

Result MockTestWithDecisionAction::test() const {
	for (int i = 0; i < 2; ++i) {
		sender_->waitForAcceptOrDecline("Test msg") ? log("Accepted") : log("Rejected");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		log(QString("step_%1...").arg(i));
	}
	return mockResult ? Result::VALUE::PASSED : Result::VALUE::FAILED;
}

MockTestWithDecisionAction::MockTestWithDecisionAction(const bool mockResult, QObject* parent) noexcept : MockTest(mockResult, parent) {}

Result MockTestWithChannelsErrorSelection::test() const {
	for (auto const& channel : device1->channels())
		if (channel.enabled()) channelsResults.at(channel.index()) = Result::VALUE::PASSED;

	std::vector<bool> markedChannelsMask[2];
	for (int i = 0; i < 2; ++i) {
		markedChannelsMask[i] = sender_->waitForChannelsSelection("Test msg", device1->channelsStates());
		for (int pos = 0; pos < markedChannelsMask[i].size(); ++pos)
			if (markedChannelsMask[i][pos])
				channelsResults.at(pos+1) = Result::VALUE::FAILED;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		log(QString("step_%1...").arg(i));
	}
	return channelsResult();
}

MockTestWithChannelsErrorSelection::MockTestWithChannelsErrorSelection(const bool mockResult, QObject* parent) noexcept : MockTest(mockResult, parent) {}

Result MockTestWithErrorHandlingSelection::test() const {
	for (int i = 0; i < 2; ++i) {
		sender_->waitForErrorHandlingType("Test error");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		log(QString("step_%1...").arg(i));
	}
	return mockResult ? Result::VALUE::PASSED : Result::VALUE::FAILED;
}

MockTestWithErrorHandlingSelection::MockTestWithErrorHandlingSelection(const bool mockResult, QObject* parent) noexcept : MockTest(mockResult, parent) {}
