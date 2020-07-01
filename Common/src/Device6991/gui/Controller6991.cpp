#include "../../../include/Device6991/gui/Controller6991.h"
#include "../../../include/gui/ConnectController.h"
#include "../../../include/gui/Controller6132.h"
#include "../../../include/gui/Controller6111.h"
#include <QThread>
#include <QMetaObject>

void Controller6991::createConnections() {
	connect(deviceThread_, &QThread::finished, deviceThread_, &QObject::deleteLater);
	connect(deviceIF_.get(), &Device6991::logMsg, [](QString const& msg) { qDebug() << "LOG: " << msg; });
	connect(deviceIF_.get(), &Device6991::reportError, [](QString const& msg) { qDebug() << "ERR: " << msg; });
	connect(deviceIF_.get(), &Device6991::acquisitionStarted, acqStartStopButton_, &TwoStateButton::stateChange1);
	connect(deviceIF_.get(), &Device6991::acquisitionStopped, acqStartStopButton_, &TwoStateButton::stateChange2);
	connect(deviceIF_.get(), &Device6991::connectedDataStream, connectDisconnectButton_, &TwoStateButton::stateChange1);
	connect(deviceIF_.get(), &Device6991::disconnectedDataStream, connectDisconnectButton_, &TwoStateButton::stateChange2);
	connect(deviceIF_.get(), &Device6991::connectedDataStream, this, &Controller6991::disableDataStreamCmbBox);
	connect(deviceIF_.get(), &Device6991::disconnectedDataStream, this, &Controller6991::enableDataStreamCmbBox);
	connect(deviceIF_.get(), &Device6991::state, statusView_, &StatusView::update);
	connect(deviceIF_.get(), &Device6991::deviceConfiguration, this, &Controller6991::setModel);
	connect(deviceIF_.get(), &Device6991::reportError, this, &Controller6991::showError);
	connect(this, &Controller6991::takeControlReq, deviceIF_.get(), &Device6991::handleTakeControlReq);
	connect(this, &Controller6991::releaseControlReq, deviceIF_.get(), &Device6991::handleReleaseControlReq);
	connect(this, &Controller6991::connectDataStreamReq, deviceIF_.get(), &Device6991::handleConnectDataStreamReq);
	connect(this, &Controller6991::disconnectDataStreamReq, deviceIF_.get(), &Device6991::handleDisconnectDataStreamReq);
	connect(this, &Controller6991::startAcqReq, deviceIF_.get(), &Device6991::handleStartAcqReq);
	connect(this, &Controller6991::stopAcqReq, deviceIF_.get(), &Device6991::handleStopAcqReq);
	connect(this, &Controller6991::configureDeviceReq, deviceIF_.get(), &Device6991::handleConfigureDeviceReq);
	connect(this, &Controller6991::controllerIdChanged, deviceIF_.get(), &Device6991::handleIdChanged);
	connect(dataStorageCheckBox_, &QCheckBox::stateChanged, deviceIF_.get(), &Device6991::handleSetStoreDataReq);
	connect(forwardDataCheckBox_, &QCheckBox::stateChanged, deviceIF_.get(), &Device6991::handleSetForwardDataReq);
	connect(actionRequestController_, &UserActionRequestController::requestConfirmationReceived, this, &Controller6991::showInformationToConfirmFromDevice);
	connect(autoRefreshTimer_, &QTimer::timeout, deviceIF_.get(), &Device6991::deviceStateRequest);
	connect(statusAutoRefreshCheckBox_, &QCheckBox::stateChanged,
		[this](int const state) {
			state == Qt::Checked ? autoRefreshTimer_->start(500) : autoRefreshTimer_->stop();
		}
	);
	connect(resfreshButton_, &QPushButton::clicked, deviceIF_.get(), &Device6991::deviceStateRequest);

	connect(setModeButton_, &QPushButton::clicked,
		[this]() {
			if (comboBoxMode_->currentData().toInt() == ControlModeEnum::CONTROLLER)
				emit takeControlReq(id());
			else
				emit releaseControlReq();		
		}
	);
	initializeStateMachine();
}

void Controller6991::initializeStateMachine() {
	auto connected = new QState();
	auto disconnected = new QState();
	auto controller = new QState(connected);
	auto listener = new QState(connected);

	connected->setInitialState(listener);

	listener->addTransition(deviceIF_.get(), &Device6991::controlGranted, controller);
	controller->addTransition(deviceIF_.get(), &Device6991::controlReleased, listener);
	disconnected->addTransition(connectController_, &ConnectController::connected, listener);
	connected->addTransition(connectController_, &ConnectController::disconnected, disconnected);

	sm_.addState(connected);
	sm_.addState(disconnected);

	connect(disconnected, &QState::entered,
		[this]() {
			modeGroup_->setEnabled(false);
			comboBoxMode_->setCurrentIndex(0);
			resfreshButton_->setEnabled(false);
			scanRateView_->setEnabled(false);
			scansPerPacketView_->setEnabled(false);
			startModeView_->setEnabled(false);
			stopModeView_->setEnabled(false);
			acqStartStopButton_->setEnabled(false);
			statusAutoRefreshCheckBox_->setEnabled(false);
			statusAutoRefreshCheckBox_->setChecked(false);
			timestampsCheckBox_->setEnabled(false);
			timestampsCheckBox_->setChecked(false);
			// TODO REMOVE THIS AFTER REPLACE DATA STREAM TEMPORARY SOLUTION WITH ONE STREAM SOLUTION ALSO CONNECTING/DISCONNECTING STREAM WILL BE AVAILABLE WITHOUT ACTIVE CONNECTION WITH DEVICE
			if (connectDisconnectButton_->text() == "Disconnect")
				emit disconnectDataStreamReq(); 
			connectDisconnectButton_->setEnabled(false);
			idEdit_->setEnabled(true);
			if (dbgMode_) {
				resgisterControllerFrontend_->setEnabled(false);
				resgisterController6991_->setEnabled(false);
				testController_->setEnabled(false);
				scpiController_->setEnabled(false);
				scpiController_->setLedIndicatorState(false);
			}
			placeHolderForChannelConfigurationController_->hide();
		}
	);
	connect(connected, &QState::entered,
		[this]() {
			modeGroup_->setEnabled(true);
			resfreshButton_->setEnabled(true);
			statusAutoRefreshCheckBox_->setEnabled(true);
			idEdit_->setEnabled(false);
			connectDisconnectButton_->setEnabled(true);
			emit controllerIdChanged(idEdit_->value());
			if (dbgMode_) {
				resgisterControllerFrontend_->setEnabled(true);
				resgisterController6991_->setEnabled(true);
				testController_->setEnabled(true);
				scpiController_->setEnabled(true);
				scpiController_->setLedIndicatorState(true);
			}
			addChannelConfigurationController();
			if (channelConfigurationController_)
				channelConfigurationController_->setEnabled(false);
		}
	);
	connect(controller, &QState::entered,
		[this]() {
			scanRateView_->setEnabled(true);
			scansPerPacketView_->setEnabled(true);
			startModeView_->setEnabled(true);
			stopModeView_->setEnabled(true);
			acqStartStopButton_->setEnabled(true);
			timestampsCheckBox_->setEnabled(true);
			if (channelConfigurationController_)
				channelConfigurationController_->setEnabled(true);
		}
	);
	connect(controller, &QState::exited,
		[this]() {
			scanRateView_->setEnabled(false);
			startModeView_->setEnabled(false);
			stopModeView_->setEnabled(false);
			acqStartStopButton_->setEnabled(false);
			if (channelConfigurationController_)
				channelConfigurationController_->setEnabled(false);
		}
	);
	sm_.setInitialState(disconnected);
	sm_.start();
}

void Controller6991::addChannelConfigurationController() {
	auto fecType = deviceIF_->readFecType(FecIdType::_1);
	QWidget* controller = nullptr;
	if (fecType && *fecType == FecType::_6111)
		controller = new Controller6111(deviceIF_.get(), this);
	else if (fecType && *fecType == FecType::_6132)
		controller = new Controller6132(deviceIF_.get(), this);
	
	if (controller) {	
		if (placeHolderForChannelConfigurationController_->layout()->itemAt(0)) {
			auto item = placeHolderForChannelConfigurationController_->layout()->replaceWidget(channelConfigurationController_, controller);
			delete item->widget();
			delete item;
		}
		else
			placeHolderForChannelConfigurationController_->layout()->addWidget(controller);
		placeHolderForChannelConfigurationController_->show();
		channelConfigurationController_ = controller;
	}
}

Configuration6991 Controller6991::model() const {
	Configuration6991 config;
	config.scanRate_ = scanRateView_->model();
	config.startMode_ = startModeView_->model();
	config.stopMode_ = stopModeView_->model(*config.scanRate_);
	config.scansPerDirectReadPacket_ = scansPerPacketView_->value();
	config.timestamps_ = timestampsCheckBox_->isChecked();
	return config;
}

void Controller6991::showError(QString const& msg) {
	QMessageBox::critical(this, "Error", QString("Error Occured: %1").arg(msg));
}

void Controller6991::showInformationToConfirmFromDevice(QString const& msg, MyPromiseVoid* barrierPromise) {
	QMessageBox::information(this, "Information", msg);
	barrierPromise->set();
}

uint32_t Controller6991::id() const {
	return idEdit_->value();
}

void Controller6991::disableDataStreamCmbBox() const {
	dataStreamComboBox_->setDisabled(true);
}

void Controller6991::enableDataStreamCmbBox() const {
	dataStreamComboBox_->setEnabled(true);
}

void Controller6991::setModel(Configuration6991 const& configuration) {
	if (configuration.scanRate_)
		scanRateView_->setModel(*configuration.scanRate_);
	scansPerPacketView_->setValue(*configuration.scansPerDirectReadPacket_);
	startModeView_->setModel(configuration.startMode_);
	stopModeView_->setModel(configuration.stopMode_);
	timestampsCheckBox_->setChecked(*configuration.timestamps_);
}

Controller6991::Controller6991(std::unique_ptr<Device6991>& devIF, bool const dbgMode, QWidget* parent)
	: QGroupBox("Controller", parent),
	deviceIF_(std::move(devIF)),
	connectController_(new ConnectController(deviceIF_.get(), this)),
	dbgMode_(dbgMode) {
	deviceIF_->enableScpiCommandsPrints(true);
	deviceIF_->setSender(actionRequestController_->createSender());
	deviceIF_->moveToThread(deviceThread_);
	deviceThread_->start();

	placeHolderForChannelConfigurationController_->hide();
	auto controller6132Layout = new QVBoxLayout;
	controller6132Layout->setContentsMargins(0, 0, 0, 0);
	placeHolderForChannelConfigurationController_->setLayout(controller6132Layout);
	comboBoxMode_->setMaximumWidth(100);
	for (auto mode : ControlModeEnum::TYPES)
		comboBoxMode_->addItem(ControlModeEnum::toString(mode), mode);

	idEdit_->setMaximum(255);
	idEdit_->setMinimum(1);
	idEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);

	auto hlayoutId = new QHBoxLayout;
	hlayoutId->addWidget(idEdit_);

	auto groupId = new QGroupBox("Id");
	groupId->setLayout(hlayoutId);

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(comboBoxMode_);
	hlayout->addWidget(setModeButton_);
	modeGroup_->setLayout(hlayout);

	dataStreamComboBox_->setMaximumWidth(30);
	dataStreamComboBox_->addItems(streams);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(dataStreamComboBox_);
	hlayout->addWidget(connectDisconnectButton_);
	dataStreamGroup_->setLayout(hlayout);

	auto hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(groupId);
	hlayout2->addWidget(connectController_);
	hlayout2->addWidget(dataStreamGroup_);

	auto vlayout = new QVBoxLayout;
	hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(scanRateView_);
	hlayout->addWidget(scansPerPacketView_);
	hlayout->addWidget(modeGroup_);
	vlayout->addLayout(hlayout);
	vlayout->addWidget(startModeView_);
	vlayout->addWidget(stopModeView_);
	vlayout->addWidget(placeHolderForChannelConfigurationController_);
	vlayout->addStretch(0);
	vlayout->addWidget(timestampsCheckBox_);
	vlayout->addWidget(dataStorageCheckBox_);
	vlayout->addWidget(forwardDataCheckBox_);
	vlayout->addWidget(statusAutoRefreshCheckBox_);

	auto hButtonslayout = new QHBoxLayout;
	hButtonslayout->addWidget(resfreshButton_);
	hButtonslayout->addWidget(acqStartStopButton_, 1, Qt::AlignRight);

	QImage bustecLogo(":\\images\\bustec.jpg");
	auto imageWidget = new QLabel;
	imageWidget->setPixmap(QPixmap::fromImage(bustecLogo));

	auto layout = new QVBoxLayout;
	layout->addWidget(imageWidget, 1, Qt::AlignCenter);
	layout->addLayout(hlayout2);
	layout->addLayout(vlayout);	
	layout->addWidget(statusView_);
	layout->addLayout(hButtonslayout);
	setLayout(layout);

	if (dbgMode_) {
		auto dbgController = new QWidget();
		resgisterControllerFrontend_ = new RegisterControllerFrontend(deviceIF_.get());
		resgisterController6991_ = new RegisterController6991(deviceIF_.get());
		testController_ = new TestsController(deviceIF_.get());
		scpiController_ = new ScpiController(deviceIF_.get());
		auto layout = new QVBoxLayout;
		layout->addWidget(resgisterControllerFrontend_);
		layout->addWidget(resgisterController6991_);
		layout->addWidget(scpiController_);
		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(testController_);
		hlayout->addLayout(layout);
		dbgController->setLayout(hlayout);
		dbgController->show();
	}

	createConnections();
}

Controller6991::~Controller6991() {
	deviceThread_->quit();///todo test this
	deviceThread_->wait();
}
