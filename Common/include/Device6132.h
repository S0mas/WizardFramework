#pragma once
#include "Device6991/Device6991.h"

class Device6132 : public QObject {
	Q_OBJECT
	FecIdType::Type id_;
	Device6991* devIF_;
	CHN_FILTER_reg CHN_FILTER_reg_{ nullptr };
	CHN_GAIN_reg CHN_GAIN_reg_{ nullptr };
	CMD_reg CMD_reg_{ nullptr };
	ADC_SEL_reg ADC_SEL_reg_{ nullptr };
	std::vector<CHNX_RAW_SAMPLE_reg> channelsRegs_;
	QString connectorName_;

	bool setupGains(std::vector<uint32_t> const& channelIds) noexcept {
		return devIF_->isFecIdle(FecType::_6132, id_) &&													
			ADC_SEL_reg_.selectChannels(id_, channelIds) &&																		
			CMD_reg_.setCmd(id_, FecCmdsType6132::GAIN_SETUP) &&																
			devIF_->testWithTimeout([this]() { return devIF_->isFecIdle(FecType::_6132, id_); }, 500);
	}

	double calculateVoltage(uint16_t const rawData) noexcept {
		double neutralZero = 0x8000;
		int16_t value =  static_cast<double>(rawData) - neutralZero;
		double lsb = 312.5; //microvolts
		double result = value * lsb;
		return result / 1000'000; //volts;
	}
public:
	Device6132(QString const& connectorName, const FecIdType::Type& fcId, Device6991* dev6991, QObject* parent = nullptr) noexcept : connectorName_(connectorName), id_(fcId), devIF_(dev6991) {
		CHN_FILTER_reg_ = CHN_FILTER_reg(devIF_);
		CHN_GAIN_reg_ = CHN_GAIN_reg(devIF_);
		CMD_reg_ = CMD_reg(devIF_);
		ADC_SEL_reg_ = ADC_SEL_reg(devIF_);
		for (int channelId = 1; channelId <= 16; ++channelId)
			channelsRegs_.push_back(CHNX_RAW_SAMPLE_reg(channelId, devIF_));
	}
	~Device6132() = default;

	bool setGain(std::vector<uint32_t> const& channelIds, GainType6132::Type const gain) noexcept {
		return CHN_GAIN_reg_.setGain(id_, channelIds, gain) && setupGains(channelIds);
	}

	bool setFilter(std::vector<uint32_t> const& channelIds, FilterType6132::Type const filter) noexcept {
		return CHN_FILTER_reg_.setFilter(id_, channelIds, filter);
	}

	std::optional<GainType6132::Type> gain(uint32_t const channelId) noexcept {
		return CHN_GAIN_reg_.gain(id_, channelId);
	}

	std::optional<FilterType6132::Type> filter(uint32_t const channelId) noexcept {
		return CHN_FILTER_reg_.filter(id_, channelId);
	}

	void setFcId(FecIdType::Type const fcId) noexcept {
		id_ = fcId;
	}

	std::optional<std::vector<std::optional<double>>> measure(std::vector<uint32_t> const& channelIds) {
		if (devIF_->isFecIdle(FecType::_6132, id_) &&
			ADC_SEL_reg_.selectChannels(id_, channelIds) &&
			CMD_reg_.setCmd(id_, FecCmdsType6132::ADC_CONVERT) &&
			devIF_->testWithTimeout([this]() { return devIF_->isFecIdle(FecType::_6132, id_); }, 500)) {
			std::vector<std::optional<double>> results(16, std::nullopt);
			for (auto id : channelIds) {
				auto g = gain(id);
				if (auto rawData = channelsRegs_[id - 1].rawData(id_); rawData && g)
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

	bool connect() noexcept {
		return devIF_->connect();
	}

	QString connectorName() const noexcept {
		return connectorName_;
	}
};

