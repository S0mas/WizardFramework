#pragma once
#include <QTcpSocket>
#include <QHostAddress>
#include "ReadingStrategy.h"

class DataCollectorClient : public QObject {
	Q_OBJECT
	ReadingStrategy* readingStrategy_;

	void makeConnections() {
		QObject::connect(socket_, &QAbstractSocket::connected, this, &DataCollectorClient::connected);
		QObject::connect(socket_, &QIODevice::readyRead, this, &DataCollectorClient::readData);
		QObject::connect(socket_, &QAbstractSocket::disconnected, this, &DataCollectorClient::disconnected);
		QObject::connect(socket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &DataCollectorClient::displayError);
	}

	void displayError() {
		emit reportError(QString(socket_->errorString()));
	}
private slots:
	void readData() {
		readingStrategy_->readData();
		if (socket_->bytesAvailable() == 0)
			emit logMsg("Socket is empty!");
	}
protected:
	QTcpSocket* socket_;
public:
	DataCollectorClient(ReadingStrategy* readingStrategy, QObject* parent = nullptr)
		: QObject(parent),
		readingStrategy_(readingStrategy),
		socket_(nullptr)
	{}

	DataCollectorClient(ReadingStrategy* readingStrategy, QTcpSocket* socket, QObject* parent = nullptr)
		: QObject(parent),
		readingStrategy_(readingStrategy),
		socket_(socket) {
		readingStrategy_->setDataStreamDevice(socket_);
		makeConnections();
	}

	void connect(QHostAddress const& address, uint32_t const port) {
		if (socket_)
			delete socket_;
		socket_ = new QTcpSocket(this);
		readingStrategy_->setDataStreamDevice(socket_);
		makeConnections();
		socket_->connectToHost(address, port);
	}

	void disconnect() noexcept {
		if (socket_) {
			socket_->disconnectFromHost();
			delete socket_;
			socket_ = nullptr;
		}
	}

	bool isConnected() const noexcept {
		return socket_ != nullptr && socket_->isOpen();
	}
signals:
	void logMsg(QString const& msg) const;
	void reportError(QString const& msg) const;
	void connected() const;
	void disconnected() const;
};