#pragma once
#include <bitset>
#include <optional>
#include "../../include/Device6991/Defines6991.h"

class Device6991;
class Register {
protected:
	uint32_t address_;
	Device6991* deviceIF;
	std::bitset<32> data_;
	bool readHw(uint32_t const mask = 0xFFFFFFFF) noexcept;
	bool writeHw() const noexcept;
public:
	Register(uint32_t const, Device6991*);
	std::optional<uint32_t> value() noexcept;
};

class FC_WR_QUEUE_EMP : public Register {
public:
	FC_WR_QUEUE_EMP(Device6991*);
};

class PCI_ERR_STCTRL : public Register {
	const int ERR_WRONG_CMD = 1;
	const int ERR_WRONG_BE = 2;
	const int ERR_ADDRLOW = 3;
	const int WATCHDOG_EN = 15;
public:
	PCI_ERR_STCTRL(Device6991*);

	//todo
	bool enablePciTargetWatchdog() noexcept { return true; }
	std::optional<uint32_t> wrongPciCmd() noexcept { return std::nullopt; }
	std::optional<bool> wrongPciA0A1AddressError() noexcept { return std::nullopt; }
	std::optional<bool> wrongPciByteEnablesError() noexcept { return std::nullopt; }
	std::optional<bool> wrongPciCmdError() noexcept { return std::nullopt; }
};

class PREF_ERR_ADDR : public Register {
public:
	PREF_ERR_ADDR(Device6991*);
};

class BOARD_ID_reg : public Register {
public:
	BOARD_ID_reg(Device6991*);
	std::optional<uint16_t> id() noexcept {
		return value() ? std::optional{ data_.to_ulong() & 0xFFFF } : std::nullopt;
	}
};

class HW_REV_reg : public Register {
public:
	HW_REV_reg(Device6991*);
	std::optional<uint8_t> pcbRev() noexcept {
		return value() ? std::optional{ (data_.to_ulong() & 0xFF0000)>>16 } : std::nullopt;
	}

	std::optional<uint8_t> fpgaBranch() noexcept {
		return value() ? std::optional{ (data_.to_ulong() & 0xF000) >> 12 } : std::nullopt;
	}

	std::optional<uint8_t> fpgaBranchRev() noexcept {
		return value() ? std::optional{ (data_.to_ulong() & 0xF00) >> 8 } : std::nullopt;
	}

	std::optional<uint8_t> fpgaRev() noexcept {
		return value() ? std::optional{ (data_.to_ulong() & 0xFF) >> 8 } : std::nullopt;
	}
};

class BOARD_CSR1_reg : public Register {
	const int FE1_PRESENT = 0;
	const int FE1_READY = 1;
	const int FE2_PRESENT = 2;
	const int FE2_READY = 3;
	const int AUX_PRESENT = 4;
public:
	BOARD_CSR1_reg(Device6991*);

	std::optional<bool> isAuxPresent() noexcept {
		return value() ? std::optional{ data_.test(AUX_PRESENT) } : std::nullopt;
	}
	std::optional<bool> isFec1Present() {
		return value() ? std::optional{ data_.test(FE1_PRESENT) } : std::nullopt;
	}
	std::optional<bool> isFec2Present() {
		return value() ? std::optional{ data_.test(FE2_PRESENT) } : std::nullopt;
	}
	std::optional<bool> isFec1Ready() {
		return value() ? std::optional{ data_.test(FE1_READY) } : std::nullopt;
	}
	std::optional<bool> isFec2Ready() {
		return value() ? std::optional{ data_.test(FE2_READY) } : std::nullopt;
	}

	std::optional<bool> isFecReady(FecIdType::Type const fecId) {
		return fecId == FecIdType::_1 ? isFec1Ready() : isFec2Ready();
	}
	std::optional<bool> isFecPresent(FecIdType::Type const fecId) {
		return fecId == FecIdType::_1 ? isFec1Present() : isFec2Present();
	}
};

class LED_CSR_reg : public Register {
	const int LEDS_EN = 31;
public:
	LED_CSR_reg(Device6991*);
};

class CL0_SPI_DATA_reg : public Register {
public:
	CL0_SPI_DATA_reg(Device6991*);
};

class CL1_SPI_DATA_reg : public Register {
public:
	CL1_SPI_DATA_reg(Device6991*);
};

class CL_SPI_CSR_reg : public Register {
	const int CL0_SEL = 24;
	const int CL1_SEL = 25;
	const int CL_TEST_LOOP = 26;
	const int CL_START_CL_BUSY = 31;

	enum Command {
		READ = 0x51,
		WRITE = 0x58,
		WRITE_LOOPBACK = 0xA1
	};

	bool isBusy() noexcept;
	void setCommand(Command const) noexcept;

public:
	CL_SPI_CSR_reg(Device6991*);
	bool startTests(bool const CL0, bool const CL1) noexcept;
	bool stopTests() noexcept;

	std::optional<bool> isTestRunning(TestTypeEnum::Type const type = TestTypeEnum::INVALID) noexcept;
};

class CL_SPI_TLCNT_reg : public Register {
public:
	CL_SPI_TLCNT_reg(Device6991*);
};

class CL0_SPI_TLERR_reg : public Register {
public:
	CL0_SPI_TLERR_reg(Device6991*);
};

class CL1_SPI_TLERR_reg : public Register {
public:
	CL1_SPI_TLERR_reg(Device6991*);
};

class DL_SPI_CSR1_reg : public Register {
	const int DL0_TEST_MODE = 24;
	const int DL1_TEST_MODE = 28;
	const int DL_EN = 31;
public:
	DL_SPI_CSR1_reg(Device6991*);
	bool enableTestMode(FecIdType::Type const fcId) noexcept;
	bool disableTestMode() noexcept;
	std::optional<bool> isTestRunning() noexcept;
	std::optional<std::pair<bool, bool>> runningTests() noexcept;
	std::optional<uint32_t> dlFsmStatus() noexcept {
		return value() ? std::optional{ static_cast<DlSclkFreqType::Type>(data_.to_ulong() & 0xF00) } : std::nullopt;
	}

	std::optional<uint32_t> dlFifoFsmStatus() noexcept {
		return value() ? std::optional{ static_cast<DlSclkFreqType::Type>(data_.to_ulong() & 0xF0) } : std::nullopt;
	}
};

class DL_SPI_CSR2_reg : public Register {
public:
	DL_SPI_CSR2_reg(Device6991*);

	bool setDl0FrameLength(DlFrameRateType::Type const frameType) noexcept {
		if (value()) {
			data_ &= 0xFFFFFFE0;
			data_ |= frameType;
			return writeHw();
		}
		return false;
	}

	bool setDl1FrameLength(DlFrameRateType::Type const frameType) noexcept {	
		if (value()) {
			data_ &= 0xFFE0FFFF;
			data_ |= frameType << 16;
			return writeHw();
		}
		return false;
	}

	bool setDlFrameLength(FecType::Type const cardType, FecIdType::Type const fcId) noexcept {
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

	std::optional<DlFrameRateType::Type> dl0FrameLength() noexcept {
		return value() ? std::optional{ static_cast<DlFrameRateType::Type>(data_.to_ulong() & 0x1F) } : std::nullopt;
	}

	std::optional<DlFrameRateType::Type> dl1FrameLength() noexcept {
		return value() ? std::optional{ static_cast<DlFrameRateType::Type>((data_.to_ulong() & 0x1F0000) >> 16) } : std::nullopt;
	}
};

class DL_SPI_CSR3_reg : public Register {

public:
	DL_SPI_CSR3_reg(Device6991*);
};

class DL0_DWORD_EN_reg : public Register {

public:
	DL0_DWORD_EN_reg(Device6991*);
};

class DL1_DWORD_EN_reg : public Register {

public:
	DL1_DWORD_EN_reg(Device6991*);
};

class DL0_SPI_TMCNT_reg : public Register {
public:
	DL0_SPI_TMCNT_reg(Device6991*);
};

class DL0_SPI_TMERR_reg : public Register {
public:
	DL0_SPI_TMERR_reg(Device6991*);
};

class DL1_SPI_TMCNT_reg : public Register {
public:
	DL1_SPI_TMCNT_reg(Device6991*);
};

class DL1_SPI_TMERR_reg : public Register {
public:
	DL1_SPI_TMERR_reg(Device6991*);
};

class SYS_TMR_CSR_reg : public Register {

public:
	SYS_TMR_CSR_reg(Device6991*);
};

class SYS_TMR_L_reg : public Register {

public:
	SYS_TMR_L_reg(Device6991*);
};

class SYS_TMR_H_reg : public Register {

public:
	SYS_TMR_H_reg(Device6991*);
};

class EVENT_LOG_CSR_reg : public Register {
	const int SW_EVENT_EN = 0;
	const int ACQST_EVENT_EN = 1;
	const int DLERR_EVENT_EN = 2;
	const int DFOERR_EVENT_EN = 3;
	const int DFUERR_EVENT_EN = 4;
	const int EVENT_LOG_FULL = 30;
	const int EVENT_LOG_EN = 31;
public:
	EVENT_LOG_CSR_reg(Device6991*);
};

class EVENT_STAMP_L_reg : public Register {

public:
	EVENT_STAMP_L_reg(Device6991*);
};

class EVENT_STAMP_H_reg : public Register {

public:
	EVENT_STAMP_H_reg(Device6991*);
};

class EVENT_CAUSE_reg : public Register {

public:
	EVENT_CAUSE_reg(Device6991*);
};

class DEBUG_CSR_reg : public Register {
	const int DEBUG_CLK_EN = 31;
public:
	DEBUG_CSR_reg(Device6991*);
	bool startClock() noexcept;
	bool stopClock() noexcept;
};

class DEBUG_CLK_RATE_reg : public Register {
	const double MAX_RATE_NANO_SECS = 35791392693.34424;
	const double RATE_STEP_NANO_SECS = 8.333333;
public:
	DEBUG_CLK_RATE_reg(Device6991*);
	bool setRate(double const rateInNano) noexcept;
	std::optional<double> rate() noexcept;
};

class ACQ_CSR_reg : public Register {
	const int ACQ_ON = 31;
	const int ACQ_STOPPED_ERR = 30;
	const int ACQ_STOP_ON_ERR_EN = 8;
public:
	ACQ_CSR_reg(Device6991*);

	std::optional<bool> isAcqActive() noexcept;
};

class ACQ_RATE_reg : public Register {

public:
	ACQ_RATE_reg(Device6991*);
};

class ACQ_ALARM_L_reg : public Register {

public:
	ACQ_ALARM_L_reg(Device6991*);
};

class ACQ_ALARM_H_reg : public Register {

public:
	ACQ_ALARM_H_reg(Device6991*);
};

class DFIFO_CSR_reg : public Register {
	const int DFIFO_EMPTY_FLAG = 31;
	const int DFIFO_FULL_FLAG = 30;
	const int DFIFO_PROG_FLAG = 29;
	const int DFIFO_OVF_FLAG = 28;
	const int DFIFO_UNF_FLAG = 27;
	const int DFIFO_TEST_MODE = 23;
public:
	DFIFO_CSR_reg(Device6991* deviceIF);
	bool startTests() noexcept;
	bool stopTests() noexcept;
	bool clearOverflow() noexcept;
	bool resetFifo() noexcept;
	bool setBlockSize(uint32_t const blockSize) noexcept;
	std::optional<int> blockSize() noexcept;
	std::optional<int> samplesInFifo() noexcept;
	std::optional<bool> isTestRunning() noexcept;
	std::optional<bool> isFifoEmpty() noexcept;
	std::optional<bool> isFifoFull() noexcept;
	std::optional<bool> isFifoProgFull() noexcept;
	std::optional<bool> overflowHappened() noexcept;
};

class DFIFO_PFLAG_THR_reg : public Register {
public:
	DFIFO_PFLAG_THR_reg(Device6991*);
	bool setThreshold(uint32_t const threshold) noexcept;
	std::optional<uint32_t> threshold() noexcept;
}; 

class DFIFO_WR_reg : public Register {

public:
	DFIFO_WR_reg(Device6991*);
};

class DFIFO : public Register {
public:
	DFIFO(Device6991*);
};

class RegisterFeCard : public Register {
protected:
	bool readHw(FecIdType::Type const fcId, uint32_t const mask = 0xFFFFFFFF) noexcept;
	bool writeHw(FecIdType::Type const fcId) const noexcept;
	bool readBoth(uint32_t const mask = 0xFFFFFFFF) noexcept;
	std::pair<std::bitset<32>, std::bitset<32>> dataBoth_;
public:
	RegisterFeCard(uint32_t const, Device6991*);
	std::pair<std::optional<uint32_t>, std::optional<uint32_t>> value(FecIdType::Type const fcId, uint32_t const mask = 0xFFFFFFFF) noexcept;
};

class FE_ID_reg : public RegisterFeCard {
public:
	FE_ID_reg(Device6991* devIF) : RegisterFeCard(FecRegistersEnum::FE_ID_reg, devIF) {}

	auto fecType(FecIdType::Type const fcId) noexcept {
		return value(fcId, 0xFFFF);
	}
};

class BOARD_CSR_reg : public RegisterFeCard {
public:
	BOARD_CSR_reg(Device6991* devIF) : RegisterFeCard(FecRegistersEnum::BOARD_CSR_reg, devIF) {}

	auto status6111(FecIdType::Type const fcId) {
		return value(fcId, 0x3F);
	}

	auto status6132(FecIdType::Type const fcId) {
		return value(fcId, 0x1F);
	}

	auto status(FecIdType::Type const fcId, FecType::Type const type) {
		return type == FecType::_6111 ? status6111(fcId) : status6132(fcId);
	}

	bool testStatus(FecIdType::Type const fcId, FecType::Type const type, uint32_t const expectedStatus) {
		auto sts = status(fcId, type);
		switch (fcId) {
		case FecIdType::BOTH:
			return sts.first && sts.second ? *sts.first == expectedStatus && *sts.second == expectedStatus : false;
		case FecIdType::_1:
			return sts.first ? *sts.first == expectedStatus : false;
		case FecIdType::_2:
			return sts.second ? *sts.second == expectedStatus : false;
		default:
			return false;
		}
	}
};

class CMD_reg : public RegisterFeCard {
public:
	CMD_reg(Device6991* devIF) : RegisterFeCard(FecRegistersEnum::CMD_reg, devIF) {}

	bool setCmd(FecIdType::Type const fcId, uint32_t const cmd) noexcept {
		auto read = value(fcId);
		if (read.first || read.second) {
			data_ &= 0xFFFF0000;
			data_ |= cmd;
			return writeHw(fcId);
		}
		return false;
	}
};

class DL_CSR_reg : public RegisterFeCard {
public:
	DL_CSR_reg(Device6991* devIF) : RegisterFeCard(FecRegistersEnum::DL_CSR_reg, devIF) {}

	bool setDlSclkFreqType(FecIdType::Type const fcId, DlSclkFreqType::Type const freq) noexcept {
		auto read = value(fcId);
		if (read.first || read.second) {
			data_ &= 0xFFFFFFF0;
			data_ |= freq;
			return writeHw(fcId);
		}
		return false;
	}

	auto dlSclkFreqType(FecIdType::Type const fcId) noexcept {
		return value(fcId, 0xF);
	}
};
