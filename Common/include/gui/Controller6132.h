#pragma once
#include <QGroupBox>
#include "../Device6991/Device6991.h"
#include "ChannelsSelectionView.h"
#include <QTableView>
#include <QInputDialog>
#include <QSortFilterProxyModel>
#include <atomic>

class Channel {
    uint32_t id_;
    FilterType6132::Type filter_;
    GainType6132::Type gain_;
    bool state_;
public:
    Channel(uint32_t const id, FilterType6132::Type const filter, GainType6132::Type const gain) noexcept :
        id_{ id }, filter_{ filter }, gain_{ gain } {}
    QString id() const noexcept { return QString::number(id_); }
    QString state() const noexcept { return state_ ? "Enabled" : "Disabled"; }
    QString filter() const noexcept { return FilterType6132::toString(filter_); }
    QString gain() const noexcept { return GainType6132::toString(gain_); }
    void setState(bool const state) noexcept { state_ = state; }
    void setFilter(FilterType6132::Type const filter) noexcept { filter_ = filter; }  
    void setGain(GainType6132::Type const gain) noexcept { gain_ = gain; }
};

class ChannelModel : public QAbstractTableModel {
    QList<Channel> data_;
public:
    ChannelModel(QObject* parent = {}) noexcept : QAbstractTableModel{ parent } {}
    int rowCount(const QModelIndex&) const noexcept override {
        return data_.count();
    }
    int columnCount(const QModelIndex&) const noexcept override {
        return 3;
    }
    QVariant data(const QModelIndex& index, int role) const noexcept override {
        if (role != Qt::DisplayRole && role != Qt::EditRole)
            return {};
        const auto& channel = data_[index.row()];
        switch (index.column()) {
        case 0:
            return channel.id();
        case 1:
            return channel.state();
        case 2:
            return channel.filter();
        case 3:
            return channel.gain();
        default:
            return {};
        };
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const noexcept override {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return {};
        switch (section) {
        case 0:
            return "Id";
        case 1:
            return "State";
        case 2:
            return "Filter";
        case 3:
            return "Gain";
        default:
            return {};
        }
    }

    void append(Channel const& channel) noexcept {
        beginInsertRows({}, data_.count(), data_.count());
        data_.append(channel);
        endInsertRows();
    }

    void setState(uint32_t const index, bool const state) noexcept {
        data_[index].setState(state);
    }

    void setFilter(uint32_t const index, FilterType6132::Type const filter) noexcept {
        data_[index].setFilter(filter);
    }

    void setGain(uint32_t const index, GainType6132::Type const gain) noexcept {
        data_[index].setGain(gain);
    }
};

class ChannelTableView : public QWidget {
    ChannelModel model_;
    uint32_t channelId = 1;
public:
    ChannelTableView(QWidget* parent = nullptr) noexcept : QWidget(parent) {
        auto layout = new QVBoxLayout(this);
        auto view = new QTableView;
        layout->addWidget(view);
        view->setModel(&model_);
    }

    void createNewRecord() noexcept {
        model_.append({ channelId++, FilterType6132::INVALID, GainType6132::INVALID });
    }

    void setState(uint32_t const index, bool const state) noexcept {
        model_.setState(index, state);
    }

    void setFilter(uint32_t const index, FilterType6132::Type const filter) noexcept {
        model_.setFilter(index, filter);
    }

    void setGain(uint32_t const index, GainType6132::Type const gain) noexcept {
        model_.setGain(index, gain);
    }
};

class Controller6132 : public QGroupBox {
	Q_OBJECT
	std::atomic<bool> fetched = false;
	QTableView* view = new QTableView;
	Device6991* devIF_ = nullptr;
	ChannelStatuses statuses_;
    ChannelTableView* channelTable_ = new ChannelTableView(this);
	const static int CHANNELS_NO_6132 = 32; // 2 frontends cards * 16 channels each
signals:
	void setFiltersReq(FilterType6132::Type const filter, std::vector<uint32_t> const& channelIds) const;
	void setGainsReq(GainType6132::Type const gain, std::vector<uint32_t> const& channelIds) const;
	void setChannelsEnabledReq(bool const state, std::vector<uint32_t> const& channelIds) const;
	void channelConfigurationReq() const;
private slots:
	void handleChannelConfiguration(ChannelsConfiguration const& configuration) noexcept;
public:
	Controller6132(Device6991* const devIF, QWidget* parent = nullptr);
};