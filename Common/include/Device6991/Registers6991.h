#pragma once
#include <bitset>
#include <optional>
#include "../../include/Device6991/Defines6991.h"

class Device6991;
class Register {
	int address_;
	Device6991* deviceIF;
protected:
	std::bitset<32> data_;
	bool readHw() noexcept;
	bool writeHw() const noexcept;
public:
	Register(int const, Device6991*);
	std::optional<uint32_t> value() noexcept;
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

class ACQ_CSR_reg : public Register {
	const int ACQ_ON = 31;
public:
	ACQ_CSR_reg(Device6991*);

	std::optional<bool> isAcqActive() noexcept;
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

class DFIFO_PFLAG_THR_reg : public Register {
public:
	DFIFO_PFLAG_THR_reg(Device6991*);
	bool setThreshold(uint32_t const threshold) noexcept;
	std::optional<uint32_t> threshold() noexcept;
}; 

class DFIFO : public Register {
public:
	DFIFO(Device6991*);
};