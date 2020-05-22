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
#include <QRegExpValidator>

#include "../AbstractDevice.h"
#include "TwoStateButton.h"

class ConnectController : public QGroupBox {
	Q_OBJECT
	AbstractDevice* devIF_;
	TwoStateButton* connectDisconnectButton_ = new TwoStateButton("Connect", [this]() { emit sendConnectReq(); }, "Disconnect", [this]() { emit sendDisconnectReq(); });
signals:
	void sendConnectReq() const;
	void sendDisconnectReq() const;
public:
	ConnectController(AbstractDevice* devIF, QWidget* parent = nullptr) : QGroupBox("Device", parent), devIF_(devIF) {
		connect(devIF, &AbstractDevice::connectionStatusChanged, [this](QString const& name, bool const status) { status ? emit connectDisconnectButton_->stateChange1() : emit connectDisconnectButton_->stateChange2(); });
		connect(this, &ConnectController::sendConnectReq, devIF, &AbstractDevice::handleConnectReq);
		connect(this, &ConnectController::sendDisconnectReq, devIF, &AbstractDevice::handleDisconnectReq);
		connect(connectDisconnectButton_, &TwoStateButton::stateChange1, this, &ConnectController::connected);
		connect(connectDisconnectButton_, &TwoStateButton::stateChange2, this, &ConnectController::disconnected);

		auto const& inputResources = devIF->inputResources();
		auto layout = new QVBoxLayout;
		for (auto const& resource : devIF->inputResources()) {
			auto descLabel = new QLabel(resource->description());
			auto lineEdit = new QLineEdit;
			lineEdit->setValidator(resource->validator());
			lineEdit->setText(resource->load());
			connect(lineEdit, &QLineEdit::editingFinished, [resource, lineEdit]() {resource->setValue(lineEdit->text()); resource->save(); });
			auto hlayout = new QHBoxLayout;
			hlayout->addWidget(descLabel);
			hlayout->addWidget(lineEdit);
			layout->addLayout(hlayout);
		}
		auto hlayout = new QHBoxLayout;
		hlayout->addStretch(0);
		hlayout->addWidget(connectDisconnectButton_, 1, Qt::AlignRight);

		layout->addLayout(hlayout);
		setLayout(layout);
	}
signals:
	void connected() const;
	void disconnected() const;
};