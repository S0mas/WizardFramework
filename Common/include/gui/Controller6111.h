#pragma once
#include <QGroupBox>
#include "../Device6991/Device6991.h"
#include "ChannelsSelectionView.h"
#include <QTableView>
#include <QInputDialog>
#include <QSortFilterProxyModel>
#include <atomic>

class Channel6111 {
    uint32_t id_;
    bool state_;
public:
    Channel6111(uint32_t const id) noexcept :
        id_{ id }{}
    QString id() const noexcept { return QString::number(id_); }
    QString state() const noexcept { return state_ ? "Enabled" : "Disabled"; }
    void setState(bool const state) noexcept { state_ = state; }
};

class ChannelModel6111 : public QAbstractTableModel {
    QList<Channel6111> data_;
public:
    ChannelModel6111(QObject* parent = {}) noexcept : QAbstractTableModel{ parent } {}
    int rowCount(const QModelIndex&) const noexcept override {
        return data_.count();
    }
    int columnCount(const QModelIndex&) const noexcept override {
        return 2;
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
        default:
            return {};
        }
    }

    void append(Channel6111 const& channel) noexcept {
        beginInsertRows({}, data_.count(), data_.count());
        data_.append(channel);
        endInsertRows();
    }

    void setState(uint32_t const index, bool const state) noexcept {
        data_[index].setState(state);
    }
};

class ChannelTableView6111 : public QWidget {
    ChannelModel6111 model_;
    uint32_t channelId = 1;
    QTableView* view = new QTableView(this);
public:
    ChannelTableView6111(QWidget* parent = nullptr) noexcept : QWidget(parent) {
        auto layout = new QVBoxLayout(this);
        layout->addWidget(view);
        view->setModel(&model_);
        setLayout(layout);
    }

    void createNewRecord() noexcept {
        model_.append({ channelId++});
    }

    void setState(uint32_t const index, bool const state) noexcept {
        model_.setState(index, state);
    }

    void update() noexcept {
        view->viewport()->update();
    }
};

class Controller6111: public QGroupBox {
	Q_OBJECT
    const static int CHANNELS_NO_6111 = 256; // 2 frontends cards * 128 channels each
    ChannelTableView6111* channelTable_ = new ChannelTableView6111(this);
    QPushButton* channelConfigurationButton_ = new QPushButton("Configure Channels");
	std::atomic<bool> fetched = false;
	Device6991* devIF_ = nullptr;
	ChannelStatuses statuses_;
    void createConnections() noexcept;
    QWidget* createConfigureChannelsWidget() noexcept;
    QDialog* createChannelsConfigurationDialog() noexcept;
signals:
	void setChannelsEnabledReq(bool const state, std::vector<uint32_t> const& channelIds) const;
	void channelConfigurationReq() const;
    void saveConfigurationToFileReq(QString const& fileName);
    void loadConfigurationFromFileReq(QString const& fileName);
private slots:
	void handleChannelConfiguration(ChannelsConfiguration const& configuration) noexcept;
public:
	Controller6111(Device6991* const devIF, QWidget* parent = nullptr);
};