#pragma once
#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QStateMachine>
#include <QTime>
#include <QTimer>
#include "../Device6991.h"
#include "../../gui/TwoStateButton.h"

class FifoTestView : public QGroupBox {
	QSpinBox* blockSizeEdit_ = new QSpinBox;
	QDoubleSpinBox* rateEdit_ = new QDoubleSpinBox;
	QSpinBox* tresholdEdit_ = new QSpinBox;
	QLabel* samplesInFifoCount_ = new QLabel;
	QLabel* overflowCounter_ = new QLabel;
	QLabel* dataError_ = new QLabel;
	QLabel* passThresholdCounter_ = new QLabel;
	QLabel* lastSample_ = new QLabel;
public:
	FifoTestView(QWidget* parent = nullptr);
	FifoTestModel::Configuration config() const noexcept;
	void setModel(FifoTestModel const& model) noexcept;
};

class DlTestView : public QGroupBox {
	QRadioButton* freq1_ = new QRadioButton(DlSclkFreqType::toString(DlSclkFreqType::_1Mhz));
	QRadioButton* freq2_ = new QRadioButton(DlSclkFreqType::toString(DlSclkFreqType::_2Mhz));
	QRadioButton* freq4_ = new QRadioButton(DlSclkFreqType::toString(DlSclkFreqType::_4Mhz));
	QRadioButton* freq5_ = new QRadioButton(DlSclkFreqType::toString(DlSclkFreqType::_5Mhz));
	QRadioButton* freq10_ = new QRadioButton(DlSclkFreqType::toString(DlSclkFreqType::_10Mhz));
public:
	DlTestView(QWidget* parent = nullptr);
	void removeSelection() noexcept;
	void defaultSelection() noexcept;
	DlSclkFreqType::Type selected() const noexcept;
};

class TestsResultView : public QGroupBox {
	Q_OBJECT
	struct ResultView {
		TestTypeEnum::Type type_;
		QLabel* testName_;
		QLabel* count_ = new QLabel("0");
		QLabel* errors_ = new QLabel("0");
		ResultView(TestTypeEnum::Type const& type) noexcept;
		void setModel(Result const& result) noexcept;
		Result model() const noexcept;
	};
	std::vector<ResultView*> resultViews_;
	QStringList resultLabels = { "Count", "Errors" };
	void addResult(TestTypeEnum::Type const type, QGridLayout * layout, uint32_t const row) noexcept;
public:
	TestsResultView(QWidget* parent = nullptr);
	TestsResultModel model() const noexcept;
	void resetModel() const noexcept;
public slots:
	void setModel(TestsResultModel const& model) const noexcept;
};

class TestSelectionView : public QGroupBox {
	Q_OBJECT
	struct TestSelectView {
		TestTypeEnum::Type type_;
		QCheckBox* checkBox_;
		TestSelectView(TestTypeEnum::Type const type) : type_(type), checkBox_(new QCheckBox(TestTypeEnum::toString(type))) {}
	};
	std::vector<TestSelectView*> testSelectViews_;
public:
	TestSelectionView(QWidget* parent = nullptr);
	void setModel(TestsSelectionModel const& model) const noexcept;
	TestsSelectionModel model() const noexcept;
signals:
	void fifoSelected(bool const selected) const;
	void dlSelected(bool const selected) const;
};

class TestsController : public QGroupBox {
	Q_OBJECT
	TestsResultView* resultView_ = new TestsResultView;
	TestSelectionView* selectionView_ = new TestSelectionView;
	QLabel* testElapsedTimeLabel_ = new QLabel("Time elapsed:");
	Device6991* deviceIF_;
	TwoStateButton* startStopTestsButton_ = new TwoStateButton("Start", [this]() { emit startTestReq({ selectionView_->model(), fifoTestView_->config(), dlTestView_->selected() }); }, "Stop", [this]() { emit stopTestReq(); });
	QStateMachine sm_;
	QTime time_;
	FifoTestView* fifoTestView_ = new FifoTestView;
	DlTestView* dlTestView_ = new DlTestView;
private:
	void createConnections() noexcept;
	void initializeStateMachine() noexcept;
	void updateTime() const noexcept;
signals:
	void startTestReq(StartTestsRequest const& request) const;
	void stopTestReq() const;
public:
	TestsController(Device6991* devIF, QWidget* parent = nullptr);
};