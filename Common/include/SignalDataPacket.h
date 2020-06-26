#pragma once
#include <array>
#include <vector>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>
#include <QHostAddress>
#include <type_traits>
#include "Device6991/Defines6991.h"

struct _6111_SamplesPack_8 {
	unsigned char raw_;
	std::array<bool, 8/*bits*/> samples() const noexcept {
		std::array<bool, 8/*bits*/> data;
		for (int i = 0; i < 8/*bits*/; ++i)
			data[i] = raw_ & (1 << i);
		return data;
	}

	_6111_SamplesPack_8& operator=(QString const& rhs) noexcept {
		raw_ = static_cast<unsigned char>(rhs.toUInt(nullptr, 16));
		return *this;
	}
};

inline QDataStream& operator<<(QDataStream& stream, _6111_SamplesPack_8 const& samplePack) {
	stream << samplePack.raw_;
	return stream;
}

inline QDataStream& operator>>(QDataStream& stream, _6111_SamplesPack_8& samplePack) {
	stream >> samplePack.raw_;
	return stream;
}

inline QTextStream& operator<<(QTextStream& stream, _6111_SamplesPack_8 const& samplePack) {
	stream << toHex(samplePack.raw_, 2);
	return stream;
}

inline QTextStream& operator>>(QTextStream& stream, _6111_SamplesPack_8& samplePack) {
	QString hex;
	stream >> hex;
	samplePack.raw_ = static_cast<unsigned char>(hex.toUInt(nullptr, 16));
	return stream;
}

struct _6132Sample {
	float sample_;
	_6132Sample& operator=(float const rhs) noexcept {
		sample_ = rhs;
		return *this;
	}
	_6132Sample& operator=(QString const& rhs) noexcept {
		sample_ = rhs.toDouble();
		return *this;
	}

	operator double() const {
		return sample_;
	}
};

inline QDataStream& operator<<(QDataStream& stream, _6132Sample const& sample) {
	stream << sample.sample_;
	return stream;
}

inline QDataStream& operator>>(QDataStream& stream, _6132Sample& sample) {
	stream >> sample.sample_;
	return stream;
}

inline QTextStream& operator<<(QTextStream& stream, _6132Sample const& sample) {
	QString formattedSample = QString("%1").arg(QString::number(sample.sample_, 'f', 9), 12, QLatin1Char('+'));
	stream << formattedSample;
	return stream;
}

inline QTextStream& operator>>(QTextStream& stream, _6132Sample& sample) {
	stream >> sample.sample_;
	return stream;
}


//inline QDataStream& operator<<(QDataStream& stream, float const& sample) {
//	auto dataPtr = reinterpret_cast<char const*>(&sample);
//	stream.writeRawData(dataPtr, 4);
//	return stream;
//}
//
//inline QDataStream& operator>>(QDataStream& stream, float& sample) {
//	auto dataPtr = reinterpret_cast<char*>(&sample);
//	stream.readRawData(dataPtr, 4);
//	return stream;
//}
//
//inline QTextStream& operator<<(QTextStream& stream, float const& sample) {
//	stream << QString::number(sample, 'g', 7);
//	return stream;
//}


struct Timestamp6991 {
	uint64_t seconds_;
	uint64_t nanoseconds_;
};

inline QDataStream& operator<<(QDataStream& stream, Timestamp6991 const& timestamp) {
	stream << timestamp.seconds_;
	stream << timestamp.nanoseconds_;
	return stream;
}

inline QDataStream& operator>>(QDataStream& stream, Timestamp6991& timestamp) {
	stream >> timestamp.seconds_;
	stream >> timestamp.nanoseconds_;
	return stream;
}

inline QTextStream& operator<<(QTextStream& stream, Timestamp6991 const& timestamp) {
	auto ts = QString("%1.%2").arg(timestamp.seconds_, 21, 10, QLatin1Char(' ')).arg(timestamp.nanoseconds_, 9, 10, QLatin1Char('0'));
	stream << ts;
	return stream;
}

inline QTextStream& operator>>(QTextStream& stream, Timestamp6991& timestamp) {
	QString word;
	stream >> word;
	auto list = word.split('.');
	timestamp.seconds_ = list[0].toDouble();
	timestamp.nanoseconds_ = list[1].toDouble();
	return stream;
}

template<typename SampleType, typename TimestampType>
struct Scan {
	using SampleT = SampleType;
	TimestampType ts_;
	bool ignoreTimestamp = false;
	std::vector<SampleType> samples_;
};

template<typename SampleType, typename TimestampType>
inline QDataStream& operator<<(QDataStream& stream, Scan<SampleType, TimestampType> const& scan) {
	if (!scan.ignoreTimestamp)
		stream << scan.ts_;
	for (auto const& sample : scan.samples_)
		stream << sample;
	return stream;
}

template<typename SampleType, typename TimestampType>
inline QDataStream& operator>>(QDataStream& stream, Scan<SampleType, TimestampType>& scan) {
	if (!scan.ignoreTimestamp)
		stream >> scan.ts_;
	for (auto& sample : scan.samples_)
		stream >> sample;
	return stream;
}

template<typename SampleType, typename TimestampType>
inline QTextStream& operator<<(QTextStream& stream, Scan<SampleType, TimestampType> const& scan) {
	if (!scan.ignoreTimestamp)
		stream << scan.ts_ << ',';
	else
		stream << "-------------------------------,";

	if (!scan.samples_.empty())
		stream << scan.samples_[0];
	for (int i = 1; i < scan.samples_.size(); ++i)
		stream << "," << scan.samples_[i];
	return stream;
}

template<typename SampleType, typename TimestampType>
inline QTextStream& operator>>(QTextStream& stream, Scan<SampleType, TimestampType>& scan) {
	QString line = stream.readLine();
	auto tokens = line.split(',');

	//read timestamp first if it is there
	if (!line.isEmpty() && line[0] != '-') {		
		scan.ignoreTimestamp = false;
		QTextStream s(&tokens[0]);
		s >> scan.ts_;
	}
	else 
		scan.ignoreTimestamp = true;

	scan.samples_.resize(tokens.size() - 1);
	for (int i = 1; i < tokens.size(); ++i)
		scan.samples_[i-1] = tokens[i];
	return stream;
}

using Scan6111 = Scan<_6111_SamplesPack_8, Timestamp6991>;
using Scan6132 = Scan<_6132Sample, Timestamp6991>;
using ScanFifo = Scan<uint32_t, Timestamp6991>;

struct Status1Part {
	uint32_t data_;
	std::array<bool, 4> linksConnectionStatus() const noexcept;
	bool linksConnectionStatus(uint32_t const linkIndex) const noexcept;
	bool fifoUnderflow() const noexcept;
	bool fifoOverflow() const noexcept;
};

struct Status2Part {
	uint32_t data_;

	uint8_t lockKey() const noexcept {
		return data_ & 0xFF000000;
	}

	uint16_t fifoCount() const noexcept {
		return data_ & 0xFFFF;
	}
};

struct HeaderPart6991 {
	uint16_t id_;
	uint16_t options_;
	uint32_t runningNumber_;
	uint32_t dataSize_;
	uint16_t numberOfScans_;
	uint16_t numberOfSamplesPerScan_;
	Status1Part status1_;
	Status2Part status2_;
	DeviceState state() const noexcept {
		DeviceState state;
		state.setFifoUnderflow(status1_.fifoUnderflow());
		state.setFifoOverflow(status1_.fifoOverflow());
		state.setLinksConnectionStatus(status1_.linksConnectionStatus());
		state.setControllerId(status2_.lockKey());
		state.setNumberOfScansInFifo(status2_.fifoCount());
		state.setState(DeviceStateEnum::ACQUISITION);
		state.setAcquisitionStoppedOnError(false);
		return state;
	}
};

inline QDataStream& operator<<(QDataStream& stream, HeaderPart6991 const& header) {
	stream << header.id_;
	stream << header.options_;
	stream << header.runningNumber_;
	stream << header.dataSize_;
	stream << header.numberOfScans_;
	stream << header.numberOfSamplesPerScan_;
	stream << header.status1_.data_;
	stream << header.status2_.data_;

	return stream;
}


inline QDataStream& operator>>(QDataStream& stream, HeaderPart6991& header) {
	stream >> header.id_;
	stream >> header.options_;
	stream >> header.runningNumber_;
	stream >> header.dataSize_;
	stream >> header.numberOfScans_;
	stream >> header.numberOfSamplesPerScan_;
	stream >> header.status1_.data_;
	stream >> header.status2_.data_;

	return stream;
}


struct SignalPacketHeader {
	DeviceType deviceType_ = DeviceType::INVALID;
	uint32_t scansNo_;
	uint32_t samplesPerScan_;
	uint32_t dataSize_;
	QHostAddress deviceAddress_;
	bool containsChannelStates_;
	uint32_t channelsCount_;
	bool timestamps_;
	std::vector<bool> channelsStates_;;
	//enabled channels

	SignalPacketHeader() {}

	SignalPacketHeader(HeaderPart6991 const& header) {
		containsChannelStates_ = false;
		if (header.id_ == 0x6111) {
			deviceType_ = DeviceType::_6111;
			samplesPerScan_ = header.numberOfSamplesPerScan_ / 8;
			channelsCount_ = 256;
		}
		else if (header.id_ == 0x6132) {
			deviceType_ = DeviceType::_6132;
			samplesPerScan_ = header.numberOfSamplesPerScan_;
			channelsCount_ = 32;
		}
		else
			samplesPerScan_ = header.numberOfSamplesPerScan_;

		dataSize_ = header.dataSize_;
		scansNo_ = header.numberOfScans_;
		timestamps_ = header.options_ & 1;
		
	}
};

inline QDataStream& operator<<(QDataStream& stream, SignalPacketHeader const& header) {
	stream << header.deviceType_;
	stream << header.scansNo_;
	stream << header.samplesPerScan_;
	stream << header.dataSize_;
	stream << header.deviceAddress_;
	stream << header.timestamps_;
	stream << header.containsChannelStates_;
	if (header.containsChannelStates_) {
		stream << header.channelsCount_;
		for(auto const state : header.channelsStates_)
			stream << state;
	}
	return stream;
}

inline QDataStream& operator>>(QDataStream& stream, SignalPacketHeader& header) {
	int type;
	stream >> type;
	header.deviceType_ = static_cast<DeviceType>(type);
	stream >> header.scansNo_;
	stream >> header.samplesPerScan_;
	stream >> header.dataSize_;
	stream >> header.deviceAddress_;
	stream >> header.timestamps_;
	stream >> header.containsChannelStates_;
	if (header.containsChannelStates_) {
		stream >> header.channelsCount_;
		header.channelsStates_.clear();
		for (int i = 0; i < header.channelsCount_; ++i) {
			bool state;
			stream >> state;
			header.channelsStates_.push_back(state);
		}
	}
	return stream;
}

template<typename ScanType>
struct SignalPacketData {
	std::vector<ScanType> scans_;
};

template<typename ScanType>
inline QDataStream& operator<<(QDataStream& stream, SignalPacketData<ScanType> const& data) {
	for (auto const& scan : data.scans_)
		stream << scan;
	return stream;
}

template<typename ScanType>
inline QDataStream& operator>>(QDataStream& stream, SignalPacketData<ScanType>& data) {
	for (auto& scan : data.scans_)
		stream >> scan;
	return stream;
}

template<typename ScanType>
inline QTextStream& operator<<(QTextStream& stream, SignalPacketData<ScanType> const& data) {
	for (auto const& scan : data.scans_)
		stream << scan << '\n';
	return stream;
}

template<typename ScansType>
struct SignalPacket {
	SignalPacketHeader header_;
	SignalPacketData<ScansType> data_;

	SignalPacket(SignalPacketHeader const& header, bool ignoreSimestamp = false) : header_(header) {
		if constexpr (std::is_same<ScansType, Scan6111>())
			header_.deviceType_ = DeviceType::_6111;
		else if constexpr (std::is_same<ScansType, Scan6132>())
			header_.deviceType_ = DeviceType::_6132;
		data_.scans_.resize(header_.scansNo_);
		for (auto& scan : data_.scans_)
			scan.samples_.resize(header_.samplesPerScan_);
		for (auto& scan : data_.scans_)
			scan.ignoreTimestamp = !header_.timestamps_;

		//if constexpr (!std::is_same<ScansType::TimestampT,void>()) {
		//	if (data_.scans_.empty() && header_.dataSize_ != 0 || (header_.dataSize_ != (data_.scans_.size() * data_.scans_[0].samples_.size() * sizeof(ScansType::SampleT) + data_.scans_.size() * sizeof(ScansType::TimestampT))))
		//		qDebug() << "Error with signal packet data alignment!";
		//}
	}

	SignalPacket(HeaderPart6991 const& header) : SignalPacket(SignalPacketHeader(header), !(header.options_ & 1)) {}

	SignalPacket() {}
};

template<typename ScanType>
inline QDataStream& operator<<(QDataStream& stream, SignalPacket<ScanType> const& packet) {
	stream << packet.header_;
	stream << packet.data_;
	return stream;
}

using SignalPacketData6111 = SignalPacketData<Scan6111>;
using SignalPacketData6132 = SignalPacketData<Scan6132>;


constexpr uint32_t forwardingPort6132 = 56132;
constexpr uint32_t forwardingPort6111 = 56111;