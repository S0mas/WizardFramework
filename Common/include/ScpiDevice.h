#pragma once
#include "AbstractDevice.h"
#include "AbstractHardwareConnector.h"
#include "ScpiIF.h"
#include <mutex>
#include <optional>

class ScpiDevice  : public AbstractDevice {
	Q_OBJECT
	ScpiIF* scpiIF_;
	mutable std::mutex hwAccess_;
	mutable QString response_;
	mutable QString lastErrorMsg_;
	bool enableCmdPrints_ = false;
	void scpiCmd(const QString& cmd) const;

	QString readError() const noexcept;
	QString readResponse() const;
	bool succeeded() const noexcept;
public:
	ScpiDevice(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF = new ScpiIFMock, QObject* parent = nullptr) noexcept;
	~ScpiDevice() override = default;
	bool invokeCmd(const QString& cmd) const noexcept;
	std::optional<QString> invokeQuery(const QString& cmd) const noexcept;
	std::optional<QString> invokeQuery(const QString& cmd, QString& eventualyErrorMsg) const noexcept;
	void enableScpiCommandsPrints(bool const enable) noexcept;
public slots:
	void asyncInvoke(const QString& cmd);
signals:
	void sendResponse(bool const status, QString const& resp);
};