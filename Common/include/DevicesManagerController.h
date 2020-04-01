#pragma once
#include <QObject>
#include <QString>
#include <map>
#include "DevicesManager.h"
#include "LoggingObject.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DevicesManagerController
///
/// @brief	A controller for handling devices managers and to communicate with it.
///
/// @author	Krzysztof Sommerfeld
/// @date	04.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DevicesManagerController : public LoggingObject {
	Q_OBJECT
	std::map<QString, bool> connectionsStatuses_;
public:
	Q_PROPERTY(bool connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	DevicesManagerController::DevicesManagerController(QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	DevicesManagerController(QObject* parent = nullptr) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	DevicesManagerController::~DevicesManagerController() override = default;
	///
	/// @brief	Default destructor.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~DevicesManagerController() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::registerDevices(const QStringList& devicesNamesIds);
	///
	/// @brief	Registers the devices described by devicesNamesIds into the connectionStatuses map.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	devicesNamesIds	List of devices names identifiers.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void registerDevices(const QStringList& devicesNamesIds);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::connectDevicesManager(const DevicesManager* devicesManager) noexcept;
	///
	/// @brief	Connects the DevicesManager object with this controller.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	devicesManager	Manager for devices.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connectDevicesManager(const DevicesManager* devicesManager) noexcept;
public slots:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool DevicesManagerController::connectionStatus() const noexcept;
	///
	/// @brief	Checks if controller is not empty and if all handled devices connection statuses are
	/// 		ok.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	True or fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool connectionStatus() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::setConnectionStatus(const QString& deviceNameId, const bool status) noexcept;
	///
	/// @brief	Sets connection status.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	deviceNameId	The device name identifier..
	/// @param 	status			The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setConnectionStatus(const QString& deviceNameId, const bool status) noexcept;
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::connectDevices() const noexcept;
	///
	/// @brief	Input signal. Forwards command to start the connecting devices procedure.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void connectDevices() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::disconnectDevices() const noexcept;
	///
	/// @brief	Input signal. Forwards command to start the disconnecting devices procedure.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void disconnectDevices() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::reportError(const QString& errorMsg) const;
	///
	/// @brief	Reports an error
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	errorMsg	Message describing the error.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void reportError(const QString& errorMsg) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::connectionStatusChanged(const bool status) const;
	///
	/// @brief	Input signal. Indicates that the connection status has been changed.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	status	True to status.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connectionStatusChanged(const bool status) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void DevicesManagerController::connectionProcedureCompleted() const;
	///
	/// @brief	Input signal. Indicates that the devices connection procedure has been completed.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connectionProcedureCompleted() const;
};