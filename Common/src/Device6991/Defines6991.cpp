#include "../../include/Device6991/Defines6991.h"
#include <bitset>

std::array<bool, 4> DeviceState::linksConnectionStatus() const noexcept {
	std::bitset<32> set(data_);
	return { set[28], set[29], set[30], set[31] };
}

void DeviceState::setLinksConnectionStatus(std::array<bool, 4> const& linksStatus) noexcept {
	std::bitset<32> set(data_);
	set[31] = linksStatus[3];
	set[30] = linksStatus[2];
	set[29] = linksStatus[1];
	set[28] = linksStatus[0];
	data_ = set.to_ulong();
}

bool DeviceState::linksConnectionStatus(uint32_t const linkIndex) const noexcept {
	std::bitset<32> set(data_);
	return set[28 + linkIndex];
}

bool DeviceState::fifoUnderflow() const noexcept {
	std::bitset<32> set(data_);
	return set[12];
}

bool DeviceState::fifoOverflow() const noexcept {
	std::bitset<32> set(data_);
	return set[13];
}

void DeviceState::setFifoUnderflow(bool const state) noexcept {
	std::bitset<32> set(data_);
	set[12] = state;
}

void DeviceState::setFifoOverflow(bool const state) noexcept {
	std::bitset<32> set(data_);
	set[13] = state;
}

bool DeviceState::acquisitionStoppedOnError() const noexcept {
	std::bitset<32> set(data_);
	return set[16];
}

void DeviceState::setAcquisitionStoppedOnError(bool const state) noexcept {
	std::bitset<32> set(data_);
	set[16] = state;
}

uint32_t DeviceState::numberOfScansInFifo() const noexcept {
	return data_ & 0xFFF;
}

void DeviceState::setNumberOfScansInFifo(uint32_t const numbersOfScansInFifo) noexcept {
	data_ &= 0xFFFFF000;
	data_ |= (numbersOfScansInFifo & 0xFFF);
}

void DeviceState::set(QString const& hexString) noexcept {
	bool conversionStatus;
	uint32_t data = hexString.toUInt(&conversionStatus, 16);
	if (!conversionStatus)
		qDebug() << "Conversion Error";
	else
		data_ = data;
}

DeviceStateEnum::Type DeviceState::state() const noexcept {
	return state_;
}

void DeviceState::setState(DeviceStateEnum::Type const state) noexcept {
	state_ = state;
}

uint32_t DeviceState::toUInt() const noexcept {
	return data_;
}

void DeviceState::setControllerId(std::optional<uint32_t> const& controllerId) noexcept {
	controllerId_ = controllerId;
}

std::optional<uint32_t> DeviceState::controllerId() const noexcept {
	return controllerId_;
}
