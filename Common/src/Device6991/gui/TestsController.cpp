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

void TestsResultView::addResult(TestTypeEnum::Type const type, QGridLayout * layout, int const row) noexcept {
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

	//FIFO AND DL TESTS ARE EXCLUDING EACH OTHER
	connect(fifoCheckBox->checkBox_, &QCheckBox::stateChanged, 
		[dl0CheckBox, dl1CheckBox](int const state) {
			if (state == Qt::Checked) {
				dl0CheckBox->checkBox_->setChecked(false);
				dl1CheckBox->checkBox_->setChecked(false);
			}
		}
	);

	connect(dl0CheckBox->checkBox_, &QCheckBox::stateChanged,
		[fifoCheckBox](int const state) {
			if (state == Qt::Checked)
				fifoCheckBox->checkBox_->setChecked(false);
		}
	);

	connect(dl1CheckBox->checkBox_, &QCheckBox::stateChanged,
		[fifoCheckBox](int const state) {
			if (state == Qt::Checked)
				fifoCheckBox->checkBox_->setChecked(false);
		}
	);

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
	connect(deviceIF_, &Device6991::testCounters, resultView_, &TestsResultView::setModel);
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
			timer->stop();
		}
	);
	connect(running, &QState::entered,
		[this, timer]() {
			selectionView_->setEnabled(false);
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
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(selectionView_);
	hlayout->addWidget(resultView_);

	auto layout = new QVBoxLayout;
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(startStopTestsButton_, 1, Qt::AlignRight);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(testElapsedTimeLabel_);
	layout->addLayout(hlayout);
	setLayout(layout);
	createConnections();
}