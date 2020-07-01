#pragma once
#include "SignalDataPacket.h"
#include "ReadingStrategy.h"
#include <functional>

template<typename HeaderType, typename ScanType>
class PacketReading : public ReadingStrategy {
	struct ReadTransaction {
		bool headerReaded_ = false;
		bool dataReaded_ = false;
	};
	ReadTransaction transaction_;
	std::function<void(HeaderType const&, SignalPacketData<ScanType> const&)> callback_ = [](auto const&, auto const&) { qDebug() << "Full data packet readed!"; };

	HeaderType header_;
	SignalPacket<ScanType> packet_;
	void readDataPart() {
		if (auto bytes = dataStream_.device()->bytesAvailable(); bytes < header_.dataSize_)
			qDebug() << QString("Waiting for full packet data... have %1 bytes, expected data size: %2").arg(bytes).arg(header_.dataSize_);
		else {
			dataStream_ >> packet_.data_;
			transaction_.dataReaded_ = true;
		}
	}

	void readHeaderPart() noexcept {
		if (auto bytes = dataStream_.device()->bytesAvailable(); bytes < sizeof(HeaderType))
			qDebug() << QString("Waiting for full packet header... have %1 bytes, expected header size: %2").arg(bytes).arg(sizeof(HeaderType));
		else {
			dataStream_ >> header_;
			packet_ = SignalPacket<ScanType>(header_);
			transaction_.headerReaded_ = true;
		}
	}
public:
	PacketReading() {
		dataStream_.setFloatingPointPrecision(QDataStream::SinglePrecision);
	}

	void readData() noexcept override {
		if (!transaction_.headerReaded_)
			readHeaderPart();
		if (transaction_.headerReaded_ && !transaction_.dataReaded_)
			readDataPart();	
		if (transaction_.headerReaded_ && transaction_.dataReaded_) {
			transaction_.headerReaded_ = false;
			transaction_.dataReaded_ = false;
			callback_(header_, packet_.data_);
			if (dataStream_.device()->bytesAvailable() > 0)
				readData();
		}
	}

	ReadingStrategy* copy() const noexcept override {
		auto copy = new PacketReading<HeaderType, ScanType>;
		copy->setDataReadingCallback(callback_);
		copy->dataStream_.setByteOrder(this->dataStream_.byteOrder());
		copy->dataStream_.setFloatingPointPrecision(QDataStream::SinglePrecision);
		return copy;
	}

	void setDataReadingCallback(std::function<void(HeaderType const&, SignalPacketData<ScanType> const&)> const& callback) noexcept {
		callback_ = callback;
	}
};