#pragma once
#include "../ScpiDevice.h"
#include "../ChannelsIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "Defines6991.h"
#include "Registers6991.h"
#include <QTcpSocket>
#include <QTextStream>
#include <QDataStream>
#include <QFile>
#include <QDataStream>
#include <future>

#include <array>
#include <atomic>
#include <bitset>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Device6991
///
/// @brief	The virtual interface of the 6991 device.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Device6991 : public ScpiDevice, public DeviceIdentityResourcesIF, public ChannelsIF {
	Q_OBJECT
	
	QTcpSocket tcpSocket_;
	QDataStream in_;
	mutable QString response_;
	bool isAcqActive_ = false;
	CL_SPI_CSR_reg CL_SPI_CSR_reg_{ this };
	CL_SPI_TLCNT_reg CL_SPI_TLCNT_reg_{ this };
	CL0_SPI_TLERR_reg CL0_SPI_TLERR_reg_{ this };
	CL1_SPI_TLERR_reg CL1_SPI_TLERR_reg_{ this };
	DL_SPI_CSR1_reg DL_SPI_CSR1_reg_{ this };
	DL0_SPI_TMCNT_reg DL0_SPI_TMCNT_reg_{ this };
	DL0_SPI_TMERR_reg DL0_SPI_TMERR_reg_{ this };
	DL1_SPI_TMCNT_reg DL1_SPI_TMCNT_reg_{ this };
	DL1_SPI_TMERR_reg DL1_SPI_TMERR_reg_{ this };
	DFIFO_CSR_reg DFIFO_CSR_reg_{ this };
	ACQ_CSR_reg ACQ_CSR_reg_{ this };
	DEBUG_CSR_reg DEBUG_CSR_reg_{ this };
	DEBUG_CLK_RATE_reg DEBUG_CLK_RATE_reg_{ this };
	DFIFO_PFLAG_THR_reg DFIFO_PFLAG_THR_reg_{ this };
	DFIFO DFIFO_{ this };
	std::future<void> future;
	std::atomic<uint32_t> dataErrors_;
	std::atomic<uint32_t> lastSample_;
	bool exiting = false;
	mutable std::mutex hwAccess_;
	bool storeData_ = false;
	QFile* dataFile_ = new QFile("data6991.txt");

	std::optional<uint32_t> readFifo() noexcept {
		return DFIFO_.value();
	}

	void restartTest() noexcept {
		//TODO
	}

	FifoTestModel fillFifoStatus() noexcept {
		FifoTestModel fifoTestModel_;
		auto count = DFIFO_CSR_reg_.samplesInFifo();
		auto overflow = DFIFO_CSR_reg_.overflowHappened();
		auto progThresholdPassed = DFIFO_CSR_reg_.isFifoProgFull();
		fifoTestModel_.config_.blockSize_ = DFIFO_CSR_reg_.blockSize();
		fifoTestModel_.config_.rate_ = DEBUG_CLK_RATE_reg_.rate();
		fifoTestModel_.config_.treshold_ = DFIFO_PFLAG_THR_reg_.threshold();
		if (overflow && *overflow)
			fifoTestModel_.overflows_++;
		if (progThresholdPassed && *progThresholdPassed)
			fifoTestModel_.passTresholdCount_++;
		if (count)
			fifoTestModel_.samplesCount_ = count;
		fifoTestModel_.lastSample_ = lastSample_;
		fifoTestModel_.dataErrorsCount_ = dataErrors_;
		return fifoTestModel_;
	}

	void fifoTest() {
		auto isRunning = DFIFO_CSR_reg_.isTestRunning();
		uint32_t expectedData = 0;
		dataErrors_ = 0;
		lastSample_ = 0;
		while (isRunning && *isRunning && !exiting) {
			auto reg = DFIFO_CSR_reg_.samplesInFifo();
			auto samplesNo = reg ? *reg : 0;
			for (auto i = 0; i < samplesNo; ++i) {
				auto read = readFifo();
				if (!read)
					//connection error
					continue;
				lastSample_ = *read;
				if (lastSample_ != expectedData++)
					dataErrors_++;
			}
			isRunning = DFIFO_CSR_reg_.isTestRunning();
		}
	}

	bool configureFifoTest(FifoTestModel::Configuration const& config) noexcept {
		bool rateChangeStatus = true;
		bool blockSizeChangeStatus = true;
		bool thresholdChangeStatus = true;
		if(config.rate_)
			rateChangeStatus = DEBUG_CLK_RATE_reg_.setRate(*config.rate_);
		if (config.blockSize_)
			blockSizeChangeStatus = DFIFO_CSR_reg_.setBlockSize(*config.blockSize_);
		if (config.treshold_)
			thresholdChangeStatus = DFIFO_PFLAG_THR_reg_.setThreshold(*config.treshold_);

		return rateChangeStatus && blockSizeChangeStatus && thresholdChangeStatus;
	}

	void startFifoTest(FifoTestModel::Configuration const& config) noexcept {
		if (DL_SPI_CSR1_reg_.stopTests() && configureFifoTest(config) && DEBUG_CSR_reg_.startClock() && DFIFO_CSR_reg_.startTests())
			future = std::async(std::launch::async, [this]() { fifoTest(); });
	}

	std::optional<bool> isTestsRunning() noexcept {
		auto clTests = CL_SPI_CSR_reg_.isTestRunning();
		auto dlTests = DL_SPI_CSR1_reg_.isTestRunning();
		auto fifoTests = DFIFO_CSR_reg_.isTestRunning();
		return clTests && dlTests && fifoTests ? std::optional(*clTests || *dlTests || *fifoTests) : std::nullopt;
	}

	QString readError() const noexcept {
		scpiCmd("SYSTem:ERR?");
		return readResponse();
	}
	bool succeeded() const noexcept {
		response_ = readResponse();
		scpiCmd("*STB?");
		std::bitset<8> stb = readResponse().toInt();
		bool eva = stb[2];
		if (eva) {
			auto error = readError();
			emit reportError(error);
			return false;
		}
		return true;
	}

	bool invokeCmd(const QString& cmd) const noexcept {
		std::lock_guard lock(hwAccess_);
		scpiCmd(cmd);
		qDebug() << cmd;
		return succeeded();
	}

	DeviceState extractStatus(QString const& stateData) noexcept {
		auto list = stateData.split(',');
		DeviceState state;
		state.setState(DeviceStateEnum::fromString(list[0]));
		state.set(list[1]);
		if (auto acqRealState = isAcquisitionActive(); acqRealState) {
			if (*acqRealState && !isAcqActive_) 
				emit acquisitionStarted();
			else if (!*acqRealState && isAcqActive_) 
				emit acquisitionStopped();
			isAcqActive_ = *acqRealState;
		}
			
		return state;
	}

	std::optional<DeviceState> status() noexcept {
		return invokeCmd(QString("SYSTem:STATe?")) ? std::optional{extractStatus(response_)} : std::nullopt;
	}

	std::optional<ScanRateModel> scanRate() const noexcept {
		if (invokeCmd(QString("CONFigure:SCAN:RATe?"))) 
			return ScanRateModel::fromString(response_);
		return std::nullopt;
	}

	void setScanRate(ScanRateModel const scanRate) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:RATe %1,%2").arg(scanRate.value_).arg(ScanRateUnitsEnum::toString(scanRate.units_)));
	}

	std::optional<AcquisitionStartModeEnum::Type> startMode() const noexcept {
		return invokeCmd(QString("CONFigure:TRIGger:STARt?")) ? std::optional(AcquisitionStartModeEnum::fromString(response_)) : std::nullopt;
	}

	void setStartMode(AcquisitionStartModeEnum::Type const mode) const noexcept {
		invokeCmd(QString("CONFigure:TRIGger:STARt '%1'").arg(AcquisitionStartModeEnum::toString(mode)));
	}

	std::optional<PtpTime> ptpTime() const noexcept {
		if (invokeCmd(QString("SYSTem:PTPTime?")))
			return PtpTime::fromString(response_);
		return std::nullopt;
	}

	std::optional<bool> stopOnError() const noexcept {
		return invokeCmd(QString("CONFigure:STOPonerr?")) ? std::optional{ response_.toInt() == 1} : std::nullopt;
	}

	void setStopOnError(bool const stopOnError) const noexcept {
		invokeCmd(QString("CONFigure:STOPonerr %1").arg(stopOnError ? "ON" : "OFF"));
	}

	std::optional<uint32_t> scansTreshold() const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:POST?")) ? std::optional{ response_.toUInt()} : std::nullopt;
	}

	void setScansTreshold(int const scansTreshold) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:POST %1").arg(scansTreshold));
	}

	std::optional<bool> timestamps() const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:TIMestamp?")) ? std::optional{ response_.toInt() == 1} : std::nullopt;
	}

	void setTimeStamps(bool const mode) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:TIMestamp %1").arg(mode ? "ON" : "OFF"));
	}

	std::optional<DeviceState> channelState() const noexcept { // TODO
		return std::nullopt;
	}

	std::optional<Temperature> temperature(TemperaturesEnum::Type const source) const noexcept {
		return invokeCmd(QString("SYSTem:TEMPerature? '%1'").arg(TemperaturesEnum::toString(source))) ? std::optional{Temperature{response_.toInt(), source}} : std::nullopt;
	}

	std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temperatures() const noexcept {
		std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temps;
		for (auto source : TemperaturesEnum::TYPES)
			temps[source] = temperature(source);	
		return temps;
	}

	std::optional<FansModeEnum::Type> fansMode() const noexcept {
		return invokeCmd(QString("SYSTem:FAN?")) ? std::optional{FansModeEnum::fromString(response_)} : std::nullopt;
	}

	void setFansMode(FansModeEnum::Type const mode) const noexcept {
		invokeCmd(QString("SYSTem:FAN %1").arg(mode));
	}

	std::optional<ClockSourceEnum::Type> clockSource() const noexcept { // TODO
		return std::nullopt;
	}

	void setClockSource(ClockSourceEnum::Type const source) const noexcept { // TODO

	}

	std::optional<PtpTime> ptpAlarm() const noexcept {
		if (invokeCmd(QString("CONFigure:TRIGger:PTP:ALARm?"))) {
			auto list = response_.split(',');
			return std::optional(PtpTime{ list[0].toInt(), list[1].toInt() });
		}
		return std::nullopt;
	}

	void setPtpAlarm(PtpTime const alarmTime) const noexcept {
		invokeCmd(QString("CONFigure:TRIGger:PTP:ALARm %1,%2").arg(alarmTime.seconds_).arg(alarmTime.nanoseconds_));
	}

	std::optional<int> scansNoPerDirectReadPacket() const noexcept { 
		return invokeCmd(QString("CONFigure:DIRectread?")) ? std::optional{ response_.toInt() } : std::nullopt;
	}

	void setScansNoPerDirectReadPacket(int const scansNo) const noexcept {
		invokeCmd(QString("CONFigure:DIRectread %1").arg(scansNo));
	}

	void openSocketConnection(QString const& addresss, int const port) noexcept {
		tcpSocket_.connectToHost(addresss, port);
	}

	void closeSocketConnection() noexcept {
		tcpSocket_.disconnectFromHost();
	}

	void storeData(std::string const& data) noexcept {		
		if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(dataFile_);
		out << data.data();
	}
private slots:
	void displayError(QAbstractSocket::SocketError socketError) noexcept {
		emit reportError(QString(tcpSocket_.errorString()));
		closeSocketConnection();
	}
	void readData() noexcept {
		in_.startTransaction();
		auto bytes = tcpSocket_.bytesAvailable();
		std::string data;
		data.resize(bytes);
		in_.readRawData(data.data(), bytes);
		if (!in_.commitTransaction()) {
			emit reportError("Reading data from socket error.");
			return;
		}
			
		qDebug() << "I read: " << QString::fromStdString(data);
		if (storeData_)
			storeData(data);
	}
public:
	using DataType = QVector<bool>;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Device6991::Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF, int const channelsNo,  QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 		   	nameId	The name identifier.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF, int const channelsNo,  QObject* parent = nullptr) noexcept : ScpiDevice(nameId, connector, scpiIF, parent), DeviceIdentityResourcesIF(nameId), ChannelsIF(channelsNo) {
		connect();
		in_.setDevice(&tcpSocket_);
		in_.setVersion(QDataStream::Qt_5_14);
		QObject::connect(&tcpSocket_, &QAbstractSocket::connected, this, &Device6991::connectedDataStream);
		QObject::connect(&tcpSocket_, &QAbstractSocket::disconnected, this, &Device6991::disconnectedDataStream);
		QObject::connect(&tcpSocket_, &QIODevice::readyRead, this, &Device6991::readData);
		QObject::connect(&tcpSocket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Device6991::displayError);
	}
	~Device6991() override {
		//exiting = true;
	}

	void saveSubtype(const QString& str) const override {}
	void saveSerialnumber(const QString& str) const override {}
	void saveFirmwareRevision(const QString& str) const override {}
	void saveDriverRevision(const QString& str) const override {}
	QString loadSubtype() const override { return ""; }
	QString loadSerialnumber() const override { return ""; }
	QString loadFirmwareRevision() const override { return ""; }
	QString loadDriverRevision() const override { return ""; }

	std::optional<bool> isAcquisitionActive() noexcept {
		return ACQ_CSR_reg_.isAcqActive();
	}
	std::optional<int> controllerId() const noexcept {
		return invokeCmd("SYSTem:LOCK?") ? std::optional{ response_.toInt()} : std::nullopt;
	}
public slots:
	void connectDataStreamRequest(int const port) {
		auto ipResource = inputResources().back();
		openSocketConnection({ ipResource->load() }, port);
	}

	void disconnectDataStreamRequest() {
		closeSocketConnection();
	}

	void deviceStateRequest() noexcept {
		if (invokeCmd("SYSTem:STATe?")) {
			auto devState = extractStatus(response_);
			devState.setControllerId(controllerId());
			emit state(devState);
		}	
	}

	void releaseControlRequest() const noexcept {
		if (invokeCmd("SYSTem:LOCK 0"))
			emit controlReleased();
	}

	void startAcquisitionRequest() noexcept {
		if (auto mode = startMode(); mode && *mode == AcquisitionStartModeEnum::IMMEDIATE && invokeCmd("MEASure:ASYNc")) {
			isAcqActive_ = true;
			emit acquisitionStarted();
		}
	}

	void stopAcquisitionRequest() noexcept {
		if (invokeCmd("MEASure:ABORt")) {
			isAcqActive_ = false;
			emit acquisitionStopped();
		}
	}

	void takeControlRequest(int const id) const noexcept {
		if (invokeCmd(QString("SYSTem:LOCK %1").arg(id)))
			emit controlGranted();
	}

	void controllerKeyRequest() const noexcept {
		if(auto id = controllerId(); id)
			emit actualControllerKey(*id);
	}

	void configurationDataRequest() const noexcept {
		Configuration6991 config;
		config.scanRate_ = scanRate();
		config.startMode_.mode_ = startMode();
		if(config.startMode_.mode_ == AcquisitionStartModeEnum::PTP_ALARM)
			config.startMode_.ptpAlarm_ = ptpAlarm();
		config.stopMode_.scansThreshold_ = scansTreshold();
		config.stopMode_.stopOnError_ = stopOnError();

		config.scansPerDirectReadPacket_ = scansNoPerDirectReadPacket();
		config.temperatures_ = temperatures();
		config.timestamps_ = timestamps();
		config.clockSource_ = clockSource();
		config.fansMode_ = fansMode();
		config.ptpTime_ = ptpTime();
		emit configuration(config);
	}

	void configurateDeviceRequest(Configuration6991 const config) const noexcept {
		if (config.scanRate_)
			setScanRate(*config.scanRate_);
		if (config.fansMode_)
			setFansMode(*config.fansMode_);
		if (config.startMode_.mode_) {
			if (*config.startMode_.mode_ == AcquisitionStartModeEnum::PTP_ALARM && config.startMode_.ptpAlarm_)
				setPtpAlarm(*config.startMode_.ptpAlarm_);
			setStartMode(*config.startMode_.mode_);
		}
		if (config.stopMode_.stopOnError_)
			setStopOnError(*config.stopMode_.stopOnError_);	
		if (config.stopMode_.scansThreshold_)
			setScansTreshold(*config.stopMode_.scansThreshold_);
		if (config.scansPerDirectReadPacket_)
			setScansNoPerDirectReadPacket(*config.scansPerDirectReadPacket_);
		if (config.timestamps_)
			setTimeStamps(*config.timestamps_);
		if (config.clockSource_)
			setClockSource(*config.clockSource_);
	}

	void startTestsRequest(StartTestsRequest const& request) noexcept {
		if (auto id = controllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
			if (invokeCmd("*DBG 1")) {
				CL_SPI_CSR_reg_.startTests(request.model.at(TestTypeEnum::CL0), request.model.at(TestTypeEnum::CL1));
				DL_SPI_CSR1_reg_.startTests(request.model.at(TestTypeEnum::DL0), request.model.at(TestTypeEnum::DL1));
				if (request.model.at(TestTypeEnum::FIFO))
					startFifoTest(request.fifoTestConfig_);						
				emit testsStarted();
				invokeCmd("*DBG 0");
			}
			else {
				invokeCmd("SYSTem:LOCK 0");
			}
		}
	}

	void stopTestsRequest() noexcept {
		if (auto id = controllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
			if (invokeCmd("*DBG 1")) {
				CL_SPI_CSR_reg_.stopTests();	
				DL_SPI_CSR1_reg_.stopTests();
				DFIFO_CSR_reg_.stopTests();
				invokeCmd("*DBG 0");
			}
		}
		else
			invokeCmd("SYSTem:LOCK 0");
	}

	void testCountersRequest() noexcept {
		if (auto testsRunning = isTestsRunning(); testsRunning && (!*testsRunning)) {
			emit testsStopped();
			return;
		}
			
		Result result_CL0;
		Result result_CL1;
		Result result_DL0;
		Result result_DL1;
		Result result_FIFO;
	
		if (auto count = CL_SPI_TLCNT_reg_.value(); count) {
			if(auto regCL0 = CL_SPI_CSR_reg_.isTestRunning(TestTypeEnum::CL0); regCL0 && *regCL0)
				result_CL0.count_ = count;
			if(auto regCL1 = CL_SPI_CSR_reg_.isTestRunning(TestTypeEnum::CL1); regCL1 && *regCL1)
				result_CL1.count_ = count;
		}			
		if (auto errors = CL0_SPI_TLERR_reg_.value(); errors)
			result_CL0.errors_ = errors;
		if (auto errors = CL1_SPI_TLERR_reg_.value(); errors)
			result_CL1.errors_ = errors;
		if (auto count = DL0_SPI_TMCNT_reg_.value(); count)
			result_DL0.count_ = count;
		if (auto errors = DL0_SPI_TMERR_reg_.value(); errors)
			result_DL0.errors_ = errors;
		if (auto count = DL1_SPI_TMCNT_reg_.value(); count)
			result_DL1.count_ = count;
		if (auto errors = DL1_SPI_TMERR_reg_.value(); errors)
			result_DL1.errors_ = errors;
		//TODO add counters read for FIFO test

		TestsResultModel result;
		result[TestTypeEnum::CL0] = result_CL0;
		result[TestTypeEnum::CL1] = result_CL1;
		result[TestTypeEnum::DL0] = result_DL0;
		result[TestTypeEnum::DL1] = result_DL1;
		result[TestTypeEnum::FIFO] = result_FIFO;
		TestsStatus status;
		status.model = result;
		if (auto isRunning = DFIFO_CSR_reg_.isTestRunning(); isRunning && *isRunning) 
			status.fifoTestModel_ = fillFifoStatus();
		emit testCounters(status);
	}

	bool writeFpgaRegister(int const address, int const data) const noexcept {
		return invokeCmd(QString("*HW:FPGA %1,%2").arg(address).arg(data));
	}

	bool writeFcRegister(int const fcId, int const address, int const data) const noexcept {
		return invokeCmd(QString("*HW:FC %1,%2,%3").arg(fcId).arg(address).arg(data));
	}

	bool writeCpuRegister(int const address, int const data) const noexcept {
		return invokeCmd(QString("*HW:REG %1,%2").arg(address).arg(data));
	}

	std::optional<int> readFpgaRegister(int const address) const noexcept {
		return invokeCmd(QString("*HW:FPGA? %1").arg(address)) ? std::optional{ response_.toInt() } : std::nullopt;
	}

	std::optional<int> readFcRegister(int const fcId, int const address) const noexcept {
		return invokeCmd(QString("*HW:FC? %1,%2").arg(fcId).arg(address)) ? std::optional{ response_.toInt() } : std::nullopt;
	}

	std::optional<int> readCpuRegister(int const address) const noexcept {
		return invokeCmd(QString("*HW:REG? %1").arg(address)) ? std::optional{ response_.toInt() } : std::nullopt;
	}

	void fcCardStateRequest(int const fcId) const noexcept {
		//todo
		bool enabled = true;
		enabled ? emit fcCardEnabled(fcId) : emit fcCardDisabled(fcId);
	}

	void setStoreData(bool const state) noexcept {
		storeData_ = state;
	}
signals:
	void actualControllerKey(int const id) const;
	void acquisitionStarted() const;
	void acquisitionStopped() const;
	void connectedDataStream() const;
	void controlGranted() const;
	void controlReleased() const;
	void disconnectedDataStream() const;
	void state(DeviceState const state) const;
	void configuration(Configuration6991 const configuration) const;
	void testsStarted() const;
	void testsStopped() const;
	void testCounters(TestsStatus const& status) const;
	void fcCardEnabled(int const fcId) const;
	void fcCardDisabled(int const fcId) const;
};
