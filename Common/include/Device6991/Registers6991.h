#pragma once
#include <bitset>
#include <optional>
#include "../../include/Device6991/Defines6991.h"

class Device6991;
class Register {
	uint32_t address_;
	Device6991* deviceIF;
protected:
	std::bitset<32> data_;
	bool readHw() noexcept;
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
	std::optional<bool> isFe1Present() {
		return value() ? std::optional{ data_.test(FE1_PRESENT) } : std::nullopt;
	}
	std::optional<bool> isFe2Present() {
		return value() ? std::optional{ data_.test(FE2_PRESENT) } : std::nullopt;
	}
	std::optional<bool> isFe1Ready() {
		return value() ? std::optional{ data_.test(FE1_READY) } : std::nullopt;
	}
	std::optional<bool> isFe2Ready() {
		return value() ? std::optional{ data_.test(FE2_READY) } : std::nullopt;
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
	bool startTests(bool const DL0, bool const DL1) noexcept;
	bool stopTests() noexcept;
	std::optional<bool> isTestRunning() noexcept;
};

class DL_SPI_CSR2_reg : public Register {

public:
	DL_SPI_CSR2_reg(Device6991*);
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