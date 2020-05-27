#include "../../../include/Device6991/gui/RegisterController.h"
#include <QMessageBox>

void TargetFrontendCardView::handleRegisterReadResp(FecIdType::Type const fecId, unsigned int const data) const noexcept {
	if (fecId == index_)
		dataLineEdit_->setText(toHex(data, 8));;
}

void TargetFrontendCardView::createConnections() noexcept {
	connect(this, &TargetFrontendCardView::stateReq, deviceIF_, &Device6991::handleFecStateReq);
	connect(this, &TargetFrontendCardView::registerReadReq, deviceIF_, &Device6991::handleFecRegisterReadReq);
	connect(this, &TargetFrontendCardView::registerWriteReq, deviceIF_, &Device6991::handleFecRegisterWriteReq);
	connect(deviceIF_, &Device6991::fecRegisterReadResp, this, &TargetFrontendCardView::handleRegisterReadResp);
	connect(parentCommandSelector_, &EnumSelector::enumChanged,
		[this](int const concreteEnum) {
			if (concreteEnum == Commands1Enum::READ)
				dataLineEdit_->setEnabled(false);
			else if (concreteEnum == Commands1Enum::WRITE)
				dataLineEdit_->setEnabled(checkBox_->isChecked());
		}
	);
	initializeStateMachine();
}

void TargetFrontendCardView::initializeStateMachine() noexcept {
	auto enabled = new QState();
	auto active = new QState(enabled);
	auto inactive = new QState(enabled);
	auto disabled = new QState();

	enabled->setInitialState(inactive);
	auto checkTransition = new CheckedTransition(checkBox_);
	checkTransition->setTargetState(active);
	inactive->addTransition(checkTransition);

	auto unchecktransition = new UncheckedTransition(checkBox_);
	unchecktransition->setTargetState(inactive);
	active->addTransition(unchecktransition);

	enabled->addTransition(deviceIF_, &Device6991::fecDisabledResp, disabled);
	disabled->addTransition(deviceIF_, &Device6991::fecEnabledResp, enabled);

	sm_.addState(enabled);
	sm_.addState(disabled);

	connect(enabled, &QState::entered,
		[this]() {
			setEnabled(true);
		}
	);
	connect(active, &QState::entered,
		[this]() {
			dataLineEdit_->setEnabled(parentCommandSelector_->value() == Commands1Enum::WRITE);
		}
	);
	connect(inactive, &QState::entered,
		[this]() {
			dataLineEdit_->setEnabled(false);
		}
	);
	connect(disabled, &QState::entered,
		[this]() {
			setEnabled(false);
			checkBox_->setChecked(false);
		}
	);
	sm_.setInitialState(disabled);
	sm_.start();
}

TargetFrontendCardView::TargetFrontendCardView(EnumSelector* parentCommandSelector, Device6991* devIF, FecIdType::Type const index, QWidget* parent)
	: QGroupBox(QString("Front End %1").arg(index), parent), index_(index), deviceIF_(devIF), parentCommandSelector_(parentCommandSelector) {
	dataLineEdit_->setMaximumWidth(70);
	dataLineEdit_->setInputMask("\\0\\xHHHHHHHH;_");
	dataLineEdit_->setText("0x00000000");
	dataLineEdit_->setEnabled(false);

	checkBox_->setChecked(false);

	auto layout = new QHBoxLayout;
	layout->addWidget(dataLineEdit_);
	layout->addWidget(checkBox_);
	setLayout(layout);

	createConnections();
}

void TargetFrontendCardView::sendCommand(Commands1Enum::Type const cmd, unsigned int const address) noexcept {
	if (isEnabled() && checkBox_->isChecked()) {
		if (cmd == Commands1Enum::READ) {
			dataLineEdit_->setText("");
			emit registerReadReq(index_, address);
		}
		else if (cmd == Commands1Enum::WRITE) {
			emit registerWriteReq(index_, address, dataLineEdit_->text().toUInt(nullptr, 16));
			dataLineEdit_->setText("");
		}
	}
}

void TargetFrontendCardView::sendStateReq() const noexcept {
	emit stateReq(index_);
}

void RegisterControllerFrontend::createConnections() noexcept {
	connect(refreshButton, &QPushButton::clicked, frontend1_, &TargetFrontendCardView::sendStateReq);
	connect(refreshButton, &QPushButton::clicked, frontend2_, &TargetFrontendCardView::sendStateReq);
	connect(executeButton, &QPushButton::clicked,
		[this]() {
			frontend1_->sendCommand(static_cast<Commands1Enum::Type>(commandSelector_->value()), addressSelector_->value());
			frontend2_->sendCommand(static_cast<Commands1Enum::Type>(commandSelector_->value()), addressSelector_->value());
		}
	);
}

RegisterControllerFrontend::RegisterControllerFrontend(Device6991* devIF, QWidget* parent)
	: QGroupBox("Front-End Cards Register Controller", parent), frontend1_(new TargetFrontendCardView(commandSelector_, devIF, FecIdType::_1)), frontend2_(new TargetFrontendCardView(commandSelector_, devIF, FecIdType::_2)){
	auto frontendsLayout = new QHBoxLayout;
	frontendsLayout->addWidget(frontend1_);
	frontendsLayout->addWidget(frontend2_);

	auto acceptRow = new QHBoxLayout;
	acceptRow->addWidget(refreshButton);
	acceptRow->addStretch(0);
	acceptRow->addWidget(executeButton, 1, Qt::AlignRight);

	auto layout = new QVBoxLayout;
	layout->addWidget(commandSelector_);
	layout->addWidget(addressSelector_);
	layout->addLayout(frontendsLayout);
	layout->addLayout(acceptRow);
	setLayout(layout);

	createConnections();
}

void RegisterController6991::handleRegisterReadResp(unsigned int const data) const noexcept {
	dataLineEdit_->setText(toHex(data, 8));
}

void RegisterController6991::createConnections() noexcept {
	connect(this, &RegisterController6991::registerReadReq, deviceIF_, &Device6991::handleFpgaRegisterReadReq);
	connect(this, &RegisterController6991::registerWriteReq, deviceIF_, &Device6991::handleFpgaRegisterWriteReq);
	connect(deviceIF_, &Device6991::fpgaRegisterReadResp, this, &RegisterController6991::handleRegisterReadResp);
	connect(executeButton, &QPushButton::clicked,
		[this]() {
			if (commandSelector_->value() == Commands2Enum::READ) {
				dataLineEdit_->setText("");
				emit registerReadReq(addressSelector_->value());
			}
			else if (commandSelector_->value() == Commands2Enum::WRITE) {
				emit registerWriteReq(addressSelector_->value(), dataLineEdit_->text().toUInt(nullptr, 16));
				dataLineEdit_->setText("");
			}
		}
	);

	connect(commandSelector_, &EnumSelector::enumChanged,
		[this](int const concreteEnum) {
			if (concreteEnum == Commands2Enum::READ)
				dataLineEdit_->setEnabled(false);
			else if(concreteEnum == Commands2Enum::WRITE)
				dataLineEdit_->setEnabled(true);
		}
	);
}

RegisterController6991::RegisterController6991(Device6991* devIF, QWidget* parent)
	: QGroupBox("6991 Register Controller", parent), deviceIF_(devIF) {
	dataLineEdit_->setMaximumWidth(70);
	dataLineEdit_->setInputMask("\\0\\xHHHHHHHH;_");
	dataLineEdit_->setText("0x00000000");
	dataLineEdit_->setEnabled(false);
	auto dataGroup = new QGroupBox("Data", this);
	auto hLayout = new QHBoxLayout;
	hLayout->addWidget(dataLineEdit_);
	hLayout->addStretch(0);
	dataGroup->setLayout(hLayout);

	auto acceptRow = new QHBoxLayout;
	acceptRow->addWidget(executeButton, 1, Qt::AlignRight);

	auto layout = new QVBoxLayout;
	layout->addWidget(commandSelector_);
	layout->addWidget(addressSelector_);
	layout->addWidget(dataGroup);
	layout->addLayout(acceptRow);
	setLayout(layout);

	createConnections();
}