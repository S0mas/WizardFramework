#pragma once
#include "../include/AbstractDevice.h"

void AbstractDevice::setConnectionStatus(const bool status) noexcept {
	if (connected_ != status) {
		connected_ = status;
		emit connectionStatusChanged(nameId_, connected_);
	}
}

AbstractDevice::AbstractDevice(const QString& nameId, AbstractHardwareConnector* connector, QObject* parent) noexcept : UserCommunicationObject(parent), nameId_(nameId), connector_(connector) {
	QObject::connect(connector_, &AbstractHardwareConnector::connectionStatus, this, &AbstractDevice::setConnectionStatus);
	QObject::connect(connector_, &AbstractHardwareConnector::reportError, this, &AbstractDevice::reportError);
	QObject::connect(connector_, &AbstractHardwareConnector::logMsg, this, &AbstractDevice::logMsg);
	connector_->setParent(this);
}

QString AbstractDevice::nameId() const noexcept {
	return nameId_;
}

bool AbstractDevice::connect() const noexcept {
	logMsg(QString("Connecting %1..").arg(nameId()));
	return connector_->connect();
}