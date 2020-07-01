#pragma once
#include "../include/AbstractDevice.h"

void AbstractDevice::setConnectionStatus(const bool status) {
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

void AbstractDevice::connect() const noexcept {
	logMsg(QString("Connecting %1..").arg(nameId()));
	connector_->connect();
}

void AbstractDevice::disconnect() const noexcept {
	logMsg(QString("Disconnecting %1..").arg(nameId()));
	connector_->disconnect();
}

void AbstractDevice::setConnectionMonitor(bool const state) const noexcept {
	logMsg(QString("Turning connection monitro %1..").arg(state ? "ON" : "OFF"));
	connector_->setConnectionMonitor(state);
}
