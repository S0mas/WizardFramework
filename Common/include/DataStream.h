#pragma once
#include <QTcpSocket>
#include <QObject>
#include "Device6991/Defines6991.h"
#include <QDataStream>
#include <QString>
#include <mutex>

class DataStream : public QObject {
	Q_OBJECT
	struct DataStreamTranastion {
		AcquisitionPacket packet_{ 0 };
		bool headerReaded_ = false;
		bool dataReaded_ = false;
	};
	QTcpSocket* tcpSocket_;
	mutable std::mutex socketAccess_;
	QDataStream in_;
	DataStreamTranastion transaction_;

	void readPacketHeader() noexcept {
		in_.startTransaction();
		auto headerPtr = &transaction_.packet_.header_;
		if (auto const bytes = tcpSocket_->bytesAvailable(); bytes < sizeof(*headerPtr))
			qDebug() << QString("Waiting for full packet header... have %1 bytes, expected header size: %2").arg(bytes).arg(sizeof(*headerPtr));
		else {
			in_.readRawData(reinterpret_cast<char*>(reinterpret_cast<unsigned char*>(headerPtr)), sizeof(*headerPtr));
			transaction_.headerReaded_ = true;
			qDebug() << headerPtr->toString();
			if (!in_.commitTransaction())
				emit reportError("Reading data from socket error.");
		}
	}

	void readPacketData() noexcept {
		in_.startTransaction();
		auto headerPtr = &transaction_.packet_.header_;
		if (auto const bytes = tcpSocket_->bytesAvailable(); bytes < headerPtr->dataSize_)
			qDebug() << QString("Waiting for full packet data... have %1 bytes, expected data size: %2").arg(bytes).arg(headerPtr->dataSize_);
		else {
			transaction_.packet_.data_.samples_.resize(headerPtr->dataSize_ / 4);
			in_.readRawData(reinterpret_cast<char*>(reinterpret_cast<unsigned char*>(transaction_.packet_.data_.samples_.data())), headerPtr->dataSize_);
			transaction_.dataReaded_ = true;
			qDebug() << transaction_.packet_.data_.toString();
			if (!in_.commitTransaction())
				emit reportError("Reading data from socket error.");
		}
	}

	void readData() noexcept {
		std::lock_guard lock(socketAccess_);
		if (!tcpSocket_)
			return;
		if (!transaction_.headerReaded_)
			readPacketHeader();
		if (transaction_.headerReaded_ && !transaction_.dataReaded_)
			readPacketData();
		if (transaction_.headerReaded_ && transaction_.dataReaded_) {
			transaction_.dataReaded_ = false;
			transaction_.headerReaded_ = false;
			//if (storeData_) TODO STORE DATA
			//	storeData(transaction_.packet_.data_.toString().toStdString());
		}
	}
public:
	DataStream(QObject* parent = nullptr) : QObject(parent) {}
	void displayError(QAbstractSocket::SocketError socketError) noexcept {
		emit reportError(QString(tcpSocket_->errorString()));
		disconnect();
	}

	void connect(QString const& addresss, uint32_t const port) noexcept {
		std::lock_guard lock(socketAccess_);
		tcpSocket_ = new QTcpSocket(this);
		QObject::connect(tcpSocket_, &QAbstractSocket::connected, this, &DataStream::connected);
		QObject::connect(tcpSocket_, &QAbstractSocket::disconnected, this, &DataStream::disconnected);
		QObject::connect(tcpSocket_, &QIODevice::readyRead, this, &DataStream::readData);
		QObject::connect(tcpSocket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &DataStream::displayError);
		in_.setDevice(tcpSocket_);
		in_.setVersion(QDataStream::Qt_5_1);
		tcpSocket_->connectToHost(addresss, port);
	}

	void disconnect() noexcept {
		std::lock_guard lock(socketAccess_);
		if (tcpSocket_) {
			tcpSocket_->disconnectFromHost();
			delete tcpSocket_;
			tcpSocket_ = nullptr;
		}
	}

	bool isConnected() noexcept {
		std::lock_guard lock(socketAccess_);
		if (tcpSocket_)
			qDebug() << tcpSocket_->bytesAvailable();
		return tcpSocket_ != nullptr && tcpSocket_->isOpen();
	}

	QString bytesToRead() {
		return tcpSocket_ ? QString::number(tcpSocket_->bytesAvailable()) : "not connected";
	}
signals:
	void reportError(QString const& msg) const;
	void connected() const;
	void disconnected() const;
};
