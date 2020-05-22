#pragma once
#include "..\include\ChannelsIF.h"

ChannelsIF::Channel::Channel() : index_(), enabled_(true) {}
ChannelsIF::Channel::Channel(const int index) noexcept : index_(index), enabled_(true) {}

int ChannelsIF::Channel::index() const noexcept {
	return index_;
}

bool ChannelsIF::Channel::enabled() const noexcept {
	return enabled_;
}

bool ChannelsIF::Channel::disabled() const noexcept {
	return !enabled_;
}

void ChannelsIF::Channel::enable() noexcept {
	enabled_ = true;
}

void ChannelsIF::Channel::disable() noexcept {
	enabled_ = false;
}

uint64_t ChannelsIF::Channel::mask() const noexcept {
	return 1ll << (index() - 1);
}

ChannelsIF::ChannelsIF(const uint32_t channelsNo) noexcept {
	for (int index = 1; index <= channelsNo; ++index)
		channels_.push_back(Channel(index));
}

std::vector<ChannelsIF::Channel>& ChannelsIF::channels() noexcept {
	return channels_;
}

const std::vector<ChannelsIF::Channel>& ChannelsIF::channels() const noexcept {
	return channels_;
}

std::vector<bool> ChannelsIF::channelsStates() const noexcept {
	std::vector<bool> vec;
	vec.reserve(channels_.size());
	for (auto const& channel : channels_)
		vec.push_back(channel.enabled());
	return vec;
}

void ChannelsIF::setChannelsStates(const std::vector<bool>& states) noexcept {
	for (auto& channel : channels_)
		(states[channel.index() - 1ll]) ? channel.enable() : channel.disable();
}

uint64_t ChannelsIF::channelsStateAsMask() const noexcept {
	uint64_t mask = 0;
	for (auto const& channel : channels_)
		if (channel.enabled()) mask |= 1ll << (channel.index() - 1);
	return mask;
}

QString ChannelsIF::channelsStateAsStrMask() const noexcept {
	QString mask;
	for (auto const& channel : channels_)
		mask.push_back(channel.enabled() ? '1' : '0');
	return mask;
}

void ChannelsIF::enableAll() noexcept {
	for (auto& channel : channels_)
		channel.enable();
}

void ChannelsIF::disableAll() noexcept {
	for (auto& channel : channels_)
		channel.disable();
}

QString ChannelsIF::toString() const noexcept {
	QString str;
	return str;
}