#include "../../../include/Device6991/gui/TestsController.h"

TestsResultView::ResultView::ResultView(TestTypeEnum::Type const& type) noexcept : type_(type), testName_(new QLabel(TestTypeEnum::toString(type))) {}

void TestsResultView::ResultView::setModel(Result const & result) noexcept {
	if(result.count_)
		count_->setText(QString::number(*result.count_));
	if(result.errors_)
		errors_->setText(QString::number(*result.errors_));
}

Result TestsResultView::ResultView::model() const noexcept {
	return { count_->text().toInt(), errors_->text().toInt() };
}

void TestsResultView::addResult(TestTypeEnum::Type const type, QGridLayout * layout, uint32_t const row) noexcept {
	auto resultView = new ResultView(type);
	layout->addWidget(resultView->testName_, row, 0, Qt::AlignLeft | Qt::AlignVCenter);
	layout->addWidget(resultView->count_, row, 1, Qt::AlignCenter);
	layout->addWidget(resultView->errors_, row, 2, Qt::AlignCenter);
	resultViews_.push_back(resultView);
}

TestsResultView::TestsResultView(QWidget * parent) : QGroupBox("Result", parent) {
	auto layout = new QGridLayout;
	for (auto resultLabel : resultLabels)
		layout->addWidget(new QLabel(resultLabel), 0, layout->columnCount(), Qt::AlignCenter);
	for (auto test : TestTypeEnum::TYPES)
		addResult(test, layout, layout->rowCount());
	setLayout(layout);
}

TestsResultModel TestsResultView::model() const noexcept {
	TestsResultModel m;
	for (auto resultView : resultViews_)
		m[resultView->type_] = resultView->model();
	return m;
}

void TestsResultView::setModel(TestsResultModel const& model) const noexcept {
	for (auto resultView : resultViews_)
		resultView->setModel(model.at(resultView->type_));
}

void TestsResultView::resetModel() const noexcept {
	for (auto resultView : resultViews_)
		resultView->setModel({ 0, 0 });
}

TestSelectionView::TestSelectionView(QWidget * parent) : QGroupBox("Selection", parent) {
	auto layout = new QVBoxLayout;
	TestSelectView* dl0CheckBox = nullptr;
	TestSelectView* dl1CheckBox = nullptr;
	TestSelectView* fifoCheckBox = nullptr;

	for (auto test : TestTypeEnum::TYPES) {
		auto testSelectView = new TestSelectView(test);
		layout->addWidget(testSelectView->checkBox_);
		testSelectViews_.push_back(testSelectView);
		if (test == TestTypeEnum::DL0)
			dl0CheckBox = testSelectView;
		else if (test == TestTypeEnum::DL1)
			dl1CheckBox = testSelectView;
		else if (test == TestTypeEnum::FIFO)
			fifoCheckBox = testSelectView;
	}

	////////////////////////////////////////////////
	//FIFO AND DL TESTS ARE EXCLUDING EACH OTHER
	connect(fifoCheckBox->checkBox_, &QCheckBox::stateChanged, 
		[this, dl0CheckBox, dl1CheckBox](int const state) {
			if (state == Qt::Checked) {
				dl0CheckBox->checkBox_->setChecked(false);
				dl1CheckBox->checkBox_->setChecked(false);
			}
			emit fifoSelected(state == Qt::Checked);		
		}
	);

	connect(dl0CheckBox->checkBox_, &QCheckBox::stateChanged,
		[this, fifoCheckBox, dl1CheckBox](int const state) {
			if (state == Qt::Checked) {
				fifoCheckBox->checkBox_->setChecked(false);
				emit dlSelected(true);
			}
			else if (!dl1CheckBox->checkBox_->isChecked())
				emit dlSelected(false);
		}
	);

	connect(dl1CheckBox->checkBox_, &QCheckBox::stateChanged,
		[this, fifoCheckBox, dl0CheckBox](int const state) {
			if (state == Qt::Checked) {
				fifoCheckBox->checkBox_->setChecked(false);
				emit dlSelected(true);
			}
			else if (!dl0CheckBox->checkBox_->isChecked())
				emit dlSelected(false);
		}
	);
	///////////////////////////////////////////////

	setLayout(layout);
}

void TestSelectionView::setModel(TestsSelectionModel const & model) const noexcept {
	for (auto view : testSelectViews_)
		view->checkBox_->setChecked(model.at(view->type_));
}

TestsSelectionModel TestSelectionView::model() const noexcept {
	TestsSelectionModel m;
	for (auto view : testSelectViews_)
		m[view->type_] = view->checkBox_->isChecked();
	return m;
}


void TestsController::createConnections() noexcept {
	connect(deviceIF_, &Device6991::testsStarted, startStopTestsButton_, &TwoStateButton::connected);
	connect(deviceIF_, &Device6991::testsStopped, startStopTestsButton_, &TwoStateButton::disconnected);
	connect(deviceIF_, &Device6991::testCounters, [this](TestsStatus const& status) { resultView_->setModel(status.model); if(selectionView_->model().at(TestTypeEnum::FIFO))fifoTestView_->setModel(status.fifoTestModel_); });
	connect(selectionView_, &TestSelectionView::fifoSelected, fifoTestView_, &FifoTestView::setEnabled);
	connect(selectionView_, &TestSelectionView::dlSelected,
		[this](bool const state) {
			dlTestView_->setEnabled(state);
			if(!state)
				dlTestView_->defaultSelection();
		}
	);
	initializeStateMachine();
}

void TestsController::initializeStateMachine() noexcept {
	auto idle = new QState();
	auto running = new QState();

	idle->addTransition(deviceIF_, &Device6991::testsStarted, running);
	running->addTransition(deviceIF_, &Device6991::testsStopped, idle);

	auto timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &TestsController::updateTime);
	connect(timer, &QTimer::timeout, deviceIF_, &Device6991::testCountersRequest);

	sm_.addState(idle);
	sm_.addState(running);

	connect(idle, &QState::entered,
		[this, timer]() {
			selectionView_->setEnabled(true);
			fifoTestView_->setEnabled(selectionView_->model().at(TestTypeEnum::FIFO));
			timer->stop();
		}
	);
	connect(running, &QState::entered,
		[this, timer]() {
			selectionView_->setEnabled(false);
			fifoTestView_->setEnabled(false);
			resultView_->resetModel();
			time_.restart();
			updateTime();
			timer->start(1000);
		}
	);
	sm_.setInitialState(idle);
	sm_.start();
}

void TestsController::updateTime() const noexcept {
	auto msecs = time_.elapsed();
	auto seconds = (msecs / 1000) % 60;
	auto minutes = (msecs / 60000) % 60;
	testElapsedTimeLabel_->setText(QString("Time elapsed: %1m %2s").arg(minutes).arg(seconds));
}

TestsController::TestsController(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, QWidget * parent) : QGroupBox("", parent) {
	deviceIF_ = new Device6991("Device6991", hwConnector, scpiIF, 256, this);
	deviceIF_->enableScpiCommandsPrints(false);
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(selectionView_);
	hlayout->addWidget(resultView_);

	auto layout = new QVBoxLayout;
	layout->addLayout(hlayout);
	layout->addWidget(fifoTestView_);
	layout->addWidget(dlTestView_);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(startStopTestsButton_, 1, Qt::AlignRight);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(testElapsedTimeLabel_);
	layout->addLayout(hlayout);
	setLayout(layout);
	createConnections();
}

FifoTestView::FifoTestView(QWidget * parent) : QGroupBox("Fifo test", parent) {
	blockSizeEdit_->setMaximum(127);
	blockSizeEdit_->setMinimum(1);
	blockSizeEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);
	blockSizeEdit_->setMaximumWidth(30);

	tresholdEdit_->setMaximum(2500);
	tresholdEdit_->setMinimum(1);
	tresholdEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);
	tresholdEdit_->setMaximumWidth(40);

	rateEdit_->setMaximum(35791392693.34424);
	rateEdit_->setMinimum(8.333333);
	rateEdit_->setDecimals(6);
	rateEdit_->setButtonSymbols(QAbstractSpinBox::NoButtons);
	rateEdit_->setMaximumWidth(80);

	auto group = new QGroupBox("Block Size");
	auto layout = new QVBoxLayout;
	auto hlayout = new QHBoxLayout;
	layout->addWidget(blockSizeEdit_);
	group->setLayout(layout);
	hlayout->addWidget(group);

	group = new QGroupBox("Rate");
	layout = new QVBoxLayout;
	layout->addWidget(rateEdit_);
	group->setLayout(layout);
	hlayout->addWidget(group);

	group = new QGroupBox("Fifo Treshold");
	layout = new QVBoxLayout;
	layout->addWidget(tresholdEdit_);
	group->setLayout(layout);
	hlayout->addWidget(group);

	auto mainLayout = new QVBoxLayout;
	mainLayout->addLayout(hlayout);

	group = new QGroupBox("Test status");
	layout = new QVBoxLayout;
	hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel("Samples count:"));
	hlayout->addWidget(samplesInFifoCount_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel("Overflows:"));
	hlayout->addWidget(overflowCounter_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel("Data Errors:"));
	hlayout->addWidget(dataError_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel("Pass treshold:"));
	hlayout->addWidget(passThresholdCounter_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel("Last sample:"));
	hlayout->addWidget(lastSample_);
	layout->addLayout(hlayout);
	group->setLayout(layout);
	mainLayout->addWidget(group);
	setLayout(mainLayout);
}

FifoTestModel::Configuration FifoTestView::config() const noexcept {
	FifoTestModel::Configuration configuration;
	configuration.blockSize_ = blockSizeEdit_->value();
	configuration.treshold_ = tresholdEdit_->value();
	configuration.rate_ = rateEdit_->value();
	return configuration;
}

void FifoTestView::setModel(FifoTestModel const & model) noexcept {
	if (model.config_.blockSize_)
		blockSizeEdit_->setValue(*model.config_.blockSize_);
	if (model.config_.rate_)
		rateEdit_->setValue(*model.config_.rate_);
	if (model.config_.treshold_)
		tresholdEdit_->setValue(*model.config_.treshold_);
	if (model.samplesCount_)
		samplesInFifoCount_->setText(QString::number(*model.samplesCount_));
	if (model.lastSample_)
		lastSample_->setText(QString::number(*model.lastSample_, 16));
	overflowCounter_->setText(QString::number(model.overflows_));
	dataError_->setText(QString::number(model.dataErrorsCount_));
	passThresholdCounter_->setText(QString::number(model.passTresholdCount_));
}

DlTestView::DlTestView(QWidget * parent) : QGroupBox("DL Clock Frequency", parent) {
	auto layout = new QHBoxLayout;
	layout->addWidget(freq1_);
	layout->addWidget(freq2_);
	layout->addWidget(freq4_);
	layout->addWidget(freq5_);
	layout->addWidget(freq10_);

	setEnabled(false);
	defaultSelection();
	setLayout(layout);
}

void DlTestView::removeSelection() noexcept {
	freq1_->setChecked(false);
	freq2_->setChecked(false);
	freq4_->setChecked(false);
	freq5_->setChecked(false);
	freq10_->setChecked(false);
}

void DlTestView::defaultSelection() noexcept {
	removeSelection();
	freq2_->setChecked(true);
}

DlSclkFreqType::Type DlTestView::selected() const noexcept {
	QRadioButton* selectedButton;
	if (freq1_->isChecked())
		selectedButton = freq1_;
	else if (freq2_->isChecked())
		selectedButton = freq2_;
	else if (freq4_->isChecked())
		selectedButton = freq4_;
	else if (freq5_->isChecked())
		selectedButton = freq5_;
	else if (freq10_->isChecked())
		selectedButton = freq10_;
	return DlSclkFreqType::fromString(selectedButton->text());
}
