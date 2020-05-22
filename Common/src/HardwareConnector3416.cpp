#include "../include/HardwareConnector3416.h"
#include "../include/Resource.h"
#include "../../../bu3416/include/bu3416.h"

std::string HardwareConnector3416::resource() const noexcept {
	return QString("TCPIP0::%1::INSTR").arg(ipResource->value()).toStdString();
}

int HardwareConnector3416::cardId() const noexcept {
	return fcIdResource->value().toInt();
}

bool HardwareConnector3416::isConnectionPossible() const {
	ViInt32 n1, n2;
	return call(bu3416_getCurrentTime, "bu3416_getCurrentTime", &n1, &n2) >= 0; //just checking if connection is ok
}

void HardwareConnector3416::connectImpl() noexcept {
	ipResource->load();
	fcIdResource->load();
	innerCall(bu3416_paramInit, "bu3416_paramInit", resource().data(), cardId(), true, true, &vi_);
}

void HardwareConnector3416::disconnectImpl() noexcept {
	call(bu3416_close, "bu3416_close");
	vi_ = 0;
}

HardwareConnector3416::HardwareConnector3416(const QString& nameId, const QString& motherBoardNameId, QObject* parent) noexcept : AbstractHardwareConnector(parent) {
	ipResource = std::make_shared<QSettingsResource>("ip" + motherBoardNameId, motherBoardNameId + " IP", Enums::GUI_Type::TYPE_1, std::make_unique<IPContentValidation>());
	fcIdResource = std::make_shared<QSettingsResource>("fcId" + nameId, nameId + " FC index", Enums::GUI_Type::TYPE_1, std::make_unique<FunctionCardIdContentValidation>());
	errorChecker = std::make_unique<ErrorChecker>(&bu3416_error_message);
}

const ViSession& HardwareConnector3416::refereceToViSession() const noexcept {
	return vi_;
}

std::vector<std::shared_ptr<AbstractDataResource>> HardwareConnector3416::resources() const noexcept {
	return { ipResource, fcIdResource };
}
