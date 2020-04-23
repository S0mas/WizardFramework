#include "../../../include/Device6991/gui/RegisterController.h"
#include <QMessageBox>

void TargetFrontendCardView::createConnections() noexcept {
	connect(this, &TargetFrontendCardView::stateReq, [this]() { deviceIF_->fcCardStateRequest(index_); });
	connect(parentCommandSelector_, &EnumSelector::enumChanged,
		[this](int const concreteEnum) {
			if (concreteEnum == Commands1Enum::READ)
				lineEdit_->setEnabled(false);
			else if (concreteEnum == Commands1Enum::WRITE)
				lineEdit_->setEnabled(checkBox_->isChecked());
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

	enabled->addTransition(deviceIF_, &Device6991::fcCardDisabled, disabled);
	disabled->addTransition(deviceIF_, &Device6991::fcCardEnabled, enabled);

	sm_.addState(enabled);
	sm_.addState(disabled);

	connect(enabled, &QState::entered,
		[this]() {
			setEnabled(true);
		}
	);
	connect(active, &QState::entered,
		[this]() {
			lineEdit_->setEnabled(parentCommandSelector_->value() == Commands1Enum::WRITE);
		}
	);
	connect(inactive, &QState::entered,
		[this]() {
			lineEdit_->setEnabled(false);
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

TargetFrontendCardView::TargetFrontendCardView(EnumSelector* parentCommandSelector, AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, int const index, QWidget* parent)
	: QGroupBox(QString("Front End %1").arg(index), parent), index_(index), parentCommandSelector_(parentCommandSelector) {
	deviceIF_ = new Device6991("Device6991", hwConnector, scpiIF, 256, this);
	lineEdit_->setMaximumWidth(70);
	lineEdit_->setInputMask("\\0\\xHHHHHHHH;_");
	lineEdit_->setText("0x00000000");
	lineEdit_->setEnabled(false);

	checkBox_->setChecked(false);

	auto layout = new QHBoxLayout;
	layout->addWidget(lineEdit_);
	layout->addWidget(checkBox_);
	setLayout(layout);

	createConnections();
}

void TargetFrontendCardView::sendCommand(Commands1Enum::Type const cmd, uint32_t const address) noexcept {
	if (isEnabled() && checkBox_->isChecked()) {
		if (cmd == Commands1Enum::READ) {
			if (auto reg = deviceIF_->readFecRegister(index_, address); reg)
				lineEdit_->setText(toHex(*reg, 8));
			else
				QMessageBox::critical(this, "Error", QString("Read operation faild for frontend card %1!").arg(index_));
		}
		else if (cmd == Commands1Enum::WRITE) {
			auto success = deviceIF_->writeFecRegister(index_, address, lineEdit_->text().toUInt(nullptr, 16));
			if (!success)
				QMessageBox::critical(this, "Error", QString("Write operation faild for frontend card %1!").arg(index_));
		}
	}
}

void RegisterControllerFrontend::createConnections() noexcept {
	connect(refreshButton, &QPushButton::clicked, frontend1_, &TargetFrontendCardView::stateReq);
	connect(refreshButton, &QPushButton::clicked, frontend2_, &TargetFrontendCardView::stateReq);
	connect(executeButton, &QPushButton::clicked,
		[this]() {
			emit frontend1_->sendCommand(static_cast<Commands1Enum::Type>(commandSelector_->value()), addressSelector_->value());
			emit frontend2_->sendCommand(static_cast<Commands1Enum::Type>(commandSelector_->value()), addressSelector_->value());
		}
	);
}

RegisterControllerFrontend::RegisterControllerFrontend(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, QWidget* parent)
	: QGroupBox("Register Controller", parent), frontend1_(new TargetFrontendCardView(commandSelector_, hwConnector, scpiIF, 1)), frontend2_(new TargetFrontendCardView(commandSelector_, hwConnector, scpiIF, 2)){
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

void RegisterController6991::createConnections() noexcept {
	connect(executeButton, &QPushButton::clicked,
		[this]() {
			if (commandSelector_->value() == Commands2Enum::READ) {
				if (auto reg = deviceIF_->readFpgaRegister(addressSelector_->value()); reg)
					lineEdit_->setText(toHex(*reg, 8));
			}				
			else if(commandSelector_->value() == Commands2Enum::WRITE)
				deviceIF_->writeFpgaRegister(addressSelector_->value(), lineEdit_->text().toUInt(nullptr, 16));
		}
	);

	connect(commandSelector_, &EnumSelector::enumChanged,
		[this](int const concreteEnum) {
			if (concreteEnum == Commands2Enum::READ)
				lineEdit_->setEnabled(false);
			else if(concreteEnum == Commands2Enum::WRITE)
				lineEdit_->setEnabled(true);
		}
	);
}

RegisterController6991::RegisterController6991(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, QWidget* parent)
	: QGroupBox("Register Controller", parent) {
	deviceIF_ = new Device6991("Device6111", hwConnector, scpiIF, 256, this);
	lineEdit_->setMaximumWidth(70);
	lineEdit_->setInputMask("\\0\\xHHHHHHHH;_");
	lineEdit_->setText("0x00000000");
	lineEdit_->setEnabled(false);
	auto dataGroup = new QGroupBox("Data", this);
	auto hLayout = new QHBoxLayout;
	hLayout->addWidget(lineEdit_);
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