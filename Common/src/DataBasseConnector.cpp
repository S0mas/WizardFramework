#include "../include/DataBasseConnector.h"
#include <QDebug>
#include <QSqlError>
#include <QString>

void QMySqlDataBaseConnector::setDataBase(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept {
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(mysqlHostName);
	db.setPort(mysqlPort);
	db.setDatabaseName(mysqlDbName);
	db.setUserName(user);
	db.setPassword(passwd);
}

void QMySqlDataBaseConnector::openConnection() noexcept {
	db.open();
}

void QMySqlDataBaseConnector::closeConnection() noexcept {
	db.close();
}

QMySqlDataBaseConnector::QMySqlDataBaseConnector(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept {
	setDataBase(mysqlHostName, mysqlPort, mysqlDbName, user, passwd);
	openConnection();
}

QMySqlDataBaseConnector::~QMySqlDataBaseConnector() {
	closeConnection();
}

bool QMySqlDataBaseConnector::isConnectionOpen() const noexcept {
	return db.isOpen();
}

void QMySqlDataBaseConnector::executeSqlQuery(const QString& sqlQuery) noexcept {
	lastQuery.clear();
	lastQuery.exec(sqlQuery);
}

bool QMySqlDataBaseConnector::wasLastQuerySuccesful() const noexcept {
	return lastQuery.isActive();
}

QSqlQuery QMySqlDataBaseConnector::getLastQuery() const noexcept {
	return lastQuery;
}

QSqlError QMySqlDataBaseConnector::getLastError() const noexcept {
	return db.lastError();
}

inline bool DataBaseConnectorMock::isConnectionOpen() const noexcept { return true; }

inline void DataBaseConnectorMock::executeSqlQuery(const QString& sqlQuery) noexcept { qDebug() << sqlQuery << " executed!"; }

inline bool DataBaseConnectorMock::wasLastQuerySuccesful() const noexcept { return true; }

inline QSqlQuery DataBaseConnectorMock::getLastQuery() const noexcept { return QSqlQuery(); }

inline QSqlError DataBaseConnectorMock::getLastError() const noexcept { return QSqlError(); }
