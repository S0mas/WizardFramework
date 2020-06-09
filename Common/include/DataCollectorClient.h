#pragma once
#include <QTcpSocket>
#include <QHostAddress>
#include "ReadingStrategy.h"
#include <mutex>

class DataCollectorClient : public QObject {
	Q_OBJECT
	ReadingStrategy* readingStrategy_;
	mutable std::recursive_mutex socketAccess_;

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
		std::lock_guard lock(socketAccess_);
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
		socket_(new QTcpSocket(this)) {
		readingStrategy_->setDataStreamDevice(socket_);
		makeConnections();
	}

	DataCollectorClient(ReadingStrategy* readingStrategy, QTcpSocket* socket, QObject* parent = nullptr)
		: QObject(parent),
		readingStrategy_(readingStrategy),
		socket_(socket) {
		readingStrategy_->setDataStreamDevice(socket_);
		makeConnections();
	}

	void connect(QHostAddress const& address, uint32_t const port) {
		std::lock_guard lock(socketAccess_);	
		socket_->connectToHost(address, port);
	}

	void disconnect() noexcept {
		std::lock_guard lock(socketAccess_);
		socket_->disconnectFromHost();
	}

	bool isConnected() const noexcept {
		std::lock_guard lock(socketAccess_);
		return socket_->isOpen();
	}

	void showDataAvailable() {
		std::lock_guard lock(socketAccess_);
		qDebug() << socket_->bytesAvailable();
	}

	uint32_t peerPort() const noexcept {
		std::lock_guard lock(socketAccess_);
		return socket_->peerPort();
	}

	QHostAddress peerAddress() const noexcept {
		std::lock_guard lock(socketAccess_);
		return socket_->peerAddress();
	}

	QHostAddress localAddress() const noexcept {
		std::lock_guard lock(socketAccess_);
		return socket_->localAddress();
	}
signals:
	void logMsg(QString const& msg) const;
	void reportError(QString const& msg) const;
	void connected() const;
	void disconnected() const;
};