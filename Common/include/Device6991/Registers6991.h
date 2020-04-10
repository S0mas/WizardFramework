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
	std::optional<int> value() noexcept;
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
	const int DFIFO_TEST_MODE = 23;
public:
	DFIFO_CSR_reg(Device6991* deviceIF);
	bool startTests() noexcept;
	bool stopTests() noexcept;
	std::optional<bool> isTestRunning() noexcept;
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

