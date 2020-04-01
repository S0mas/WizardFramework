#include "../include/ScpiDevice.h"
#include <visa.h>

QString ScpiDevice::readResponse() const {
	unsigned long ret;
	char data[256];
	connector_->call(viRead, "SCPI buffor read:", reinterpret_cast<unsigned char*>(data), sizeof(data) - 1, &ret);
	data[ret - 1] = '\0';
	return data;
}

void ScpiDevice::scpiCmd(const QString& cmd) const {
	connector_->call(viWrite, QString("SCPI cmd sended: %1").arg(cmd), reinterpret_cast<unsigned char*>(cmd.toStdString().data()), strlen(cmd.toStdString().data()) + 1, nullptr);
}

ScpiDevice::ScpiDevice(const QString& nameId, AbstractHardwareConnector* connector, QObject* parent) noexcept : AbstractDevice(nameId, connector, parent) {}
