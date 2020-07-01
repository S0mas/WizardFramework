#pragma once
#include <QTcpSocket>
#include <QObject>
#include "Device6991/Defines6991.h"
#include <QDataStream>
#include <QString>
#include <QFile>
#include <QTimer>
#include <mutex>
#include <memory>
#include "PacketReading.h"
#include "DataCollectorClient.h"
#include "LoggingObject.h"

//TODO MEMORY LEAKS
template<typename ScanType/*Scan6111 or Scan6132*/>
class DataStream {
	PacketReading<HeaderPart6991, ScanType> readingStrat_;
	DataCollectorClient* client_;
	QTcpSocket* forwardSocket_;
	mutable std::mutex stateUpdateLock_;
	DeviceState state_;
	QFile* dataFile_ = nullptr;
	QString header_;
	std::vector<bool> channelsStates_;
	
	bool storeData_ = false;
	bool forwardData_ = false;
	bool shouldForwardChannelMask_ = true;
	uint32_t forwardingPort_ = 0;
	std::vector<std::function<void(HeaderPart6991 const&, SignalPacketData<ScanType> const&)>> callbacks_;

	void initializeFileWithHeader() noexcept {
		QTextStream in(dataFile_);
		in << header_;
	}
	
	void storeData(SignalPacketData<ScanType> const& data) noexcept {
		if (storeData_) {
			if (dataFile_ && !dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
				return;
			if (dataFile_->size() == 0)
				initializeFileWithHeader();
			QTextStream in(dataFile_);
			in << data;
		}
	}

	void forwardData(HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) noexcept {
		if (forwardData_) {
			if (forwardSocket_->state() == QTcpSocket::SocketState::UnconnectedState)
				forwardSocket_->connectToHost(QHostAddress("127.0.0.1"), forwardingPort_);		
			if(forwardSocket_->state() != QTcpSocket::SocketState::ConnectedState)
				return;				

			QDataStream stream(forwardSocket_);
			SignalPacketHeader signalPacketHeader(header);
			if (shouldForwardChannelMask_) {
				shouldForwardChannelMask_ = false;
				signalPacketHeader.containsChannelStates_ = true;
				for (auto const state : channelsStates_)
					signalPacketHeader.channelsStates_.push_back(state);
			}
			signalPacketHeader.deviceAddress_ = client_->peerAddress();
			stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
			stream << signalPacketHeader;
			stream << data;
		}
	}

	void updateDeviceState(HeaderPart6991 const& header) noexcept {
		std::lock_guard l(stateUpdateLock_);
		state_ = header.state();
	}

	void doWithData(HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) {
		for (auto const& callback : callbacks_)
			callback(header, data);
	}
public:
	DataStream(uint32_t const forwardingPort = 0, QObject* membersParent = nullptr) : forwardingPort_(forwardingPort) {
		client_ = new DataCollectorClient(&readingStrat_, membersParent);
		readingStrat_.setEndianness(QDataStream::LittleEndian);
		readingStrat_.setDataReadingCallback([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { doWithData(header, data); });
		callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { storeData(data); });	
		callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { updateDeviceState(header); });	
		if (forwardingPort_ != 0) {
			forwardSocket_ = new QTcpSocket(membersParent);
			callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { forwardData(header, data); });
		}
	}

	void connect(QString const& address, uint32_t const port) noexcept {
		client_->connect(QHostAddress(address), port);
		//emit logMsg(QString("connecting %1:%2").arg(address).arg(port));
	}

	void disconnect() noexcept {
		client_->disconnect();
	}

	bool isConnected() const noexcept {
		return client_->isConnected();
	}

	void setStoreData(bool const state) noexcept {
		storeData_ = state;
	}

	void setForwardData(bool const state) noexcept {
		forwardData_ = state;
		if (!forwardData_) {
			forwardSocket_->disconnectFromHost();
			shouldForwardChannelMask_ = true;
		}
		else 
			forwardSocket_->connectToHost(QHostAddress("127.0.0.1"), forwardingPort_);
	}

	void addCallback(std::function<void(HeaderPart6991 const&, SignalPacketData<ScanType> const&)> const& callback) {
		callbacks_.push_back(callback);
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

	DeviceState deviceState() const noexcept {
		std::lock_guard l(stateUpdateLock_);
		return state_;
	}

	void setDataFile(QFile* dataFile) noexcept {
		if (dataFile_ && dataFile_->isOpen())
			dataFile_->close();
		dataFile_ = dataFile;
	}

	void setHeader(QString const& header) noexcept {
		header_ = header;
	}

	void setChannelConfiguration(std::vector<bool> const& channelsStates) noexcept {
		channelsStates_ = channelsStates;
		shouldForwardChannelMask_ = true;
	}
};
