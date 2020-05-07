#include "../../include/Device6991/Registers6991.h"
#include "../../include/Device6991/Device6991.h"

bool Register::readHw(uint32_t const mask, uint32_t const shiftRight) noexcept {
	if (auto reg = deviceIF->readFpgaRegister(address_); reg) {
		if(shiftRight >= 0)
			data_ = (*reg & mask) >> shiftRight;
		else
			data_ = (*reg & mask) << -shiftRight;
		return true;
	}
	deviceIF->reportError(QString("Faild to read register. Reg address: %1").arg(toHex(address_, 8)));
	return false;
}

bool Register::writeHw() const noexcept {
	auto result = deviceIF->writeFpgaRegister(address_, data_.to_ulong());
	if(!result)
		deviceIF->reportError(QString("Faild to write register. Reg address: %1").arg(toHex(address_, 8)));
	return result;
}

Register::Register(uint32_t address, Device6991* deviceIF) : address_(address), deviceIF(deviceIF) {}

std::optional<uint32_t> Register::value(uint32_t const mask, uint32_t const shiftRight) noexcept {
	return readHw(mask, shiftRight) ? std::optional{ data_.to_ulong() } : std::nullopt;
}

bool RegisterFeCard::readHw(FecIdType::Type const fcId, uint32_t const mask, uint32_t const shiftRight) noexcept {
	if (auto reg = deviceIF->readFecRegister(fcId, address_); reg) {
		if (shiftRight >= 0)
			data_ = (*reg & mask) >> shiftRight;
		else
			data_ = (*reg & mask) << -shiftRight;
		return true;
	}
	return false;
}

bool RegisterFeCard::writeHw(FecIdType::Type const fcId) const noexcept {
	return deviceIF->writeFecRegister(fcId, address_, data_.to_ulong());
}

bool RegisterFeCard::readBoth(uint32_t const mask, uint32_t const shiftRight) noexcept {
	if (auto reg = deviceIF->readBothFecRegisters(address_); reg) {	
		if (shiftRight >= 0) 
			dataBoth_ = { ((*reg).first & mask) >> shiftRight, ((*reg).second & mask) >> shiftRight };		
		else 
			dataBoth_ = { ((*reg).first & mask) << -shiftRight, ((*reg).second & mask) << -shiftRight };
		return true;
	}
	return false;
}

RegisterFeCard::RegisterFeCard(uint32_t address, Device6991* deviceIF) : Register(address, deviceIF) {}

std::pair<std::optional<uint32_t>, std::optional<uint32_t>> RegisterFeCard::value(FecIdType::Type const fcId, uint32_t const mask, uint32_t const shiftRight) noexcept {
	if(fcId == FecIdType::BOTH)
		return readBoth(mask, shiftRight) ? std::pair<std::optional<uint32_t>, std::optional<uint32_t>>{dataBoth_.first.to_ulong(), dataBoth_.second.to_ulong()} : std::pair{std::nullopt, std::nullopt};
	else {
		if (auto read = readHw(fcId, mask, shiftRight); read)
			return fcId == FecIdType::_1 ? std::pair<std::optional<uint32_t>, std::optional<uint32_t>>{data_.to_ulong(), std::nullopt} : std::pair<std::optional<uint32_t>, std::optional<uint32_t>>{ std::nullopt, data_.to_ulong() };
		return std::pair{ std::nullopt, std::nullopt };
	}
}

FC_WR_QUEUE_EMP::FC_WR_QUEUE_EMP(Device6991* deviceIF) : Register(RegistersEnum::FC_WR_QUEUE_EMP, deviceIF) {}

PCI_ERR_STCTRL::PCI_ERR_STCTRL(Device6991* deviceIF) : Register(RegistersEnum::PCI_ERR_STCTRL, deviceIF) {}

PREF_ERR_ADDR::PREF_ERR_ADDR(Device6991* deviceIF) : Register(RegistersEnum::PREF_ERR_ADDR, deviceIF) {}

BOARD_ID_reg::BOARD_ID_reg(Device6991* deviceIF) : Register(RegistersEnum::BOARD_ID_reg, deviceIF) {}

HW_REV_reg::HW_REV_reg(Device6991* deviceIF) : Register(RegistersEnum::HW_REV_reg, deviceIF) {}

BOARD_CSR1_reg::BOARD_CSR1_reg(Device6991* deviceIF) : Register(RegistersEnum::BOARD_CSR1_reg, deviceIF) {}

LED_CSR_reg::LED_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::LED_CSR_reg, deviceIF) {}

CL0_SPI_DATA_reg::CL0_SPI_DATA_reg(Device6991* deviceIF) : Register(RegistersEnum::CL0_SPI_DATA_reg, deviceIF) {}

CL1_SPI_DATA_reg::CL1_SPI_DATA_reg(Device6991* deviceIF) : Register(RegistersEnum::CL1_SPI_DATA_reg, deviceIF) {}

bool CL_SPI_CSR_reg::isBusy() noexcept {
	return data_.test(CL_START_CL_BUSY);
}

void CL_SPI_CSR_reg::setCommand(Command const cmd) noexcept {
	data_ &= 0xFF00FFFF;
	data_ |= cmd << 16;
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
	if (readHw()) {
		if (type == TestTypeEnum::DL0)
			return data_.test(CL_TEST_LOOP) && data_.test(CL0_SEL);
		else if (type == TestTypeEnum::DL1)
			return data_.test(CL_TEST_LOOP) && data_.test(CL1_SEL);
		else
			return data_.test(CL_TEST_LOOP) && (data_.test(CL0_SEL) || data_.test(CL1_SEL));
	}
	return std::nullopt;
}

CL_SPI_TLCNT_reg::CL_SPI_TLCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::CL_SPI_TLCNT_reg, deviceIF) {}

CL0_SPI_TLERR_reg::CL0_SPI_TLERR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL0_SPI_TLERR_reg, deviceIF) {}

CL1_SPI_TLERR_reg::CL1_SPI_TLERR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL1_SPI_TLERR_reg, deviceIF) {}

DL_SPI_CSR1_reg::DL_SPI_CSR1_reg(Device6991* deviceIF) : Register(RegistersEnum::DL_SPI_CSR1_reg, deviceIF) {}

bool DL_SPI_CSR1_reg::enableTestMode(FecIdType::Type const fcId) noexcept {
	if (readHw()) {	
		data_.set(DL0_TEST_MODE, fcId == FecIdType::BOTH || fcId == FecIdType::_1);
		data_.set(DL1_TEST_MODE, fcId == FecIdType::BOTH || fcId == FecIdType::_2);
		data_.set(DL_EN);
		return writeHw();
	}
	return false;
}

bool DL_SPI_CSR1_reg::disableTestMode() noexcept {
	if (readHw()) {
		data_.reset(DL0_TEST_MODE);
		data_.reset(DL1_TEST_MODE);
		data_.reset(DL_EN);
		return writeHw();
	}
	return false;
}

std::optional<bool> DL_SPI_CSR1_reg::isTestRunning(TestTypeEnum::Type const type) noexcept {
	if (readHw()) {
		if (type == TestTypeEnum::DL0)
			return data_.test(DL_EN) && data_.test(DL0_TEST_MODE);
		else if (type == TestTypeEnum::DL1)
			return data_.test(DL_EN) && data_.test(DL1_TEST_MODE);
		else 
			return data_.test(DL_EN) && (data_.test(DL0_TEST_MODE) || data_.test(DL1_TEST_MODE));
	}
	return std::nullopt;
}

DL_SPI_CSR2_reg::DL_SPI_CSR2_reg(Device6991* deviceIF) : Register(RegistersEnum::DL_SPI_CSR2_reg, deviceIF) {}

DL_SPI_CSR3_reg::DL_SPI_CSR3_reg(Device6991* deviceIF) : Register(RegistersEnum::DL_SPI_CSR3_reg, deviceIF) {}

DL0_DWORD_EN_reg::DL0_DWORD_EN_reg(Device6991* deviceIF) : Register(RegistersEnum::DL0_DWORD_EN_reg, deviceIF) {}

DL1_DWORD_EN_reg::DL1_DWORD_EN_reg(Device6991* deviceIF) : Register(RegistersEnum::DL1_DWORD_EN_reg, deviceIF) {}

DL0_SPI_TMCNT_reg::DL0_SPI_TMCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::DL0_SPI_TMCNT_reg, deviceIF) {}

DL0_SPI_TMERR_reg::DL0_SPI_TMERR_reg(Device6991* deviceIF) : Register(RegistersEnum::DL0_SPI_TMERR_reg, deviceIF) {}

DL1_SPI_TMCNT_reg::DL1_SPI_TMCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::DL1_SPI_TMCNT_reg, deviceIF) {}

DL1_SPI_TMERR_reg::DL1_SPI_TMERR_reg(Device6991* deviceIF) : Register(RegistersEnum::DL1_SPI_TMERR_reg, deviceIF) {}

SYS_TMR_CSR_reg::SYS_TMR_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::SYS_TMR_CSR_reg, deviceIF) {}

SYS_TMR_L_reg::SYS_TMR_L_reg(Device6991* deviceIF) : Register(RegistersEnum::SYS_TMR_L_reg, deviceIF) {}

SYS_TMR_H_reg::SYS_TMR_H_reg(Device6991* deviceIF) : Register(RegistersEnum::SYS_TMR_H_reg, deviceIF) {}

EVENT_LOG_CSR_reg::EVENT_LOG_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::EVENT_LOG_CSR_reg, deviceIF) {}

EVENT_STAMP_L_reg::EVENT_STAMP_L_reg(Device6991* deviceIF) : Register(RegistersEnum::EVENT_STAMP_L_reg, deviceIF) {}

EVENT_STAMP_H_reg::EVENT_STAMP_H_reg(Device6991* deviceIF) : Register(RegistersEnum::EVENT_STAMP_H_reg, deviceIF) {}

EVENT_CAUSE_reg::EVENT_CAUSE_reg(Device6991* deviceIF) : Register(RegistersEnum::EVENT_CAUSE_reg, deviceIF) {}

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
	if (rateInNano >= RATE_STEP_NANO_SECS && rateInNano <= MAX_RATE_NANO_SECS) {
		data_ = rateInNano / RATE_STEP_NANO_SECS;
		return writeHw();
	}
	return false;
}

std::optional<double> DEBUG_CLK_RATE_reg::rate() noexcept {
	return readHw() ? std::optional{ static_cast<double>(data_.to_ullong()) * RATE_STEP_NANO_SECS } : std::nullopt;
}

ACQ_CSR_reg::ACQ_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::ACQ_CSR_reg, deviceIF) {}

std::optional<bool> ACQ_CSR_reg::isAcqActive() noexcept {
	return readHw() ? std::optional{ data_.test(ACQ_ON) } : std::nullopt;
}

ACQ_RATE_reg::ACQ_RATE_reg(Device6991* deviceIF) : Register(RegistersEnum::ACQ_RATE_reg, deviceIF) {}

ACQ_ALARM_L_reg::ACQ_ALARM_L_reg(Device6991* deviceIF) : Register(RegistersEnum::ACQ_ALARM_L_reg, deviceIF) {}

ACQ_ALARM_H_reg::ACQ_ALARM_H_reg(Device6991* deviceIF) : Register(RegistersEnum::ACQ_ALARM_H_reg, deviceIF) {}

DFIFO_CSR_reg::DFIFO_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::DFIFO_CSR_reg, deviceIF) {}

bool DFIFO_CSR_reg::enableTestMode() noexcept {
	if (readHw()) {
		data_.set(DFIFO_TEST_MODE);
		return writeHw();
	}
	return false;
}

bool DFIFO_CSR_reg::disableTestMode() noexcept {
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
	return readHw() ? std::optional{ (data_.to_ulong() & 0x7F0000) >> 16 } : std::nullopt;
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

DFIFO_WR_reg::DFIFO_WR_reg(Device6991* deviceIF) : Register(RegistersEnum::DFIFO_WR_reg, deviceIF) {}

DFIFO::DFIFO(Device6991* deviceIF) : Register(RegistersEnum::DFIFO, deviceIF) {}
