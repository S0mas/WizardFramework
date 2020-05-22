#pragma once
#include <QString>
#include "Defines6991.h"

class Command {
	uint32_t cmdValue_;
	uint32_t registerAddress_;
	uint32_t data_;
public:
	Command(uint32_t const cmdValue, uint32_t const registerAddress, uint32_t const data) : cmdValue_(cmdValue), registerAddress_(registerAddress), data_(data) {}

	QString toString() const noexcept {
		return QString("cmd: %1 address: %2 data: %3").arg(toHex(cmdValue_, 2), toHex(registerAddress_, 4), toHex(data_, 8));
	}
};