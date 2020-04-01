#pragma once
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "DataBasseConnector.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DatabaseHandler
///
/// @brief	A database handler.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DatabaseHandler {
	std::unique_ptr<DataBaseConnectorInterface> dbConnection_ = nullptr;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	DatabaseHandler::DatabaseHandler()
	///
	/// @brief	Default constructor, it tries to connect to the database and reports error if fails.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	DatabaseHandler() {
		dbConnection_ = std::make_unique<QMySqlDataBaseConnector>("192.168.168.222", 3306, "prodtest", "operator", "EG1878-Nd");
		if (!dbConnection_->isConnectionOpen())
			qDebug() << "Error while connecting Database! :" << dbConnection_->getLastError().text();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QStringList DatabaseHandler::users() noexcept
	///
	/// @brief	Gets the users list from the database.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	A QStringList.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QStringList users() noexcept {
		dbConnection_->executeSqlQuery("SELECT uname FROM users");
		auto query = dbConnection_->getLastQuery();
		QStringList users;
		while (query.next())
			users.append(query.value(0).toString());
		users.append("TEST");

		return users;
	}
};