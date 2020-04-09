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
#include <QStateMachine>
#include <QTime>
#include <QTimer>
#include "../Device6991.h"
#include <map>
#include <set>
#include "../../gui/TwoStateButton.h"

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
	void addResult(TestTypeEnum::Type const type, QGridLayout * layout, int const row) noexcept;
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
};

class TestsView : public QGroupBox {
	Q_OBJECT
	TestsResultView* resultView_ = new TestsResultView;
	TestSelectionView* selectionView_ = new TestSelectionView;
	QLabel* testElapsedTimeLabel_ = new QLabel("Time elapsed:");
	Device6991* deviceIF_;
	TwoStateButton* startStopTestsButton_ = new TwoStateButton("Start", [this]() { deviceIF_->startTestsRequest(selectionView_->model()); }, "Stop", [this]() {deviceIF_->stopTestsRequest(); });
	QStateMachine sm_;
	QTime time_;
private:
	void createConnections() noexcept;
	void initializeStateMachine() noexcept;
	void updateTime() const noexcept;
public:
	TestsView(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, QWidget* parent = nullptr);
};