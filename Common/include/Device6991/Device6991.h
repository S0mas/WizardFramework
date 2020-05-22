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
#include "../DataStream.h"
#include <QFile>
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
class Device6991;

struct FecRegs {
	FecRegs(FecIdType::Type const id, Device6991* devIF) : id_(id), devIF_(devIF) {}
	FecIdType::Type id_;
	Device6991* devIF_;
	FE_ID_reg FE_ID_reg_{ id_, devIF_ };
	BOARD_CSR_reg BOARD_CSR_reg_{ id_, devIF_ };
	CMD_reg CMD_reg_{ id_, devIF_ };
	DL_CSR_reg DL_CSR_reg_{ id_, devIF_ };
};

class Device6991 : public ScpiDevice, public DeviceIdentityResourcesIF {
	Q_OBJECT
	friend class FifoTest;
	friend class DlTests;
	friend class ClTests;
	friend class Device6132;
	mutable std::mutex hwAccess_;
	mutable QString response_;

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

	std::array<FecRegs, 3> fecRegs_{ FecRegs{FecIdType::BOTH, this}, FecRegs{FecIdType::_1, this}, FecRegs{FecIdType::_2, this} };

	FifoTest fifoTest_{ this };
	DlTests dlTest_{ this };
	ClTests clTest_{ this };

	DataStream* dataStream_ = new DataStream(this);

	bool enablePrint_ = true;
	bool isAcqActive_ = false;

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
		std::bitset<8> stb = readResponse().toUInt(nullptr, 16);
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
		//TODO UNCOMMENT WHEN RDY
		//state.set(list[1]);
		//if (auto acqRealState = isAcquisitionActive(); acqRealState) {
		//	if (*acqRealState && !isAcqActive_) 
		//		emit acquisitionStarted();
		//	else if (!*acqRealState && isAcqActive_) 
		//		emit acquisitionStopped();
		//	isAcqActive_ = *acqRealState;
		//}
			
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
public:
	using DataType = QVector<bool>;
	Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF,  QObject* parent = nullptr) noexcept : ScpiDevice(nameId, connector, scpiIF, parent), DeviceIdentityResourcesIF(nameId) {
		QObject::connect(dataStream_, &DataStream::reportError, this, &Device6991::reportError);
		QObject::connect(dataStream_, &DataStream::connected, this, &Device6991::connectedDataStream);
		QObject::connect(dataStream_, &DataStream::disconnected, this, &Device6991::disconnectedDataStream);
	}
	~Device6991() override = default;

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
	void deviceStateRequest() noexcept {
		if (invokeCmd("SYSTem:STATe?")) {
			auto devState = extractStatus(response_);
			devState.setControllerId(controllerId());
			emit state(devState);
		}	
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
		auto success = invokeCmd(QString("*HW:FEC? 0, #h%1").arg(toHex(address, 8)));
		if (success) {
			auto list = response_.split(';');
			return std::optional{ std::pair{list[0].toUInt(nullptr, 16), list[1].toUInt(nullptr, 16)} };
		}
		return  std::nullopt;
	}

	bool writeBothFecRegisters(uint32_t const address, uint32_t const data1, uint32_t const data2) const noexcept {
		return invokeCmd(QString("*HW:FEC 0, #h%1, #h%2, #h%3").arg(toHex(address, 8), toHex(data1, 8), toHex(data2, 8)));
	}

	std::optional<uint32_t> readCpuRegister(uint32_t const address) const noexcept {
		return invokeCmd(QString("*HW:REG? #h%1").arg(toHex(address, 8))) ? std::optional{ response_.toUInt(nullptr, 16) } : std::nullopt;
	}

	void setStoreData(bool const state) noexcept {
		dataStream_->setStoreData(state);
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

	std::optional<FecType::Type> readFecType(FecIdType::Type const fecId) noexcept {
		return fecRegs_[fecId].FE_ID_reg_.fecType();
	}

	bool isFecIdle(FecIdType::Type fecId, FecType::Type const type) noexcept {
		return fecRegs_[fecId].BOARD_CSR_reg_.isIdle(type);
	}

	bool isFecIdle(FecIdType::Type fecId) noexcept {
		auto fecType = readFecType(fecId);
		return fecType && fecRegs_[fecId].BOARD_CSR_reg_.isIdle(*fecType);
	}

	bool setProperDlFrame(FecIdType::Type fecId, FecType::Type const type) noexcept {
		return DL_SPI_CSR2_reg_.setDlFrameLength(type, fecId);
	}

	bool setProperDlFrame(FecIdType::Type fecId) noexcept {
		auto fecType = readFecType(fecId);
		return fecType && DL_SPI_CSR2_reg_.setDlFrameLength(*fecType, fecId);
	}
public slots:
	void handleFpgaRegisterReadReq(uint32_t const address) const noexcept {
		if (auto reg = readFpgaRegister(address); reg) {
			emit fpgaRegisterReadResp(*reg);
			emit logMsg(QString("Fpga register read successful, address:%1, data:%2").arg(toHex(address, 8), toHex(*reg, 8)));
		}
		else
			emit reportError(QString("Failed to read fpga register, address:%1").arg(toHex(address, 8)));
	}

	void handleFecRegisterReadReq(FecIdType::Type const fecId, uint32_t const address) const noexcept {
		if (fecId != FecIdType::BOTH) {
			if (auto reg = readFecRegister(fecId, address); reg) {
				emit fecRegisterReadResp(fecId, *reg);
				emit logMsg(QString("Fec register read successful, fecId:%1, address:%2, data:%3").arg(fecId).arg(toHex(address, 8), toHex(*reg, 8)));
			}
			else
				emit reportError(QString("Failed to read fec register, fecId:%1, address:%2").arg(fecId).arg(toHex(address, 8)));
		}
		else {
			if (auto reg = readBothFecRegisters(address); reg) {
				emit fecRegisterReadResp(FecIdType::_1, (*reg).first);
				emit fecRegisterReadResp(FecIdType::_2, (*reg).second);
				emit logMsg(QString("Fecs registers read successful, fecId:%1, address:%2, data1:%3, data2:%4").arg(fecId).arg(toHex(address, 8), toHex((*reg).first, 8), toHex((*reg).second, 8)));
			}
			else
				emit reportError(QString("Failed to read fecs registers, fecId:%1, address:%2").arg(fecId).arg(toHex(address, 8)));
		}
	}

	void handleFpgaRegisterWriteReq(uint32_t const address, uint32_t const data) const noexcept {
		if (writeFpgaRegister(address, data))
			emit logMsg(QString("Fpga register write successful, address:%1, data:%2").arg(toHex(address, 8), toHex(data, 8)));
		else
			emit reportError(QString("Failed to write fpga register, address:%1, data:%2").arg(toHex(address, 8), toHex(data, 8)));			
	}

	void handleFecRegisterWriteReq(FecIdType::Type const fecId, uint32_t const address, uint32_t const data) const noexcept {
		if (writeFecRegister(fecId, address, data))
			emit logMsg(QString("Fec register write successful, fecId:%1, address:%2, data:%3").arg(fecId).arg(toHex(address, 8), toHex(data, 8)));
		else
			emit reportError(QString("Failed to write fec register, fecId:%1, address:%2, data:%3").arg(fecId).arg(toHex(address, 8), toHex(data, 8)));
	}
			
	//TODO when implementation will be ready on the device
	void handleFecStateReq(FecIdType::Type const fecId) const noexcept {
		//if (auto present = BOARD_CSR1_reg_.isFecPresent(fecId); present && *present)
		emit fecEnabledResp(fecId);
		//else
		//	emit fcCardDisabled(fecId);
	}

	void handleTakeControlReq(int const id) const noexcept {
		if (invokeCmd(QString("SYSTem:LOCK %1").arg(id)))
			emit controlGranted();
	}

	void handleReleaseControlReq() const noexcept {
		if (invokeCmd("SYSTem:LOCK 0"))
			emit controlReleased();
	}

	void handleConnectDataStreamReq(int const dataStreamId) {
		auto ipResource = inputResources().back();
		dataStream_->connect({ ipResource->load() }, 16100 + dataStreamId);
	}

	void handleDisconnectDataStreamReq() {
		dataStream_->disconnect();
	}

	void handleConfigureDeviceReq(Configuration6991 const& config) const {
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

	void handleStartAcqReq() {
		if (auto mode = startMode(); mode && *mode == AcquisitionStartModeEnum::IMMEDIATE && invokeCmd("MEASure:ASYNc")) {
			isAcqActive_ = true;
			emit acquisitionStarted();
		}
	}

	void handleStopAcqReq() {
		if (invokeCmd("MEASure:ABORt")) {
			isAcqActive_ = false;
			emit acquisitionStopped();
		}
	}

	void handleStartTestsReq(StartTestsRequest const& request) noexcept {
		handleStopTestsReq();
		//starting tests order matters, fifo test cant be run with dl test and dl test must be started before cl
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

	void handleStopTestsReq() noexcept {
		//stopping tests order matters, cl test must be stopped first
		if (auto id = controllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
			clTest_.stopTest();
			dlTest_.stopTest();
			fifoTest_.stopTest();
			invokeCmd("SYSTem:LOCK 0");
		}
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
	void fecEnabledResp(int const fcId) const;
	void fecDisabledResp(int const fcId) const;
	void fpgaRegisterReadResp(unsigned int const data) const;
	void fecRegisterReadResp(FecIdType::Type const fecId, unsigned int const data) const;
};
