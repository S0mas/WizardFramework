#include "../../include/Device6991/Registers6991.h"
#include "../../include/Device6991/Device6991.h"

bool Register::readHw() noexcept {
	if (auto reg = deviceIF->readFpgaRegister(address_); reg) {
		data_ = *reg;
		return true;
	}
	return false;
}

bool Register::writeHw() const noexcept {
	return deviceIF->writeFpgaRegister(address_, data_.to_ulong());
}

Register::Register(int const address, Device6991* deviceIF) : address_(address), deviceIF(deviceIF) {}

std::optional<uint32_t> Register::value() noexcept {
	return readHw() ? std::optional{ data_.to_ulong() } : std::nullopt;
}

bool CL_SPI_CSR_reg::isBusy() noexcept {
	return data_.test(CL_START_CL_BUSY);
}

void CL_SPI_CSR_reg::setCommand(Command const cmd) noexcept {
	data_ &= 0xFFFFFF00;
	data_ |= cmd;
}

CL_SPI_CSR_reg::CL_SPI_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL_SPI_CSR_reg, deviceIF) {}
bool CL_SPI_CSR_reg::startTests(bool const CL0, bool const CL1) noexcept {
	if (CL0 || CL1) {
		readHw();
		data_.set(CL0_SEL, CL0);
		data_.set(CL1_SEL, CL1);
		setCommand(WRITE_LOOPBACK);
		data_.set(CL_TEST_LOOP);

		if (!isBusy()) {
			data_.set(CL_START_CL_BUSY);
			return writeHw();
		}
	}
	return false;
}

bool CL_SPI_CSR_reg::stopTests() noexcept {
	if (readHw()) {
		data_.reset(CL_TEST_LOOP);
		return writeHw();
	}
	return false;
}

std::optional<bool> CL_SPI_CSR_reg::isTestRunning(TestTypeEnum::Type const type) noexcept {
	if(type == TestTypeEnum::CL0)
		return readHw() ? std::optional{ data_.test(CL_TEST_LOOP) && data_.test(CL0_SEL) } : std::nullopt;
	else if(type == TestTypeEnum::CL1)
		return readHw() ? std::optional{ data_.test(CL_TEST_LOOP) && data_.test(CL1_SEL) } : std::nullopt;
	else {
		if (readHw()) {
			auto c1 = data_.test(CL_TEST_LOOP);
			auto c2 = data_.test(CL0_SEL);
			auto c3 = data_.test(CL1_SEL);
			return std::optional{ c1 && (c2 || c3) };
		}
		else
			return std::nullopt;
	}
		//return readHw() ? std::optional{ data_.test(CL_TEST_LOOP) && (data_.test(CL0_SEL) || data_.test(CL1_SEL)) } : std::nullopt;
}

DL_SPI_CSR1_reg::DL_SPI_CSR1_reg(Device6991* deviceIF) : Register(RegistersEnum::DL_SPI_CSR1_reg, deviceIF) {}

bool DL_SPI_CSR1_reg::startTests(bool const DL0, bool const DL1) noexcept {
	if (DL0 || DL1) {
		readHw();
		data_.set(DL0_TEST_MODE, DL0);
		data_.set(DL1_TEST_MODE, DL1);
		data_.set(DL_EN);
		return writeHw();
	}
	return false;
}

bool DL_SPI_CSR1_reg::stopTests() noexcept {
	if (readHw()) {
		data_.reset(DL0_TEST_MODE);
		data_.reset(DL1_TEST_MODE);
		data_.reset(DL_EN);
		return writeHw();
	}
	return false;
}

std::optional<bool> DL_SPI_CSR1_reg::isTestRunning() noexcept {
	return readHw() ? std::optional{ data_.test(DL_EN) && (data_.test(DL0_TEST_MODE) || data_.test(DL1_TEST_MODE)) } : std::nullopt;
}

DFIFO_CSR_reg::DFIFO_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::DFIFO_CSR_reg, deviceIF) {}

bool DFIFO_CSR_reg::startTests() noexcept {
	if (readHw()) {
		data_.set(DFIFO_TEST_MODE);
		return writeHw();
	}
	return false;
}

bool DFIFO_CSR_reg::stopTests() noexcept {
	if (readHw()) {
		data_.reset(DFIFO_TEST_MODE);
		return writeHw();
	}
	return false;
}

bool DFIFO_CSR_reg::clearOverflow() noexcept {
	if (readHw()) {
		data_.set(DFIFO_OVF_FLAG);
		return writeHw();
	}
	return false;
}

bool DFIFO_CSR_reg::resetFifo() noexcept {
	if (readHw()) {
		data_ &= 0xF8FFFFFF;
		data_ |= 0x06000000;
		return writeHw();
	}
	return false;
}

bool DFIFO_CSR_reg::setBlockSize(uint32_t const blockSize) noexcept {
	if (readHw() && blockSize > 0 && blockSize < 128) {
		data_ &= 0xFF80FFFF;
		data_ |= blockSize << 16;
		return writeHw();
	}
	return false;
}

std::optional<int> DFIFO_CSR_reg::blockSize() noexcept {
	return readHw() ? std::optional{ (data_.to_ulong() & 0x7F0000)>>16 } : std::nullopt;
}

std::optional<int> DFIFO_CSR_reg::samplesInFifo() noexcept {
	return readHw() ? std::optional{ data_.to_ulong() & 0x1FFF } : std::nullopt;
}

std::optional<bool> DFIFO_CSR_reg::isTestRunning() noexcept {
	return readHw() ? std::optional{ data_.test(DFIFO_TEST_MODE) } : std::nullopt;
}

std::optional<bool> DFIFO_CSR_reg::isFifoEmpty() noexcept {
	return readHw() ? std::optional{ data_.test(DFIFO_EMPTY_FLAG) } : std::nullopt;
}

std::optional<bool> DFIFO_CSR_reg::isFifoFull() noexcept {
	return readHw() ? std::optional{ data_.test(DFIFO_FULL_FLAG) } : std::nullopt;
}

std::optional<bool> DFIFO_CSR_reg::isFifoProgFull() noexcept {
	return readHw() ? std::optional{ data_.test(DFIFO_PROG_FLAG) } : std::nullopt;
}

std::optional<bool> DFIFO_CSR_reg::overflowHappened() noexcept {
	return readHw() ? std::optional{ data_.test(DFIFO_OVF_FLAG) } : std::nullopt;
}

CL_SPI_TLCNT_reg::CL_SPI_TLCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::CL_SPI_TLCNT_reg, deviceIF) {}

CL0_SPI_TLERR_reg::CL0_SPI_TLERR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL0_SPI_TLERR_reg, deviceIF) {}

CL1_SPI_TLERR_reg::CL1_SPI_TLERR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL1_SPI_TLERR_reg, deviceIF) {}

DL0_SPI_TMCNT_reg::DL0_SPI_TMCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::DL0_SPI_TMCNT_reg, deviceIF) {}

DL0_SPI_TMERR_reg::DL0_SPI_TMERR_reg(Device6991* deviceIF) : Register(RegistersEnum::DL0_SPI_TMERR_reg, deviceIF) {}

DL1_SPI_TMCNT_reg::DL1_SPI_TMCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::DL1_SPI_TMCNT_reg, deviceIF) {}

DL1_SPI_TMERR_reg::DL1_SPI_TMERR_reg(Device6991* deviceIF) : Register(RegistersEnum::DL1_SPI_TMERR_reg, deviceIF) {}

ACQ_CSR_reg::ACQ_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::ACQ_CSR_reg, deviceIF) {}

std::optional<bool> ACQ_CSR_reg::isAcqActive() noexcept {
	return readHw() ? std::optional{ data_.test(ACQ_ON) } : std::nullopt;
}

DEBUG_CSR_reg::DEBUG_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::DEBUG_CSR_reg, deviceIF) {}

bool DEBUG_CSR_reg::startClock() noexcept {
	if (readHw()) {
		data_.set(DEBUG_CLK_EN);
		return writeHw();
	}
	return false;
}

bool DEBUG_CSR_reg::stopClock() noexcept {
	if (readHw()) {
		data_.reset(DEBUG_CLK_EN);
		return writeHw();
	}
	return false;
}

DEBUG_CLK_RATE_reg::DEBUG_CLK_RATE_reg(Device6991* deviceIF) : Register(RegistersEnum::DEBUG_CLK_RATE_reg, deviceIF) {}

bool DEBUG_CLK_RATE_reg::setRate(double const rateInNano) noexcept {
	if (rateInNano > RATE_STEP_NANO_SECS && rateInNano < MAX_RATE_NANO_SECS) {
		data_ = rateInNano / RATE_STEP_NANO_SECS;
		return writeHw();
	}
	return false;
}

std::optional<double> DEBUG_CLK_RATE_reg::rate() noexcept {
	return readHw() ? std::optional{ data_.to_ullong() * RATE_STEP_NANO_SECS } : std::nullopt;
}

DFIFO_PFLAG_THR_reg::DFIFO_PFLAG_THR_reg(Device6991* deviceIF) : Register(RegistersEnum::DFIFO_PFLAG_THR_reg, deviceIF) {}


bool DFIFO_PFLAG_THR_reg::setThreshold(uint32_t const threshold) noexcept {
	if (readHw() && threshold >= 0 && threshold < 8192) {
		data_ &= 0xFFFFE000;
		data_ |= threshold;
		return writeHw();
	}
	return false;
}

std::optional<uint32_t> DFIFO_PFLAG_THR_reg::threshold() noexcept {
	return readHw() ? std::optional{ data_.to_ullong() & 0x1FFF } : std::nullopt;
}

DFIFO::DFIFO(Device6991* deviceIF) : Register(RegistersEnum::DFIFO, deviceIF) {}
