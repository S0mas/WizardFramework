#pragma once
#include <bitset>
#include <optional>

class Device6991;
class Register {
	int address_;
	Device6991* deviceIF;
protected:
	std::bitset<32> data_;
	bool readHw() noexcept;
	bool writeHw() const noexcept;
public:
	Register(int const address, Device6991* deviceIF);
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
	void setCommand(Command const cmd) noexcept;
public:
	enum TargetSelection {
		FE1,
		FE2,
		BOTH
	};

	CL_SPI_CSR_reg(Device6991* deviceIF);
	bool startTests(TargetSelection const target) noexcept;
	bool stopTests() noexcept;

	std::optional<bool> isTestRunning() noexcept;
};

class DL_SPI_CSR1_reg : public Register {
	const int DL0_TEST_MODE = 24;
	const int DL1_TEST_MODE = 28;
	const int DL_EN = 31;
public:
	enum TargetSelection {
		FE1,
		FE2,
		BOTH
	};

	DL_SPI_CSR1_reg(Device6991* deviceIF);
	bool startTests(TargetSelection const target) noexcept;
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

/*DFIFO_CSR_reg

 – 
Bit
Access & Default
Description
31
RO
0x0
DFIFO_EMPTY_FLAG
The DFIFO_EMPTY_FLAG indicates that Data FIFO is empty.
Read:
0: Data FIFO not empty
1: Data FIFO empty 
30
RO
0x0
DFIFO_FULL_FLAG
The DFIFO_FULL_FLAG indicates that Data FIFO is full.
Read:
0: Data FIFO not full
1: Data FIFO full
29
RO
0x0
DFIFO_PROG_FLAG
The DFIFO_PROG_FLAG indicates that Data FIFO reached programmed threshold: the amount of data in FIFO is not less that the number setup in .
Read:
0: Data FIFO not almost empty
1: Data FIFO almost full
28
RW1C
0x0
DFIFO_OVF_FLAG
The DFIFO_OVF_FLAG indicates that the overflow when writing to Data FIFO happened (writing to full FIFO).
This bit is cleared during reset of the FIFO or can be cleared by writing ‘1’ to it.
Write:
0: no effect
1: clears the flag if flag was asserted and no overflow condition at the same time
Read:
0: No overflow
1: Data FIFO overflow happened
27
RW1C
0x0
DFIFO_UNF_FLAG
The DFIFO_UNF_FLAG indicates that the underflow when reading from Data FIFO happened (reading from empty FIFO).
This bit is cleared during reset of the FIFO or can be cleared by writing ‘1’ to it.
Write:
0: no effect
1: clears the flag if flag was asserted and no overflow condition at the same time
Read:
0: No underflow
1: Data FIFO underflow happened 
26:24
WO


DFIFO_RST
These bits are used to reset (initialize) the Data FIFO.
Write:
Writing 0x4 causes data FIFO reset 
Other : no effect
23
RW
0x0
DFIFO_TEST_MODE
Test Mode allows to write to Data FIFO linear pattern in automated mode. The rate of writing every data to FIFO is defined by TBD. Linear pattern starts from 0 and is 32-bit wide: every next data written is incremented by one.
Test Mode can be started only if Data Link is not enabled with DL_EN bit.
Write:
0: Test Mode disabled
1: Test Mode enabled 
Read:
Returns previously written value 
15:13
RO
0x0
Reserved
12:0
RO
0x0
DFIFO_CNT[11:0]
DFIFO_CNT bits gives current number of the data stored in Data FIFO.
Read:
Returns current number of data stored in the Data FIFO  
*/