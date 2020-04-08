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
#include <vector>
#include "Command.h"
#include "Defines6991.h"
#include <map>

//dbg
#include <thread>
#include <chrono>

class HardwareMock6991 : public QObject {
	Q_OBJECT
	Configuration6991 config_;
	PtpTime ptpTime_;
	mutable std::bitset<8> stb_ = 0;
	mutable QString lastError_;
	std::array<QTcpServer*, 4> servers_;
	std::array<QTcpSocket*, 4> sockets_;
	std::map<int, int> fpgaRegs_;
	std::map<int, int> cpuRegs_;
	std::array<std::map<int, int>, 2> fcRegs_;
	int controllerId_ = 0;

	bool isAcqActive_ = false;
	bool dbgModeOn_;
	mutable bool state = false;
	virtual bool isDeviceAvailable() const noexcept {
		state = !state;
		return state;
	}
	void updateCounters() noexcept {
		std::vector<TestType> activetests;
		for (auto test : testTypes)
			if (testSelectionModel_.at(test))
				activetests.push_back(test);
		auto testErrorId = QRandomGenerator::global()->bounded(0, activetests.size());

		resultModel_.at(activetests[testErrorId]).errors_++;
		for (auto test : activetests)
			resultModel_.at(test).count_++;
	}
	QTimer* timer_;
	TestsSelectionModel testSelectionModel_;
	TestsResultModel resultModel_;

	void setError(QString const& errorMsg) const noexcept {
		lastError_ = errorMsg;
		stb_.set(2);
	}
public:
	HardwareMock6991(QObject* parent = nullptr) : QObject(parent) {
		int port = 1;
		for (int i = 0; i < servers_.size(); ++i) {
			servers_[i] = new QTcpServer(this);
			servers_[i]->setMaxPendingConnections(1);
			servers_[i]->listen(QHostAddress::LocalHost, i+1);
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

	QString readError() const noexcept {
		stb_.set(2, false);
		auto tmp = lastError_;
		lastError_ = "";
		return tmp;
	}

	int stb() const noexcept {
		return stb_.to_ulong();
	}

	DeviceState status() const noexcept {
		DeviceState status;
		return status;
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
	}

	PtpTime ptpTime() const noexcept {
		return ptpTime_;
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

	void startAcquisition() noexcept {
		if(!isAcqActive_)
			isAcqActive_ = true;
		else
			setError("ERR: The acquisition is already active.");
	}

	void stopAcquisition() noexcept {
		if (isAcqActive_)
			isAcqActive_ = false;
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
};
