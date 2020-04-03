#pragma once
#include "AbstractDevice.h"
#include "AbstractHardwareConnector.h"
#include "ScpiIF.h"

class ScpiDevice  : public AbstractDevice {
	Q_OBJECT
	ScpiIF* scpiIF_;
protected:
	QString readResponse() const;
	void scpiCmd(const QString& cmd) const;
public:
	ScpiDevice(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF = new ScpiIFMock, QObject* parent = nullptr) noexcept;
	~ScpiDevice() override = default;
};