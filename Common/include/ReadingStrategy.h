#pragma once
#include <QDataStream>
#include <QIODevice>
#include <QString>

class ReadingStrategy {
protected:
	QDataStream dataStream_;
public:
	ReadingStrategy() = default;
	virtual void readData() noexcept = 0;
	virtual ReadingStrategy* copy() const noexcept = 0;
	void setDataStreamDevice(QIODevice* device) noexcept {
		dataStream_.setDevice(device);
		dataStream_.setVersion(QDataStream::Qt_5_1);
	}

	void setEndianness(QDataStream::ByteOrder order) noexcept {
		dataStream_.setByteOrder(order);
	}
};