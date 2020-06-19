#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include "../Device6132.h"
#include "ChannelsSelectionView.h"

enum ConfigAction {
	SET_GAIN,
	SET_FILTER,
	SET_ENABLED
};

enum Target {
	ALL = 0
};

class Controller6132;

class Controller6132 : public QGroupBox {
	Q_OBJECT
	Device6991* devIF_ = nullptr;
	ChannelStatuses statuses_;
	const static int CHANNELS_NO_6132 = 32; // 2 frontends cards * 16 channels each
signals:
	void setFiltersReq(FilterType6132::Type const filter, std::vector<uint32_t> const& channelIds) const;
	void setGainsReq(GainType6132::Type const gain, std::vector<uint32_t> const& channelIds) const;
	void setChannelsEnabledReq(bool const state, std::vector<uint32_t> const& channelIds) const;
public:
	Controller6132(Device6991* const devIF, QWidget* parent = nullptr)
		: QGroupBox("6132 configuration", parent),
		devIF_(devIF),
		statuses_{CHANNELS_NO_6132} {
		connect(this, &Controller6132::setFiltersReq, devIF_, &Device6991::handleSetFiltersReq);
		connect(this, &Controller6132::setGainsReq, devIF_, &Device6991::handleSetGainsReq);
		connect(this, &Controller6132::setChannelsEnabledReq, devIF_, &Device6991::handleSetChannelsEnabledReq);
		
		auto channelConfiguration = new QPushButton("Select Channels");
		connect(channelConfiguration, &QPushButton::clicked,
			[this]() {
				QDialog* selectionDialog_ = new QDialog(this);
				selectionDialog_->setModal(true);
				auto layout = new QVBoxLayout(selectionDialog_);
				ChannelsSelectionView* channelsView_ = new ChannelsSelectionView(statuses_, statuses_.size() / CHANNELS_NO_6132, "Channels Controller", 2, selectionDialog_);
				layout->addWidget(channelsView_);

				auto comboBoxAction = new QComboBox;
				comboBoxAction->addItem("SET_GAIN", SET_GAIN);
				comboBoxAction->addItem("SET_FILTER", SET_FILTER);
				comboBoxAction->addItem("SET_ENABLED", SET_ENABLED);
				auto comboBoxValue = new QComboBox;
				comboBoxValue->setMinimumWidth(60);
				auto hlayout = new QHBoxLayout;
				hlayout->addWidget(comboBoxAction);
				hlayout->addWidget(comboBoxValue);
				auto setButton = new QPushButton("Set");

				hlayout->addWidget(setButton);

				layout->addLayout(hlayout);

				connect(comboBoxAction, QOverload<QString const&>::of(&QComboBox::currentIndexChanged),
					[comboBoxValue](QString const& action) {
						comboBoxValue->clear();
						if (action == "SET_GAIN")
							for (auto gain : GainType6132::TYPES)
								comboBoxValue->addItem(GainType6132::toString(gain), gain);
						if (action == "SET_FILTER")
							for (auto filter : FilterType6132::TYPES)
								comboBoxValue->addItem(FilterType6132::toString(filter), filter);
						if (action == "SET_ENABLED")
							for (auto state : { "Enable", "Disable" })
								comboBoxValue->addItem(state, state == "Enable");
					}
				);

				connect(setButton, &QPushButton::clicked,
					[comboBoxAction, comboBoxValue, this]() {
						bool result = true;
						if (comboBoxAction->currentData().toUInt() == SET_GAIN)
							emit setGainsReq(static_cast<GainType6132::Type>(comboBoxValue->currentData().toUInt()), statuses_.allEnabled());
						else if (comboBoxAction->currentData().toUInt() == SET_FILTER)
							emit setFiltersReq(static_cast<FilterType6132::Type>(comboBoxValue->currentData().toUInt()), statuses_.allEnabled());
						else if (comboBoxAction->currentData().toUInt() == SET_ENABLED)
							emit setChannelsEnabledReq(comboBoxValue->currentData().toBool(), statuses_.allEnabled());
						if (!result)
							QMessageBox::critical(this, "Error", QString("Faild to %1.").arg(comboBoxAction->currentText()));
					}
				);

				selectionDialog_->setLayout(layout);
				emit comboBoxAction->currentIndexChanged(comboBoxAction->currentText());
				selectionDialog_->open();	
			}
		);

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(channelConfiguration);

		setLayout(hlayout);
	}
};