#pragma once
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QDebug>
#include <QString>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QRandomGenerator>
#include <array>
#include <algorithm>
#include <vector>
#include "Command.h"
#include "Defines6991.h"
#include "Registers6991.h"
#include <map>

//dbg
#include <thread>
#include <chrono>

class HardwareMock6991 : public QObject {
	Q_OBJECT
	const int ERROR_DEBUG_PRIVATE_REGISTER = 0xFACE;
	Configuration6991 config_;
	PtpTime ptpTime_;
	std::bitset<8> stb_ = 0;
	QString lastError_;
	DeviceStateEnum::Type state_ = DeviceStateEnum::INIT;
	std::array<QTcpServer*, 4> servers_;
	std::array<QTcpSocket*, 4> sockets_;
	std::map<int, int> fpgaRegs_;
	std::map<int, int> cpuRegs_;
	std::array<std::map<int, int>, 2> fcRegs_;
	QTimer* acqStartTimer_;
	int controllerId_ = 0;

	bool dbgModeOn_ = false;
	bool areTestsRunning = false;
	bool acquisitionStoppedOnError_ = false;

	mutable bool state = false;

	void checkError() noexcept {
		if (fpgaRegs_[ERROR_DEBUG_PRIVATE_REGISTER] != 0) {
			setError("DEBUG_ERROR_TRIGGERED");
			fpgaRegs_[ERROR_DEBUG_PRIVATE_REGISTER] = 0;
		}
	}

	void checkTestsRegs() noexcept {
		bool testsStarted = false;
		bool testsStopped = true;
		const int CL0_SEL = 24;
		const int CL1_SEL = 25;
		const int CL_TEST_LOOP = 26;
		const int CL_START_CL_BUSY = 31;
		std::bitset<32> regCL = fpgaRegs_[RegistersEnum::CL_SPI_CSR_reg];

		const int DL0_TEST_MODE = 24;
		const int DL1_TEST_MODE = 28;
		const int DL_EN = 31;
		std::bitset<32> regDL = fpgaRegs_[RegistersEnum::DL_SPI_CSR1_reg];

		const int DFIFO_TEST_MODE = 23;
		std::bitset<32> regFIFO = fpgaRegs_[RegistersEnum::DFIFO_CSR_reg];
		if (!areTestsRunning) {
			if (regCL[CL_TEST_LOOP] && regCL[CL_START_CL_BUSY]) {
				testsStarted = true;
				if (regCL[CL0_SEL])
					testSelectionModel_[TestTypeEnum::CL0] = true;
				if (regCL[CL1_SEL])
					testSelectionModel_[TestTypeEnum::CL1] = true;
			}
			if (regDL[DL_EN]) {
				testsStarted = true;
				if (regDL[DL0_TEST_MODE])
					testSelectionModel_[TestTypeEnum::DL0] = true;
				if (regDL[DL1_TEST_MODE])
					testSelectionModel_[TestTypeEnum::DL1] = true;
			}
			if (regFIFO[DFIFO_TEST_MODE]) {
				testsStarted = true;
				testSelectionModel_[TestTypeEnum::FIFO] = true;
			}
			if (testsStarted)
				startTests();
		}
		else {
			if (!regCL[CL_TEST_LOOP]) {
				testSelectionModel_[TestTypeEnum::CL0] = false;
				testSelectionModel_[TestTypeEnum::CL1] = false;
			}
			if (!regDL[DL_EN]) {
				testSelectionModel_[TestTypeEnum::DL0] = false;
				testSelectionModel_[TestTypeEnum::DL1] = false;
			}
			if (!regFIFO[DFIFO_TEST_MODE])
				testSelectionModel_[TestTypeEnum::FIFO] = false;
			for (auto test : testSelectionModel_)
				if (test.second)
					testsStopped = false;
			if (testsStopped)
				stopTests();
		}

		checkError();
	}

	void updateCounters() noexcept {
		std::vector<RegistersEnum::Type> countersRegsToUpdate;
		if (testSelectionModel_[TestTypeEnum::CL0] || testSelectionModel_[TestTypeEnum::CL1]) {
			countersRegsToUpdate.push_back(RegistersEnum::CL_SPI_TLCNT_reg);
			if (QRandomGenerator::global()->generate() % 4 == 0 && testSelectionModel_[TestTypeEnum::CL0])
				countersRegsToUpdate.push_back(RegistersEnum::CL0_SPI_TLERR_reg);
			if (QRandomGenerator::global()->generate() % 4 == 0 && testSelectionModel_[TestTypeEnum::CL1])
				countersRegsToUpdate.push_back(RegistersEnum::CL1_SPI_TLERR_reg);
		}
		if (testSelectionModel_[TestTypeEnum::DL0]) {
			countersRegsToUpdate.push_back(RegistersEnum::DL0_SPI_TMCNT_reg);
			if (QRandomGenerator::global()->generate() % 4 == 0 && testSelectionModel_[TestTypeEnum::DL0])
				countersRegsToUpdate.push_back(RegistersEnum::DL0_SPI_TMERR_reg);
		}
		if (testSelectionModel_[TestTypeEnum::DL1]) {
			countersRegsToUpdate.push_back(RegistersEnum::DL1_SPI_TMCNT_reg);
			if (QRandomGenerator::global()->generate() % 4 == 0 && testSelectionModel_[TestTypeEnum::DL1])
				countersRegsToUpdate.push_back(RegistersEnum::DL1_SPI_TMERR_reg);
		}
		for (auto regType : countersRegsToUpdate)
			++fpgaRegs_[regType];
	}
	QTimer* timer_;
	TestsSelectionModel testSelectionModel_;

	void setError(QString const& errorMsg) noexcept {
		lastError_ = errorMsg;
		stb_.set(2);
		if (isAcqActive() && stopOnError()) {
			stopAcquisition();
			state_ = DeviceStateEnum::ERROR;
			acquisitionStoppedOnError_ = true;
		}
	}
public:
	HardwareMock6991(QObject* parent = nullptr) : QObject(parent) {
		fpgaRegs_[ERROR_DEBUG_PRIVATE_REGISTER] = 0;
		timer_ = new QTimer(this);
		acqStartTimer_ = new QTimer(this);
		auto checkTestsRegsTimer = new QTimer(this);
		connect(timer_, &QTimer::timeout, this, &HardwareMock6991::updateCounters);

		connect(checkTestsRegsTimer, &QTimer::timeout, this, &HardwareMock6991::checkTestsRegs);
		fpgaRegs_[RegistersEnum::CL_SPI_CSR_reg] = 0;
		fpgaRegs_[RegistersEnum::DL_SPI_CSR1_reg] = 0;
		fpgaRegs_[RegistersEnum::DFIFO_CSR_reg] = 0;
		fpgaRegs_[0x4680] = 0; //ACQ_CSR_reg
		checkTestsRegsTimer->start(500);
		int port = 1;
		for (int i = 0; i < servers_.size(); ++i) {
			servers_[i] = new QTcpServer(this);
			servers_[i]->setMaxPendingConnections(1);
			servers_[i]->listen(QHostAddress::LocalHost, i+1);
			sockets_[i] = nullptr;
			connect(servers_[i], &QTcpServer::newConnection,
				[this, i]() {
					sockets_[i] = servers_[i]->nextPendingConnection();
					connect(sockets_[i], &QAbstractSocket::disconnected,
						[this, i]() {
							servers_[i]->listen(QHostAddress::LocalHost, i + 1);
							sockets_[i]->deleteLater();
							sockets_[i] = nullptr;
						}
					);
					servers_[i]->close(); 
				}
			);
		}
	}

	QString readError() noexcept {
		stb_.set(2, false);
		auto tmp = lastError_;
		lastError_ = "";
		state_ = isAcqActive() ? DeviceStateEnum::ACQUISITION : DeviceStateEnum::IDLE;	
		return tmp;
	}

	int stb() const noexcept {
		return stb_.to_ulong();
	}

	bool isFifoOverflow() const noexcept {
		return false; // TODO
	}

	bool isFifoUnderflow() const noexcept {
		return false; // TODO
	}

	int scansInFifo() const noexcept {
		return 500;
	}

	bool wasAcqStoppedOnError() const noexcept {
		return acquisitionStoppedOnError_; 
	}

	DeviceState status() const noexcept {
		DeviceState state;
		state.setFifoOverflow(isFifoOverflow());
		state.setFifoUnderflow(isFifoUnderflow());
		state.setNumberOfScansInFifo(scansInFifo());
		state.setAcquisitionStoppedOnError(wasAcqStoppedOnError());
		std::array<bool, 4> links;
		std::transform(sockets_.begin(), sockets_.end(), links.begin(),
			[](QTcpSocket* socket) { return socket != nullptr; });
		state.setLinksConnectionStatus(links);
		state.setState(state_);
		return state;
	}

	ScanRateModel scanRate() const noexcept {
		return *config_.scanRate_;
	}

	void setScanRate(ScanRateModel const scanRate) noexcept {
		config_.scanRate_ = scanRate;
	}

	AcquisitionStartModeEnum::Type startMode() const noexcept {
		return *config_.startMode_.mode_;
	}

	void setStartMode(AcquisitionStartModeEnum::Type const mode) noexcept {
		config_.startMode_.mode_ = mode;
		if (mode == AcquisitionStartModeEnum::PTP_ALARM) {
			acqStartTimer_->stop();
			auto mseconds = (ptpAlarm().seconds_ - ptpTime().seconds_) * 1000;
			(mseconds > 0) ? acqStartTimer_->singleShot(mseconds, this, &HardwareMock6991::startAcquisition) : startAcquisition();
		}		
		else
			acqStartTimer_->stop();
	}

	PtpTime ptpTime() const noexcept {
		PtpTime time;
		time.seconds_ = static_cast<int>(QDateTime::currentDateTime().currentSecsSinceEpoch());
		return time;
	}

	bool stopOnError() const noexcept {
		return *config_.stopMode_.stopOnError_;
	}

	void setStopOnError(bool const stopOnError) noexcept {
		config_.stopMode_.stopOnError_ = stopOnError;
	}

	uint32_t scansTreshold() const noexcept {
		return *config_.stopMode_.scansThreshold_;
	}

	void setScansTreshold(uint32_t const scansTreshold) noexcept {
		config_.stopMode_.scansThreshold_ = scansTreshold;
	}

	bool timestamps() const noexcept {
		return *config_.timestamps_;
	}

	void setTimeStamps(bool const mode) noexcept {
		config_.timestamps_ = mode;
	}

	void channelState() const noexcept { // TODO
		
	}

	Temperature temperature(TemperaturesEnum::Type const source) const noexcept {
		return *config_.temperatures_[source];
	}

	std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temperatures() const noexcept {
		return config_.temperatures_;
	}

	FansModeEnum::Type fansMode() const noexcept {
		return *config_.fansMode_;
	}

	void setFansMode(FansModeEnum::Type const mode) noexcept {
		config_.fansMode_ = mode;
	}

	ClockSourceEnum::Type clockSource() const noexcept {
		return *config_.clockSource_;
	}

	void setClockSource(ClockSourceEnum::Type const source) noexcept {
		config_.clockSource_ = source;
	}

	PtpTime ptpAlarm() const noexcept {
		return *config_.startMode_.ptpAlarm_;
	}

	void setPtpAlarm(PtpTime const alarmTime) noexcept {
		config_.startMode_.ptpAlarm_ = alarmTime;
		if (startMode() == AcquisitionStartModeEnum::PTP_ALARM) {
			acqStartTimer_->stop();
			auto mseconds = (ptpAlarm().seconds_ - ptpTime().seconds_) * 1000;
			(mseconds > 0) ? acqStartTimer_->singleShot(mseconds, this, &HardwareMock6991::startAcquisition) : startAcquisition();
		}
	}

	int scansNoPerDirectReadPacket() const noexcept {
		return *config_.scansPerDirectReadPacket_;
	}

	void setScansNoPerDirectReadPacket(int const scansNo) noexcept {
		if (scansNo < 1 || scansNo > 1000)
			setError("ERR: Provided wrong scans number - it should be in <1 - 1000> range.");
		else
			config_.scansPerDirectReadPacket_ = scansNo;
	}

	void releaseControl() noexcept {
		controllerId_ = 0;
	}

	bool isAcqActive() const noexcept {
		return fpgaRegs_.at(0x4680) & 0x80000000;
	}

	void setAcqActive(bool const state) noexcept {
		state ? fpgaRegs_[0x4680] |= 0x80000000 : fpgaRegs_[0x4680] &= 0x7FFFFFFF;
	}

	void startAcquisition() noexcept {
		if (!isAcqActive()) {
			setAcqActive(true);
			acquisitionStoppedOnError_ = false;
			state_ = DeviceStateEnum::ACQUISITION;
		}
		else
			setError("ERR: The acquisition is already active.");
	}

	void stopAcquisition() noexcept {
		if (isAcqActive()) {
			setAcqActive(false);
			state_ = DeviceStateEnum::IDLE;
		}
		else
			setError("ERR: The acquisition is already stopped.");
	}

	void takeControl(int const id) noexcept {
		if (controllerId_ == 0)
			controllerId_ = id;
		else
			setError(QString("ERR: Control is already taken by: %1").arg(id));
	}

	int controllerId() const noexcept {
		return controllerId_;
	}

	int voltage() const noexcept {
		return 100;
	}

	void setDbgMode(bool const state) noexcept {
		dbgModeOn_ = state;
	}

	bool dbgMode() const noexcept {
		return dbgModeOn_;
	}

	void writeFpgaReg(int const address, int const data) noexcept {
		fpgaRegs_[address] = data;
	}

	void writeCpuReg(int const address, int const data) noexcept {
		cpuRegs_[address] = data;
	}

	void writeFcReg(int const fcId, int const address, int const data) noexcept {
		fcRegs_[fcId - 1][address] = data;
	}

	int readFpgaReg(int const address) const noexcept {
		return fpgaRegs_.find(address) != fpgaRegs_.end() ? fpgaRegs_.at(address) : 0xFFFFFFFF;
	}

	int readCpuReg(int const address) const noexcept {
		return cpuRegs_.find(address) != cpuRegs_.end() ? cpuRegs_.at(address) : 0xFFFFFFFF;
	}

	int readFcReg(int const fcId, int const address) const noexcept {
		return fcRegs_[fcId - 1].find(address) != fcRegs_[fcId - 1].end() ? fcRegs_[fcId - 1].at(address) : 0xFFFFFFFF;
	}

	void startTests() noexcept {
		fpgaRegs_[RegistersEnum::CL_SPI_TLCNT_reg] = 0;
		fpgaRegs_[RegistersEnum::CL0_SPI_TLERR_reg] = 0;
		fpgaRegs_[RegistersEnum::CL1_SPI_TLERR_reg] = 0;
		fpgaRegs_[RegistersEnum::DL0_SPI_TMCNT_reg] = 0;
		fpgaRegs_[RegistersEnum::DL0_SPI_TMERR_reg] = 0;
		fpgaRegs_[RegistersEnum::DL1_SPI_TMCNT_reg] = 0;
		fpgaRegs_[RegistersEnum::DL1_SPI_TMERR_reg] = 0;
		timer_->start(1000);
		areTestsRunning = true; 
		std::bitset<32> reg = fpgaRegs_[RegistersEnum::CL_SPI_CSR_reg];
		const int CL_START_CL_BUSY = 31;
		reg[CL_START_CL_BUSY] = false;
		fpgaRegs_[RegistersEnum::CL_SPI_CSR_reg] = reg.to_ulong();
	}

	void stopTests() noexcept {
		timer_->stop();
		areTestsRunning = false;
	}
};
