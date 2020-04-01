#pragma once
#include "../include/DevicesManager.h"

void DevicesManager::connectDevices() const noexcept {
	for (auto const& device : devices_)
		device->connect();
	emit connectionProcedureCompleted();
}

void DevicesManager::disconnectDevices() const noexcept {
	for (auto const& device : devices_)
		device->connect();
}

void DevicesManager::addDevice(AbstractDevice* device) noexcept {
	device->setParent(this);
	connect(device, &AbstractDevice::logMsg, this, &DevicesManager::logMsg);
	connect(device, &AbstractDevice::reportError, this, &DevicesManager::reportError);
	devices_.push_back(device);
}

AbstractDevice* DevicesManager::getDevice(const QString& nameId) noexcept {
	for (auto const& device : devices_)
		if (device->nameId() == nameId)
			return device;
	return nullptr;
}

const std::vector<AbstractDevice*>& DevicesManager::devices() const noexcept {
	return devices_;
}
