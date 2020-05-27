#include "../../include/Device6991/Registers6991.h"
#include "../../include/Device6991/Device6991.h"

bool Register::readHw(uint32_t const mask, uint32_t const shiftRight) noexcept {
	return readHwFunction_(mask, shiftRight);
}

bool Register::writeHw(uint32_t const data, uint32_t const mask, uint32_t const shiftLeft) noexcept {
	return writeHwFunction_(data, mask, shiftLeft);
}

bool Register::writeHw() noexcept {
	return writeHwFunctionNoArgs_();
}

Register::Register(uint32_t address) : address_(address) {}

std::optional<bool> Register::testBit(int const bit) noexcept {
	if (readHw())
		return data_.test(bit);
	return false;
}

bool Register::setBit(int const bit, bool const state) noexcept {
	return writeHw(state ? 1 : 0, 1 << bit, bit);
}

bool Register::testReg(uint32_t const expected, uint32_t const mask, uint32_t const shiftRight, bool const verbose) noexcept {
	return testRegFunction_(expected, mask, shiftRight, verbose);
}

//bool RegisterFeCard::readBoth(uint32_t const mask, uint32_t const shiftRight) noexcept {
//	if (auto reg = deviceIF->readBothFecRegisters(address_); reg) {	
//		if (shiftRight >= 0) 
//			dataBoth_ = { ((*reg).first & mask) >> shiftRight, ((*reg).second & mask) >> shiftRight };		
//		else 
//			dataBoth_ = { ((*reg).first & mask) << -shiftRight, ((*reg).second & mask) << -shiftRight };
//		return true;
//	}
//	return false;
//}

FC_WR_QUEUE_EMP::FC_WR_QUEUE_EMP(Device6991* deviceIF) : Register6991(RegistersEnum::FC_WR_QUEUE_EMP, deviceIF) {}

PCI_ERR_STCTRL::PCI_ERR_STCTRL(Device6991* deviceIF) : Register6991(RegistersEnum::PCI_ERR_STCTRL, deviceIF) {}

PREF_ERR_ADDR::PREF_ERR_ADDR(Device6991* deviceIF) : Register6991(RegistersEnum::PREF_ERR_ADDR, deviceIF) {}

BOARD_ID_reg::BOARD_ID_reg(Device6991* deviceIF) : Register6991(RegistersEnum::BOARD_ID_reg, deviceIF) {}

std::optional<uint16_t> BOARD_ID_reg::id() noexcept {
	return value(0xFFFF);
}

HW_REV_reg::HW_REV_reg(Device6991* deviceIF) : Register6991(RegistersEnum::HW_REV_reg, deviceIF) {}

std::optional<uint8_t> HW_REV_reg::pcbRev() noexcept {
	return value(0xFF0000, 16);
}

std::optional<uint8_t> HW_REV_reg::fpgaBranch() noexcept {
	return value(0xF000, 12);
}

std::optional<uint8_t> HW_REV_reg::fpgaBranchRev() noexcept {
	return value(0xF00, 8);
}

std::optional<uint8_t> HW_REV_reg::fpgaRev() noexcept {
	return value(0xFF);
}

BOARD_CSR1_reg::BOARD_CSR1_reg(Device6991* deviceIF) : Register6991(RegistersEnum::BOARD_CSR1_reg, deviceIF) {}

std::optional<bool> BOARD_CSR1_reg::isAuxPresent() noexcept {
	return testBit(AUX_PRESENT);
}

std::optional<bool> BOARD_CSR1_reg::isFec1Present() {
	return testBit(FE1_PRESENT);
}

std::optional<bool> BOARD_CSR1_reg::isFec2Present() {
	return testBit(FE2_PRESENT);
}

std::optional<bool> BOARD_CSR1_reg::isFec1Ready() {
	return testBit(FE1_READY);
}

std::optional<bool> BOARD_CSR1_reg::isFec2Ready() {
	return testBit(FE2_READY);
}

std::optional<bool> BOARD_CSR1_reg::isFecReady(FecIdType::Type const fecId) {
	return fecId == FecIdType::_1 ? isFec1Ready() : isFec2Ready();
}

std::optional<bool> BOARD_CSR1_reg::isFecPresent(FecIdType::Type const fecId) {
	return fecId == FecIdType::_1 ? isFec1Present() : isFec2Present();
}

LED_CSR_reg::LED_CSR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::LED_CSR_reg, deviceIF) {}

CL0_SPI_DATA_reg::CL0_SPI_DATA_reg(Device6991* deviceIF) : Register6991(RegistersEnum::CL0_SPI_DATA_reg, deviceIF) {}

CL1_SPI_DATA_reg::CL1_SPI_DATA_reg(Device6991* deviceIF) : Register6991(RegistersEnum::CL1_SPI_DATA_reg, deviceIF) {}

bool CL_SPI_CSR_reg::isBusy() noexcept {
	return data_.test(CL_START_CL_BUSY);
}

void CL_SPI_CSR_reg::setCommand(Command const cmd) noexcept {
	data_ &= 0xFF00FFFF;
	data_ |= cmd << 16;
}

CL_SPI_CSR_reg::CL_SPI_CSR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::CL_SPI_CSR_reg, deviceIF) {}
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
		if (type == TestTypeEnum::CL0)
			return data_.test(CL_TEST_LOOP) && data_.test(CL0_SEL);
		else if (type == TestTypeEnum::CL1)
			return data_.test(CL_TEST_LOOP) && data_.test(CL1_SEL);
		else
			return data_.test(CL_TEST_LOOP) && (data_.test(CL0_SEL) || data_.test(CL1_SEL));
	}
	return std::nullopt;
}

CL_SPI_TLCNT_reg::CL_SPI_TLCNT_reg(Device6991* deviceIF) : Register6991(RegistersEnum::CL_SPI_TLCNT_reg, deviceIF) {}

CL0_SPI_TLERR_reg::CL0_SPI_TLERR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::CL0_SPI_TLERR_reg, deviceIF) {}

CL1_SPI_TLERR_reg::CL1_SPI_TLERR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::CL1_SPI_TLERR_reg, deviceIF) {}

DL_SPI_CSR1_reg::DL_SPI_CSR1_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL_SPI_CSR1_reg, deviceIF) {}

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

std::optional<uint32_t> DL_SPI_CSR1_reg::dlFsmStatus() noexcept {
	return value(0xF00, 8);
}

std::optional<uint32_t> DL_SPI_CSR1_reg::dlFifoFsmStatus() noexcept {
	return value(0xF0, 4);
}

bool DL_SPI_CSR1_reg::isDlReciverIdle(bool const verbose) noexcept {
	return testReg(0, 0xF00, 8, verbose) && testReg(0, 0xF0, 4, verbose);
}

DL_SPI_CSR2_reg::DL_SPI_CSR2_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL_SPI_CSR2_reg, deviceIF) {}

bool DL_SPI_CSR2_reg::setDl0FrameLength(DlFrameRateType::Type const frameType) noexcept {
	return writeHw(frameType, 0x3F);
}

bool DL_SPI_CSR2_reg::setDl1FrameLength(DlFrameRateType::Type const frameType) noexcept {
	return writeHw(frameType, 0x3F0000, 16);
}

bool DL_SPI_CSR2_reg::setDlFrameLength(FecType::Type const cardType, FecIdType::Type const fcId) noexcept {
	DlFrameRateType::Type rateType = DlFrameRateType::INVALID;
	if (cardType == FecType::_6111 || cardType == FecType::_6171)
		rateType = DlFrameRateType::BU6111_BU6171;
	else if (cardType == FecType::_6132)
		rateType = DlFrameRateType::BU6132;
	switch (fcId) {
	case FecIdType::BOTH:
		return setDl0FrameLength(rateType) && setDl1FrameLength(rateType);
	case FecIdType::_1:
		return setDl0FrameLength(rateType);
	case FecIdType::_2:
		return setDl0FrameLength(rateType);
	default:
		return false;
	}
}

std::optional<uint32_t> DL_SPI_CSR2_reg::dl0FrameLength() noexcept {
	return value(0x1F);
}

std::optional<uint32_t> DL_SPI_CSR2_reg::dl1FrameLength() noexcept {
	return value(0x1F0000, 16);
}

DL_SPI_CSR3_reg::DL_SPI_CSR3_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL_SPI_CSR3_reg, deviceIF) {}

DL0_DWORD_EN_reg::DL0_DWORD_EN_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL0_DWORD_EN_reg, deviceIF) {}

DL1_DWORD_EN_reg::DL1_DWORD_EN_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL1_DWORD_EN_reg, deviceIF) {}

DL0_SPI_TMCNT_reg::DL0_SPI_TMCNT_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL0_SPI_TMCNT_reg, deviceIF) {}

DL0_SPI_TMERR_reg::DL0_SPI_TMERR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL0_SPI_TMERR_reg, deviceIF) {}

DL1_SPI_TMCNT_reg::DL1_SPI_TMCNT_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL1_SPI_TMCNT_reg, deviceIF) {}

DL1_SPI_TMERR_reg::DL1_SPI_TMERR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DL1_SPI_TMERR_reg, deviceIF) {}

SYS_TMR_CSR_reg::SYS_TMR_CSR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::SYS_TMR_CSR_reg, deviceIF) {}

SYS_TMR_L_reg::SYS_TMR_L_reg(Device6991* deviceIF) : Register6991(RegistersEnum::SYS_TMR_L_reg, deviceIF) {}

SYS_TMR_H_reg::SYS_TMR_H_reg(Device6991* deviceIF) : Register6991(RegistersEnum::SYS_TMR_H_reg, deviceIF) {}

EVENT_LOG_CSR_reg::EVENT_LOG_CSR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::EVENT_LOG_CSR_reg, deviceIF) {}

EVENT_STAMP_L_reg::EVENT_STAMP_L_reg(Device6991* deviceIF) : Register6991(RegistersEnum::EVENT_STAMP_L_reg, deviceIF) {}

EVENT_STAMP_H_reg::EVENT_STAMP_H_reg(Device6991* deviceIF) : Register6991(RegistersEnum::EVENT_STAMP_H_reg, deviceIF) {}

EVENT_CAUSE_reg::EVENT_CAUSE_reg(Device6991* deviceIF) : Register6991(RegistersEnum::EVENT_CAUSE_reg, deviceIF) {}

DEBUG_CSR_reg::DEBUG_CSR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DEBUG_CSR_reg, deviceIF) {}

bool DEBUG_CSR_reg::startClock() noexcept {
	return setBit(DEBUG_CLK_EN, true);
}

bool DEBUG_CSR_reg::stopClock() noexcept {
	return setBit(DEBUG_CLK_EN, false);
}

DEBUG_CLK_RATE_reg::DEBUG_CLK_RATE_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DEBUG_CLK_RATE_reg, deviceIF) {}

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

ACQ_CSR_reg::ACQ_CSR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::ACQ_CSR_reg, deviceIF) {}

std::optional<bool> ACQ_CSR_reg::isAcqActive() noexcept {
	return testBit(ACQ_ON);
}

ACQ_RATE_reg::ACQ_RATE_reg(Device6991* deviceIF) : Register6991(RegistersEnum::ACQ_RATE_reg, deviceIF) {}

ACQ_ALARM_L_reg::ACQ_ALARM_L_reg(Device6991* deviceIF) : Register6991(RegistersEnum::ACQ_ALARM_L_reg, deviceIF) {}

ACQ_ALARM_H_reg::ACQ_ALARM_H_reg(Device6991* deviceIF) : Register6991(RegistersEnum::ACQ_ALARM_H_reg, deviceIF) {}

DFIFO_CSR_reg::DFIFO_CSR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DFIFO_CSR_reg, deviceIF) {}

bool DFIFO_CSR_reg::enableTestMode() noexcept {
	return setBit(DFIFO_TEST_MODE, true);
}

bool DFIFO_CSR_reg::disableTestMode() noexcept {
	return setBit(DFIFO_TEST_MODE, false);
}

bool DFIFO_CSR_reg::clearOverflow() noexcept {
	return setBit(DFIFO_OVF_FLAG, false);
}

bool DFIFO_CSR_reg::resetFifo() noexcept {
	return writeHw(0x6, 0x7000000, 24);
}

bool DFIFO_CSR_reg::setBlockSize(uint32_t const blockSize) noexcept {
	if (blockSize > 0 && blockSize < 128)
		return writeHw(blockSize, 0x7F0000);
	return false;
}

std::optional<uint32_t> DFIFO_CSR_reg::blockSize() noexcept {
	return value(0x7F0000, 16);
}

std::optional<uint32_t> DFIFO_CSR_reg::samplesInFifo() noexcept {
	return value(0x1FFF);
}

std::optional<bool> DFIFO_CSR_reg::isTestRunning() noexcept {
	return testBit(DFIFO_TEST_MODE);
}

std::optional<bool> DFIFO_CSR_reg::isFifoEmpty() noexcept {
	return testBit(DFIFO_EMPTY_FLAG);
}

std::optional<bool> DFIFO_CSR_reg::isFifoFull() noexcept {
	return testBit(DFIFO_FULL_FLAG);
}

std::optional<bool> DFIFO_CSR_reg::isFifoProgFull() noexcept {
	return testBit(DFIFO_PROG_FLAG);
}

std::optional<bool> DFIFO_CSR_reg::overflowHappened() noexcept {
	return testBit(DFIFO_OVF_FLAG);
}

DFIFO_PFLAG_THR_reg::DFIFO_PFLAG_THR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DFIFO_PFLAG_THR_reg, deviceIF) {}

bool DFIFO_PFLAG_THR_reg::setThreshold(uint32_t const threshold) noexcept {
	if (threshold >= 0 && threshold < 8192)
		return writeHw(threshold, 0x1FFF);
	return false;
}

std::optional<uint32_t> DFIFO_PFLAG_THR_reg::threshold() noexcept {
	return value(0x1FFF);
}

DFIFO_WR_reg::DFIFO_WR_reg(Device6991* deviceIF) : Register6991(RegistersEnum::DFIFO_WR_reg, deviceIF) {}

DFIFO::DFIFO(Device6991* deviceIF) : Register6991(RegistersEnum::DFIFO, deviceIF) {}

Register6991::Register6991(uint32_t const address, Device6991* devIF_) : Register(address) {
	readRegisterFunction_ = [devIF_](uint32_t const address) { return devIF_->readFpgaRegister(address); };
	writeRegisterFunction_ = [devIF_](uint32_t const address, uint32_t const data) { return devIF_->writeFpgaRegister(address, data); };
	reportErrorFunction_ = [devIF_](QString const& msg) { emit devIF_->reportError(msg); };
}

RegisterFec::RegisterFec(FecIdType::Type const id, uint32_t const address, Device6991* devIF) : Register(address), id_(id), devIF_(devIF) {
	if (id_ != FecIdType::BOTH) {
		readRegisterFunction_ = [this](uint32_t const address) { return devIF_->readFecRegister(id_, address); };
		writeRegisterFunction_ = [this](uint32_t const address, uint32_t const data) { return devIF_->writeFecRegister(id_, address, data); };
	}
	else {
		readHwFunction_ = [this](uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftRight = 0) {
			if (auto regs = devIF_->readBothFecRegisters(address_); regs) {
				data_ = ((*regs).first & mask) >> shiftRight;
				dataSecond_ = ((*regs).second & mask) >> shiftRight;
				return true;
			}
			reportErrorFunction_(QString("Faild to read both fec registers. Reg address: %1").arg(toHex(address_, 8)));
			return false;
		};
		writeHwFunction_ = [this](uint32_t const data, uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftLeft = 0) {
			if (mask != 0xFFFFFFFF) {
				if (!readHw())
					return false;
			}
			data_ &= ~mask;
			data_ |= data << shiftLeft;
			dataSecond_ &= ~mask; 
			dataSecond_ |= data << shiftLeft;
			return writeHwFunctionNoArgs_();
		};
		testRegFunction_ = [this](uint32_t const expected, uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftRight = 0, bool const verbose = true) {
			if (readHw(mask, shiftRight)) {
				if (data_ == expected && dataSecond_ == expected)
					return true;
				else if (verbose)
					reportErrorFunction_(QString("One of values is different than expected. Address: %1 Expected: %2, Read1:%3, Read2:%4, Mask:%5, ShiftedRight:%6")
						.arg(toHex(address_, 8), toHex(expected, 8), toHex(data_.to_ulong(), 8), toHex(dataSecond_.to_ulong(), 8), toHex(mask, 8), QString::number(shiftRight)));
			}
			return false;
		};
		//TODO replace impl with commented one after write both regs command with different data each will be ready
		writeHwFunctionNoArgs_ = [this]() { return devIF_->writeFecRegister(FecIdType::BOTH, address_, data_.to_ulong());/*return devIF_->writeBothFecRegisters(address_, data_.to_ulong(), dataSecond_.to_ulong());*/};
	}
	reportErrorFunction_ = [this](QString const& msg) { emit devIF_->reportError(msg); };
}

std::optional<FecType::Type> FE_ID_reg::fecType() noexcept {
	return value<FecType::Type>(0xFFFF);
}

bool BOARD_CSR_reg::testStatus(FecType::Type const type, uint32_t const expectedStatus, bool const verbose) noexcept {
	return testReg(expectedStatus, type == FecType::_6111 ? MASK_STATUS_6111 : MASK_STATUS_6132, 0, verbose);
}

bool BOARD_CSR_reg::isIdle(FecType::Type const type, bool const verbose) noexcept {
	return type == FecType::_6111 ? testStatus(type, FecStatusType6111::IDLE) : testStatus(type, FecStatusType6132::IDLE, verbose);
}

bool CMD_reg::setCmd(uint32_t const cmd) noexcept {
	return writeHw(cmd, 0xFFFF);
}

bool DL_CSR_reg::setDlSclkFreqType(DlSclkFreqType::Type const freq) noexcept {
	return writeHw(freq, 0xF);
}

std::optional<DlSclkFreqType::Type> DL_CSR_reg::dlSclkFreqType() noexcept {
	return value<DlSclkFreqType::Type>(0xF);
}

bool CHN_FILTER_reg::setFilters(std::vector<uint32_t> const& channelIds, FilterType6132::Type const filter) noexcept {
	uint32_t data = 0;
	uint32_t mask = 0;
	for (auto id : channelIds) {
		data |= filter << ((id - 1) * BITS_PER_CHANNEL);
		mask |= MASK_PER_CHANNEL << ((id - 1) * BITS_PER_CHANNEL);
	}
	return writeHw(data, mask);
}

std::optional<FilterType6132::Type> CHN_FILTER_reg::filter(uint32_t const channelId) noexcept {
	if (auto const f = filters(); f)
		return (*f)[channelId - 1];
	return std::nullopt;
}

std::optional<std::vector<FilterType6132::Type>> CHN_FILTER_reg::filters() noexcept {
	if (!readHw())
		return std::nullopt;
	std::vector<FilterType6132::Type> filters;
	for (auto id : { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 })
		filters.push_back(static_cast<FilterType6132::Type>(data_.to_ulong() & (MASK_PER_CHANNEL << (id - 1))));
	return filters;
}

bool CHN_GAIN_reg::setGains(std::vector<uint32_t> const& channelIds, GainType6132::Type const gain) noexcept {
	uint32_t data = 0;
	uint32_t mask = 0;
	for (auto id : channelIds) {
		data |= gain << ((id - 1) * BITS_PER_CHANNEL);
		mask |= MASK_PER_CHANNEL << ((id - 1) * BITS_PER_CHANNEL);
	}
	return writeHw(data, mask);
}

std::optional<GainType6132::Type> CHN_GAIN_reg::gain(uint32_t const channelId) noexcept {
	if (auto const f = gains(); f)
		return (*f)[channelId - 1];
	return std::nullopt;
}

std::optional<std::vector<GainType6132::Type>> CHN_GAIN_reg::gains() noexcept {
	if (!readHw())
		return std::nullopt;
	std::vector<GainType6132::Type> gains;
	for (auto id : { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 })
		gains.push_back(static_cast<GainType6132::Type>(data_.to_ulong() & (MASK_PER_CHANNEL << (id - 1))));
	return gains;
}

bool ADC_SEL_reg::selectChannels(std::vector<uint32_t> const& channelIds) noexcept {
	data_ = 0;
	for (auto id : channelIds)
		data_ |= 1 << (id - 1);
	return writeHw();
}

std::optional<uint16_t> CHNX_RAW_SAMPLE_reg::rawData() noexcept {
	return value(0xFFFF0000, 16);
}

std::optional<uint16_t> CHNX_RAW_SAMPLE_reg::flags() noexcept {
	return value(0xFFFF);
}
