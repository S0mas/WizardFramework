#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <array>

struct ReadTransaction {
	bool headerReaded_ = false;
	bool dataReaded_ = false;
};

class DataCollectorServer : public QObject {
	Q_OBJECT
	QTcpServer* server_;
	uint32_t port_;

	void disconnected() {	
		server_->listen(QHostAddress::LocalHost, port_);
		socket_->deleteLater();
		socket_ = nullptr;
		qDebug() << "Client disconnected!";
	}

	void connected() {
		socket_ = server_->nextPendingConnection();
		connect(socket_, &QIODevice::readyRead, this, &DataCollectorServer::readData);
		connect(socket_, &QAbstractSocket::disconnected, this, &DataCollectorServer::disconnected);
		connect(socket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &DataCollectorServer::displayError);
		dataStream_.setDevice(socket_);
		dataStream_.setVersion(QDataStream::Qt_5_1);
		server_->close();
		qDebug() << "Client connected!";
	}

	void displayError() {
		emit reportError(QString(socket_->errorString()));
	}
protected:
	QTcpSocket* socket_;
	QDataStream dataStream_;
	ReadTransaction transaction_;
	virtual void readData() noexcept = 0;
public:
	DataCollectorServer(uint32_t const port, QObject* parent = nullptr) : QObject(parent), port_(port){
		server_ = new QTcpServer(this);
		server_->setMaxPendingConnections(1);
		server_->listen(QHostAddress::LocalHost, port_);
		socket_ = nullptr;
		connect(server_, &QTcpServer::newConnection, this, &DataCollectorServer::connected);
	}
signals:
	void reportError(QString const& msg) const;
};