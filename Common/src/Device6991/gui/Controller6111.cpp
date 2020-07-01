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
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include "../../../include/gui/Controller6111.h"

enum ConfigAction {
	SET_ENABLED
};

enum Target {
	ALL = 0
};

void Controller6111::createConnections() noexcept {
	connect(this, &Controller6111::setChannelsEnabledReq, devIF_, &Device6991::handleSetChannelsEnabledReq);
	connect(this, &Controller6111::channelConfigurationReq, devIF_, &Device6991::handleChannelsConfigurationReq);
	connect(devIF_, &Device6991::channelConfiguration, this, &Controller6111::handleChannelConfiguration);
	connect(this, &Controller6111::saveConfigurationToFileReq, devIF_, &Device6991::handleSaveChannelConfigurationToFileReq);
	connect(this, &Controller6111::loadConfigurationFromFileReq, devIF_, &Device6991::handleLoadChannelConfigurationFromFileReq);

	auto showChannelsConfigurationDialog = createChannelsConfigurationDialog();
	connect(channelConfigurationButton_, &QPushButton::clicked, this, &Controller6111::channelConfigurationReq);
	connect(channelConfigurationButton_, &QPushButton::clicked, showChannelsConfigurationDialog , &QDialog::exec);
}

QWidget* Controller6111::createConfigureChannelsWidget() noexcept {
	QWidget* configureChannels = new QWidget(this);
	auto layout = new QVBoxLayout(configureChannels);
	ChannelsSelectionView* channelsView_ = new ChannelsSelectionView(statuses_, statuses_.size() / 16, "Select Channels", 4, configureChannels);
	layout->addWidget(channelsView_);

	auto comboBoxAction = new QComboBox;
	comboBoxAction->addItem("Set Enabled", SET_ENABLED);
	auto comboBoxValue = new QComboBox;
	comboBoxValue->setMinimumWidth(60);
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(comboBoxAction);
	hlayout->addWidget(comboBoxValue);
	auto setButton = new QPushButton("Execute");

	hlayout->addWidget(setButton);

	layout->addLayout(hlayout);

	connect(comboBoxAction, static_cast<void(QComboBox::*)(QString const&)>(&QComboBox::currentIndexChanged),
		[comboBoxValue](QString const& action) {
			comboBoxValue->clear();
			if (action == "Set Enabled")
				for (auto state : { "Enable", "Disable" })
					comboBoxValue->addItem(state, state == "Enable");
		}
	);

	connect(setButton, &QPushButton::clicked,
		[comboBoxAction, comboBoxValue, this]() {
			bool result = true;
			if (comboBoxAction->currentData().toUInt() == SET_ENABLED)
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

QDialog* Controller6111::createChannelsConfigurationDialog() noexcept {
	QDialog* showChannelsConfigurationDialog = new QDialog(this, Qt::Sheet);
	showChannelsConfigurationDialog->setModal(true);
	auto buttonSave = new QPushButton("Save");
	auto buttonLoad = new QPushButton("Load");
	auto buttonOk = new QPushButton("Ok");
	auto hlayout = new QHBoxLayout;
	hlayout->addStretch();
	hlayout->addWidget(buttonSave);
	hlayout->addWidget(buttonLoad);
	hlayout->addWidget(buttonOk);
	connect(buttonSave, &QPushButton::clicked, 
		[this, showChannelsConfigurationDialog]() {
			auto fileName = QInputDialog::getText(showChannelsConfigurationDialog, "Input file name", "File name:", QLineEdit::Normal, "channels configuration");
			emit saveConfigurationToFileReq(fileName);
		}
	);
	connect(buttonLoad, &QPushButton::clicked,
		[this, showChannelsConfigurationDialog]() {
			auto fileName = QFileDialog::getOpenFileName(this, tr("Open Configuration File"),
				QDir::currentPath(),
				tr("Text (*.txt)"));
			emit loadConfigurationFromFileReq(fileName);
			emit channelConfigurationReq();
			channelTable_->update();
		}
	);
	connect(buttonOk, &QPushButton::clicked, showChannelsConfigurationDialog, &QDialog::accept);
	auto layout = new QVBoxLayout(showChannelsConfigurationDialog);
	layout->addWidget(channelTable_);
	layout->addWidget(createConfigureChannelsWidget());
	layout->addLayout(hlayout);

	showChannelsConfigurationDialog->setLayout(layout);
	showChannelsConfigurationDialog->setMinimumSize(465, 740);
	showChannelsConfigurationDialog->setSizeGripEnabled(true);
	return showChannelsConfigurationDialog;
}

void Controller6111::handleChannelConfiguration(ChannelsConfiguration const& configuration) {
	if (configuration.states_ && configuration.filters_ && configuration.gains_) {
		for (int i = 0; i < configuration.states_->size(); ++i)
			channelTable_->setState(i, configuration.states_->at(i));	
	}
	channelTable_->update();
}

Controller6111::Controller6111(Device6991* const devIF, QWidget* parent)
	: QGroupBox("6132 configuration", parent),
	devIF_(devIF),
	statuses_{ CHANNELS_NO_6111 } {

	for (int i = 1; i <= CHANNELS_NO_6111; ++i)
		channelTable_->createNewRecord();

	createConnections();
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(channelConfigurationButton_);
	setLayout(hlayout);
}
