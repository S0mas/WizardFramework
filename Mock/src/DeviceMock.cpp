#include "../include/DeviceMock.h"

void DeviceMock::saveSerialnumber(const QString& str) const {
	qDebug() << "serial number saved";
}

void DeviceMock::saveSubtype(const QString& str) const {
	qDebug() << "subtype saved";
}

void DeviceMock::saveFirmwareRevision(const QString& str) const {
	qDebug() << "firmware revision saved";
}

void DeviceMock::saveDriverRevision(const QString& str) const {
	qDebug() << "driver revision saved";
}

QString DeviceMock::loadSerialnumber() const {
	return "12345678";
}

QString DeviceMock::loadSubtype() const {
	return "XX";
}

QString DeviceMock::loadFirmwareRevision() const {
	return "1.00000";
}

QString DeviceMock::loadDriverRevision() const {
	return "2.00000";
}

DeviceMock::DeviceMock(const QString& nameId, QObject* parent) noexcept : AbstractDevice(nameId, new HardwareConnectorMock(nameId), parent), DeviceIdentityResourcesIF(nameId), ChannelsIF(32) {}
