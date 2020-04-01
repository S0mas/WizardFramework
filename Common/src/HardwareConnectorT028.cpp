#include "../include/HardwareConnectorT028.h"
#include <t028.h>

bool HardwareConnectorT028::isConnectionPossible() const {
	ViSession session = 0;
	auto result = false;
	{
		bu3100_genEnv_t* masterEnv = nullptr;
		ViChar descr[256];
		innerCall(viGetAttribute, "viGetAttribute", *masterViSession, VI_ATTR_RSRC_NAME, descr);
		innerCall(viGetAttribute, "viGetAttribute", *masterViSession, VI_ATTR_USER_DATA, &masterEnv);
		result = innerCall(viOpen, "viOpen", masterEnv->rmSession, descr, VI_NULL, VI_NULL, &session) >= 0;
	}
	if (session > 0)
		innerCall(viClose, "viClose", session);
	return result;
}

void HardwareConnectorT028::connectImpl() noexcept {
	innerCall(t028_init, "t028_init", *masterViSession, true, true, &vi_);
}

void HardwareConnectorT028::disconnectImpl() noexcept {
	call(t028_close, "t028_close");
	vi_ = 0;
}

HardwareConnectorT028::HardwareConnectorT028(const ViSession* masterViSession, QObject* parent) noexcept : AbstractHardwareConnector(parent), masterViSession(masterViSession) {
	errorChecker = std::make_unique<ErrorChecker>(&t028_error_message);
}

std::vector<std::shared_ptr<AbstractDataResource>> HardwareConnectorT028::resources() const noexcept {
	return {};
}
