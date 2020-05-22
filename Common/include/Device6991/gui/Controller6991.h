#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStateMachine>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QMessageBox>
#include <QVBoxLayout>

#include <memory>

#include "../Device6991.h"
#include "AcquisitionStopModeView.h"
#include "AcquisitionStartModeView.h"
#include "../../gui/TwoStateButton.h"
#include "ScanRateView.h"
#include "StatusView.h"
#include "TestsController.h"
#include "Controller6991.h"
#include "RegisterController.h"

class ConnectController;
class Controller6132;

class Controller6991 : public QGroupBox {
	//TODO
	// Data Storage implementation
	Q_OBJECT
	std::unique_ptr<Device6991> deviceIF_;
	ConnectController* connectController_;
	QStateMachine sm_;
	QTime time_;
	QSpinBox* idEdit_ = new QSpinBox;
	QWidget* modeGroup_ = new QGroupBox("ControlMode");
	QComboBox* comboBoxMode_ = new QComboBox;
	QPushButton* setModeButton_ = new QPushButton("Set");
	QWidget* dataStreamGroup_ = new QGroupBox("Data Stream");
	QComboBox* dataStreamComboBox_ = new QComboBox;
	TwoStateButton* connectDisconnectButton_ = new TwoStateButton("Connect", [this]() { emit connectDataStreamReq(dataStreamComboBox_->currentText().toUInt()); }, "Disconnect", [this]() { emit disconnectDataStreamReq(); });
	TwoStateButton* acqStartStopButton_ = new TwoStateButton("Start", [this]() { emit configureDeviceReq(model()); emit startAcqReq(); }, "Stop", [this]() { emit stopAcqReq(); });
	ScanRateView* scanRateView_ = new ScanRateView;
	QWidget* clockSourceGroup_ = new QGroupBox("Clock Source");
	QComboBox* clockSourceComboBox = new QComboBox;
	AcquisitionStartModeView* startModeView_ = new AcquisitionStartModeView;
	AcquisitionStopModeView* stopModeView_ = new AcquisitionStopModeView;
	QPushButton* resfreshButton_ = new QPushButton("Refresh");
	QPushButton* stopButton_ = new QPushButton("Stop");
	QPushButton* startButton_ = new QPushButton("Start");
	QCheckBox* dataStorageCheckBox_ = new QCheckBox("Data Storage");
	QCheckBox* statusAutoRefreshCheckBox_ = new QCheckBox("Auto Status Refresh");
	StatusView* statusView_ = new StatusView;
	QWidget* placeHolderForController6132_ = new QWidget;
	Controller6132* controller6132_;
	RegisterControllerFrontend* resgisterControllerFrontend_ = nullptr;
	RegisterController6991* resgisterController6991_ = nullptr;
	TestsController* testController_ = nullptr;
	bool dbgMode_ = false;
	QStringList streams = { "1", "2", "3", "4" };
	QThread* deviceThread_ = new QThread(this);
private:
	void createConnections() noexcept;
	void initializeStateMachine() noexcept;
	void addController6132IfNeeded() noexcept;
	Configuration6991 model() const noexcept;
	void showError(QString const&) noexcept;
	uint32_t id() const noexcept;
private slots:
	void setModel(Configuration6991 const& model) noexcept;
signals:
	void takeControlReq(int const id) const;
	void releaseControlReq() const;
	void connectDataStreamReq(int const dataStreamId) const;
	void disconnectDataStreamReq() const;
	void configureDeviceReq(Configuration6991 const& config) const;
	void startAcqReq() const;
	void stopAcqReq() const;
public:
	Controller6991(std::unique_ptr<Device6991>& devIF, bool const dbgMode = false, QWidget* parent = nullptr);
	~Controller6991() override;
};