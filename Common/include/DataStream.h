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
	QTimer* fileResetTimer_;
	QString channelConfigHeader_;
	void resetFileTimout() {
		closeCurrentFile();
		fileResetTimer_->stop();
	}
	void createFileForDataCollection() noexcept {
		auto dateTime = QDateTime::currentDateTimeUtc().toString("dd.mm.yyyy_hh-mm-ss");
		dataFile_ = std::make_unique<QFile>(QString("Data_%1.csv").arg(dateTime));
		if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream in(dataFile_.get());
		in << channelConfigHeader_ << '\n';
		
	}
	QTcpSocket* forwardSocket_;
	mutable std::mutex stateUpdateLock_;
	DeviceState state_;
	std::unique_ptr<QFile> dataFile_;
	
	bool storeData_ = false;
	bool forwardData_ = false;
	uint32_t forwardingPort_ = 0;
	std::vector<std::function<void(HeaderPart6991 const&, SignalPacketData<ScanType> const&)>> callbacks_;

	void closeCurrentFile() {
		if (dataFile_) {
			dataFile_->close();
			dataFile_.reset();
		}
	}
	
	void storeData(SignalPacketData<ScanType> const& data) noexcept {
		if (storeData_) {
			if (!dataFile_)
				createFileForDataCollection();
			if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
				return;
			QTextStream in(dataFile_.get());
			in << data;
			fileResetTimer_->start(10000);
		}
	}

	void forwardData(HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) noexcept {
		if (forwardData_) {
			if(forwardSocket_->state() == QTcpSocket::SocketState::UnconnectedState)
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
		fileResetTimer_ =  new QTimer(membersParent);
		readingStrat_.setEndianness(QDataStream::LittleEndian);
		readingStrat_.setDataReadingCallback([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { doWithData(header, data); });
		callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { storeData(data); });	
		callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { updateDeviceState(header); });	
		if (forwardingPort_ != 0) {
			forwardSocket_ = new QTcpSocket(membersParent);
			callbacks_.push_back([this](HeaderPart6991 const& header, SignalPacketData<ScanType> const& data) { forwardData(header, data); });
		}

		QObject::connect(fileResetTimer_, &QTimer::timeout, 
			[this] {
				resetFileTimout();
			}); // temporary solution before feature with subscribing for info about acq stop msg will be implemented
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
		if (!forwardData_)
			forwardSocket_->disconnectFromHost();
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

	void setDataFileHeaderWithChannelsIds(std::vector<bool> const& channelStates) noexcept {
		QString header = "|";

		for(int i = 0; i < channelStates.size(); ++i)
			if (channelStates[i]) {
				int channelId = i + 1;
				double x = 12 - QString::number(channelId).size();
				int additional = (QString::number(channelId).size() % 2 == 0) ? 0 : 1;
				header += QString("%1%2%3|").arg(QString(x / 2, ' ')).arg(channelId).arg(QString(x / 2 + additional, ' '));
			}
				
		channelConfigHeader_ = header;
	}
};
