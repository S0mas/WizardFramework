#include "../include/AbstractHardwareConnector.h"
#include <QTimer>

void AbstractHardwareConnector::clearSession() noexcept {
	vi_ = 0;
}

bool AbstractHardwareConnector::isSessionAvailable() const noexcept {
	return vi_ != 0;
}

void AbstractHardwareConnector::sendConnectionStatus() const noexcept {
	emit connectionStatus(isSessionAvailable() && isConnectionPossible());
	if (monitorConnection_)
		QTimer::singleShot(2000, this, &AbstractHardwareConnector::sendConnectionStatus);
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
		sendConnectionStatus();
	}
	else
		logMsg("FAILED");
}

void AbstractHardwareConnector::disconnect() noexcept {
	if (isSessionAvailable()) {
		disconnectImpl();
		clearSession();
		monitorConnection_ = false;
		sendConnectionStatus();		
	}
}

std::optional<QString> AbstractHardwareConnector::lastError() const noexcept {
	return errorChecker->errorOccured() ? std::optional<QString>(errorChecker->lastErrorMsg()) : std::nullopt;
}
