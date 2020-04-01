#include "../include/HardwareConnector6716.h"
#include "../include/Resource.h"
#include <bu6716.h>
#include <visa.h>

std::string HardwareConnector6716::resource() const noexcept {
	return QString("TCPIP0::%1::5025::SOCKET").arg(ipResource->value()).toStdString();
}

bool HardwareConnector6716::isConnectionPossible() const {
	ViSession rmSession = 0;
	ViSession session = 0;
	auto result = innerCall(viOpenDefaultRM, "viOpenDefaultRM", &rmSession) >= 0 && innerCall(viOpen, "viOpen", rmSession, resource().data(), 0, 0, &session) >= 0;
	if (session > 0)
		innerCall(viClose, "viClose", session);
	if (rmSession > 0)
		innerCall(viClose, "viClose", rmSession);
	return result;
}

void HardwareConnector6716::connectImpl() noexcept {
	ipResource->load();
	innerCall(bu6716_init, "bu6716_init", resource().data(), true, true, &vi_);
}

void HardwareConnector6716::disconnectImpl() noexcept {
	call(bu6716_close, "bu6716_close");
	vi_ = 0;
}

HardwareConnector6716::HardwareConnector6716(const QString& nameId, QObject* parent) noexcept : AbstractHardwareConnector(parent) {
	ipResource = std::make_shared<QSettingsResource>("ip" + nameId, nameId + " IP", Enums::GUI_Type::TYPE_1, std::make_unique<IPContentValidation>());
	errorChecker = std::make_unique<ErrorChecker>(&bu6716_error_message);
}

std::vector<std::shared_ptr<AbstractDataResource>> HardwareConnector6716::resources() const noexcept {
	return { ipResource };
}
