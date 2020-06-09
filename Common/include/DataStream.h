#pragma once
#include <QTcpSocket>
#include <QObject>
#include "Device6991/Defines6991.h"
#include <QDataStream>
#include <QString>
#include <QFile>
#include "PacketReading.h"
#include "DataCollectorClient.h"
#include "LoggingObject.h"
//TODO MEMORY LEAKS
template<typename ScanType/*Scan6111 or Scan6132*/>
class DataStream {
	PacketReading<HeaderPart6991, ScanType> readingStrat_;
	DataCollectorClient* client_;
	QTcpSocket* forwardSocket_;

	QFile* dataFile_;
	
	bool storeData_ = false;
	bool forwardData_ = false;
	uint32_t forwardingPort_ = 0c;
	std::vector<std::function<void(HeaderPart6991 const&, SignalPacketData<ScanType> const&)>> callbacks_;
	
	void storeData(SignalPacketData<ScanType> const& data) noexcept {
		if (storeData_) {
			if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
				return;
			QTextStream out(dataFile_);
			out << data;
		}
	}

	void forwardData(HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) noexcept {
		if (forwardData_) {
			if(forwardSocket_->state() != QTcpSocket::ConnectedState)
				forwardSocket_->connectToHost(QHostAddress("127.0.0.1"), forwardingPort_);

			QDataStream stream(forwardSocket_);
			SignalPacketHeader signalPacketHeader(header);
			signalPacketHeader.deviceAddress_ = client_->peerAddress();
			stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
			stream.startTransaction();
			stream << signalPacketHeader;
			stream << data;
			stream.commitTransaction();
		}
	}

	void doWithData(HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) {
		for (auto const& callback : callbacks_)
			callback(header, data);
	}
public:
	DataStream(uint32_t const forwardingPort = 0, QObject* membersParent = nullptr) : forwardingPort_(forwardingPort) {
		client_ = new DataCollectorClient(&readingStrat_, membersParent);
		dataFile_ = new QFile("data.csv", membersParent);
		readingStrat_.setEndianness(QDataStream::LittleEndian);
		readingStrat_.setDataReadingCallback([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { doWithData(header, data); });
		callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { storeData(data); });	
		if (forwardingPort_ != 0) {
			forwardSocket_ = new QTcpSocket(membersParent);
			callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { forwardData(header, data); });
		}
	}

	void connect(QString const& address, uint32_t const port) noexcept {
		client_->connect(QHostAddress(address), port);
		dataFile_->resize(0);
		//emit logMsg(QString("connecting %1:%2").arg(address).arg(port));
	}

	void disconnect() noexcept {
		client_->disconnect();
	}

	bool isConnected() noexcept {
		return client_->isConnected();
	}

	void setStoreData(bool const state) noexcept {
		storeData_ = state;
	}

	void setForwardData(bool const state) noexcept {
		forwardData_ = state;
		if (!forwardData_)
			forwardSocket_->disconnectFromHost();
		else 
			forwardSocket_->connectToHost(QHostAddress("127.0.0.1"), forwardingPort_);
	}

	void clearDataFile() {
		dataFile_->resize(0);
	}

	void addCallback(std::function<void(HeaderPart6991 const&, SignalPacketData<ScanType> const&)> const& callback) {
		callbacks_.push_back(callback);
	}
	//TODO: MAKE BETTER SOLUTION IT IS WORKAROUND
	void removeFifoCallback() {
		if (callbacks_.size() == 3)
			callbacks_.pop_back();
	}

	DataCollectorClient* client() {
		return client_;
	}

	uint32_t peerPort() const noexcept {
		return client_->peerPort();
	}

	QHostAddress peerAddress() const noexcept {
		return client_->peerAddress();
	}
};
