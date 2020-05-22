#pragma once
#include <map>
#include <QPushButton>
#include <QMutex>
#include <QDebug>

struct ChannelStatus {
	const uint32_t id_;
	bool enabled_;
	ChannelStatus(const int id) noexcept : id_(id), enabled_(false) {}
};

class ChannelStatuses {
	std::map<uint32_t, ChannelStatus> statuses_;
public:
	ChannelStatuses(const int channelsNo) noexcept {
		for (uint32_t i = 1; i <= channelsNo; ++i)
			statuses_.insert({ i, ChannelStatus(i) });
	}

	void disable(const uint32_t id) noexcept {
		statuses_.at(id).enabled_ = false;
	}

	void disableAll() noexcept {
		for(auto& status : statuses_)
			status.second.enabled_ = false;
	}

	void enable(const uint32_t id) noexcept {
		statuses_.at(id).enabled_ = true;
	}

	void enableAll() noexcept {
		for (auto& status : statuses_)
			status.second.enabled_ = true;
	}

	void set(const uint32_t id, const bool state) noexcept {
		statuses_.at(id).enabled_ = state;
	}

	bool status(const uint32_t id) const noexcept {
		return statuses_.at(id).enabled_;
	}

	int size() const noexcept {
		return statuses_.size();
	}

	QVector<bool> all() const noexcept {
		QVector<bool> res;
		res.reserve(size());
		for (auto const& status : statuses_)
			res.push_back(status.second.enabled_);
		return res;
	}

	std::vector<uint32_t> allEnabled() const noexcept {
		std::vector<uint32_t> res;
		res.reserve(size());
		for (auto const& status : statuses_)
			if(status.second.enabled_)
				res.push_back(status.first);
		return res;
	}
};

class ChannelSelectionButton : public QPushButton {
	Q_OBJECT
	uint32_t id_;
public:
	ChannelSelectionButton(const uint32_t id, QWidget* parent = nullptr) : QPushButton(QString::number(id), parent), id_(id){
		setFixedWidth(25);
		setCheckable(true);
	}

	uint32_t id() const noexcept {
		return id_;
	}
};