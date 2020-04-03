#pragma once
#include "AbstractHardwareConnector.h"
#include <QObject>
#include "UserCommunicationObject.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	AbstractDevice
///
/// @brief	An abstract device class. Responsible for communication with the hardware.
///
/// @author	Krzysztof Sommerfeld
/// @date	04.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractDevice : public UserCommunicationObject {
	Q_OBJECT
	QString nameId_;
	bool connected_ = false;
private slots:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDevice::setConnectionStatus(const bool status) noexcept;
	///
	/// @brief	Sets the connection status.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	status	The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setConnectionStatus(const bool status) noexcept;
protected:
	AbstractHardwareConnector* connector_;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractDevice::AbstractDevice(const QString& nameId, AbstractHardwareConnector* connector, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 		   	nameId   	The name identifier.
	/// @param [in]	connector	The connector to the device - it takes the ownership of it.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractDevice(const QString& nameId, AbstractHardwareConnector* connector, QObject* parent = nullptr) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractDevice::~AbstractDevice() override = default;
	///
	/// @brief	Default destructor.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~AbstractDevice() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	template<typename Function, typename ...Args> int AbstractDevice::invokeFunction(Function&& function, const QString& functionName, Args&& ...arguments) const
	///
	/// @brief	Executes the function and handles the error if occured.
	///
	/// @tparam	Function	Type of the function.
	/// @tparam	...Args 	Types of the arguments.
	/// @param function		The function.
	/// @param functionName	Name of the function.
	/// @param ...arguments The arguments pack.
	///
	/// @returns	The status returned by the function call.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Function, typename ...Args>
	int invokeFunction(Function&& function, const QString& functionName, Args&& ...arguments) const {
		auto status = connector_->call(std::forward<Function>(function), functionName, std::forward<Args>(arguments)...);
		if (auto const error = connector_->lastError())
			sender_->waitForErrorHandlingType(*error);
		return status;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractDevice::nameId() const noexcept;
	///
	/// @brief	Gets the name identifier.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	A QString.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString nameId() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDevice::connect() const noexcept;
	///
	/// @brief	Connects this device.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connect() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	auto AbstractDevice::inputResources() const noexcept
	///
	/// @brief	Gets the resources of the device that needs some user input.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The vector of input resources.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	auto AbstractDevice::inputResources() const noexcept {
		return connector_->resources();
	}
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDevice::connectionStatusChanged(const QString& deviceNameId, const bool connected) const;
	///
	/// @brief	Output signal. Indicates the change of the connectionStatus member.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	deviceNameId	The device name identifir.
	/// @param 	connected   	The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connectionStatusChanged(const QString& deviceNameId, const bool connected) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDevice::reportError(const QString& errorMsg) const;
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
	/// @fn	void AbstractDevice::logMsg(const QString& msg) const;
	///
	/// @brief	Logs a message.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	msg	The message.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void logMsg(const QString& msg) const;
};
