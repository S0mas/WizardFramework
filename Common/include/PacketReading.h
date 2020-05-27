#pragma once
#include "SignalDataPacket.h"
#include "ReadingStrategy.h"
#include <functional>

template<typename HeaderType, typename DataType>
class PacketReading : public ReadingStrategy {
	struct ReadTransaction {
		bool headerReaded_ = false;
		bool dataReaded_ = false;
	};
	ReadTransaction transaction_;
	HeaderType header_;
	SignalDataPacket::Data<DataType> data_;
	std::function<void(HeaderType const&, SignalDataPacket::Data<DataType> const&)> callback_ = [](auto const&, auto const&) { qDebug() << "Full data packet readed!"; };

	void readDataPart() {
		if (auto bytes = dataStream_.device()->bytesAvailable(); bytes < data_.samples_.size() * sizeof(DataType))
			qDebug() << QString("Waiting for full packet data... have %1 bytes, expected data size: %2").arg(bytes).arg(data_.samples_.size() * sizeof(DataType));
		else {
			dataStream_.startTransaction();
			dataStream_.readRawData(reinterpret_cast<char*>(data_.samples_.data()), data_.samples_.size() * sizeof(DataType));
			transaction_.dataReaded_ = true;
			if (!dataStream_.commitTransaction())
				qDebug() << "Reading packet data from socket error.";
		}
	}

	void readHeaderPart() noexcept {
		if (auto bytes = dataStream_.device()->bytesAvailable(); bytes < sizeof(header_))
			qDebug() << QString("Waiting for full packet header... have %1 bytes, expected header size: %2").arg(bytes).arg(sizeof(header_));
		else {
			dataStream_.startTransaction();
			dataStream_.readRawData(reinterpret_cast<char*>(&header_), sizeof(header_));
			transaction_.headerReaded_ = true;
			if (!dataStream_.commitTransaction())
				qDebug() << "Reading packet header from socket error.";
		}
	}
public:
	PacketReading() = default;

	void readData() noexcept override {
		if (!transaction_.headerReaded_)
			readHeaderPart();
		if (transaction_.headerReaded_ && !transaction_.dataReaded_) {
			data_.resize(header_.dataSize_ / sizeof(DataType));
			readDataPart();
		}
		if (transaction_.headerReaded_ && transaction_.dataReaded_) {
			transaction_.headerReaded_ = false;
			transaction_.dataReaded_ = false;
			callback_(header_, data_);
			if (dataStream_.device()->bytesAvailable() > 0)
				readData();
		}
	}

	ReadingStrategy* copy() const noexcept override {
		return new PacketReading<HeaderType, DataType>;
	}

	void setDataReadingCallback(std::function<void(HeaderType const&, SignalDataPacket::Data<DataType> const&)> const& callback) noexcept {
		callback_ = callback;
	}
};