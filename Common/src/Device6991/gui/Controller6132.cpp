#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QtSql>
#include <QSqlTableModel>
#include <QLabel>
#include <QLineEdit>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include "../../../include/gui/Controller6132.h"

enum ConfigAction {
	SET_GAIN,
	SET_FILTER,
	SET_ENABLED
};

enum Target {
	ALL = 0
};

void Controller6132::createConnections() noexcept {
	connect(this, &Controller6132::setFiltersReq, devIF_, &Device6991::handleSetFiltersReq);
	connect(this, &Controller6132::setGainsReq, devIF_, &Device6991::handleSetGainsReq);
	connect(this, &Controller6132::setChannelsEnabledReq, devIF_, &Device6991::handleSetChannelsEnabledReq);
	connect(this, &Controller6132::channelConfigurationReq, devIF_, &Device6991::handleChannelsConfigurationReq);
	connect(devIF_, &Device6991::channelConfiguration, this, &Controller6132::handleChannelConfiguration);

	auto showChannelsConfigurationDialog = createChannelsConfigurationDialog();
	connect(channelConfigurationButton_, &QPushButton::clicked, this, &Controller6132::channelConfigurationReq);
	connect(channelConfigurationButton_, &QPushButton::clicked, showChannelsConfigurationDialog , &QDialog::exec);
}

QWidget* Controller6132::createConfigureChannelsWidget() noexcept {
	QWidget* configureChannels = new QWidget(this);
	auto layout = new QVBoxLayout(configureChannels);
	ChannelsSelectionView* channelsView_ = new ChannelsSelectionView(statuses_, statuses_.size() / 16, "Select Channels", 2, configureChannels);
	layout->addWidget(channelsView_);

	auto comboBoxAction = new QComboBox;
	comboBoxAction->addItem("Set Gain", SET_GAIN);
	comboBoxAction->addItem("Set Filter", SET_FILTER);
	comboBoxAction->addItem("Set Enabled", SET_ENABLED);
	auto comboBoxValue = new QComboBox;
	comboBoxValue->setMinimumWidth(60);
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(comboBoxAction);
	hlayout->addWidget(comboBoxValue);
	auto setButton = new QPushButton("Execute");

	hlayout->addWidget(setButton);

	layout->addLayout(hlayout);

	connect(comboBoxAction, QOverload<QString const&>::of(&QComboBox::currentIndexChanged),
		[comboBoxValue](QString const& action) {
			comboBoxValue->clear();
			if (action == "Set Gain")
				for (auto gain : GainType6132::TYPES)
					comboBoxValue->addItem(GainType6132::toString(gain), gain);
			if (action == "Set Filter")
				for (auto filter : FilterType6132::TYPES)
					comboBoxValue->addItem(FilterType6132::toString(filter), filter);
			if (action == "Set Enabled")
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
			emit channelConfigurationReq();
		}
	);
	emit comboBoxAction->currentIndexChanged(comboBoxAction->currentText());
	configureChannels->setLayout(layout);
	return configureChannels;
}

QDialog* Controller6132::createChannelsConfigurationDialog() noexcept {
	QDialog* showChannelsConfigurationDialog = new QDialog(this, Qt::Sheet);
	showChannelsConfigurationDialog->setModal(true);
	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, &QDialogButtonBox::accepted, showChannelsConfigurationDialog, &QDialog::accept);
	auto layout = new QVBoxLayout(showChannelsConfigurationDialog);
	layout->addWidget(channelTable_);
	layout->addWidget(createConfigureChannelsWidget());
	layout->addWidget(buttonBox);

	showChannelsConfigurationDialog->setLayout(layout);
	showChannelsConfigurationDialog->setMinimumSize(465, 740);
	showChannelsConfigurationDialog->setSizeGripEnabled(true);
	return showChannelsConfigurationDialog;
}

void Controller6132::handleChannelConfiguration(ChannelsConfiguration const& configuration) noexcept {
	if (configuration.states_ && configuration.filters_ && configuration.gains_) {
		for (int i = 0; i < configuration.states_->size(); ++i) {
			channelTable_->setState(i, configuration.states_->at(i));
			channelTable_->setFilter(i, configuration.filters_->at(i));
			channelTable_->setGain(i, configuration.gains_->at(i));
		}		
	}
	channelTable_->update();
}

Controller6132::Controller6132(Device6991* const devIF, QWidget* parent)
	: QGroupBox("6132 configuration", parent),
	devIF_(devIF),
	statuses_{ CHANNELS_NO_6132 } {

	for (int i = 1; i <= CHANNELS_NO_6132; ++i)
		channelTable_->createNewRecord();

	createConnections();
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(channelConfigurationButton_);
	setLayout(hlayout);
}
