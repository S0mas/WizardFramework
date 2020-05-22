#include "../include/HardwareConnectorMock.h"
#include "../../Common/include/Resource.h"
#include <QString>
#include <QDebug>

bool HardwareConnectorMock::isConnectionPossible() const {
	qDebug() << QString("Checking '%1' connection status").arg(ipResource->nameId());
	return true;
}

void HardwareConnectorMock::connectImpl() noexcept {
	qDebug() << QString("Connected to '%1'").arg(ipResource->nameId());
	vi_ = 1;
}

void HardwareConnectorMock::disconnectImpl() noexcept {
	qDebug() << QString("Disconnected from '%1'").arg(ipResource->nameId());
	vi_ = 0;
}

HardwareConnectorMock::HardwareConnectorMock(const QString& nameId, QObject* parent) noexcept : AbstractHardwareConnector(parent) {
	ipResource = std::make_shared<QSettingsResource>("ip" + nameId, nameId + " ip", Enums::GUI_Type::TYPE_1, std::make_unique<IPContentValidation>());
	errorChecker = std::make_unique<ErrorChecker>([](...) {return 0; });
}

std::vector<std::shared_ptr<AbstractDataResource>> HardwareConnectorMock::resources() const noexcept {
	return { ipResource };
}
