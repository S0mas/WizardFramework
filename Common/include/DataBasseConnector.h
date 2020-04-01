#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <memory>

class QSqlError;
class QString;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DataBaseConnectorInterface
///
/// @brief	A data base connector interface.
///
/// @author	Krzysztof Sommerfeld
/// @date	04.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DataBaseConnectorInterface {
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual DataBaseConnectorInterface::~DataBaseConnectorInterface() = default;
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual ~DataBaseConnectorInterface() = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual bool DataBaseConnectorInterface::isConnectionOpen() const noexcept = 0;
	///
	/// @brief	Queries if the connection is open.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	True if the connection is open, false if not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual bool isConnectionOpen() const noexcept = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void DataBaseConnectorInterface::executeSqlQuery(const QString& sqlQuery) noexcept = 0;
	///
	/// @brief	Executes the SQL query operation.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	sqlQuery	The SQL query.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void executeSqlQuery(const QString& sqlQuery) noexcept = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual bool DataBaseConnectorInterface::wasLastQuerySuccesful() const noexcept = 0;
	///
	/// @brief	Determines if the last query was succesful.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	True if it succeeded, false if it failed.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual bool wasLastQuerySuccesful() const noexcept = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual QSqlQuery DataBaseConnectorInterface::getLastQuery() const noexcept = 0;
	///
	/// @brief	Gets the last query.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The last query.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual QSqlQuery getLastQuery() const noexcept = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual QSqlError DataBaseConnectorInterface::getLastError() const noexcept = 0;
	///
	/// @brief	Gets the last error.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The last error.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual QSqlError getLastError() const noexcept = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DataBaseConnectorMock
///
/// @brief	A data base connector mock. This class cannot be inherited.
///
/// @author	Krzysztof Sommerfeld
/// @date	04.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DataBaseConnectorMock final : public  DataBaseConnectorInterface {
public:
	bool isConnectionOpen() const noexcept final;
	void executeSqlQuery(const QString& sqlQuery) noexcept final;
	bool wasLastQuerySuccesful() const noexcept final;
	QSqlQuery getLastQuery() const noexcept final;
	QSqlError getLastError() const noexcept final;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	QMySqlDataBaseConnector
///
/// @brief	MySQL data base connector. This class cannot be inherited.
///
/// @author	Krzysztof Sommerfeld
/// @date	04.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class QMySqlDataBaseConnector final : public DataBaseConnectorInterface {
	QSqlDatabase db;
	QSqlQuery lastQuery;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void final::setDataBase(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept;
	///
	/// @brief	Sets data base connection credentials.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	mysqlHostName	Name of the mysql host.
	/// @param 	mysqlPort	 	The mysql port.
	/// @param 	mysqlDbName  	Name of the mysql database.
	/// @param 	user		 	The user.
	/// @param 	passwd		 	The passwd.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setDataBase(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void final::openConnection() noexcept;
	///
	/// @brief	Opens the connection.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void openConnection() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void final::closeConnection() noexcept;
	///
	/// @brief	Closes the connection.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void closeConnection() noexcept;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	final::QMySqlDataBaseConnector(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept;
	///
	/// @brief	Constructor, connects to the database using mysql.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	mysqlHostName	Name of the mysql host.
	/// @param 	mysqlPort	 	The mysql port.
	/// @param 	mysqlDbName  	Name of the mysql database.
	/// @param 	user		 	The user.
	/// @param 	passwd		 	The password.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QMySqlDataBaseConnector(const QString& mysqlHostName, const int mysqlPort, const QString& mysqlDbName, const QString& user, const QString& passwd) noexcept;
	~QMySqlDataBaseConnector() override;
	bool isConnectionOpen() const noexcept final;
	void executeSqlQuery(const QString& sqlQuery) noexcept final;
	bool wasLastQuerySuccesful() const noexcept final;
	QSqlQuery getLastQuery() const noexcept final;
	QSqlError getLastError() const noexcept final;
};
