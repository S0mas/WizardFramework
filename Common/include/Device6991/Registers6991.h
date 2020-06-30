#pragma once
#include <optional>
#include <functional>
#include "../../include/Device6991/Defines6991.h"

class Device6991;
class Register {
protected:
	uint32_t address_;
	uint32_t data_;
	std::function<std::optional<uint32_t>(uint32_t)> readRegisterFunction_;
	std::function<bool(uint32_t, uint32_t)> writeRegisterFunction_;
	std::function<void(QString const&)> reportErrorFunction_;
	std::function<bool(uint32_t, uint32_t)> readHwFunction_ = [this](uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftRight = 0) {
		if (auto reg = readRegisterFunction_(address_); reg) {
			data_ = (*reg & mask) >> shiftRight;
			return true;
		}
		reportErrorFunction_(QString("Faild to read register. Reg address: %1").arg(toHex(address_, 8)));
		return false;
	};

	std::function<bool()> writeHwFunctionNoArgs_ = [this]() {
		auto result = writeRegisterFunction_(address_, data_);
		if (!result)
			reportErrorFunction_(QString("Faild to write register. Reg address: %1").arg(toHex(address_, 8)));
		return result;
	};

	std::function<bool(uint32_t, uint32_t, uint32_t)> writeHwFunction_ = [this](uint32_t const data, uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftLeft = 0) {
		if (mask != 0xFFFFFFFF) {
			if (!readHw())
				return false;
		}
		data_ &= ~mask;
		data_ |= data << shiftLeft;
		return writeHwFunctionNoArgs_();
	};

	std::function<bool(uint32_t, uint32_t, uint32_t, bool)> testRegFunction_ = [this](uint32_t const expected, uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftRight = 0, bool const verbose = true) {
		if (auto reg = value(mask, shiftRight); reg) {
			if (*reg == expected)
				return true;
			else if (verbose)
				reportErrorFunction_(QString("Value different than expected. Address: %1 Expected: %2, Read:%3, Mask:%4, ShiftedRight:%5")
					.arg(toHex(address_, 8), toHex(expected, 8), toHex(*reg, 8), toHex(mask, 8), QString::number(shiftRight)));
		}
		return false;
	};

	bool readHw(uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftRight = 0) noexcept;
	bool writeHw(uint32_t const data, uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftLeft = 0) noexcept;
	bool writeHw() noexcept;
public:
	Register(uint32_t const);
	template<typename T = uint32_t>
	std::optional<T> value(uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftRight = 0) noexcept {
		return readHw(mask, shiftRight) ? std::optional{ static_cast<T>(data_) } : std::nullopt;
	}
	std::optional<bool> testBit(int const bit) noexcept;
	bool setBit(int const bit, bool const state) noexcept;
	bool testReg(uint32_t const expected, uint32_t const mask = 0xFFFFFFFF, uint32_t const shiftRight = 0, bool const verbose = true) noexcept;
};

class Register6991 : public Register {
public:
	Register6991(uint32_t const address, Device6991* devIF_);
};

class RegisterFec: public Register {
	FecIdType::Type id_;
	uint32_t dataSecond_;
	Device6991* devIF_;
public:
	RegisterFec(FecIdType::Type const id, uint32_t const address, Device6991* devIF);
};

class FC_WR_QUEUE_EMP : public Register6991 {
public:
	FC_WR_QUEUE_EMP(Device6991*);
};

class PCI_ERR_STCTRL : public Register6991 {
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

class PREF_ERR_ADDR : public Register6991 {
public:
	PREF_ERR_ADDR(Device6991*);
};

class BOARD_ID_reg : public Register6991 {
public:
	BOARD_ID_reg(Device6991*);
	std::optional<uint16_t> id() noexcept;
};

class HW_REV_reg : public Register6991 {
public:
	HW_REV_reg(Device6991*);
	std::optional<uint8_t> pcbRev() noexcept;
	std::optional<uint8_t> fpgaBranch() noexcept;
	std::optional<uint8_t> fpgaBranchRev() noexcept;
	std::optional<uint8_t> fpgaRev() noexcept;
};

class BOARD_CSR1_reg : public Register6991 {
	const int FE1_PRESENT = 0;
	const int FE1_READY = 1;
	const int FE2_PRESENT = 2;
	const int FE2_READY = 3;
	const int AUX_PRESENT = 4;
public:
	BOARD_CSR1_reg(Device6991*);

	std::optional<bool> isAuxPresent() noexcept;
	std::optional<bool> isFec1Present();
	std::optional<bool> isFec2Present();
	std::optional<bool> isFec1Ready();
	std::optional<bool> isFec2Ready();
	std::optional<bool> isFecReady(FecIdType::Type const fecId);
	std::optional<bool> isFecPresent(FecIdType::Type const fecId);
};

class LED_CSR_reg : public Register6991 {
	const int LEDS_EN = 31;
public:
	LED_CSR_reg(Device6991*);
};

class CL0_SPI_DATA_reg : public Register6991 {
public:
	CL0_SPI_DATA_reg(Device6991*);
};

class CL1_SPI_DATA_reg : public Register6991 {
public:
	CL1_SPI_DATA_reg(Device6991*);
};

class CL_SPI_CSR_reg : public Register6991 {
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

class CL_SPI_TLCNT_reg : public Register6991 {
public:
	CL_SPI_TLCNT_reg(Device6991*);
};

class CL0_SPI_TLERR_reg : public Register6991 {
public:
	CL0_SPI_TLERR_reg(Device6991*);
};

class CL1_SPI_TLERR_reg : public Register6991 {
public:
	CL1_SPI_TLERR_reg(Device6991*);
};

class DL_SPI_CSR1_reg : public Register6991 {
	const int DL0_TEST_MODE = 24;
	const int DL1_TEST_MODE = 28;
	const int DL_EN = 31;
public:
	DL_SPI_CSR1_reg(Device6991*);
	bool enableTestMode(FecIdType::Type const fcId) noexcept;
	bool disableTestMode() noexcept;
	std::optional<bool> isTestRunning(TestTypeEnum::Type const type = TestTypeEnum::INVALID) noexcept;
	std::optional<uint32_t> dlFsmStatus() noexcept;
	std::optional<uint32_t> dlFifoFsmStatus() noexcept;
	bool isDlReciverIdle(bool const verbose = true) noexcept;
};

class DL_SPI_CSR2_reg : public Register6991 {
public:
	DL_SPI_CSR2_reg(Device6991*);
	bool setDl0FrameLength(DlFrameRateType::Type const frameType) noexcept;
	bool setDl1FrameLength(DlFrameRateType::Type const frameType) noexcept;
	bool setDlFrameLength(FecType::Type const cardType, FecIdType::Type const fcId) noexcept;
	std::optional<uint32_t> dl0FrameLength() noexcept;
	std::optional<uint32_t> dl1FrameLength() noexcept;
};

class DL_SPI_CSR3_reg : public Register6991 {
public:
	DL_SPI_CSR3_reg(Device6991*);
};

class DL0_DWORD_EN_reg : public Register6991 {
public:
	DL0_DWORD_EN_reg(Device6991*);
};

class DL1_DWORD_EN_reg : public Register6991 {
public:
	DL1_DWORD_EN_reg(Device6991*);
};

class DL0_SPI_TMCNT_reg : public Register6991 {
public:
	DL0_SPI_TMCNT_reg(Device6991*);
};

class DL0_SPI_TMERR_reg : public Register6991 {
public:
	DL0_SPI_TMERR_reg(Device6991*);
};

class DL1_SPI_TMCNT_reg : public Register6991 {
public:
	DL1_SPI_TMCNT_reg(Device6991*);
};

class DL1_SPI_TMERR_reg : public Register6991 {
public:
	DL1_SPI_TMERR_reg(Device6991*);
};

class SYS_TMR_CSR_reg : public Register6991 {
public:
	SYS_TMR_CSR_reg(Device6991*);
};

class SYS_TMR_L_reg : public Register6991 {
public:
	SYS_TMR_L_reg(Device6991*);
};

class SYS_TMR_H_reg : public Register6991 {
public:
	SYS_TMR_H_reg(Device6991*);
};

class EVENT_LOG_CSR_reg : public Register6991 {
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

class EVENT_STAMP_L_reg : public Register6991 {
public:
	EVENT_STAMP_L_reg(Device6991*);
};

class EVENT_STAMP_H_reg : public Register6991 {
public:
	EVENT_STAMP_H_reg(Device6991*);
};

class EVENT_CAUSE_reg : public Register6991 {
public:
	EVENT_CAUSE_reg(Device6991*);
};

class DEBUG_CSR_reg : public Register6991 {
	const int DEBUG_CLK_EN = 31;
public:
	DEBUG_CSR_reg(Device6991*);
	bool startClock() noexcept;
	bool stopClock() noexcept;
};

class DEBUG_CLK_RATE_reg : public Register6991 {
	const double MAX_RATE_NANO_SECS = 35791392693.344235;
	const double RATE_STEP_NANO_SECS = 8.333333;
public:
	DEBUG_CLK_RATE_reg(Device6991*);
	bool setRate(double const rateInNano) noexcept;
	std::optional<double> rate() noexcept;
};

class ACQ_CSR_reg : public Register6991 {
	const int ACQ_ON = 31;
	const int ACQ_STOPPED_ERR = 30;
	const int ACQ_STOP_ON_ERR_EN = 8;
public:
	ACQ_CSR_reg(Device6991*);
	std::optional<bool> isAcqActive() noexcept;
};

class ACQ_RATE_reg : public Register6991 {
public:
	ACQ_RATE_reg(Device6991*);
};

class ACQ_ALARM_L_reg : public Register6991 {
public:
	ACQ_ALARM_L_reg(Device6991*);
};

class ACQ_ALARM_H_reg : public Register6991 {
public:
	ACQ_ALARM_H_reg(Device6991*);
};

class DFIFO_CSR_reg : public Register6991 {
	const int DFIFO_EMPTY_FLAG = 31;
	const int DFIFO_FULL_FLAG = 30;
	const int DFIFO_PROG_FLAG = 29;
	const int DFIFO_OVF_FLAG = 28;
	const int DFIFO_UNF_FLAG = 27;
	const int DFIFO_TEST_MODE = 23;
public:
	DFIFO_CSR_reg(Device6991* deviceIF);
	bool enableTestMode() noexcept;
	bool disableTestMode() noexcept;
	bool clearOverflow() noexcept;
	bool resetFifo() noexcept;
	bool setBlockSize(uint32_t const blockSize) noexcept;
	std::optional<uint32_t> blockSize() noexcept;
	std::optional<uint32_t> samplesInFifo() noexcept;
	std::optional<bool> isTestRunning() noexcept;
	std::optional<bool> isFifoEmpty() noexcept;
	std::optional<bool> isFifoFull() noexcept;
	std::optional<bool> isFifoProgFull() noexcept;
	std::optional<bool> overflowHappened() noexcept;
};

class DFIFO_PFLAG_THR_reg : public Register6991 {
public:
	DFIFO_PFLAG_THR_reg(Device6991*);
	bool setThreshold(uint32_t const threshold) noexcept;
	std::optional<uint32_t> threshold() noexcept;
}; 

class DFIFO_WR_reg : public Register6991 {
public:
	DFIFO_WR_reg(Device6991*);
};

class DFIFO : public Register6991 {
public:
	DFIFO(Device6991*);
};

class FE_ID_reg : public RegisterFec {
public:
	FE_ID_reg(FecIdType::Type const fcId, Device6991* devIF) : RegisterFec(fcId, FecRegistersEnum::FE_ID_reg, devIF) {}
	std::optional<FecType::Type> fecType() noexcept;
};

class BOARD_CSR_reg : public RegisterFec {
	const int MASK_STATUS_6111 = 0x3F;
	const int MASK_STATUS_6132 = 0x1F;
public:
	BOARD_CSR_reg(FecIdType::Type const fcId, Device6991* devIF) : RegisterFec(fcId, FecRegistersEnum::BOARD_CSR_reg, devIF) {}
	bool testStatus(FecType::Type const type, uint32_t const expectedStatus, bool const verbose = true) noexcept;
	bool isIdle(FecType::Type const type, bool const verbose = true) noexcept;
};

class CMD_reg : public RegisterFec {
public:
	CMD_reg(FecIdType::Type const fcId, Device6991* devIF) : RegisterFec(fcId, FecRegistersEnum::CMD_reg, devIF) {}
	bool setCmd(uint32_t const cmd) noexcept;
};

class DL_CSR_reg : public RegisterFec {
public:
	DL_CSR_reg(FecIdType::Type const fcId, Device6991* devIF) : RegisterFec(fcId, FecRegistersEnum::DL_CSR_reg, devIF) {}
	bool setDlSclkFreqType(DlSclkFreqType::Type const freq) noexcept;
	std::optional<DlSclkFreqType::Type> dlSclkFreqType() noexcept;
};

class CHN_FILTER_reg : public RegisterFec {
	static const uint32_t BITS_PER_CHANNEL = 2;
	static const uint32_t MASK_PER_CHANNEL = 0x3;
public:
	CHN_FILTER_reg(FecIdType::Type const fcId, Device6991* devIF) : RegisterFec(fcId, FecRegistersEnum::CHN_FILTER_reg, devIF) {}
	bool setFilters(std::vector<uint32_t> const& channelIds, FilterType6132::Type const filter) noexcept;
	std::optional<FilterType6132::Type> filter(uint32_t const channelId) noexcept;
	std::optional<std::vector<FilterType6132::Type>> filters() noexcept;
};

class CHN_GAIN_reg : public RegisterFec {
	static const uint32_t BITS_PER_CHANNEL = 2;
	static const uint32_t MASK_PER_CHANNEL = 0x3;
public:
	CHN_GAIN_reg(FecIdType::Type const fcId, Device6991* devIF) : RegisterFec(fcId, FecRegistersEnum::CHN_GAIN_reg, devIF) {}
	bool setGains(std::vector<uint32_t> const& channelIds, GainType6132::Type const gain) noexcept;
	std::optional<GainType6132::Type> gain(uint32_t const channelId) noexcept;
	std::optional<std::vector<GainType6132::Type>> gains() noexcept;
};

class ADC_SEL_reg : public RegisterFec {
public:
	ADC_SEL_reg(FecIdType::Type const fcId, Device6991* devIF) : RegisterFec(fcId, FecRegistersEnum::ADC_SEL_reg, devIF) {}
	bool selectChannels(std::vector<uint32_t> const& channelIds) noexcept;
};

class CHNX_RAW_SAMPLE_reg : public RegisterFec {
public:
	CHNX_RAW_SAMPLE_reg(FecIdType::Type const fcId, uint32_t const channelId = 1, Device6991* devIF = nullptr) : RegisterFec(fcId, FecRegistersEnum::CHN1_RAW_SAMPLE_reg + channelId - 1, devIF) {}
	std::optional<uint16_t> rawData() noexcept;
	std::optional<uint16_t> flags() noexcept;
};
