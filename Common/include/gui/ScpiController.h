#pragma once
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include "../ScpiDevice.h"
#include "LedIndicator.h"

class ScpiController : public QGroupBox {
	Q_OBJECT
	QLineEdit* commandLineEdit_ = new QLineEdit(this);
	QTextEdit* responseTextEdit_ = new QTextEdit(this);
	QPushButton* executeButton_ = new QPushButton("Execute", this);
	LedIndicator* ledIndicator_ = new LedIndicator(this);
	void makeConnections(ScpiDevice* device) noexcept {
		connect(this, &ScpiController::sendCmd, device, &ScpiDevice::asyncInvoke);
		connect(device, &ScpiDevice::sendResponse, this, &ScpiController::resposeRecived);
		connect(executeButton_, &QPushButton::clicked, this, &ScpiController::execute);
	}
private slots:
	void execute() {
		if (!commandLineEdit_->text().isEmpty()) {
			emit sendCmd(commandLineEdit_->text());
			executeButton_->setDisabled(true);
			executeButton_->setText("Processing..");
		}	
	}
	void resposeRecived(bool const status, QString const& response) {
		ledIndicator_->setState(status);
		responseTextEdit_->setText(response == "" ? "Empty response!" : response);
		executeButton_->setDisabled(false);
		executeButton_->setText("Execute");
	}
public:
	ScpiController(ScpiDevice* device, QWidget* parent = nullptr) : QGroupBox("Scpi Controller", parent) {
		commandLineEdit_->setPlaceholderText("Put your command here.");
		responseTextEdit_->setPlaceholderText("Your response will show up here.");
		responseTextEdit_->setMaximumHeight(80);
		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(ledIndicator_);
		hlayout->addStretch(0);
		hlayout->addWidget(executeButton_, 1, Qt::AlignRight);

		auto vlayout = new QVBoxLayout;
		vlayout->addWidget(commandLineEdit_);
		vlayout->addWidget(responseTextEdit_);
		vlayout->addLayout(hlayout);
		vlayout->addStretch(0);

		makeConnections(device);
		setLayout(vlayout);
	}

	void setLedIndicatorState(bool const state) noexcept {
		ledIndicator_->setState(state);
	}
signals:
	void sendCmd(QString const& cmd) const;
};

