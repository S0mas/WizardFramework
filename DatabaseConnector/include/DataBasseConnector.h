#pragma once
#define QT_DEBUG_PLUGINS 1
#include <QSqlDatabase>
#include <memory>
class QSqlQuery;
class QSqlError;
class QString;

class DataBaseConnectorInterface {
public:
	virtual ~DataBaseConnectorInterface() = default;
	virtual bool isConnectionOpen() const noexcept = 0;
	virtual void executeSqlQuery(const QString& sqlQuery) noexcept = 0;
	virtual bool wasLastQuerySuccesful() const noexcept = 0;
	virtual QSqlQuery getLastQuery() const noexcept = 0;
	virtual QSqlError getLastError() const noexcept = 0;

};

class DataBaseConnectorMock final : public  DataBaseConnectorInterface {
public:
	bool isConnectionOpen() const noexcept final;
	void executeSqlQuery(const QString& sqlQuery) noexcept final;
	bool wasLastQuerySuccesful() const noexcept final;
	QSqlQuery getLastQuery() const noexcept final;
	QSqlError getLastError() const noexcept final;

};

class QMySqlDataBaseConnector final : public DataBaseConnectorInterface {
	QSqlDatabase db;
	std::unique_ptr<QSqlQuery> lastQuery;

	void setDataBase(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept;
	void openConnection() noexcept;
	void closeConnection() noexcept;
public:
	QMySqlDataBaseConnector(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept;
	~QMySqlDataBaseConnector()	;
	bool isConnectionOpen() const noexcept final;
	void executeSqlQuery(const QString& sqlQuery) noexcept final;
	bool wasLastQuerySuccesful() const noexcept final;
	QSqlQuery getLastQuery() const noexcept final;
	QSqlError getLastError() const noexcept final;
};
