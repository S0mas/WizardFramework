#include "../include/ScpiDevice.h"
#include <bitset>

QString ScpiDevice::readResponse() const {
	unsigned long ret = 1;
	char data[256];
	connector_->call([this](auto vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) { return scpiIF_->read(vi, dataPtr, size, ret); }, "SCPI buffor read:", reinterpret_cast<unsigned char*>(data), sizeof(data) - 1, &ret);
	data[ret] = '\0';
	return data;
}

void ScpiDevice::scpiCmd(const QString& cmd) const {
	unsigned long ret;
	connector_->call([this](auto vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) { return scpiIF_->write(vi, dataPtr, size, ret); }, QString("SCPI cmd sended: %1").arg(cmd), reinterpret_cast<unsigned char*>(cmd.toStdString().data()), strlen(cmd.toStdString().data()) + 1, &ret);
}

ScpiDevice::ScpiDevice(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF, QObject* parent) noexcept : AbstractDevice(nameId, connector, parent), scpiIF_(scpiIF) {}
