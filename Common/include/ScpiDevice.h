#pragma once
#include "AbstractDevice.h"
#include "AbstractHardwareConnector.h"

class ScpiDevice  : public AbstractDevice {
	Q_OBJECT
protected:
	QString readResponse() const;
	void scpiCmd(const QString& cmd) const;
public:
	ScpiDevice(const QString& nameId, AbstractHardwareConnector* connector, QObject* parent = nullptr) noexcept;
	~ScpiDevice() override = default;
};