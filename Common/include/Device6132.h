#pragma once
#include "Device6991/Device6991.h"

class Device6132 : public QObject {
	Q_OBJECT
	FecIdType::Type id_;
	Device6991* devIF_;
	CMD_reg CMD_reg_{ id_, devIF_ };
	ADC_SEL_reg ADC_SEL_reg_{ id_, devIF_ };
	std::vector<CHNX_RAW_SAMPLE_reg> channelsRegs_;

	double calculateVoltage(uint16_t const rawData) noexcept {
		double neutralZero = 0x8000;
		int16_t value =  static_cast<double>(rawData) - neutralZero;
		double lsb = 312.5; //microvolts
		double result = value * lsb;
		return result / 1000'000; //volts;
	}
public:
	Device6132(const FecIdType::Type& fcId, Device6991* dev6991, QObject* parent = nullptr) noexcept : id_(fcId), devIF_(dev6991) {
		for (int channelId = 1; channelId <= 16; ++channelId)
			channelsRegs_.push_back(CHNX_RAW_SAMPLE_reg(id_, channelId, devIF_));
	}
	~Device6132() = default;

	bool setGain(std::vector<uint32_t> const& channelIds, GainType6132::Type const gain) noexcept {
		return devIF_->setGain(gain, channelIds);
	}

	bool setFilter(std::vector<uint32_t> const& channelIds, FilterType6132::Type const filter) noexcept {
		return devIF_->setFilter(filter, channelIds);
	}

	std::optional<GainType6132::Type> gain(uint32_t const channelId) noexcept {
		return devIF_->gain(channelId);
	}

	std::optional<FilterType6132::Type> filter(uint32_t const channelId) noexcept {
		return devIF_->filter(channelId);
	}

	void setFcId(FecIdType::Type const fcId) noexcept {
		id_ = fcId;
	}

	std::optional<std::vector<std::optional<double>>> measure(std::vector<uint32_t> const& channelIds) {
		if (devIF_->isFecIdle(id_, FecType::_6132) &&
			ADC_SEL_reg_.selectChannels(channelIds) &&
			CMD_reg_.setCmd(FecCmdsType6132::ADC_CONVERT) &&
			devIF_->testWithTimeout([this]() { return devIF_->isFecIdle(id_, FecType::_6132); }, 500)) {
			std::vector<std::optional<double>> results(16, std::nullopt);
			for (auto id : channelIds) {
				auto g = gain(id);
				if (auto rawData = channelsRegs_[id - 1].rawData(); rawData && g)
					results[id-1] = calculateVoltage(*rawData) / GainType6132::toMeaningValue(*g);
			}
			return results;
		}
		return std::nullopt;
	}

	bool isValidDev() noexcept {
		auto type = devIF_->readFecType(id_);
		return type ? *type == FecType::_6132 : false;
	}

	void connect() noexcept {
		devIF_->connect();
	}

	Device6991* parentDevice() const noexcept {
		return devIF_;
	}
};

