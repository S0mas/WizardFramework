#pragma once
#include "../SmartEnum.h"
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
	int value_;
	ScanRateUnitsEnum::Type units_;
};

struct PtpTime {
	int32_t seconds_;
	int32_t nanoseconds_;
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
public:
	std::array<bool, 4> linksConnectionStatus() const noexcept {
		return { data_[31], data_[30], data_[29], data_[28] };
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

	bool acquisitionStoppedOnError() const noexcept {
		return data_[16];
	}

	unsigned int numberOfScansInFifo() const noexcept {
		return data_.to_ulong() & 0xFFF;
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
	std::optional<std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()>> temperatures_;
	std::optional<FansModeEnum::Type> fansMode_;
	std::optional<ClockSourceEnum::Type> clockSource_;
	std::optional<PtpTime> ptpTime_;
	AcquisitionStartModeModel startMode_;
	AcquisitionStopModeModel stopMode_;
	std::optional<int> scansPerDirectReadPacket_;
	std::optional<bool> timestamps_;
};


enum class TestType {
	CL0,
	CL1,
	DL0,
	DL1,
	FIFO,
	UNKNOWN
};

inline std::vector<TestType> testTypes = { TestType::CL0 , TestType::CL1, TestType::DL0, TestType::DL1, TestType::FIFO };
inline QString toString(TestType const test) noexcept {
	switch (test) {
	case TestType::CL0:
		return "CL0";
	case TestType::CL1:
		return "CL1";
	case TestType::DL0:
		return "DL0";
	case TestType::DL1:
		return "DL1";
	case TestType::FIFO:
		return "FIFO";
	default:
		return "UNKNOWN";
	}
}

struct Result {
	int count_;
	int errors_;
};

using TestsResultModel = std::map<TestType, Result>;
using TestsSelectionModel = std::map<TestType, bool>;
