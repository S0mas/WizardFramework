#pragma once
#include "../ScpiIF.h"
#include <array>
#include <bitset>
#include <optional>
#include <QDebug>
#include <QDateTime>
#include <QTime>

inline QString toHex(unsigned int const value, int const width = 2) noexcept {
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
	int32_t seconds_;
	int32_t nanoseconds_;
	QString toString() const noexcept {
		return QString("%1,%2").arg(seconds_).arg(nanoseconds_);
	}

	static PtpTime fromString(QString const& str) noexcept {
		auto list = str.split(',');
		return { list[0].toInt(), list[1].toInt() };
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
		WRITE_LOOPBACK = 0x53,
		CUSTOM = 0x00,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 4> TYPES = { READ, WRITE, WRITE_LOOPBACK, CUSTOM };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
		case READ:
			return "READ";
		case WRITE:
			return "WRITE";
		case WRITE_LOOPBACK:
			return "WRITE_LOOPBACK";
		case CUSTOM:
			return "CUSTOM";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "READ")
			return READ;
		else if (mode == "WRITE")
			return WRITE;
		else if (mode == "WRITE_LOOPBACK")
			return WRITE_LOOPBACK;
		else if (mode == "CUSTOM")
			return CUSTOM;
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
		CL_SPI_CSR_reg = 0x4488,
		CL_SPI_TLCNT_reg = 0x44A0,
		CL0_SPI_TLERR_reg = 0x44A4,
		CL1_SPI_TLERR_reg = 0x44A8,
		DL_SPI_CSR1_reg = 0x4500,
		DL0_SPI_TMCNT_reg = 0x4520,
		DL0_SPI_TMERR_reg = 0x4524,
		DL1_SPI_TMCNT_reg = 0x4528,
		DL1_SPI_TMERR_reg = 0x452C,
		DFIFO_CSR_reg = 0x4700,
		INVALID = -1
	};

	inline constexpr static std::array<Type, 10> TYPES = { CL_SPI_CSR_reg, CL_SPI_TLCNT_reg, CL0_SPI_TLERR_reg,
		CL1_SPI_TLERR_reg, DL_SPI_CSR1_reg, DL0_SPI_TMCNT_reg, DL0_SPI_TMERR_reg, DL1_SPI_TMCNT_reg, DL1_SPI_TMERR_reg, DFIFO_CSR_reg };

	static QString toString(Type const mode) noexcept {
		switch (mode) {
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
		case DL0_SPI_TMCNT_reg:
			return "DL0_SPI_TMCNT_reg";
		case DL0_SPI_TMERR_reg:
			return "DL0_SPI_TMERR_reg";
		case DL1_SPI_TMCNT_reg:
			return "DL1_SPI_TMCNT_reg";
		case DL1_SPI_TMERR_reg:
			return "DL1_SPI_TMERR_reg";
		case DFIFO_CSR_reg:
			return "DFIFO_CSR_reg";
		default:
			return "invalid";
		}
	}

	static Type fromString(QString const& mode) noexcept {
		if (mode == "CL_SPI_CSR_reg")
			return CL_SPI_CSR_reg;
		else if (mode == "CL_SPI_TLCNT_reg")
			return CL_SPI_TLCNT_reg;
		else if (mode == "CL0_SPI_TLERR_reg")
			return CL0_SPI_TLERR_reg;
		if (mode == "CL1_SPI_TLERR_reg")
			return CL1_SPI_TLERR_reg;
		else if (mode == "DL_SPI_CSR1_reg")
			return DL_SPI_CSR1_reg;
		else if (mode == "DL0_SPI_TMCNT_reg")
			return DL0_SPI_TMCNT_reg;
		if (mode == "DL0_SPI_TMERR_reg")
			return DL0_SPI_TMERR_reg;
		else if (mode == "DL1_SPI_TMCNT_reg")
			return DL1_SPI_TMCNT_reg;
		else if (mode == "DL1_SPI_TMERR_reg")
			return DL1_SPI_TMERR_reg;
		else if (mode == "DFIFO_CSR_reg")
			return DFIFO_CSR_reg;
		return INVALID;
	}
};

struct Temperature {
	int temperature_;
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
	std::optional<int> controllerId_;
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

	bool linksConnectionStatus(int const linkIndex) const noexcept {
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

	unsigned int numberOfScansInFifo() const noexcept {
		return data_.to_ulong() & 0xFFF;
	}

	void setNumberOfScansInFifo(unsigned int const numbersOfScansInFifo) noexcept {
		data_ &= 0xFFFFF000;
		data_ |= (numbersOfScansInFifo & 0xFFF);
	}

	void set(QString const& hexString) noexcept {
		bool conversionStatus;
		unsigned int data = hexString.toUInt(&conversionStatus, 16);
		if (!conversionStatus)
			qDebug() << "Conversion Error";
		else
			data_ = data;
	}

	void set(unsigned int const data) noexcept {
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

	void setControllerId(std::optional<int> const& controllerId) noexcept {
		controllerId_ = controllerId;
	}

	std::optional<int> controllerId() const noexcept {
		return controllerId_;
	}
};

struct StatusPart {
	uint32_t data_;
	std::array<bool, 4> linksConnectionStatus() const noexcept {
		std::bitset<32> bits(data_);
		return { bits[31], bits[30], bits[29], bits[28] };
	}

	bool linksConnectionStatus(int const linkIndex) const noexcept {
		std::bitset<32> bits(data_);
		return bits[28 + linkIndex];
	}

	uint8_t lockKey() const noexcept {
		return data_ && 0xFF;
	}
};

struct HeaderPart {
	uint16_t id_;
	uint16_t runningNumber_;
	uint16_t dataSize_;
	uint16_t numberOfScans_;
	uint16_t numberOfSamples_;
	StatusPart status_;
	uint16_t options_;
};

struct DataPart {

};

struct AcquisitionPacket {
	HeaderPart header_;
	DataPart data_;
};

struct AcquisitionStartModeModel {
	std::optional<AcquisitionStartModeEnum::Type> mode_;
	std::optional<PtpTime> ptpAlarm_;
};

struct AcquisitionStopModeModel {
	std::optional<int> scansThreshold_;
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
	std::optional<int> scansPerDirectReadPacket_;
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

struct Result {
	std::optional<int> count_;
	std::optional<int> errors_;
};

using TestsResultModel = std::map<TestTypeEnum::Type, Result>;
using TestsSelectionModel = std::map<TestTypeEnum::Type, bool>;
