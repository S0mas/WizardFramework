#pragma once
#include <QDataStream>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include "DataCollectorClient.h"

class DataCollectorServer : public QObject {
	Q_OBJECT
	QTcpServer* server_;
	uint32_t port_;
	ReadingStrategy* readingStrategy_;

	void disconnected() {	
		emit logMsg("Client disconnected!");
		emit clientDisconnected();
	}

	void connected() {
		auto socket_ = server_->nextPendingConnection();
		connect(socket_, &QAbstractSocket::disconnected, this, &DataCollectorServer::disconnected);
		auto client = new DataCollectorClient(readingStrategy_->copy(), socket_, this);
		emit newClientConnected(client);
		emit logMsg("Client connected!");
	}

	void displayError(QAbstractSocket::SocketError socketError) {
		emit reportError(QString::number(socketError));
	}
public:
	DataCollectorServer(uint32_t const port, ReadingStrategy* readingStrategy, QObject* parent = nullptr) : QObject(parent), port_(port), readingStrategy_(readingStrategy) {
		server_ = new QTcpServer(this);
		server_->listen(QHostAddress::LocalHost, port_);
		connect(server_, &QTcpServer::newConnection, this, &DataCollectorServer::connected);
		connect(server_, &QTcpServer::acceptError, this, &DataCollectorServer::displayError);
	}
signals:
	void logMsg(QString const& msg) const;
	void reportError(QString const& msg) const;
	void newClientConnected(DataCollectorClient* client) const;
	void clientDisconnected() const;
};
