#include "../include/AbstractHardwareConnector.h"
#include <QTimer>

void AbstractHardwareConnector::clearSession() noexcept {
	vi_ = 0;
}

bool AbstractHardwareConnector::isSessionAvailable() const noexcept {
	return vi_ != 0;
}

void AbstractHardwareConnector::sendConnectionStatus() const noexcept {
	if (monitorConnection_) {
		QTimer::singleShot(2000, this, &AbstractHardwareConnector::sendConnectionStatus);
		emit connectionStatus(isSessionAvailable() && isConnectionPossible());
	}
}

AbstractHardwareConnector::AbstractHardwareConnector(QObject* parent) noexcept : QObject(parent)  {}

unsigned long AbstractHardwareConnector::vi() const noexcept {
	return vi_;
}

void AbstractHardwareConnector::connect() noexcept {
	disconnect();
	connectImpl();
	if (isSessionAvailable()) {
		logMsg(QString("Connection established, viSession: %1").arg(vi()));
		monitorConnection_ = true;
		sendConnectionStatus();
	}
	else
		logMsg("FAILED");
}

void AbstractHardwareConnector::disconnect() noexcept {
	if (isSessionAvailable()) {
		disconnectImpl();
		clearSession();
		emit connectionStatus(false);
		monitorConnection_ = false;
	}
}

std::optional<QString> AbstractHardwareConnector::lastError() const noexcept {
	return errorChecker->errorOccured() ? std::optional<QString>(errorChecker->lastErrorMsg()) : std::nullopt;
}
