#pragma once
#include "../include/DevicesManagerController.h"

DevicesManagerController::DevicesManagerController(QObject* parent) noexcept : LoggingObject(parent) {}

void DevicesManagerController::registerDevices(const QStringList& devicesNamesIds) {
	for (auto const& nameId : devicesNamesIds)
		connectionsStatuses_[nameId] = false;
}

void DevicesManagerController::connectDevicesManager(const DevicesManager* devicesManager) noexcept {
	for (auto const& device : devicesManager->devices())
		QObject::connect(device, &AbstractDevice::connectionStatusChanged, this, &DevicesManagerController::setConnectionStatus);
	QObject::connect(this, &DevicesManagerController::connectDevices, devicesManager, &DevicesManager::connectDevices);
	QObject::connect(this, &DevicesManagerController::disconnectDevices, devicesManager, &DevicesManager::disconnectDevices);
	QObject::connect(devicesManager, &DevicesManager::connectionProcedureCompleted, this, &DevicesManagerController::connectionProcedureCompleted);
	QObject::connect(devicesManager, &DevicesManager::reportError, this, &DevicesManagerController::reportError);

	// connect to printer
	QObject::connect(devicesManager, &DevicesManager::logMsg, &printer, &PrintInterface::addLog);
}

bool DevicesManagerController::connectionStatus() const noexcept {
	for (auto const& record : connectionsStatuses_)
		if (!record.second)
			return false;
	return !connectionsStatuses_.empty();
}

void DevicesManagerController::setConnectionStatus(const QString& deviceNameId, const bool status) noexcept {
	if (connectionsStatuses_.at(deviceNameId) != status) {
		connectionsStatuses_[deviceNameId] = status;
		emit connectionStatusChanged(connectionStatus());
	}
}
