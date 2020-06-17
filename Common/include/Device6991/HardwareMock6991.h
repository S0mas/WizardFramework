#pragma once
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
#include "../SignalDataPacket.h"
#include <QRandomGenerator>
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
	uint32_t controllerId_ = 0;

	bool dbgModeOn_ = false;
	bool areTestsRunning = false;
	bool acquisitionStoppedOnError_ = false;

	mutable bool state = false;
	QTimer* timer_ = new QTimer(this);
	QTimer* dataSendingTimer = new QTimer(this);
	QTimer* acqStartTimer_ = new QTimer(this);
	TestsSelectionModel testSelectionModel_;

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

	void setError(QString const& errorMsg) noexcept {
		lastError_ = errorMsg;
		stb_.set(2);
		if (isAcqActive() && stopOnError()) {
			stopAcquisition();
			state_ = DeviceStateEnum::ERROR;
			acquisitionStoppedOnError_ = true;
		}
	}

	void startDataSending() const noexcept {
		//dataSendingTimer->start(100);
	}

	void stopDataSending() const noexcept {
		//dataSendingTimer->stop();
	}

	int packetNo_ = 0;

	void mock6111(QTcpSocket* socket) {
		uint32_t channelsToMeasure = 256;
		uint32_t sampleSize = 1;
		uint32_t scans = 10;

		HeaderPart6991 header_;
		header_.id_ = 0x6111;
		header_.options_ = 1;
		header_.runningNumber_ = packetNo_++;
		header_.numberOfScans_ = scans;
		header_.numberOfSamplesPerScan_ = channelsToMeasure;
		header_.dataSize_ = header_.numberOfSamplesPerScan_ * scans * sampleSize / 8;
		header_.status1_ = { 0 };
		header_.status2_ = { 0 };

		SignalPacket<Scan6111> packet(header_);
		static int seconds2 = 0;
		for (auto& scan : packet.data_.scans_) {
			if (header_.options_) {
				scan.ts_.seconds_ = seconds2++;
				scan.ts_.nanoseconds_ = 123456789;
			}
			for (auto& sample : scan.samples_)
				sample.raw_ = QRandomGenerator::global()->bounded(0xFF);
		}

		if (socket->isOpen()) {
			QDataStream stream(socket);
			stream.setByteOrder(QDataStream::LittleEndian);
			stream << header_;
			stream << packet.data_;
		}
	}

	void mock6132(QTcpSocket* socket) {
		uint32_t channelsToMeasure = 32;
		uint32_t sampleSize = 4;
		uint32_t scans = 10;

		HeaderPart6991 header_;
		header_.id_ = 0x6132;
		header_.options_ = 1;
		header_.runningNumber_ = packetNo_++;
		header_.numberOfScans_ = scans;
		header_.numberOfSamplesPerScan_ = channelsToMeasure;
		header_.dataSize_ = header_.numberOfSamplesPerScan_ * scans * sampleSize;
		header_.status1_ = { 0 };
		header_.status2_ = { 0 };

		SignalPacket<Scan6132> packet(header_);
		static int seconds1 = 0;
		for (auto& scan : packet.data_.scans_) {
			if (header_.options_) {
				scan.ts_.seconds_ = seconds1++;
				scan.ts_.nanoseconds_ = 123456789;
			}
			for (auto& sample : scan.samples_)
				sample = QRandomGenerator::global()->bounded(-5, 5);
		}

		if (socket->isOpen()) {
			QDataStream stream(socket);
			stream.setByteOrder(QDataStream::LittleEndian);
			stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
			stream << header_;
			stream << packet.data_;
		}
	}

	void sendMockData(QTcpSocket* socket) {
		//REMEMBER TO CHANGE fcRegs_[FecIdType::_1 - 1][FecRegistersEnum::FE_ID_reg] = 0xbbbb6111  or = 0xbbbb6132;
		//					 fcRegs_[FecIdType::_2 - 1][FecRegistersEnum::FE_ID_reg] = SAME!!!!!!!! (line 242 and 243)
		//mock6132(socket); 
		mock6111(socket);
	}
public:
	HardwareMock6991(QObject* parent = nullptr) : QObject(parent) {
		fpgaRegs_[ERROR_DEBUG_PRIVATE_REGISTER] = 0;
		auto checkTestsRegsTimer = new QTimer(this);
		connect(timer_, &QTimer::timeout, this, &HardwareMock6991::updateCounters);
		connect(dataSendingTimer, &QTimer::timeout, [this]() {for (auto socket : sockets_) if (socket) { sendMockData(socket); }});
		connect(checkTestsRegsTimer, &QTimer::timeout, this, &HardwareMock6991::checkTestsRegs);
		fpgaRegs_[RegistersEnum::CL_SPI_CSR_reg] = 0;
		fpgaRegs_[RegistersEnum::DL_SPI_CSR1_reg] = 0;
		fpgaRegs_[RegistersEnum::DFIFO_CSR_reg] = 8191;
		fpgaRegs_[RegistersEnum::ACQ_CSR_reg] = 0;
		fpgaRegs_[RegistersEnum::BOARD_CSR1_reg] = 0x0000001F;
		fcRegs_[FecIdType::_1 - 1][FecRegistersEnum::FE_ID_reg] = 0xbbbb6111;
		fcRegs_[FecIdType::_2 - 1][FecRegistersEnum::FE_ID_reg] = 0xbbbb6111;
		checkTestsRegsTimer->start(500);
		for (int i = 0; i < servers_.size(); ++i) {
			servers_[i] = new QTcpServer(this);
			servers_[i]->setMaxPendingConnections(1);
			servers_[i]->listen(QHostAddress::LocalHost, 16100 + i + 1);
			qDebug() << "start listening on :  " << QHostAddress::LocalHost << "    port:" << 16100 + i + 1;
			sockets_[i] = nullptr;
			connect(servers_[i], &QTcpServer::newConnection,
				[this, i]() {
					sockets_[i] = servers_[i]->nextPendingConnection();
					connect(sockets_[i], &QAbstractSocket::disconnected,
						[this, i]() {
							servers_[i]->listen(QHostAddress::LocalHost, 16100+i + 1);
							qDebug() << "start listening on :  " << QHostAddress::LocalHost << "    port:" << 16100 + i + 1;
							sockets_[i]->deleteLater();
							sockets_[i] = nullptr;
						}
					);
					servers_[i]->close(); 
				}
			);
		}
		dataSendingTimer->start(100);
	}

	QString readError() noexcept {
		stb_.set(2, false);
		auto tmp = lastError_;
		lastError_ = "";
		state_ = isAcqActive() ? DeviceStateEnum::ACQUISITION : DeviceStateEnum::IDLE;	
		return tmp;
	}

	uint32_t stb() const noexcept {
		return stb_.to_ulong();
	}

	bool isFifoOverflow() const noexcept {
		return false; // TODO
	}

	bool isFifoUnderflow() const noexcept {
		return false; // TODO
	}

	uint32_t scansInFifo() const noexcept {
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

	uint32_t scansNoPerDirectReadPacket() const noexcept {
		return *config_.scansPerDirectReadPacket_;
	}

	void setScansNoPerDirectReadPacket(uint32_t const scansNo) noexcept {
		if (scansNo < 1 || scansNo > 1000)
			setError("ERR: Provided wrong scans number - it should be in <1 - 1000> range.");
		else
			config_.scansPerDirectReadPacket_ = scansNo;
	}

	void releaseControl() noexcept {
		controllerId_ = 0;
	}

	bool isAcqActive() const noexcept {
		return fpgaRegs_.at(RegistersEnum::ACQ_CSR_reg) & 0x80000000;
	}

	void setAcqActive(bool const state) noexcept {
		state ? fpgaRegs_[RegistersEnum::ACQ_CSR_reg] |= 0x80000000 : fpgaRegs_[RegistersEnum::ACQ_CSR_reg] &= 0x7FFFFFFF;
	}

	void startAcquisition() noexcept {
		if (!isAcqActive()) {
			setAcqActive(true);
			acquisitionStoppedOnError_ = false;
			state_ = DeviceStateEnum::ACQUISITION;
			startDataSending();
		}
		else
			setError("ERR: The acquisition is already active.");
	}

	void stopAcquisition() noexcept {
		if (isAcqActive()) {
			setAcqActive(false);
			state_ = DeviceStateEnum::IDLE;
			stopDataSending();
		}
		else
			setError("ERR: The acquisition is already stopped.");
	}

	void takeControl(uint32_t const id) noexcept {
		if (controllerId_ == 0)
			controllerId_ = id;
		else
			setError(QString("ERR: Control is already taken by: %1").arg(id));
	}

	uint32_t controllerId() const noexcept {
		return controllerId_;
	}

	uint32_t voltage() const noexcept {
		return 100;
	}

	void setDbgMode(bool const state) noexcept {
		dbgModeOn_ = state;
	}

	bool dbgMode() const noexcept {
		return dbgModeOn_;
	}

	void writeFpgaReg(uint32_t const address, uint32_t const data) noexcept {
		fpgaRegs_[address] = data;
	}

	void writeCpuReg(uint32_t const address, uint32_t const data) noexcept {
		cpuRegs_[address] = data;
	}

	void writeFcReg(uint32_t const fcId, uint32_t const address, uint32_t const data) noexcept {
		fcRegs_[fcId - 1][address] = data;
	}

	uint32_t readFpgaReg(uint32_t const address) const noexcept {
		static uint32_t c = 0;
		if (address == RegistersEnum::DFIFO)
			return c++;
		return fpgaRegs_.find(address) != fpgaRegs_.end() ? fpgaRegs_.at(address) : 0xFFFFFFFF;
	}

	uint32_t readCpuReg(uint32_t const address) const noexcept {
		return cpuRegs_.find(address) != cpuRegs_.end() ? cpuRegs_.at(address) : 0xFFFFFFFF;
	}

	uint32_t readFcReg(uint32_t const fcId, uint32_t const address) const noexcept {
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
		fpgaRegs_[RegistersEnum::DL_SPI_CSR1_reg] = 0;
		fpgaRegs_[RegistersEnum::DFIFO] = 0;
		fcRegs_[FecIdType::_1 - 1][FecRegistersEnum::BOARD_CSR_reg] = 0x1;
		fcRegs_[FecIdType::_2 - 1][FecRegistersEnum::BOARD_CSR_reg] = 0x1;
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
