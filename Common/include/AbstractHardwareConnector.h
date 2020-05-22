#pragma once
#include "AbstractDataResource.h"
#include "ErrorChecker.h"
#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	AbstractHardwareConnector
///
/// @brief	Responsible for mantain the connection to the device and for controls it.
///
/// @author	Krzysztof Sommerfeld
/// @date	05.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractHardwareConnector : public QObject {
	Q_OBJECT
	mutable bool monitorConnection_ = false;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractHardwareConnector::clearSession() noexcept;
	///
	/// @brief	Clears the session.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void clearSession() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool AbstractHardwareConnector::isSessionAvailable() const noexcept;
	///
	/// @brief	Queries if the session with the device is available
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	True if the session is available, false if not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool isSessionAvailable() const noexcept;
private slots:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractHardwareConnector::sendConnectionStatus() const noexcept;
	///
	/// @brief	Sends the connection status and starts the timer for invoking itself.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void sendConnectionStatus() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual bool AbstractHardwareConnector::isConnectionPossible() const = 0;
	///
	/// @brief	Queries if the connection to the device is possible.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	True if connection possible, false if not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual bool isConnectionPossible() const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void AbstractHardwareConnector::connectImpl() noexcept = 0;
	///
	/// @brief	The implementation of connect procedure.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void connectImpl() noexcept = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void AbstractHardwareConnector::disconnectImpl() noexcept = 0;
	///
	/// @brief	The implementation of disconnect procedure.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void disconnectImpl() noexcept = 0;
protected:
	std::unique_ptr<ErrorChecker> errorChecker = nullptr;
	unsigned long vi_ = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	template<typename Function, typename ...Args> int AbstractHardwareConnector::innerCall(Function&& function, const QString& functionName, Args&& ...arguments) const noexcept
	///
	/// @brief	Same as call but it doesnt add the vi of the connector to the call.
	///
	/// @tparam	Function	Type of the function.
	/// @tparam	...Args 	Types of the arguments.
	/// @param function		The function.
	/// @param functionName	Name of the function.
	/// @param ...arguments The arguments pack.
	///
	/// @returns	The status of the function call.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Function, typename ...Args>
	int innerCall(Function&& function, const QString& functionName, Args&& ...arguments) const noexcept {
		//logMsg(QString("Called %1").arg(functionName);// todo print args
		auto status = errorChecker->checkError(std::forward<Function>(function), std::forward<Args>(arguments)...);
		if (auto const error = lastError())
			emit reportError(*error);
		return status;
	}
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractHardwareConnector::AbstractHardwareConnector(QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractHardwareConnector(QObject* parent = nullptr) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractHardwareConnector::~AbstractHardwareConnector() override = default;
	///
	/// @brief	Default destructor.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~AbstractHardwareConnector() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual std::vector<std::shared_ptr<AbstractDataResource>> AbstractHardwareConnector::resources() const noexcept = 0;
	///
	/// @brief	Gets the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The connector resources.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual std::vector<std::shared_ptr<AbstractDataResource>> resources() const noexcept = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	unsigned long AbstractHardwareConnector::vi() const noexcept;
	///
	/// @brief	Gets the vi.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	A vi session of the conector.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned long vi() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	template<typename Function, typename ...Args> int AbstractHardwareConnector::call(Function&& function, const QString& functionName, Args&& ...arguments) const noexcept
	///
	/// @brief	Calls a function and forwards its reports error if it occured.
	///
	/// @tparam	Function	Type of the function.
	/// @tparam	...Args 	Types of the arguments.
	/// @param function		The function.
	/// @param functionName	Name of the function.
	/// @param ...arguments The arguments pack.
	///
	/// @returns	The status of the function call.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Function, typename ...Args>
	int call(Function&& function, const QString& functionName, Args&& ...arguments) const noexcept {
		//logMsg(QString("Called %1").arg(functionName);// todo print args
		auto status = errorChecker->checkError(vi(), std::forward<Function>(function), std::forward<Args>(arguments)...);
		if (auto const error = lastError())
			emit reportError(*error);
		return status;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractHardwareConnector::connect() noexcept;
	///
	/// @brief	Connects to the hardware.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connect() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractHardwareConnector::disconnect() noexcept;
	///
	/// @brief	Disconnects from the hardware.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void disconnect() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::optional<QString> AbstractHardwareConnector::lastError() const noexcept;
	///
	/// @brief	Gets the last error occured.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The last error occured.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::optional<QString> lastError() const noexcept;
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractHardwareConnector::connectionStatus(const bool status) const;
	///
	/// @brief	The output signal. Forwards the connection status.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	status	value of status.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void connectionStatus(const bool status) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractHardwareConnector::reportError(const QString& errorMsg) const;
	///
	/// @brief	Reports an error.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	errorMsg	Message describing the error.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void reportError(const QString& errorMsg) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractHardwareConnector::logMsg(const QString& errorMsg) const;
	///
	/// @brief	Logs a message.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	errorMsg	Message describing the error.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void logMsg(const QString& errorMsg) const;
};


