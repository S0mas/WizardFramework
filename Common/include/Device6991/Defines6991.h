#pragma once
#include "../ScpiIF.h"
#include <array>
#include <bitset>
#include <optional>
#include <QDebug>
#include <QDateTime>
#include <QTime>

inline QString toHex(uint32_t const value, uint32_t const width = 2) noexcept {
	return QString("0x%1").arg(value, width, 16, QLatin1Char('0'));
}

struct ControlModeEnum {
	enum Type {
		LISTENER,
		CONTROLLER,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 2> TYPES = { LISTENER, CONTROLLER };

	inline static QString toString(Type const mode) noexcept {
		switch (mode) {
		case LISTENER:
			return "Listener";
		case CONTROLLER:
			return "Controller";
		default:
			return "invalid";
		}
	}

	inline static Type fromString(QString const& mode) noexcept {
		if (mode == "Listener")
			return LISTENER;
		else if (mode == "Controller")
			return CONTROLLER;
		else
			return INVALID;
	}
};

struct AcquisitionStopModeEnum {
	enum Type {
		MANUAL,
		SCANS_TRESHOLD,
		TIME,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 3> TYPES = { MANUAL, SCANS_TRESHOLD, TIME };

	inline static QString toString(Type const mode) noexcept {
		switch (mode) {
		case MANUAL:
			return "Manual";
		case SCANS_TRESHOLD:
			return "Scans treshold";
		case TIME:
			return "Time";
		default:
			return "invalid";
		}
	}

	inline static Type fromString(QString const& mode) noexcept {
		if (mode == "Manual")
			return MANUAL;
		else if (mode == "Scans treshold")
			return SCANS_TRESHOLD;
		else if (mode == "Time")
			return TIME;
		return INVALID;
	}
};

struct AcquisitionStartModeEnum {
	enum Type {
		IMMEDIATE,
		PTP_ALARM,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 2> TYPES = { IMMEDIATE, PTP_ALARM };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case IMMEDIATE:
			return "Immediate";
		case PTP_ALARM:
			return "PTP Alarm";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "Immediate")
			return IMMEDIATE;
		else if (mode == "PTP Alarm")
			return PTP_ALARM;
		return INVALID;
	}
};

struct ScanRateUnitsEnum {
	enum Type {
		HZ,
		US,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 2> TYPES = { HZ, US };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case HZ:
			return "Hz";
		case US:
			return "us";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "Hz")
			return HZ;
		else if (mode == "us")
			return US;
		return INVALID;
	}
};

struct ScanRateModel {
	uint32_t value_;
	ScanRateUnitsEnum::Type units_;
	QString toString() const noexcept {
		return QString("%1,%2").arg(value_).arg(ScanRateUnitsEnum::toString(units_));
	}
	static ScanRateModel fromString(QString const& str) noexcept {
		auto list = str.split(',');
		return { list[0].toUInt(), ScanRateUnitsEnum::fromString(list[1])};
	}
};

struct PtpTime {
	int64_t seconds_;
	int64_t nanoseconds_;
	QString toString() const noexcept {
		return QString("%1,%2").arg(seconds_).arg(nanoseconds_);
	}

	static PtpTime fromString(QString const& str) noexcept {
		auto list = str.split(',');
		return { list[0].toUInt(), list[1].toUInt() };
	}
};

struct TemperaturesEnum {
	enum Type {
		CPU,
		POWER,
		DOWN,
		UP,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 4> TYPES = { CPU, POWER, DOWN, UP };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case CPU:
			return "CPU";
		case POWER:
			return "POWER";
		case DOWN:
			return "DOWN";
		case UP:
			return "UP";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "CPU")
			return CPU;
		else if (mode == "POWER")
			return POWER;
		else if (mode == "DOWN")
			return DOWN;
		else if (mode == "UP")
			return UP;
		return INVALID;
	}
};

struct Commands1Enum {
	Commands1Enum() = default;
	enum Type {
		READ = 0x51,
		WRITE = 0x52,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 2> TYPES = { READ, WRITE};

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case READ:
			return "READ";
		case WRITE:
			return "WRITE";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "READ")
			return READ;
		else if (mode == "WRITE")
			return WRITE;
		return INVALID;
	}
};

struct Commands2Enum {
	Commands2Enum() = default;
	enum Type {
		READ,
		WRITE,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 2> TYPES = { READ, WRITE };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case READ:
			return "READ";
		case WRITE:
			return "WRITE";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "READ")
			return READ;
		else if (mode == "WRITE")
			return WRITE;
		return INVALID;
	}
};

struct RegistersEnum {
	RegistersEnum() = default;
	enum Type {
		FC_WR_QUEUE_EMP = 0x0010,
		PCI_ERR_STCTRL = 0x0014,
		PREF_ERR_ADDR = 0x0018,
		BOARD_ID_reg = 0x4400,
		HW_REV_reg = 0x4404,
		BOARD_CSR1_reg = 0x4408,
		LED_CSR_reg = 0x4418,
		CL0_SPI_DATA_reg = 0x4480,
		CL1_SPI_DATA_reg = 0x4484,
		CL_SPI_CSR_reg = 0x4488,
		CL_SPI_TLCNT_reg = 0x44A0,
		CL0_SPI_TLERR_reg = 0x44A4,
		CL1_SPI_TLERR_reg = 0x44A8,
		DL_SPI_CSR1_reg = 0x4500,
		DL_SPI_CSR2_reg = 0x4504,
		DL_SPI_CSR3_reg = 0x4508,
		DL0_DWORD_EN_reg = 0x4510,
		DL1_DWORD_EN_reg = 0x4518,
		DL0_SPI_TMCNT_reg = 0x4520,
		DL0_SPI_TMERR_reg = 0x4524,
		DL1_SPI_TMCNT_reg = 0x4528,
		DL1_SPI_TMERR_reg = 0x452C,
		SYS_TMR_CSR_reg = 0x4580,
		SYS_TMR_L_reg = 0x4584,
		SYS_TMR_H_reg = 0x4586,
		EVENT_LOG_CSR_reg = 0x4600,
		EVENT_STAMP_L_reg = 0x4608,
		EVENT_STAMP_H_reg = 0x460C,
		EVENT_CAUSE_reg = 0x4610,
		DEBUG_CSR_reg = 0x4620,
		DEBUG_CLK_RATE_reg = 0x4624,
		ACQ_CSR_reg = 0x4680,
		ACQ_RATE_reg = 0x4684,
		ACQ_ALARM_L_reg = 0x4688,
		ACQ_ALARM_H_reg = 0x468C,
		DFIFO_CSR_reg = 0x4700,
		DFIFO_PFLAG_THR_reg = 0x4704,
		DFIFO_WR_reg = 0x4708,
		DFIFO = 0x10000,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 39> TYPES = { FC_WR_QUEUE_EMP, PCI_ERR_STCTRL, PREF_ERR_ADDR, BOARD_ID_reg, HW_REV_reg, BOARD_CSR1_reg, LED_CSR_reg,
		CL_SPI_CSR_reg, CL_SPI_TLCNT_reg, CL0_SPI_TLERR_reg, CL1_SPI_TLERR_reg, DL_SPI_CSR1_reg, DL0_SPI_TMCNT_reg, DL0_SPI_TMERR_reg, DL1_SPI_TMCNT_reg, DL1_SPI_TMERR_reg,
		SYS_TMR_CSR_reg, SYS_TMR_L_reg, SYS_TMR_H_reg, EVENT_LOG_CSR_reg, EVENT_STAMP_L_reg, EVENT_STAMP_H_reg, EVENT_CAUSE_reg, DEBUG_CSR_reg, DEBUG_CLK_RATE_reg,
		ACQ_CSR_reg, ACQ_RATE_reg, ACQ_ALARM_L_reg, ACQ_ALARM_H_reg,  DFIFO_CSR_reg, DFIFO_PFLAG_THR_reg, DFIFO_WR_reg, DFIFO };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case FC_WR_QUEUE_EMP:
			return "FC_WR_QUEUE_EMP";
		case PCI_ERR_STCTRL:
			return "PCI_ERR_STCTRL";
		case PREF_ERR_ADDR:
			return "PREF_ERR_ADDR";
		case BOARD_ID_reg:
			return "BOARD_ID_reg";
		case HW_REV_reg:
			return "HW_REV_reg";
		case BOARD_CSR1_reg:
			return "BOARD_CSR1_reg";
		case LED_CSR_reg:
			return "LED_CSR_reg";
		case CL0_SPI_DATA_reg:
			return "CL0_SPI_DATA_reg";
		case CL1_SPI_DATA_reg:
			return "CL1_SPI_DATA_reg";
		case CL_SPI_CSR_reg:
			return "CL_SPI_CSR_reg";
		case CL_SPI_TLCNT_reg:
			return "CL_SPI_TLCNT_reg";
		case CL0_SPI_TLERR_reg:
			return "CL0_SPI_TLERR_reg";
		case CL1_SPI_TLERR_reg:
			return "CL1_SPI_TLERR_reg";
		case DL_SPI_CSR1_reg:
			return "DL_SPI_CSR1_reg";
		case DL_SPI_CSR2_reg:
			return "DL_SPI_CSR2_reg";
		case DL_SPI_CSR3_reg:
			return "DL_SPI_CSR3_reg";
		case DL0_DWORD_EN_reg:
			return "DL0_DWORD_EN_reg";
		case DL1_DWORD_EN_reg:
			return "DL1_DWORD_EN_reg";
		case DL0_SPI_TMCNT_reg:
			return "DL0_SPI_TMCNT_reg";
		case DL0_SPI_TMERR_reg:
			return "DL0_SPI_TMERR_reg";
		case DL1_SPI_TMCNT_reg:
			return "DL1_SPI_TMCNT_reg";
		case DL1_SPI_TMERR_reg:
			return "DL1_SPI_TMERR_reg";
		case SYS_TMR_CSR_reg:
			return "SYS_TMR_CSR_reg";
		case SYS_TMR_L_reg:
			return "SYS_TMR_L_reg";
		case SYS_TMR_H_reg:
			return "SYS_TMR_H_reg";
		case EVENT_LOG_CSR_reg:
			return "EVENT_LOG_CSR_reg";
		case EVENT_STAMP_L_reg:
			return "EVENT_STAMP_L_reg";
		case EVENT_STAMP_H_reg:
			return "EVENT_STAMP_H_reg";
		case EVENT_CAUSE_reg:
			return "EVENT_CAUSE_reg";
		case DEBUG_CSR_reg:
			return "DEBUG_CSR_reg";
		case DEBUG_CLK_RATE_reg:
			return "DEBUG_CLK_RATE_reg";
		case DFIFO_CSR_reg:
			return "DFIFO_CSR_reg";
		case ACQ_CSR_reg:
			return "ACQ_CSR_reg";
		case ACQ_RATE_reg:
			return "ACQ_RATE_reg";
		case ACQ_ALARM_L_reg:
			return "ACQ_ALARM_L_reg";
		case ACQ_ALARM_H_reg:
			return "ACQ_ALARM_H_reg";
		case DFIFO_PFLAG_THR_reg:
			return "DFIFO_PFLAG_THR_reg";
		case DFIFO_WR_reg:
			return "DFIFO_WR_reg";
		case DFIFO:
			return "DFIFO";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "FC_WR_QUEUE_EMP")
			return FC_WR_QUEUE_EMP;
		if (mode == "PCI_ERR_STCTRL")
			return PCI_ERR_STCTRL;
		if (mode == "PREF_ERR_ADDR")
			return PREF_ERR_ADDR;
		if (mode == "BOARD_ID_reg")
			return BOARD_ID_reg;
		if (mode == "HW_REV_reg")
			return HW_REV_reg;
		if (mode == "BOARD_CSR1_reg")
			return BOARD_CSR1_reg;
		if (mode == "LED_CSR_reg")
			return LED_CSR_reg;
		if (mode == "CL0_SPI_DATA_reg")
			return CL0_SPI_DATA_reg;
		if (mode == "CL1_SPI_DATA_reg")
			return CL1_SPI_DATA_reg;
		if (mode == "CL_SPI_CSR_reg")
			return CL_SPI_CSR_reg;
		if (mode == "CL_SPI_TLCNT_reg")
			return CL_SPI_TLCNT_reg;
		if (mode == "CL0_SPI_TLERR_reg")
			return CL0_SPI_TLERR_reg;
		if (mode == "CL1_SPI_TLERR_reg")
			return CL1_SPI_TLERR_reg;
		if (mode == "DL_SPI_CSR1_reg")
			return DL_SPI_CSR1_reg;
		if (mode == "DL_SPI_CSR2_reg")
			return DL_SPI_CSR2_reg;
		if (mode == "DL_SPI_CSR3_reg")
			return DL_SPI_CSR3_reg;
		if (mode == "DL0_DWORD_EN_reg")
			return DL0_DWORD_EN_reg;
		if (mode == "DL1_DWORD_EN_reg")
			return DL1_DWORD_EN_reg;
		if (mode == "DL0_SPI_TMCNT_reg")
			return DL0_SPI_TMCNT_reg;
		if (mode == "DL0_SPI_TMERR_reg")
			return DL0_SPI_TMERR_reg;
		if (mode == "DL1_SPI_TMCNT_reg")
			return DL1_SPI_TMCNT_reg;
		if (mode == "DL1_SPI_TMERR_reg")
			return DL1_SPI_TMERR_reg;
		if (mode == "SYS_TMR_CSR_reg")
			return SYS_TMR_CSR_reg;
		if (mode == "SYS_TMR_L_reg")
			return SYS_TMR_L_reg;
		if (mode == "SYS_TMR_H_reg")
			return SYS_TMR_H_reg;
		if (mode == "EVENT_LOG_CSR_reg")
			return EVENT_LOG_CSR_reg;
		if (mode == "EVENT_STAMP_L_reg")
			return EVENT_STAMP_L_reg;
		if (mode == "EVENT_STAMP_H_reg")
			return EVENT_STAMP_H_reg;
		if (mode == "EVENT_CAUSE_reg")
			return EVENT_CAUSE_reg;
		if (mode == "DEBUG_CSR_reg")
			return DEBUG_CSR_reg;
		if (mode == "DEBUG_CLK_RATE_reg")
			return DEBUG_CLK_RATE_reg;
		if (mode == "DFIFO_CSR_reg")
			return DFIFO_CSR_reg;
		if (mode == "ACQ_CSR_reg")
			return ACQ_CSR_reg;
		if (mode == "ACQ_RATE_reg")
			return ACQ_RATE_reg;
		if (mode == "ACQ_ALARM_L_reg")
			return ACQ_ALARM_L_reg;
		if (mode == "ACQ_ALARM_H_reg")
			return ACQ_ALARM_H_reg;
		if (mode == "DFIFO_PFLAG_THR_reg")
			return DFIFO_PFLAG_THR_reg;
		if (mode == "DFIFO_WR_reg")
			return DFIFO_WR_reg;
		if (mode == "DFIFO")
			return DFIFO;
		return INVALID;
	}
};

struct FecRegistersEnum {
	FecRegistersEnum() = default;
	enum Type {
		FE_ID_reg = 0x0000,
		BOARD_CSR_reg = 0x0002,
		CMD_reg = 0x0003,
		DL_CSR_reg = 0x0004,
		CHN_FILTER_reg = 0x0100,
		CHN_GAIN_reg = 0x0101,
		ADC_SEL_reg = 0x0103,
		CHN1_RAW_SAMPLE_reg = 0x3000,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 8> TYPES = { FE_ID_reg, BOARD_CSR_reg, CMD_reg, DL_CSR_reg, CHN_FILTER_reg, CHN_GAIN_reg, ADC_SEL_reg, CHN1_RAW_SAMPLE_reg };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case FE_ID_reg:
			return "FE_ID_reg";
		case BOARD_CSR_reg:
			return "BOARD_CSR_reg";
		case CMD_reg:
			return "CMD_reg";
		case DL_CSR_reg:
			return "DL_CSR_reg";
		case CHN_FILTER_reg:
			return "CHN_FILTER_reg";
		case CHN_GAIN_reg:
			return "CHN_GAIN_reg";
		case ADC_SEL_reg:
			return "ADC_SEL_reg";
		case CHN1_RAW_SAMPLE_reg:
			return "CHN1_RAW_SAMPLE_reg";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "FE_ID_reg")
			return FE_ID_reg;
		if (mode == "BOARD_CSR_reg")
			return BOARD_CSR_reg;
		if (mode == "CMD_reg")
			return CMD_reg;
		if (mode == "DL_CSR_reg")
			return DL_CSR_reg;
		if (mode == "CHN_FILTER_reg")
			return CHN_FILTER_reg;
		if (mode == "CHN_GAIN_reg")
			return CHN_GAIN_reg;
		if (mode == "ADC_SEL_reg")
			return ADC_SEL_reg;
		if (mode == "CHN1_RAW_SAMPLE_reg")
			return CHN1_RAW_SAMPLE_reg;
		return INVALID;
	}
};

struct Temperature {
	uint32_t temperature_;
	TemperaturesEnum::Type source_;
};

struct FansModeEnum {
	enum Type {
		OFF,
		SILENT,
		NORMAL,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 3> TYPES = { OFF, SILENT, NORMAL };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case OFF:
			return "OFF";
		case SILENT:
			return "SILENT";
		case NORMAL:
			return "NORMAL";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "OFF")
			return OFF;
		else if (mode == "SILENT")
			return SILENT;
		else if (mode == "NORMAL")
			return NORMAL;
		return INVALID;
	}
};

struct ClockSourceEnum {
	enum Type {
		PTP,
		DEBUG,
		SOFTWARE,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 3> TYPES = { PTP, DEBUG, SOFTWARE };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case PTP:
			return "PTP";
		case DEBUG:
			return "DEBUG";
		case SOFTWARE:
			return "SOFTWARE";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "PTP")
			return PTP;
		else if (mode == "DEBUG")
			return DEBUG;
		else if (mode == "SOFTWARE")
			return SOFTWARE;
		return INVALID;
	}
};

struct DeviceStateEnum {
	enum Type {
		IDLE,
		INIT,
		ARMED,
		ACQUISITION,
		ERROR,
		TEST,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 6> TYPES = { IDLE, INIT, ARMED, ACQUISITION, ERROR, TEST};

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case IDLE:
			return "IDLE";
		case INIT:
			return "INIT";
		case ARMED:
			return "ARMED";
		case ACQUISITION:
			return "ACQUISITION";
		case ERROR:
			return "ERROR";
		case TEST:
			return "TEST";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "IDLE")
			return IDLE;
		else if (mode == "INIT")
			return INIT;
		else if (mode == "ARMED")
			return ARMED;
		if (mode == "ACQUISITION")
			return ACQUISITION;
		else if (mode == "ERROR")
			return ERROR;
		else if (mode == "TEST")
			return TEST;
		return INVALID;
	}
};

class DeviceState {
	std::bitset<32> data_;
	DeviceStateEnum::Type state_;
	std::optional<uint32_t> controllerId_;
public:
	std::array<bool, 4> linksConnectionStatus() const noexcept {
		return { data_[28], data_[29], data_[30], data_[31] };
	}

	void setLinksConnectionStatus(std::array<bool, 4> const& linksStatus) noexcept {
		data_[31] = linksStatus[3];
		data_[30] = linksStatus[2];
		data_[29] = linksStatus[1];
		data_[28] = linksStatus[0];
	}

	bool linksConnectionStatus(uint32_t const linkIndex) const noexcept {
		return data_[28 + linkIndex];
	}

	bool fifoUnderflow() const noexcept {
		return data_[12];
	}

	bool fifoOverflow() const noexcept {
		return data_[13];
	}

	void setFifoUnderflow(bool const state) noexcept {
		data_[12] = state;
	}

	void setFifoOverflow(bool const state) noexcept {
		data_[13] = state;
	}

	bool acquisitionStoppedOnError() const noexcept {
		return data_[16];
	}

	void setAcquisitionStoppedOnError(bool const state) noexcept {
		data_[16] = state;
	}

	uint32_t numberOfScansInFifo() const noexcept {
		return data_.to_ulong() & 0xFFF;
	}

	void setNumberOfScansInFifo(uint32_t const numbersOfScansInFifo) noexcept {
		data_ &= 0xFFFFF000;
		data_ |= (numbersOfScansInFifo & 0xFFF);
	}

	void set(QString const& hexString) noexcept {
		bool conversionStatus;
		uint32_t data = hexString.toUInt(&conversionStatus, 16);
		if (!conversionStatus)
			qDebug() << "Conversion Error";
		else
			data_ = data;
	}

	void set(uint32_t const data) noexcept {
		data_ = data;
	}

	DeviceStateEnum::Type state() const noexcept {
		return state_;
	}

	void setState(DeviceStateEnum::Type const state) noexcept {
		state_ = state;
	}

	uint32_t toUInt() const noexcept {
		return data_.to_ulong();
	}

	void setControllerId(std::optional<uint32_t> const& controllerId) noexcept {
		controllerId_ = controllerId;
	}

	std::optional<uint32_t> controllerId() const noexcept {
		return controllerId_;
	}
};

struct Status1Part {
	uint32_t data_;
	std::array<bool, 4> linksConnectionStatus() const noexcept {
		std::bitset<32> bits(data_);
		return { bits[31], bits[30], bits[29], bits[28] };
	}

	bool linksConnectionStatus(uint32_t const linkIndex) const noexcept {
		std::bitset<32> bits(data_);
		return bits[28 + linkIndex];
	}

	uint8_t lockKey() const noexcept {
		return data_ && 0xFF;
	}
};

struct Status2Part {
	uint32_t data_;
	std::array<bool, 4> linksConnectionStatus() const noexcept {
		std::bitset<32> bits(data_);
		return { bits[31], bits[30], bits[29], bits[28] };
	}

	bool linksConnectionStatus(uint32_t const linkIndex) const noexcept {
		std::bitset<32> bits(data_);
		return bits[28 + linkIndex];
	}

	uint8_t lockKey() const noexcept {
		return data_ && 0xFF;
	}
};

struct HeaderPart {
	uint16_t id_;
	uint16_t options_;
	uint32_t runningNumber_;
	uint32_t dataSize_;
	uint16_t numberOfScans_;
	uint16_t numberOfSamples_;
	Status1Part status1_;
	Status2Part status2_;

	QString toString() const noexcept {
		return QString("Id: %1, Options: %2, RunningNumber: %3, DataSize: %4, Scans: %5, Samples: %6, Status 1: %7, Status 2: %8")
			.arg(toHex(id_, 4), toHex(options_, 4), toHex(runningNumber_, 8), toHex(dataSize_, 8), toHex(numberOfScans_, 4), toHex(numberOfSamples_, 4), toHex(status1_.data_, 8), toHex(status2_.data_, 8));
	}
};

struct Timestamp {
	uint64_t seconds_;
	uint32_t nanoseconds_;
};

struct Sample {
	//Timestamp ts_;
	uint32_t rawData_;
};

struct DataPart {
	DataPart(uint32_t const samples) {
		samples_.resize(samples);
	}
	
	std::vector<float> samples_;
	QString toString() const noexcept {
		QString result;
		for (auto i = 0; i < samples_.size(); i++)
			result += QString("%1 ; ").arg(toHex(samples_[i], 8));
		return result;
	}
};

struct AcquisitionPacket {
	AcquisitionPacket(uint32_t const samples) {
		data_ = DataPart(samples);
	}
	HeaderPart header_;
	DataPart data_{ 0 };
};

struct AcquisitionStartModeModel {
	std::optional<AcquisitionStartModeEnum::Type> mode_;
	std::optional<PtpTime> ptpAlarm_;
};

struct AcquisitionStopModeModel {
	std::optional<uint32_t> scansThreshold_;
	std::optional<bool> stopOnError_;
};

struct Configuration6991 {
	std::optional<ScanRateModel> scanRate_;
	std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temperatures_;
	std::optional<FansModeEnum::Type> fansMode_;
	std::optional<ClockSourceEnum::Type> clockSource_;
	std::optional<PtpTime> ptpTime_;
	AcquisitionStartModeModel startMode_;
	AcquisitionStopModeModel stopMode_;
	std::optional<uint32_t> scansPerDirectReadPacket_;
	std::optional<bool> timestamps_;
};


struct TestTypeEnum {
	enum Type {
		CL0,
		CL1,
		DL0,
		DL1,
		FIFO,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 5> TYPES = { CL0, CL1, DL0, DL1, FIFO };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case CL0:
			return "CL0";
		case CL1:
			return "CL1";
		case DL0:
			return "DL0";
		case DL1:
			return "DL1";
		case FIFO:
			return "FIFO";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "CL0")
			return CL0;
		else if (mode == "CL1")
			return CL1;
		else if (mode == "DL0")
			return DL0;
		if (mode == "DL1")
			return DL1;
		else if (mode == "FIFO")
			return FIFO;
		return INVALID;
	}
};

struct DlSclkFreqType {
	enum Type {
		_1Mhz = 0x0,
		_2Mhz = 0x1,
		_4Mhz = 0x2,
		_5Mhz = 0x3,
		_10Mhz = 0x4,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 5> TYPES = { _1Mhz, _2Mhz, _4Mhz, _5Mhz, _10Mhz };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case _1Mhz:
			return "1Mhz";
		case _2Mhz:
			return "2Mhz";
		case _4Mhz:
			return "4Mhz";
		case _5Mhz:
			return "5Mhz";
		case _10Mhz:
			return "10Mhz";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "1Mhz")
			return _1Mhz;
		if (mode == "2Mhz")
			return _2Mhz;
		if (mode == "4Mhz")
			return _4Mhz;
		if (mode == "5Mhz")
			return _5Mhz;
		if (mode == "10Mhz")
			return _10Mhz;
		return INVALID;
	}
};

struct Result {
	std::optional<uint32_t> count_;
	std::optional<uint32_t> errors_;
};

using TestsResultModel = std::map<TestTypeEnum::Type, Result>;
using TestsSelectionModel = std::map<TestTypeEnum::Type, bool>;
struct FifoTestModel {
	struct Configuration {
		std::optional<uint32_t> blockSize_;
		std::optional<double> rate_;
		std::optional<uint32_t> treshold_;
	};

	Configuration config_;
	std::optional<uint32_t> samplesCount_;
	std::optional<uint32_t> lastSample_;
	uint32_t overflows_ = 0;
	uint32_t dataErrorsCount_ = 0;
	uint32_t passTresholdCount_ = 0;
};

struct StartTestsRequest {
	TestsSelectionModel model;
	FifoTestModel::Configuration fifoTestConfig_;
	DlSclkFreqType::Type clockFreq_;
};

struct TestsStatus {
	TestsResultModel model;
	FifoTestModel fifoTestModel_;
};

struct DlFrameRateType {
	enum Type {
		BU6111_BU6171 = 0x4,
		BU6132 = 0x10,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 2> TYPES = { BU6111_BU6171, BU6132 };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case BU6111_BU6171:
			return "BU6111_BU6171";
		case BU6132:
			return "BU6132";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "BU6111_BU6171")
			return BU6111_BU6171;
		else if (mode == "BU6132")
			return BU6132;
		return INVALID;
	}
};

struct FecCmdsType6111 {
	enum Type {
		ACQ_START = 0xEA01,
		ACQ_STOP = 0xEA02,
		DI_LATCH = 0xEA11,
		DL_TEST_START = 0xEA81,
		DL_TEST_STOP = 0xEA82,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 5> TYPES = { ACQ_START, ACQ_STOP, DI_LATCH, DL_TEST_START, DL_TEST_STOP };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case ACQ_START:
			return "ACQ_START";
		case ACQ_STOP:
			return "ACQ_STOP";
		case DI_LATCH:
			return "DI_LATCH";
		case DL_TEST_START:
			return "DL_TEST_START";
		case DL_TEST_STOP:
			return "DL_TEST_STOP";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "ACQ_START")
			return ACQ_START;
		if (mode == "ACQ_STOP")
			return ACQ_STOP;
		if (mode == "DI_LATCH")
			return DI_LATCH;
		if (mode == "DL_TEST_START")
			return DL_TEST_START;
		if (mode == "DL_TEST_STOP")
			return DL_TEST_STOP;
		return INVALID;
	}
};

struct FecCmdsType6132 {
	enum Type {
		ACQ_START = 0xEA01,
		ACQ_STOP = 0xEA02,
		ADC_INIT = 0xEA11,
		ADC_SETUP = 0xEA12,
		ADC_CONVERT = 0xEA13,
		ADC_REG_ACCESS = 0xEA14,
		GAIN_SETUP = 0xEA15,
		ADC_TEST_START = 0xEA81,
		ADC_TEST_STOP = 0xEA82,
		DL_TEST_START = 0xEA83,
		DL_TEST_STOP = 0xEA84,
		COMBO_TEST_START = 0xEA85,
		COMBO_TEST_STOP = 0xEA86,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 13> TYPES = { ACQ_START, ACQ_STOP, ADC_INIT, ADC_SETUP, ADC_CONVERT, ADC_REG_ACCESS, GAIN_SETUP, ADC_TEST_START, ADC_TEST_STOP,
		DL_TEST_START, DL_TEST_STOP, COMBO_TEST_START, COMBO_TEST_STOP };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case ACQ_START:
			return "ACQ_START";
		case ACQ_STOP:
			return "ACQ_STOP";
		case ADC_INIT:
			return "ADC_INIT";
		case ADC_SETUP:
			return "ADC_SETUP";
		case ADC_CONVERT:
			return "ADC_CONVERT";
		case ADC_REG_ACCESS:
			return "ADC_REG_ACCESS";
		case GAIN_SETUP:
			return "GAIN_SETUP";
		case ADC_TEST_START:
			return "ADC_TEST_START";
		case ADC_TEST_STOP:
			return "ADC_TEST_STOP";
		case DL_TEST_START:
			return "DL_TEST_START";
		case DL_TEST_STOP:
			return "DL_TEST_STOP";
		case COMBO_TEST_START:
			return "COMBO_TEST_START";
		case COMBO_TEST_STOP:
			return "COMBO_TEST_STOP";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "ACQ_START")
			return ACQ_START;
		if (mode == "ACQ_STOP")
			return ACQ_STOP;
		if (mode == "ADC_INIT")
			return ADC_INIT;
		if (mode == "ADC_SETUP")
			return ADC_SETUP;
		if (mode == "ADC_CONVERT")
			return ADC_CONVERT;
		if (mode == "ADC_REG_ACCESS")
			return ADC_REG_ACCESS;
		if (mode == "GAIN_SETUP")
			return GAIN_SETUP;
		if (mode == "ADC_TEST_START")
			return ADC_TEST_START;
		if (mode == "ADC_TEST_STOP")
			return ADC_TEST_STOP;
		if (mode == "DL_TEST_START")
			return DL_TEST_START;
		if (mode == "DL_TEST_STOP")
			return DL_TEST_STOP;
		if (mode == "COMBO_TEST_START")
			return COMBO_TEST_START;
		if (mode == "COMBO_TEST_STOP")
			return COMBO_TEST_STOP;
		return INVALID;
	}
};

struct FecStatusType6111 {
	enum Type {
		INIT = 0x00,
		IDLE = 0x01,
		ACQUISITION = 0x11,
		DL_TEST = 0x21,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 4> TYPES = { INIT, IDLE, ACQUISITION, DL_TEST };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case INIT:
			return "INIT";
		case IDLE:
			return "IDLE";
		case ACQUISITION:
			return "ACQUISITION";
		case DL_TEST:
			return "DL_TEST";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "INIT")
			return INIT;
		if (mode == "IDLE")
			return IDLE;
		if (mode == "ACQUISITION")
			return ACQUISITION;
		if (mode == "DL_TEST")
			return DL_TEST;
		return INVALID;
	}
};

struct FecStatusType6132 {
	enum Type {
		PON = 0x00,
		PONADC = 0x01,
		PONW4R = 0x02,
		IDLE = 0x03,
		ADC_INI = 0x04,
		ADC_SET = 0x05,
		ADC_REG = 0x06,
		ADC_CONV = 0x07,
		GAIN_SET = 0x08,
		ACQ_INI = 0x09,
		ACQ = 0x0A,
		ACQ_END = 0x0B,
		ADC_TEST_INI = 0x0C,
		ADC_TEST = 0x0D,
		ADC_TEST_END = 0x0E,
		DL_TEST = 0x0F,
		DL_TEST_END = 0x10,
		COMBO_TEST_INI = 0x11,
		COMBO_TEST = 0x12,
		COMBO_TEST_END1 = 0x13,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 20> TYPES = { PON, PONADC, PONW4R, IDLE, ADC_INI, ADC_SET, ADC_REG, ADC_CONV,
		GAIN_SET, ACQ_INI, ACQ, ACQ_END, ADC_TEST_INI, ADC_TEST, ADC_TEST_END, DL_TEST, DL_TEST_END, COMBO_TEST_INI, COMBO_TEST, COMBO_TEST_END1 };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case PON:
			return "PON";
		case PONADC:
			return "PONADC";
		case PONW4R:
			return "PONW4R";
		case IDLE:
			return "IDLE";
		case ADC_INI:
			return "ADC_INI";
		case ADC_SET:
			return "ADC_SET";
		case ADC_REG:
			return "ADC_REG";
		case ADC_CONV:
			return "ADC_CONV";
		case GAIN_SET:
			return "GAIN_SET";
		case ACQ_INI:
			return "ACQ_INI";
		case ACQ:
			return "ACQ";
		case ACQ_END:
			return "ACQ_END";
		case ADC_TEST_INI:
			return "ADC_TEST_INI";
		case ADC_TEST:
			return "ADC_TEST";
		case ADC_TEST_END:
			return "ADC_TEST_END";
		case DL_TEST:
			return "DL_TEST";
		case DL_TEST_END:
			return "DL_TEST_END";
		case COMBO_TEST_INI:
			return "COMBO_TEST_INI";
		case COMBO_TEST:
			return "COMBO_TEST";
		case COMBO_TEST_END1:
			return "COMBO_TEST_END1";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "PON")
			return PON;
		if (mode == "PONADC")
			return PONADC;
		if (mode == "PONW4R")
			return PONW4R;
		if (mode == "IDLE")
			return IDLE;
		if (mode == "ADC_INI")
			return ADC_INI;
		if (mode == "ADC_SET")
			return ADC_SET;
		if (mode == "ADC_REG")
			return ADC_REG;
		if (mode == "ADC_CONV")
			return ADC_CONV;
		if (mode == "GAIN_SET")
			return GAIN_SET;
		if (mode == "ACQ_INI")
			return ACQ_INI;
		if (mode == "ACQ")
			return ACQ;
		if (mode == "ACQ_END")
			return ACQ_END;
		if (mode == "ADC_TEST_INI")
			return ADC_TEST_INI;
		if (mode == "ADC_TEST")
			return ADC_TEST;
		if (mode == "ADC_TEST_END")
			return ADC_TEST_END;
		if (mode == "DL_TEST")
			return DL_TEST;
		if (mode == "DL_TEST_END")
			return DL_TEST_END;
		if (mode == "COMBO_TEST_INI")
			return COMBO_TEST_INI;
		if (mode == "COMBO_TEST")
			return COMBO_TEST;
		if (mode == "COMBO_TEST_END1")
			return COMBO_TEST_END1;
		return INVALID;
	}
};

struct FecType {
	enum Type {
		_6111 = 0x6111,
		_6132 = 0x6132,
		_6171 = 0x6171,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 3> TYPES = { _6111, _6132, _6171 };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case _6111:
			return "6111";
		case _6132:
			return "6132";
		case _6171:
			return "6171";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "6111")
			return _6111;
		if (mode == "6132")
			return _6132;
		if (mode == "6171")
			return _6171;
		return INVALID;
	}
};

struct FecIdType {
	enum Type {
		BOTH = 0,
		_1 = 1,
		_2 = 2,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 4> TYPES = { BOTH, _1, _2 };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case BOTH:
			return "BOTH";
		case _1:
			return "1";
		case _2:
			return "2";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "BOTH")
			return BOTH;
		if (mode == "1")
			return _1;
		if (mode == "2")
			return _2;
		return INVALID;
	}
};

struct GainType6132 {
	enum Type {
		_1 = 0,
		_2 = 1,
		_4 = 2,
		_8 = 3,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 4> TYPES = { _1, _2, _4, _8 };

	static QString toString(Type const gain) noexcept {
		switch (gain) {
		case _1:
			return "1";
		case _2:
			return "2";
		case _4:
			return "4";
		case _8:
			return "8";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& gain) noexcept {
		if (gain == "1")
			return _1;
		if (gain == "2")
			return _2;
		if (gain == "4")
			return _4;
		if (gain == "8")
			return _8;
		return INVALID;
	}

	static uint32_t toMeaningValue(GainType6132::Type const& gain) noexcept {
		if(gain == _1) 
			return 1;
		else if (gain == _2)
			return 2;
		else if (gain == _4)
			return 4;
		else
			return 8;
	}
};

struct FilterType6132 {
	enum Type {
		_10Hz = 0,
		_100Hz = 1,
		_1000Hz = 2,
		BYPASS = 3,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 4> TYPES = { _10Hz, _100Hz, _1000Hz, BYPASS };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case _10Hz:
			return "10Hz";
		case _100Hz:
			return "100Hz";
		case _1000Hz:
			return "1000Hz";
		case BYPASS:
			return "BYPASS";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "10Hz")
			return _10Hz;
		if (mode == "100Hz")
			return _100Hz;
		if (mode == "1000Hz")
			return _1000Hz;
		if (mode == "BYPASS")
			return BYPASS;
		return INVALID;
	}
};

struct AdcCmdType {
	enum Type {
		ADC_INIT = 0x00,
		ADC_SETUP = 0x01,
		ADC_CONVERT = 0x11,
		ADC_REG_ACCESS = 0x21,
		ADC_TEST_START = 0x33,
		GAIN_SETUP = 0x44,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 6> TYPES = { ADC_INIT, ADC_SETUP, ADC_CONVERT, ADC_REG_ACCESS, ADC_TEST_START, GAIN_SETUP };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case ADC_INIT:
			return "ADC_INIT";
		case ADC_SETUP:
			return "ADC_SETUP";
		case ADC_CONVERT:
			return "ADC_CONVERT";
		case ADC_REG_ACCESS:
			return "ADC_REG_ACCESS";
		case ADC_TEST_START:
			return "ADC_TEST_START";
		case GAIN_SETUP:
			return "GAIN_SETUP";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "ADC_INIT")
			return ADC_INIT;
		if (mode == "ADC_SETUP")
			return ADC_SETUP;
		if (mode == "ADC_CONVERT")
			return ADC_CONVERT;
		if (mode == "ADC_REG_ACCESS")
			return ADC_REG_ACCESS;
		if (mode == "ADC_TEST_START")
			return ADC_TEST_START;
		if (mode == "GAIN_SETUP")
			return GAIN_SETUP;
		return INVALID;
	}
};