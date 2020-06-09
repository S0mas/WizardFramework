#pragma once
#include "AbstractDevice.h"
#include "AbstractHardwareConnector.h"
#include "ScpiIF.h"
#include <mutex>
#include <optional>
#include <bitset>

class ScpiDevice  : public AbstractDevice {
	Q_OBJECT
	ScpiIF* scpiIF_;
	mutable std::mutex hwAccess_;
	mutable QString response_;
	mutable QString lastErrorMsg_;
	bool enableCmdPrints_ = false;
	void scpiCmd(const QString& cmd) const;

	QString readError() const noexcept {
		scpiCmd("SYSTem:ERR?");
		return readResponse();
	}

	QString readResponse() const;

	bool succeeded() const noexcept {
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
public:
	ScpiDevice(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF = new ScpiIFMock, QObject* parent = nullptr) noexcept;
	~ScpiDevice() override = default;
	
	bool invokeCmd(const QString& cmd) const noexcept {
		std::lock_guard lock(hwAccess_);
		scpiCmd(cmd);
		if (enableCmdPrints_)
			qDebug() << cmd;
		return succeeded();
	}

	std::optional<QString> invokeQuery(const QString& cmd) const noexcept {
		std::lock_guard lock(hwAccess_);
		scpiCmd(cmd);
		if (enableCmdPrints_)
			qDebug() << cmd;
		if (succeeded())
			return response_;
		return std::nullopt;
	}

	std::optional<QString> invokeQuery(const QString& cmd, QString& eventualyErrorMsg) const noexcept {
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

	void enableScpiCommandsPrints(bool const enable) noexcept {
		enableCmdPrints_ = enable;
	}
public slots:
	void asyncInvoke(const QString& cmd) {
		QString error;
		auto resp = invokeQuery(cmd, error);
		emit sendResponse(resp.has_value(), resp.has_value() ? *resp : error);
	}
signals:
	void sendResponse(bool const status, QString const& resp);
};