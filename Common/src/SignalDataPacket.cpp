#include "../include/SignalDataPacket.h"
#include <bitset>

std::array<bool, 4> Status1Part::linksConnectionStatus() const noexcept {
	std::bitset<32> bits(data_);
	return { bits[31], bits[30], bits[29], bits[28] };
}

bool Status1Part::linksConnectionStatus(uint32_t const linkIndex) const noexcept {
	std::bitset<32> bits(data_);
	return bits[28 + linkIndex];
}

bool Status1Part::fifoUnderflow() const noexcept {
	return data_ & (1 << 16);
}

bool Status1Part::fifoOverflow() const noexcept {
	return data_ & (1 << 17);
}
