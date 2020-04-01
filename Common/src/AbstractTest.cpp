#include "..\include\AbstractTest.h"
#include "..\include\Exceptions.h"
#include <thread>
#include <chrono>
#include <QCoreApplication>
#include <QThread>

QString AbstractTest::summary() const noexcept {
	QString summary;
	summary += QString("Test result summary: %1\n").arg(name());
	summary += QString("Result: %1\n").arg(result().toString());
	return summary;
}

AbstractTest::AbstractTest(const QString& name, QObject* parent) noexcept : UserCommunicationObject(parent), name_(name) {}

const QString& AbstractTest::name() const noexcept {
	return name_;
}

void AbstractTest::run() const {
	log(QString("%1 -----------> STARTED").arg(name_));
	result_ = Result::VALUE::FAILED;
	try {
		preTestSetUp();
		result_ = test();
		postTestCleanUp();
	}
	catch (const SkipTestException& exc) {
		log(QString("User choose to skip the test after error occured. Error msg: %1").arg(exc.what()));
	}
	auto const& sum = summary();
	log(QString("%1 -----------> %2\n%3\n****** ***** ***** ***** ***** *****").arg(name_, result_.toString(), sum));
	emit testSummary(sum);
}

bool AbstractTest::passed() const noexcept {
	return result_ == Result::VALUE::PASSED;
}

bool AbstractTest::wasRunned() const noexcept {
	return result_ != Result::VALUE::NOT_RUNNED;
}

void AbstractTest::setShouldBeRun(const bool value) noexcept {
	shouldBeRun_ = value;
	emit shouldBeRunChanged(shouldBeRun_);
}

void AbstractTest::reset() noexcept {
	result_ = Result::VALUE::NOT_RUNNED;
	additionalReset();
}

Result AbstractTest::result() const noexcept {
	return result_;
}

bool AbstractTest::shouldBeRun() const noexcept {
	return shouldBeRun_;
}

AbstractChannelsTest::AbstractChannelsTest(const QString& name, const int channelsCount, QObject* parent) noexcept : AbstractTest(name, parent) {
	for(int i = 1; i <= channelsCount; ++i)
		channelsResults[i] = Result::VALUE::NOT_RUNNED;
}

Result AbstractChannelsTest::channelsResult() const noexcept {
	if(std::find_if(channelsResults.begin(), channelsResults.end(), [](auto const& result) { return result.second == Result::VALUE::FAILED; }) != channelsResults.end())
		return Result::VALUE::FAILED;
	else if (std::find_if(channelsResults.begin(), channelsResults.end(), [](auto const& result) { return result.second == Result::VALUE::NOT_RUNNED; }) != channelsResults.end())
		return Result::VALUE::PARTIALLY;
	return Result::VALUE::PASSED;
}

void AbstractChannelsTest::additionalReset() noexcept {
	for (auto& result : channelsResults)
		result.second = Result::VALUE::NOT_RUNNED;
}

QString AbstractChannelsTest::summary() const noexcept {
	QString summary;
	summary += QString("Test result summary: %1\n").arg(name());
	int channelId = 1;
	for (auto const& result : channelsResults)
		summary += QString("Channel %1 --- %2\n").arg(channelId++).arg(result.second.toString());
	summary += QString("Result: %1\n").arg(result().toString());
	return summary;
}
