#pragma once
#include "PrintInterface.h"
#include <QThread>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	LoggingObject
///
/// @brief	A logging object class - it should be inherited by any object that needs to handle logging from its components or connected objects.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LoggingObject : public QObject {
	Q_OBJECT
protected:
	PrintInterface printer;
	QThread printerThread;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	LoggingObject::LoggingObject(QObject* parent = nullptr) noexcept
	///
	/// @brief	Constructor, starts the printer thread.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	LoggingObject(QObject* parent = nullptr) noexcept : QObject(parent) {
		connect(&printerThread, &QThread::started, &printer, &PrintInterface::printingProcess);
		connect(&printer, &PrintInterface::printLog, this, &LoggingObject::logMsg);
		printer.moveToThread(&printerThread);
		printerThread.start();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	LoggingObject::~LoggingObject() override
	///
	/// @brief	Destructor, stops the printer thread.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~LoggingObject() override {
		printerThread.requestInterruption();
		printerThread.quit();
		printerThread.wait();
	}
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void LoggingObject::logMsg(const QString& msg) const;
	///
	/// @brief	Sends the log message.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	msg	The message.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void logMsg(const QString& msg) const;
};