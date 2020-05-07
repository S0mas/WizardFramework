#pragma once
#include "../ScpiDevice.h"
#include "../ChannelsIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "Defines6991.h"
#include "FifoTest.h"
#include "ClTest.h"
#include "DlTest.h"
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

class Device6991;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Device6991
///
/// @brief	The virtual interface of the 6991 device.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Device6991 : public ScpiDevice, public DeviceIdentityResourcesIF {
	Q_OBJECT
	friend class FifoTest;
	friend class DlTests;
	friend class ClTests;
	FC_WR_QUEUE_EMP FC_WR_QUEUE_EMP_{ this };
	PCI_ERR_STCTRL PCI_ERR_STCTRL_{ this };
	PREF_ERR_ADDR PREF_ERR_ADDR_{ this };
	BOARD_ID_reg BOARD_ID_reg_{ this };
	HW_REV_reg HW_REV_reg_{ this };
	BOARD_CSR1_reg BOARD_CSR1_reg_{ this };
	LED_CSR_reg LED_CSR_reg_{ this };
	CL0_SPI_DATA_reg CL0_SPI_DATA_reg_{ this };
	CL1_SPI_DATA_reg CL1_SPI_DATA_reg_{ this };
	CL_SPI_CSR_reg CL_SPI_CSR_reg_{ this };
	CL_SPI_TLCNT_reg CL_SPI_TLCNT_reg_{ this };
	CL0_SPI_TLERR_reg CL0_SPI_TLERR_reg_{ this };
	CL1_SPI_TLERR_reg CL1_SPI_TLERR_reg_{ this };
	DL_SPI_CSR1_reg DL_SPI_CSR1_reg_{ this };
	DL_SPI_CSR2_reg DL_SPI_CSR2_reg_{ this };
	DL_SPI_CSR3_reg DL_SPI_CSR3_reg_{ this };
	DL0_DWORD_EN_reg DL0_DWORD_EN_reg_{ this };
	DL1_DWORD_EN_reg DL1_DWORD_EN_reg_{ this };
	DL0_SPI_TMCNT_reg DL0_SPI_TMCNT_reg_{ this };
	DL0_SPI_TMERR_reg DL0_SPI_TMERR_reg_{ this };
	DL1_SPI_TMCNT_reg DL1_SPI_TMCNT_reg_{ this };
	DL1_SPI_TMERR_reg DL1_SPI_TMERR_reg_{ this };
	SYS_TMR_CSR_reg SYS_TMR_CSR_reg_{ this };
	SYS_TMR_L_reg SYS_TMR_L_reg_{ this };
	SYS_TMR_H_reg SYS_TMR_H_reg_{ this };
	EVENT_LOG_CSR_reg EVENT_LOG_CSR_reg_{ this };
	EVENT_STAMP_L_reg EVENT_STAMP_L_reg_{ this };
	EVENT_STAMP_H_reg EVENT_STAMP_H_reg_{ this };
	EVENT_CAUSE_reg EVENT_CAUSE_reg_{ this };
	DEBUG_CSR_reg DEBUG_CSR_reg_{ this };
	DEBUG_CLK_RATE_reg DEBUG_CLK_RATE_reg_{ this };
	ACQ_CSR_reg ACQ_CSR_reg_{ this };
	ACQ_RATE_reg ACQ_RATE_reg_{ this };
	ACQ_ALARM_L_reg ACQ_ALARM_L_reg_{ this };
	ACQ_ALARM_H_reg ACQ_ALARM_H_reg_{ this };
	DFIFO_CSR_reg DFIFO_CSR_reg_{ this };
	DFIFO_PFLAG_THR_reg DFIFO_PFLAG_THR_reg_{ this };
	DFIFO_WR_reg DFIFO_WR_reg_{ this };
	DFIFO DFIFO_{ this };

	//FEC REGS
	FE_ID_reg FE_ID_reg_{ this };
	BOARD_CSR_reg BOARD_CSR_reg_{ this };
	CMD_reg CMD_reg_{ this };
	DL_CSR_reg DL_CSR_reg_{ this };

	FifoTest fifoTest_{ this };
	DlTests dlTest_{ this };
	ClTests clTest_{ this };
	
	QTcpSocket* tcpSocket_ = nullptr;
	QDataStream in_;
	mutable QString response_;
	bool isAcqActive_ = false;

	bool exiting = false;
	mutable std::mutex hwAccess_;
	bool storeData_ = false;
	QFile* dataFile_ = new QFile("data6991.txt");
	bool enablePrint_ = true;

	bool isAnyTestRunning() noexcept {
		return clTest_.isRunning() || dlTest_.isRunning() || fifoTest_.isRunning();
	}

	QString readError() const noexcept {
		scpiCmd("SYSTem:ERR?");
		return readResponse();
	}

	bool succeeded() const noexcept {
		response_ = readResponse();
		if (enablePrint_)
			qDebug() << "Response:" << response_;
		scpiCmd("*STB?");
		std::bitset<8> stb = readResponse().toUInt(nullptr, 16);//TODO CHECK IF IT SHOULDNT BE INTERPRETED AS HEX
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
		if(enablePrint_)
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
		return invokeCmd(QString("CONFigure:STOPonerr?")) ? std::optional{ response_.toUInt() == 1} : std::nullopt;
	}

	void setStopOnError(bool const stopOnError) const noexcept {
		invokeCmd(QString("CONFigure:STOPonerr %1").arg(stopOnError ? "ON" : "OFF"));
	}

	std::optional<uint32_t> scansTreshold() const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:POST?")) ? std::optional{ response_.toUInt()} : std::nullopt;
	}

	void setScansTreshold(uint32_t const scansTreshold) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:POST %1").arg(scansTreshold));
	}

	std::optional<bool> timestamps() const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:TIMestamp?")) ? std::optional{ response_.toUInt() == 1} : std::nullopt;
	}

	void setTimeStamps(bool const mode) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:TIMestamp %1").arg(mode ? "ON" : "OFF"));
	}

	std::optional<DeviceState> channelState() const noexcept { // TODO
		return std::nullopt;
	}

	std::optional<Temperature> temperature(TemperaturesEnum::Type const source) const noexcept {
		return invokeCmd(QString("SYSTem:TEMPerature? '%1'").arg(TemperaturesEnum::toString(source))) ? std::optional{Temperature{response_.toUInt(), source}} : std::nullopt;//TODO check temp response format
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
			return std::optional(PtpTime{ list[0].toUInt(), list[1].toUInt() });
		}
		return std::nullopt;
	}

	void setPtpAlarm(PtpTime const alarmTime) const noexcept {
		invokeCmd(QString("CONFigure:TRIGger:PTP:ALARm %1,%2").arg(alarmTime.seconds_).arg(alarmTime.nanoseconds_));
	}

	std::optional<uint32_t> scansNoPerDirectReadPacket() const noexcept {
		return invokeCmd(QString("CONFigure:DIRectread?")) ? std::optional{ response_.toUInt() } : std::nullopt;
	}

	void setScansNoPerDirectReadPacket(uint32_t const scansNo) const noexcept {
		invokeCmd(QString("CONFigure:DIRectread %1").arg(scansNo));
	}

	void openSocketConnection(QString const& addresss, uint32_t const port) noexcept {
		tcpSocket_ = new QTcpSocket(this);
		QObject::connect(tcpSocket_, &QAbstractSocket::connected, this, &Device6991::connectedDataStream);
		QObject::connect(tcpSocket_, &QAbstractSocket::disconnected, this, &Device6991::disconnectedDataStream);
		QObject::connect(tcpSocket_, &QIODevice::readyRead, this, &Device6991::readData);
		QObject::connect(tcpSocket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Device6991::displayError);
		in_.setDevice(tcpSocket_);
		in_.setVersion(QDataStream::Qt_5_12);
		tcpSocket_->connectToHost(addresss, port);
	}

	void closeSocketConnection() noexcept {
		tcpSocket_->disconnectFromHost();
		delete tcpSocket_;
	}

	void storeData(std::string const& data) noexcept {		
		if (!dataFile_->isOpen() && !dataFile_->open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(dataFile_);
		out << data.data();
	}


private slots:
	void displayError(QAbstractSocket::SocketError socketError) noexcept {
		emit reportError(QString(tcpSocket_->errorString()));
		closeSocketConnection();
	}

	void readData() noexcept {
		in_.startTransaction();
		auto bytes = tcpSocket_->bytesAvailable();

		HeaderPart header;
		in_.readRawData(reinterpret_cast<char*>(reinterpret_cast<unsigned char*>(&header)), sizeof(header));
		
		if (!in_.commitTransaction()) {
			emit reportError("Reading data from socket error.");
			return;
		}

		AcquisitionPacket acqPacket(header.dataSize_/4);
		acqPacket.header_ = header;
		in_.startTransaction();
		in_.readRawData(reinterpret_cast<char*>(reinterpret_cast<unsigned char*>(acqPacket.data_.samples_.data())), header.dataSize_);

		if (fifoTest_.isRunning())
			fifoTest_.validateData(acqPacket.data_.samples_);

		if (!in_.commitTransaction()) {
			emit reportError("Reading data from socket error.");
			return;
		}

		//if (storeData_)
			//storeData()
	}
public:
	using DataType = QVector<bool>;
	Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF,  QObject* parent = nullptr) noexcept : ScpiDevice(nameId, connector, scpiIF, parent), DeviceIdentityResourcesIF(nameId) {
		QObject::connect(this, &Device6991::logMsg, [](QString const& msg) {qDebug() << "LOG: " << msg; });
		QObject::connect(this, &Device6991::reportError, [](QString const& msg) {qDebug() << "ERR: " << msg; });
	}
	~Device6991() override {
		exiting = true;
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
	std::optional<uint32_t> controllerId() const noexcept {
		return invokeCmd("SYSTem:LOCK?") ? std::optional{ response_.toUInt()} : std::nullopt;
	}
public slots:
	void connectDataStreamRequest(uint32_t const port) {
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

	void takeControlRequest(uint32_t const id) const noexcept {
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
			setScansNoPerDirectReadPacket(10);
			dlTest_.startTest(request.model.at(TestTypeEnum::DL0), request.model.at(TestTypeEnum::DL1), request.clockFreq_);
			if (request.model.at(TestTypeEnum::FIFO))
				fifoTest_.startTest(request.fifoTestConfig_);
			clTest_.startTest(request.model.at(TestTypeEnum::CL0), request.model.at(TestTypeEnum::CL1));
			emit testsStarted();
			invokeCmd("SYSTem:LOCK 0");
		}
	}

	void stopTestsRequest() noexcept {
		if (auto id = controllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
			clTest_.stopTest();
			dlTest_.stopTest();
			fifoTest_.stopTest();
			invokeCmd("SYSTem:LOCK 0");
		}		
	}

	void testCountersRequest() noexcept {
		if (!isAnyTestRunning()) {
			emit testsStopped();
			return;
		}
			
		Result result_CL0;
		Result result_CL1;
		Result result_DL0;
		Result result_DL1;
		Result result_FIFO;
		result_FIFO.errors_ = fifoTest_.errors();
		result_FIFO.count_ = fifoTest_.count();

		bool cl0Running = clTest_.isRunning(TestTypeEnum::CL0);
		bool cl1Running = clTest_.isRunning(TestTypeEnum::CL1);
		bool dl0Running = dlTest_.isRunning(TestTypeEnum::DL0);
		bool dl1Running = dlTest_.isRunning(TestTypeEnum::DL1);
	
		if (auto count = clTest_.count(); count) {
			if(cl0Running)
				result_CL0.count_ = count;
			if(cl1Running)
				result_CL1.count_ = count;
		}

		if (auto errors = clTest_.errors(TestTypeEnum::CL0); errors && cl0Running)
			result_CL0.errors_ = errors;
		if (auto errors = clTest_.errors(TestTypeEnum::CL1); errors && cl1Running)
			result_CL1.errors_ = errors;
		if (auto count = dlTest_.count(TestTypeEnum::DL0); count && dl0Running)
			result_DL0.count_ = count;
		if (auto errors = dlTest_.errors(TestTypeEnum::DL0); errors && dl0Running)
			result_DL0.errors_ = errors;
		if (auto count = dlTest_.count(TestTypeEnum::DL1); count && dl1Running)
			result_DL1.count_ = count;
		if (auto errors = dlTest_.errors(TestTypeEnum::DL1); errors && dl1Running)
			result_DL1.errors_ = errors;

		TestsResultModel result;
		result[TestTypeEnum::CL0] = result_CL0;
		result[TestTypeEnum::CL1] = result_CL1;
		result[TestTypeEnum::DL0] = result_DL0;
		result[TestTypeEnum::DL1] = result_DL1;
		result[TestTypeEnum::FIFO] = result_FIFO;
		TestsStatus status;
		status.model = result;
		if (fifoTest_.isRunning())
			status.fifoTestModel_ = fifoTest_.model();
		emit testCounters(status);
	}

	bool writeFpgaRegister(uint32_t const address, uint32_t const data) const noexcept {
		return invokeCmd(QString("*HW:FPGA #h%1, #h%2").arg(toHex(address, 8)).arg(toHex(data, 8)));
	}

	bool writeFecRegister(uint32_t const fcId, uint32_t const address, uint32_t const data) const noexcept {
		return invokeCmd(QString("*HW:FEC %1, #h%2, #h%3").arg(fcId).arg(toHex(address, 8)).arg(toHex(data, 8)));
	}

	bool writeCpuRegister(uint32_t const address, uint32_t const data) const noexcept {
		return invokeCmd(QString("*HW:REG #h%1, #h%2").arg(toHex(address, 8)).arg(toHex(data, 8)));
	}

	std::optional<uint32_t> readFpgaRegister(uint32_t const address) const noexcept {
		return invokeCmd(QString("*HW:FPGA? #h%1").arg(toHex(address, 8))) ? std::optional{ response_.toUInt(nullptr, 16) } : std::nullopt;
	}

	std::optional<uint32_t> readFecRegister(uint32_t const fcId, uint32_t const address) const noexcept {
		return invokeCmd(QString("*HW:FEC? %1, #h%2").arg(fcId).arg(toHex(address, 8))) ? std::optional{ response_.toUInt(nullptr, 16) } : std::nullopt;
	}

	std::optional<std::pair<uint32_t, uint32_t>> readBothFecRegisters(uint32_t const address) const noexcept {
		auto success = invokeCmd(QString("*HW:FEC? 0, #h%2").arg(toHex(address, 8)));
		if (success) {
			auto list = response_.split(';');
			return std::optional{ std::pair{list[0].toUInt(nullptr, 16), list[1].toUInt(nullptr, 16)} };
		}
		return  std::nullopt;
	}

	std::optional<uint32_t> readCpuRegister(uint32_t const address) const noexcept {
		return invokeCmd(QString("*HW:REG? #h%1").arg(toHex(address, 8))) ? std::optional{ response_.toUInt(nullptr, 16) } : std::nullopt;
	}

	void fcCardStateRequest(FecIdType::Type const fecId) noexcept {
		if (auto present = BOARD_CSR1_reg_.isFecPresent(fecId); present && *present)
			emit fcCardEnabled(fecId);
		else
			emit fcCardDisabled(fecId);
	}

	void setStoreData(bool const state) noexcept {
		storeData_ = state;
	}

	bool testReg(std::function<std::optional<uint32_t>()> const readFunction, uint32_t const address, uint32_t const expected, uint32_t const mask = 0xFFFFFFFF, bool const verbose = true) noexcept {
		auto value = readFunction();
		bool result = value ? ((*value)&mask) == expected : false;
		if (result)
			return true;
		else if (value && verbose)
			emit reportError(QString("Value different than expected. Address: %1 Expected: %2, Read:%3, Mask:%4")
			.arg(QString::number(address, 16)).arg(QString::number(expected, 16)).arg(QString::number(*value, 16)).arg(QString::number(mask, 16)));
		else if(verbose)
			emit reportError(QString("Couldnt read register. Address: %1").arg(QString::number(address, 16)));
		return false;
	}

	bool testFpgaReg(uint32_t const address, uint32_t const expected, uint32_t const mask = 0xFFFFFFFF, bool const verbose = true) noexcept {
		return testReg([this, address]() { return readFpgaRegister(address); }, address, expected, mask, verbose);
	}

	bool testFecReg(uint32_t const fcId, int32_t const address, uint32_t const expected, uint32_t const mask = 0xFFFFFFFF, bool const verbose = true) noexcept {
		return testReg([this, fcId, address]() {
			auto result = readFecRegister(fcId, address);
			if(!result)
				emit reportError(QString("FROM FC ID: %1").arg(fcId));
			return result;
		}
		, address, expected, mask, verbose);
	}

	bool testCpuReg(uint32_t const address, uint32_t const expected, uint32_t const mask = 0xFFFFFFFF, bool const verbose = true) noexcept {
		return testReg([this, address]() { return readCpuRegister(address); }, address, expected, mask, verbose);
	}

	bool testWithTimeout(std::function<bool()> const& functionCondition, uint32_t const timeoutInMilliseconds = 0x7FFFFFFF, uint32_t const breakTimeInMilliseconds = 0) {
		QTime startTime;
		startTime.start();
		while (startTime.elapsed() < timeoutInMilliseconds) {
			if (functionCondition()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(breakTimeInMilliseconds));
				return true;
			}
		}
		reportError("Timeout.");
		return false;
	}

	void enableScpiCommandsPrints(bool const enable) noexcept {
		enablePrint_ = enable;
	}

	bool isFecIdle(FecType::Type const type, FecIdType::Type const fcId) noexcept {
		uint32_t expectedVal = type == FecType::_6111 ? FecStatusType6111::IDLE : FecStatusType6132::IDLE;
		return BOARD_CSR_reg_.testStatus(fcId, type, expectedVal);
	}

	bool checkIfDlReceiverStateIsIdle() noexcept {
		if (auto fifoFsmStatus = DL_SPI_CSR1_reg_.dlFifoFsmStatus(); fifoFsmStatus) {
			if (auto fsmStatus = DL_SPI_CSR1_reg_.dlFsmStatus(); fsmStatus) {
				if (*fifoFsmStatus != 0) {
					emit reportError("FifoFsmStatus is not 0.");
					return false;
				}
				if (*fsmStatus != 0) {
					emit reportError("FsmStatus is not 0.");
					return false;
				}
				return true;
			}
		}
		return false;
	}

	std::optional<FecType::Type> readFecType(FecIdType::Type const fcId) noexcept {
		auto read = FE_ID_reg_.fecType(fcId);
		if (read.first && fcId == FecIdType::BOTH || fcId == FecIdType::_1) 
			return static_cast<FecType::Type>(*read.first);	
		if (read.second && fcId == FecIdType::BOTH || fcId == FecIdType::_2)
			return static_cast<FecType::Type>(*read.second);
		return std::nullopt;
	}
signals:
	void actualControllerKey(uint32_t const id) const;
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
	void fcCardEnabled(uint32_t const fcId) const;
	void fcCardDisabled(uint32_t const fcId) const;
};
