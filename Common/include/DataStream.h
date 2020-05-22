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

class DataStream : public QObject {
	Q_OBJECT
	DataCollectorClient* client;
	QTcpSocket* forwardSocket_;
	mutable std::mutex socketAccess_;
	QFile* dataFile_ = new QFile("data.csv", this);
	QTextStream out{ dataFile_};
	bool storeData_ = false;
	bool forwardData_ = false;

	void storeData(std::vector<float> const& scan) noexcept {
		if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		if(!scan.empty())
			out << scan[0];
		for (int i = 1; i < scan.size(); ++i)
			out << ',' << QString::number(scan[i], 'g', 6);
		out << '\n';
	}

	void forwardData(HeaderPart const& header, SignalDataPacket::Data<float> const& data) noexcept {
		SignalDataPacket::Header newheader;
		newheader.dataSize_ = header.dataSize_;
		newheader.sampleType_ = SignalDataPacket::SampleType::Sample6132;
		newheader.scansNo_ = header.numberOfScans_;

		forwardSocket_->write(reinterpret_cast<const char*>(&newheader), sizeof(newheader));
		forwardSocket_->write(reinterpret_cast<const char*>(data.samples_.data()), data.samples_.size() * 4);
		qDebug() << "data forwarded!";
	}

	void doWithData(HeaderPart const& header, SignalDataPacket::Data<float> const& data) {
		qDebug() << "doWithData!";
		if (storeData_)
			storeData(data.samples_);
		if (forwardData_)
			forwardData(header, data);
	}

public:
	DataStream(QObject* parent = nullptr) : QObject(parent) {
		setForwardData(true);/*todo remove dbg only*/
	}

	void connect(QString const& address, uint32_t const port) noexcept {
		std::lock_guard lock(socketAccess_);
		auto readingStrat = new PacketReading<HeaderPart, float>;
		readingStrat->setDataReadingCallback([this](HeaderPart const& header, SignalDataPacket::Data<float> const& data) { doWithData(header, data); });
		client = new DataCollectorClient(QHostAddress(address), port, readingStrat, this);
		QObject::connect(client, &DataCollectorClient::connected, this, &DataStream::connected);
		QObject::connect(client, &DataCollectorClient::disconnected, this, &DataStream::disconnected);
		QObject::connect(client, &DataCollectorClient::logMsg, this, &DataStream::logMsg);
		QObject::connect(client, &DataCollectorClient::reportError, this, &DataStream::reportError);
		client->connect();
		qDebug() << "connecting  :  " << address << "    port:" << port;
	}

	void disconnect() noexcept {
		std::lock_guard lock(socketAccess_);
		if (client) {
			client->disconnect();
			delete client;
		}
	}

	bool isConnected() noexcept {
		std::lock_guard lock(socketAccess_);
		return client && client->isConnected();
	}

	void setStoreData(bool const state) noexcept {
		storeData_ = state;
	}

	void setForwardData(bool const state) noexcept {
		if (state) {
			forwardSocket_ = new QTcpSocket(this);
			forwardSocket_->connectToHost("127.0.0.1", 1);
			forwardData_ = state;
		}		
		else {
			forwardData_ = state;
			forwardSocket_->disconnectFromHost();
			delete forwardSocket_;
		}
	}
signals:
	void logMsg(QString const& msg) const;
	void reportError(QString const& msg) const;
	void connected() const;
	void disconnected() const;
};
