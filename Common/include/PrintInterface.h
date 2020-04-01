#pragma once
#include <QString>
#include <QObject>
#include <QThread>
#include <thread>
#include <chrono>
#include <QCoreApplication>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	PrintInterface
///
/// @brief	A print interface. Decreases the number of signals for msg logging.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PrintInterface : public QObject {
	Q_OBJECT
	mutable QString logs;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	PrintInterface::PrintInterface(QObject* parent = nullptr) noexcept
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	PrintInterface(QObject* parent = nullptr) noexcept : QObject(parent) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	PrintInterface::~PrintInterface() override
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~PrintInterface() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void PrintInterface::printingProcess() const noexcept
	///
	/// @brief	Handles incoming signals and forwards gathered log every half second.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void printingProcess() const noexcept {
		while (!QThread::currentThread()->isInterruptionRequested()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			if (!logs.isEmpty()) {
				emit printLog(logs);
				logs.clear();
			}
			QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
		}
	}
public slots:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void PrintInterface::addLog(const QString& msg) const noexcept
	///
	/// @brief	Adds a msg to the log member.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	msg	The message.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void addLog(const QString& msg) const noexcept {
		logs += msg + "\n";
	}
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void PrintInterface::printLog(const QString& msg) const;
	///
	/// @brief	Forwards the log msg.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	msg	The message.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void printLog(const QString& msg) const;
};