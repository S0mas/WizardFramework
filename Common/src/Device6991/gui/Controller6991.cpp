#include "../../../include/Device6991/gui/Controller6991.h"
#include "../../../include/gui/ConnectController.h"
#include "../../../include/gui/Controller6132.h"

void Controller6991::createConnections() noexcept {
	connect(deviceIF_, &Device6991::acquisitionStarted, acqStartStopButton_, &TwoStateButton::connected);
	connect(deviceIF_, &Device6991::acquisitionStopped, acqStartStopButton_, &TwoStateButton::disconnected);
	connect(deviceIF_, &Device6991::connectedDataStream, connectDisconnectButton_, &TwoStateButton::connected);
	connect(deviceIF_, &Device6991::disconnectedDataStream, connectDisconnectButton_, &TwoStateButton::disconnected);
	connect(deviceIF_, &Device6991::connectedDataStream, connectDisconnectButton_, [this]() {dataStreamComboBox_->setDisabled(true); });
	connect(deviceIF_, &Device6991::disconnectedDataStream, connectDisconnectButton_, [this]() {dataStreamComboBox_->setEnabled(true); });
	connect(deviceIF_, &Device6991::state, statusView_, &StatusView::update);
	connect(deviceIF_, &Device6991::configuration, this, &Controller6991::setModel);
	connect(deviceIF_, &Device6991::reportError, this, &Controller6991::showError);
	connect(dataStorageCheckBox_, &QCheckBox::stateChanged, deviceIF_, &Device6991::setStoreData);

	connect(setModeButton_, &QPushButton::clicked,
		[this]() {
			if (comboBoxMode_->currentData().toInt() == ControlModeEnum::CONTROLLER)
				deviceIF_->takeControlRequest(id());
			else
				deviceIF_->releaseControlRequest();

			auto autoRefreshTimer = new QTimer(this);
			connect(autoRefreshTimer, &QTimer::timeout, deviceIF_, &Device6991::deviceStateRequest);
			connect(statusAutoRefreshCheckBox_, &QCheckBox::stateChanged,
				[autoRefreshTimer](int const state) {
					state == Qt::Checked ? autoRefreshTimer->start(500) : autoRefreshTimer->stop();
				}
			);
			connect(resfreshButton_, &QPushButton::clicked, deviceIF_, &Device6991::deviceStateRequest);
		}
	);
	initializeStateMachine();
}

void Controller6991::initializeStateMachine() noexcept {
	auto connected = new QState();
	auto disconnected = new QState();
	auto controller = new QState(connected);
	auto listener = new QState(connected);

	connected->setInitialState(listener);

	listener->addTransition(deviceIF_, &Device6991::controlGranted, controller);
	controller->addTransition(deviceIF_, &Device6991::controlReleased, listener);
	disconnected->addTransition(connectController_, &ConnectController::connected, listener);
	connected->addTransition(connectController_, &ConnectController::disconnected, disconnected);

	sm_.addState(connected);
	sm_.addState(disconnected);

	connect(disconnected, &QState::entered,
		[this]() {
			modeGroup_->setEnabled(false);
			comboBoxMode_->setCurrentIndex(0);
			resfreshButton_->setEnabled(false);
			clockSourceGroup_->setEnabled(false);
			scanRateView_->setEnabled(false);
			startModeView_->setEnabled(false);
			stopModeView_->setEnabled(false);
			acqStartStopButton_->setEnabled(false);
			statusAutoRefreshCheckBox_->setEnabled(false);
			statusAutoRefreshCheckBox_->setChecked(false);
			idEdit_->setEnabled(true);
			if (dbgMode_) {
				resgisterControllerFrontend_->setEnabled(false);
				resgisterController6991_->setEnabled(false);
				testController_->setEnabled(false);
			}
			placeHolderForController6132_->hide();
		}
	);
	connect(connected, &QState::entered,
		[this]() {
			modeGroup_->setEnabled(true);
			resfreshButton_->setEnabled(true);
			statusAutoRefreshCheckBox_->setEnabled(true);
			idEdit_->setEnabled(false);
			if (dbgMode_) {
				resgisterControllerFrontend_->setEnabled(true);
				resgisterController6991_->setEnabled(true);
				testController_->setEnabled(true);
			}
			addController6132IfNeeded();
		}
	);
	connect(controller, &QState::entered,
		[this]() {
			clockSourceGroup_->setEnabled(true);
			scanRateView_->setEnabled(true);
			startModeView_->setEnabled(true);
			stopModeView_->setEnabled(true);
			acqStartStopButton_->setEnabled(true);
		}
	);
	connect(controller, &QState::exited,
		[this]() {
			clockSourceGroup_->setEnabled(false);
			scanRateView_->setEnabled(false);
			startModeView_->setEnabled(false);
			stopModeView_->setEnabled(false);
			acqStartStopButton_->setEnabled(false);
		}
	);
	sm_.setInitialState(disconnected);
	sm_.start();
}

void Controller6991::addController6132IfNeeded() noexcept {
	Device6132* _6132AtFec1 = nullptr;
	Device6132* _6132AtFec2 = nullptr;
	if (auto fecType = deviceIF_->readFecType(FecIdType::_1); fecType && *fecType == FecType::_6132)
		_6132AtFec1 = new Device6132(FecIdType::_1, deviceIF_);
	if (auto fecType = deviceIF_->readFecType(FecIdType::_2); fecType && *fecType == FecType::_6132)
		_6132AtFec2 = new Device6132(FecIdType::_2, deviceIF_);
	if (_6132AtFec1 || _6132AtFec2) {
		auto newController6132 = new Controller6132(_6132AtFec1, _6132AtFec2, this);
		if (placeHolderForController6132_->layout()->itemAt(0)) {
			auto item = placeHolderForController6132_->layout()->replaceWidget(controller6132_, newController6132);
			delete item->widget();
			delete item;
		}
		else
			placeHolderForController6132_->layout()->addWidget(newController6132);

		placeHolderForController6132_->show();
		controller6132_ = newController6132;
	}
}

Configuration6991 Controller6991::model() const noexcept {
	Configuration6991 config;
	config.scanRate_ = scanRateView_->model();
	config.startMode_ = startModeView_->model();
	config.stopMode_ = stopModeView_->model(*config.scanRate_);
	config.clockSource_ = static_cast<ClockSourceEnum::Type>(clockSourceComboBox->currentData().toInt());
	config.scansPerDirectReadPacket_ = 10;
	//TODO
	//config.timestamps_ =
	//config.scansPerDirectReadPacket_ = 
	//config.fansMode_ =
	return config;
}

void Controller6991::showError(QString const& msg) noexcept {
	QMessageBox::critical(this, "Error", QString("Error Occured: %1").arg(msg));
}

uint32_t Controller6991::id() const noexcept {
	return idEdit_->value();
}

void Controller6991::setModel(Configuration6991 const& configuration) noexcept {
	if (configuration.scanRate_)
		scanRateView_->setModel(*configuration.scanRate_);
	startModeView_->setModel(configuration.startMode_);
	stopModeView_->setModel(configuration.stopMode_);
	if (configuration.clockSource_)
		clockSourceComboBox->setCurrentIndex(*configuration.clockSource_);
	//TODO
	//if (configuration.fansMode_)
	//	//
	//if (configuration.timestamps_)
	//	//
	//if (configuration.scansPerDirectReadPacket_)
	//	//
	//if (configuration.ptpTime_)
	//	//
}

Controller6991::Controller6991(Device6991* devIF, bool const dbgMode, QWidget* parent)
	: QGroupBox("Controller", parent),
	deviceIF_(devIF),
	connectController_(new ConnectController(deviceIF_, this)),
	dbgMode_(dbgMode) {

	placeHolderForController6132_->hide();
	auto controller6132Layout = new QVBoxLayout;
	controller6132Layout->setContentsMargins(0, 0, 0, 0);
	placeHolderForController6132_->setLayout(controller6132Layout);
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

	for (auto clock : ClockSourceEnum::TYPES)
		clockSourceComboBox->addItem(ClockSourceEnum::toString(clock), clock);
	clockSourceComboBox->setMaximumWidth(80);

	auto vlayout = new QVBoxLayout;
	vlayout->addWidget(clockSourceComboBox);
	clockSourceGroup_->setLayout(vlayout);

	vlayout = new QVBoxLayout;
	hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(scanRateView_);
	hlayout->addWidget(clockSourceGroup_);
	hlayout->addWidget(modeGroup_);
	vlayout->addLayout(hlayout);
	vlayout->addWidget(startModeView_);
	vlayout->addWidget(stopModeView_);
	vlayout->addWidget(placeHolderForController6132_);
	vlayout->addStretch(0);
	vlayout->addWidget(dataStorageCheckBox_);
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
		resgisterControllerFrontend_ = new RegisterControllerFrontend(deviceIF_);
		resgisterController6991_ = new RegisterController6991(deviceIF_);
		testController_ = new TestsController(deviceIF_);
		auto layout = new QVBoxLayout;
		layout->addWidget(resgisterControllerFrontend_);
		layout->addWidget(resgisterController6991_);
		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(testController_);
		hlayout->addLayout(layout);
		dbgController->setLayout(hlayout);
		dbgController->show();
	}

	createConnections();
}