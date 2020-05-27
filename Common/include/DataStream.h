#pragma once
#include <QTcpSocket>
#include <QObject>
#include "Device6991/Defines6991.h"
#include <QDataStream>
#include <QString>
#include <QFile>
#include <mutex>
#include "PacketReading.h"
#include "DataCollectorClient.h"
#include "LoggingObject.h"
//TODO MEMORY LEAKS
template<typename DataType>
class DataStream {
	PacketReading<HeaderPart, DataType> readingStrat_;
	DataCollectorClient* client_;
	QTcpSocket* forwardSocket_;
	mutable std::mutex socketAccess_;
	QFile* dataFile_;
	
	bool storeData_ = false;
	bool forwardData_ = false;
	std::vector<std::function<void(HeaderPart const&, SignalDataPacket::Data<DataType> const&)>> callbacks_;
	
	void storeData(std::vector<DataType> const& scan) noexcept {
		if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(dataFile_);
		if(!scan.empty())
			out << scan[0];
		for (int i = 1; i < scan.size(); ++i)
			out << ',' << scan[i];
		out << '\n';
		//emit logMsg("Data stored!");
	}

	void forwardData(HeaderPart const& header, SignalDataPacket::Data<DataType> const& data) noexcept {
		SignalDataPacket::Header newheader;
		newheader.dataSize_ = header.dataSize_;
		newheader.sampleType_ = SignalDataPacket::SampleType::Sample6132;
		newheader.scansNo_ = header.numberOfScans_;

		forwardSocket_->write(reinterpret_cast<const char*>(&newheader), sizeof(newheader));
		forwardSocket_->write(reinterpret_cast<const char*>(data.samples_.data()), data.samples_.size() * sizeof(DataType));
		//emit logMsg("Data forwarded!");
	}

	void doWithData(HeaderPart const& header, SignalDataPacket::Data<DataType> const& data) {
		//emit logMsg("Data received!");
		for (auto const& callback : callbacks_)
			callback(header, data);
	}

public:
	DataStream(QObject* membersParent = nullptr) {
		client_ = new DataCollectorClient(&readingStrat_, membersParent);
		dataFile_ = new QFile("data.csv", membersParent);
		setForwardData(true);/*todo remove dbg only*/	
		readingStrat_.setDataReadingCallback([this](HeaderPart const& header, SignalDataPacket::Data<DataType> const& data) { doWithData(header, data); });
		callbacks_.push_back([this](HeaderPart const& header, SignalDataPacket::Data<DataType> const& data) { forwardData(header, data); });
		callbacks_.push_back([this](HeaderPart const& header, SignalDataPacket::Data<DataType> const& data) { storeData(data.samples_); });
	}

	
	void connect(QString const& address, uint32_t const port) noexcept {
		std::lock_guard lock(socketAccess_);
		client_->connect(QHostAddress(address), port);
		dataFile_->resize(0);
		//emit logMsg(QString("connecting %1:%2").arg(address).arg(port));
	}

	void disconnect() noexcept {
		std::lock_guard lock(socketAccess_);
		client_->disconnect();
	}

	bool isConnected() noexcept {
		std::lock_guard lock(socketAccess_);
		return client_->isConnected();
	}

	void setStoreData(bool const state) noexcept {
		storeData_ = state;
	}

	void setForwardData(bool const state) noexcept {
		if (state) {
			forwardSocket_ = new QTcpSocket;//TODO remove memory leak
			forwardSocket_->connectToHost("127.0.0.1", 1);
			forwardData_ = state;
		}		
		else {
			forwardData_ = state;
			forwardSocket_->disconnectFromHost();
			delete forwardSocket_;
		}
	}

	void clearDataFile() {
		dataFile_->resize(0);
	}

	void addCallback(std::function<void(HeaderPart const&, SignalDataPacket::Data<DataType> const&)> const& callback) {
		callbacks_.push_back(callback);
	}

	DataCollectorClient* client() {
		return client_;
	}
};
