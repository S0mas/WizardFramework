#include "../include/HardwareConnector6100.h"
#include "../include/Resource.h"
#include "../../../bu6100/include/bu6100.h"
#include "../../../visa/include/visa.h"

std::string HardwareConnector6100::resource() const noexcept {
	return QString("TCPIP0::%1::INSTR").arg(ipResource->value()).toStdString();
}

bool HardwareConnector6100::isConnectionPossible() const {
	ViSession rmSession = 0;
	ViSession session = 0;
	auto result = innerCall(viOpenDefaultRM, "viOpenDefaultRM", &rmSession) >= 0 && innerCall(viOpen, "viOpen", rmSession, resource().data(), 0, 0, &session) >= 0;
	if (session > 0)
		innerCall(viClose, "viClose", session);
	if (rmSession > 0)
		innerCall(viClose, "viClose", rmSession);
	return result;
}

void HardwareConnector6100::connectImpl() noexcept {
	ipResource->load();
	innerCall(bu6100_init, "bu6100_init", resource().data(), true, false, &vi_);
}

void HardwareConnector6100::disconnectImpl() noexcept {
	call(bu6100_close, "bu6100_close");
	vi_ = 0;
}

HardwareConnector6100::HardwareConnector6100(const QString& nameId, QObject* parent) noexcept : AbstractHardwareConnector(parent) {
	ipResource = std::make_shared<QSettingsResource>("ip" + nameId, nameId + " IP", Enums::GUI_Type::TYPE_1, std::make_unique<IPContentValidation>());
	errorChecker = std::make_unique<ErrorChecker>(&bu6100_error_message);
}

std::vector<std::shared_ptr<AbstractDataResource>> HardwareConnector6100::resources() const noexcept {
	return { ipResource };
}
