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
	SET_FILTER
};

enum Target {
	ALL = 0
};

class Controller6132;

class Controller6132 : public QGroupBox {
	Q_OBJECT
	Device6132* devIF1_ = nullptr;
	Device6132* devIF2_ = nullptr;
	ChannelStatuses statuses_;
	const static int CHANNELS_NO_6132 = 16;
public:
	Controller6132(Device6132* const fc1, Device6132* const fc2 = nullptr, QWidget* parent = nullptr)
		: QGroupBox("6132 configuration", parent),
		devIF1_(fc1),
		devIF2_(fc2),
		statuses_{fc1&&fc2 ? CHANNELS_NO_6132*2 : CHANNELS_NO_6132} {
		auto comboBoxAction = new QComboBox;
		comboBoxAction->addItem("SET_GAIN", SET_GAIN);
		comboBoxAction->addItem("SET_FILTER", SET_FILTER);
		auto comboBoxValue = new QComboBox;
		auto channelsSelectionButton = new QPushButton("Select Channels");
		connect(channelsSelectionButton, &QPushButton::clicked, 
			[this]() {
				QDialog* selectionDialog_ = new QDialog(this);
				selectionDialog_->setModal(true);
				auto layout = new QVBoxLayout(selectionDialog_);
				ChannelsSelectionView* channelsView_ = new ChannelsSelectionView(statuses_, statuses_.size() / CHANNELS_NO_6132, "Channels Controller", 2, selectionDialog_);
				layout->addWidget(channelsView_);
				selectionDialog_->setLayout(layout);
				selectionDialog_->open();
			}
		);
		auto setButton = new QPushButton("Set");

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(comboBoxAction);
		hlayout->addWidget(comboBoxValue);
		hlayout->addWidget(channelsSelectionButton);
		hlayout->addWidget(setButton);

		connect(comboBoxAction, QOverload<QString const&>::of(&QComboBox::currentIndexChanged),
			[comboBoxValue](QString const& action) {
				comboBoxValue->clear();
				if (action == "SET_GAIN")
					for (auto gain : GainType6132::TYPES)
						comboBoxValue->addItem(GainType6132::toString(gain), gain);
				if (action == "SET_FILTER")
					for (auto filter : FilterType6132::TYPES)
						comboBoxValue->addItem(FilterType6132::toString(filter), filter);
			}
		);
		emit comboBoxAction->currentIndexChanged(comboBoxAction->currentText());

		connect(setButton, &QPushButton::clicked,
			[comboBoxAction, comboBoxValue, this]() {
				bool result = true;
				std::vector<uint32_t> channelsCtrl1;
				std::vector<uint32_t> channelsCtrl2;
				for (auto channelId : statuses_.allEnabled())
					if (channelId > 16)
						channelsCtrl2.push_back(channelId - 16);
					else
						channelsCtrl1.push_back(channelId);
				if (!devIF1_ && devIF2_)
					channelsCtrl2 = channelsCtrl1;

				if (comboBoxAction->currentData().toUInt() == SET_GAIN) {
					if(devIF1_ && !channelsCtrl1.empty())
						result = devIF1_->setGain(channelsCtrl1, static_cast<GainType6132::Type>(comboBoxValue->currentData().toUInt()));
					if(devIF2_ && !channelsCtrl2.empty())
						result &= devIF2_->setGain(channelsCtrl2, static_cast<GainType6132::Type>(comboBoxValue->currentData().toUInt()));
				}		
				else if (comboBoxAction->currentData().toUInt() == SET_FILTER) {
					if (devIF1_ && !channelsCtrl1.empty())
						result = devIF1_->setFilter(channelsCtrl1, static_cast<FilterType6132::Type>(comboBoxValue->currentData().toUInt()));
					if (devIF2_ && !channelsCtrl2.empty())
						result &= devIF2_->setFilter(channelsCtrl2, static_cast<FilterType6132::Type>(comboBoxValue->currentData().toUInt()));
				}
				if (!result)
					QMessageBox::critical(this, "Error", QString("Faild to %1.").arg(comboBoxAction->currentText()));
			}
		);
		setLayout(hlayout);
	}
};