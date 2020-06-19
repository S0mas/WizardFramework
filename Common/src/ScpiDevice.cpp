#include "../include/ScpiDevice.h"
#include <bitset>

QString ScpiDevice::readResponse() const {
	unsigned long ret = 1;
	char data[256];
	connector_->call([this](auto vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) { return scpiIF_->read(vi, dataPtr, size, ret); }, "SCPI buffor read:", reinterpret_cast<unsigned char*>(data), sizeof(data) - 1, &ret);
	data[ret] = '\0';
	return data;
}

bool ScpiDevice::succeeded() const noexcept {
	response_ = readResponse();
	if (enableCmdPrints_)
		qDebug() << "Response:" << response_;
	scpiCmd("*STB?");
	std::bitset<8> stb = readResponse().toUInt(nullptr, 16);
	bool eva = stb[2];
	if (eva) {
		lastErrorMsg_ = readError();
		emit reportError(lastErrorMsg_);
		return false;
	}
	return true;
}

void ScpiDevice::scpiCmd(const QString& cmd) const {
	unsigned long ret;
	connector_->call([this](auto vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) { return scpiIF_->write(vi, dataPtr, size, ret); }, QString("SCPI cmd sended: %1").arg(cmd), reinterpret_cast<unsigned char*>(cmd.toStdString().data()), strlen(cmd.toStdString().data()) + 1, &ret);
}

QString ScpiDevice::readError() const noexcept {
	scpiCmd("SYSTem:ERR?");
	return readResponse();
}

ScpiDevice::ScpiDevice(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF, QObject* parent) noexcept : AbstractDevice(nameId, connector, parent), scpiIF_(scpiIF) {}

bool ScpiDevice::invokeCmd(const QString& cmd) const noexcept {
	std::lock_guard lock(hwAccess_);
	scpiCmd(cmd);
	if (enableCmdPrints_)
		qDebug() << cmd;
	return succeeded();
}

std::optional<QString> ScpiDevice::invokeQuery(const QString& cmd) const noexcept {
	std::lock_guard lock(hwAccess_);
	scpiCmd(cmd);
	if (enableCmdPrints_)
		qDebug() << cmd;
	if (succeeded())
		return response_;
	return std::nullopt;
}

std::optional<QString> ScpiDevice::invokeQuery(const QString& cmd, QString& eventualyErrorMsg) const noexcept {
	std::lock_guard lock(hwAccess_);
	scpiCmd(cmd);
	if (enableCmdPrints_)
		qDebug() << cmd;
	if (succeeded())
		return response_;
	else
		eventualyErrorMsg = lastErrorMsg_;
	return std::nullopt;
}

void ScpiDevice::enableScpiCommandsPrints(bool const enable) noexcept {
	enableCmdPrints_ = enable;
}

void ScpiDevice::asyncInvoke(const QString& cmd) {
	QString error;
	auto resp = invokeQuery(cmd, error);
	emit sendResponse(resp.has_value(), resp.has_value() ? *resp : error);
}
