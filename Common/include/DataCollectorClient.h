#pragma once
#include <QTcpSocket>
#include <QHostAddress>
#include "ReadingStrategy.h"

class DataCollectorClient : public QObject {
	Q_OBJECT
	QHostAddress address_;
	uint32_t port_;
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
	}
protected:
	QTcpSocket* socket_;
public:
	DataCollectorClient(QHostAddress const& address, uint32_t const port, ReadingStrategy* readingStrategy, QObject* parent = nullptr)
		: QObject(parent),
		address_(address),
		port_(port),
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

	void connect() {
		if (socket_)
			delete socket_;
		socket_ = new QTcpSocket(this);
		readingStrategy_->setDataStreamDevice(socket_);
		makeConnections();
		socket_->connectToHost(address_, port_);
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