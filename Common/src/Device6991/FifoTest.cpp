#include "../../include/Device6991/FifoTest.h"
#include "../../include/Device6991/Device6991.h"

FifoTest::FifoTest(Device6991 * devIF) : devIF_(devIF) {
	dataFile_ = new QFile("FifoTestErrors.csv", devIF);
}

void FifoTest::validateData(std::vector<uint32_t> const & data) {
	for (auto const& sample : data) {
		if (expected_++ != sample) {
			if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
				return;
			QTextStream out(dataFile_);
			out << expected_ - 1 << ", " << sample << '\n';
			++errors_;
			expected_ = sample + 1;
		}
		++count_;

	}		
	if (!data.empty())
		lastSample_ = data.back();
}

FifoTestModel FifoTest::model() noexcept {
	FifoTestModel model;
	auto count = devIF_->DFIFO_CSR_reg_.samplesInFifo();
	auto overflow = devIF_->DFIFO_CSR_reg_.overflowHappened();
	auto progThresholdPassed = devIF_->DFIFO_CSR_reg_.isFifoProgFull();
	model.config_.blockSize_ = devIF_->DFIFO_CSR_reg_.blockSize();
	model.config_.treshold_ = devIF_->DFIFO_PFLAG_THR_reg_.threshold();
	if (overflow && *overflow)
		model.overflows_++;
	if (progThresholdPassed && *progThresholdPassed)
		model.passTresholdCount_++;
	if (count)
		model.samplesCount_ = count;
	model.lastSample_ = lastSample_;
	model.dataErrorsCount_ = errors_;
	return model;
}

bool FifoTest::configure(FifoTestModel::Configuration const & config) noexcept {
	bool rateChangeStatus = true;
	bool blockSizeChangeStatus = true;
	bool thresholdChangeStatus = true;
	if (config.rate_)
		rateChangeStatus = devIF_->DEBUG_CLK_RATE_reg_.setRate(*config.rate_);
	if (config.blockSize_)
		blockSizeChangeStatus = devIF_->DFIFO_CSR_reg_.setBlockSize(*config.blockSize_);
	if (config.treshold_)
		thresholdChangeStatus = devIF_->DFIFO_PFLAG_THR_reg_.setThreshold(*config.treshold_);

	return rateChangeStatus && blockSizeChangeStatus && thresholdChangeStatus;
}

bool FifoTest::startTest(FifoTestModel::Configuration const & config) noexcept {
	count_ = 0;
	errors_ = 0;
	expected_ = 0;
	dataFile_->resize(0);
	devIF_->dataStream_.clearDataFile();
	if (!devIF_->dataStream_.isConnected()) {
		devIF_->reportError("Connection with data stream is not open/valid. Please open/fix it before starting the test.");
		return false;
	}
	return devIF_->invokeCmd("*DBG 4") &&
		devIF_->DFIFO_CSR_reg_.resetFifo() &&
		devIF_->DFIFO_CSR_reg_.isFifoEmpty() &&
		devIF_->DFIFO_CSR_reg_.enableTestMode() &&
		configure(config) &&
		devIF_->DEBUG_CSR_reg_.startClock();
}

bool FifoTest::stopTest() noexcept {
	return devIF_->DEBUG_CSR_reg_.stopClock() &&
		devIF_->DFIFO_CSR_reg_.disableTestMode() &&
		devIF_->invokeCmd("*DBG 0");
}

uint64_t FifoTest::errors() const noexcept {
	return errors_;
}

uint64_t FifoTest::count() const noexcept {
	return count_;
}

bool FifoTest::isRunning() const noexcept {
	auto isRunning = devIF_->DFIFO_CSR_reg_.isTestRunning();
	return isRunning ? *isRunning : false;
}
