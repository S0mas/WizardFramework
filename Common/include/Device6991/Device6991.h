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

	//Temporary solution TODO create proper stream only
	DataStream<ScanFifo> dataStreamFifo_{ 0, this };
	DataStream<Scan6111> dataStream6111_{ forwardingPort6111, this };
	DataStream<Scan6132> dataStream6132_{ forwardingPort6132, this };
	int withStreamShouldReconnectAfterFifoTest = 0;
	int portToReconnectAfterFifoTest = 0;
	QHostAddress addressToRecnnectAfterFifoTest;
	///////////////////////////////////////////////////

	bool isAcqActive_ = false;

	bool isAnyTestRunning() noexcept {
		return clTest_.isRunning() || dlTest_.isRunning() || fifoTest_.isRunning();
	}

	void showWarningAboutCommunicationWithDeviceDuringAcq() const noexcept {
		sender_->waitForConfirmation("Please, keep in mind that any type of communication with the device during acquisition will have impact on the performance of the whole process.");
	}

	DeviceState extractStatus(QString const& stateData) noexcept {
		auto list = stateData.split(',');
		DeviceState state;
		state.setState(DeviceStateEnum::fromString(list[0]));
		//TODO UNCOMMENT WHEN RDY
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
		auto resp = invokeQuery(QString("SYSTem:STATe?"));
		return resp ? std::optional{extractStatus(*resp)} : std::nullopt;
	}

	bool enableChannal(uint32_t const channelId, bool const state = true) noexcept {
		return invokeCmd(QString("CONFigure%1:ENABled %2").arg(channelId).arg(state ? 1 : 0));
	}

	std::optional<ScanRateModel> scanRate() const noexcept {
		if (auto rate = invokeQuery(QString("CONFigure:SCAN:RATe?")); rate)
			return ScanRateModel::fromString(*rate);
		return std::nullopt;
	}

	bool setScanRate(ScanRateModel const scanRate) const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:RATe %1,%2").arg(scanRate.value_).arg(ScanRateUnitsEnum::toString(scanRate.units_)));
	}

	std::optional<AcquisitionStartModeEnum::Type> startMode() const noexcept {
		if (auto mode = invokeQuery(QString("CONFigure:TRIGger:STARt?")); mode)
			return std::optional(AcquisitionStartModeEnum::fromString(*mode));
		return std::nullopt;
	}

	bool setStartMode(AcquisitionStartModeEnum::Type const mode) const noexcept {
		return invokeCmd(QString("CONFigure:TRIGger:STARt '%1'").arg(AcquisitionStartModeEnum::toString(mode)));
	}

	std::optional<PtpTime> ptpTime() const noexcept {
		if (auto ptpTime = invokeQuery(QString("SYSTem:PTPTime?")); ptpTime)
			return PtpTime::fromString(*ptpTime);
		return std::nullopt;
	}

	std::optional<bool> stopOnError() const noexcept {
		if (auto stopOnError = invokeQuery(QString("CONFigure:STOPonerr?")); stopOnError)
			return (*stopOnError).toUInt() == 1;
		return std::nullopt;
	}

	bool setStopOnError(bool const stopOnError) const noexcept {
		return invokeCmd(QString("CONFigure:STOPonerr %1").arg(stopOnError ? 1 : 0));
	}

	std::optional<uint32_t> scansTreshold() const noexcept {
		if (auto scanTreshold = invokeQuery(QString("CONFigure:SCAN:POST?")); scanTreshold)
			return (*scanTreshold).toUInt();
		return std::nullopt;
	}

	bool setScansTreshold(uint32_t const scansTreshold) const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:POST %1").arg(scansTreshold));
	}

	std::optional<bool> timestamps() const noexcept {
		if (auto timestamp = invokeQuery(QString("CONFigure:SCAN:TIMestamp?")); timestamp)
			return (*timestamp).toUInt() == 1;
		return std::nullopt;
	}

	bool setTimeStamps(bool const mode) const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:TIMestamp %1").arg(mode ? 1 : 0));
	}

	std::optional<DeviceState> channelState() const noexcept { // TODO
		return std::nullopt;
	}

	std::optional<Temperature> temperature(TemperaturesEnum::Type const source) const noexcept {
		if (auto temp = invokeQuery(QString("SYSTem:TEMPerature? '%1'").arg(TemperaturesEnum::toString(source))); temp)
			return Temperature{ (*temp).toUInt(), source };//TODO check temp response format
		return std::nullopt;
	}

	std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temperatures() const noexcept {
		std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temps;
		for (auto source : TemperaturesEnum::TYPES)
			temps[source] = temperature(source);	
		return temps;
	}

	std::optional<FansModeEnum::Type> fansMode() const noexcept {
		if (auto fansMode = invokeQuery(QString("SYSTem:FAN?")); fansMode)
			return FansModeEnum::fromString(*fansMode);
		return std::nullopt;
	}

	bool setFansMode(FansModeEnum::Type const mode) const noexcept {
		return invokeCmd(QString("SYSTem:FAN %1").arg(mode));
	}

	std::optional<ClockSourceEnum::Type> clockSource() const noexcept { // TODO
		return std::nullopt;
	}

	void setClockSource(ClockSourceEnum::Type const source) const noexcept { // TODO

	}

	std::optional<PtpTime> ptpAlarm() const noexcept {
		if (auto ptpAlarm = invokeQuery(QString("CONFigure:TRIGger:PTP:ALARm?")); ptpAlarm) {
			auto list = (*ptpAlarm).split(',');
			return PtpTime{ list[0].toUInt(), list[1].toUInt() };
		}
		return std::nullopt;
	}

	bool setPtpAlarm(PtpTime const alarmTime) const noexcept {
		return invokeCmd(QString("CONFigure:TRIGger:PTP:ALARm %1,%2").arg(alarmTime.seconds_).arg(alarmTime.nanoseconds_));
	}

	std::optional<uint32_t> scansNoPerDirectReadPacket() const noexcept {
		if (auto scansPerPacket = invokeQuery(QString("CONFigure:DIRectread?")); scansPerPacket)
			return (*scansPerPacket).toUInt();
		return std::nullopt;
	}

	bool setScansNoPerDirectReadPacket(uint32_t const scansNo) const noexcept {
		return invokeCmd(QString("CONFigure:DIRectread %1").arg(scansNo));
	}
public:
	Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF,  QObject* parent = nullptr) noexcept : ScpiDevice(nameId, connector, scpiIF, parent), DeviceIdentityResourcesIF(nameId) {
		QObject::connect(dataStreamFifo_.client(), &DataCollectorClient::connected, this, &Device6991::connectedDataStream);
		QObject::connect(dataStreamFifo_.client(), &DataCollectorClient::disconnected, this, &Device6991::disconnectedDataStream);
		QObject::connect(dataStreamFifo_.client(), &DataCollectorClient::logMsg, this, &Device6991::logMsg);
		QObject::connect(dataStreamFifo_.client(), &DataCollectorClient::reportError, this, &Device6991::reportError);
		dataStreamFifo_.addCallback([this](HeaderPart6991 const& header, SignalPacketData<ScanFifo> const& data) { if (fifoTest_.isRunning()) for (auto const& scan : data.scans_)fifoTest_.validateData(scan.samples_); });

		QObject::connect(dataStream6111_.client(), &DataCollectorClient::connected, this, &Device6991::connectedDataStream);
		QObject::connect(dataStream6111_.client(), &DataCollectorClient::disconnected, this, &Device6991::disconnectedDataStream);
		QObject::connect(dataStream6111_.client(), &DataCollectorClient::logMsg, this, &Device6991::logMsg);
		QObject::connect(dataStream6111_.client(), &DataCollectorClient::reportError, this, &Device6991::reportError);

		QObject::connect(dataStream6132_.client(), &DataCollectorClient::connected, this, &Device6991::connectedDataStream);
		QObject::connect(dataStream6132_.client(), &DataCollectorClient::disconnected, this, &Device6991::disconnectedDataStream);
		QObject::connect(dataStream6132_.client(), &DataCollectorClient::logMsg, this, &Device6991::logMsg);
		QObject::connect(dataStream6132_.client(), &DataCollectorClient::reportError, this, &Device6991::reportError);
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
		if (auto lock = invokeQuery(QString("SYSTem:LOCK?")); lock)
			return (*lock).toUInt();
		return std::nullopt;
	}
public slots:
	void deviceStateRequest() noexcept {
		if (dataStream6111_.isConnected() && isAcqActive_)
			emit state(dataStream6111_.deviceState());
		else if (dataStream6132_.isConnected() && isAcqActive_) {
			emit state(dataStream6132_.deviceState());
		}
		else if (dataStreamFifo_.isConnected() && isAcqActive_) {
			emit state(dataStreamFifo_.deviceState());
		}
		else {
			if (auto st = status(); st) {
				st->setControllerId(controllerId());
				emit state(*st);
			}
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
		if (auto reg = invokeQuery(QString("*HW:FPGA? #h%1").arg(toHex(address, 8))); reg)
			return (*reg).toUInt(nullptr, 16);
		return std::nullopt;
	}

	std::optional<uint32_t> readFecRegister(uint32_t const fcId, uint32_t const address) const noexcept {
		if (auto reg = invokeQuery(QString("*HW:FEC? %1, #h%2").arg(fcId).arg(toHex(address, 8))); reg)
			return (*reg).toUInt(nullptr, 16);
		return std::nullopt;
	}

	std::optional<std::pair<uint32_t, uint32_t>> readBothFecRegisters(uint32_t const address) const noexcept {
		if (auto regs = invokeQuery(QString("*HW:FEC? 0, #h%1").arg(toHex(address, 8))); regs) {
			auto list = (*regs).split(';');
			return std::pair{list[0].toUInt(nullptr, 16), list[1].toUInt(nullptr, 16)};
		}
		return std::nullopt;
	}

	bool writeBothFecRegisters(uint32_t const address, uint32_t const data1, uint32_t const data2) const noexcept {
		return invokeCmd(QString("*HW:FEC 0, #h%1, #h%2, #h%3").arg(toHex(address, 8), toHex(data1, 8), toHex(data2, 8)));
	}

	std::optional<uint32_t> readCpuRegister(uint32_t const address) const noexcept {
		if (auto reg = invokeQuery(QString("*HW:REG? #h%1").arg(toHex(address, 8))); reg)
			return (*reg).toUInt(nullptr, 16);
		return std::nullopt;
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
	void handleSetStoreDataReq(bool const state) noexcept {
		dataStreamFifo_.setStoreData(state);
		dataStream6111_.setStoreData(state);
		dataStream6132_.setStoreData(state);
	}

	void handleSetForwardDataReq(bool const state) noexcept {
		dataStream6111_.setForwardData(state);
		dataStream6132_.setForwardData(state);
	}

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
			
	//TODO uncomment when implementation will be ready on the device
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

	//temporary solution TODO create and mantain only one stream
	void handleConnectDataStreamReq(int const dataStreamId) {
		auto ipResource = inputResources().back();
		auto type1 = readFecType(FecIdType::_1);
		auto type2 = readFecType(FecIdType::_2);
		if (type1 && *type1 == FecType::_6111)
			dataStream6111_.connect({ ipResource->load() }, 16100 + dataStreamId);
		else if (type1 && *type1 == FecType::_6132)
			dataStream6132_.connect({ ipResource->load() }, 16100 + dataStreamId);
		else if (type2 && *type2 == FecType::_6111)
			dataStream6111_.connect({ ipResource->load() }, 16100 + dataStreamId);
		else if (type2 && *type2 == FecType::_6132)
			dataStream6132_.connect({ ipResource->load() }, 16100 + dataStreamId);
		else
			reportError("Data stream connection is not supported if there is no frontend cards recognized!");
	}

	//temporary solution TODO create and mantain only one stream
	void handleDisconnectDataStreamReq() {
		if (dataStream6111_.isConnected())
			dataStream6111_.disconnect();
		else if (dataStream6132_.isConnected())
			dataStream6132_.disconnect();
		else if (dataStreamFifo_.isConnected())
			dataStreamFifo_.disconnect();
		return;
	}

	void handleConfigureDeviceReq(Configuration6991 const& config) {
		invokeCmd("*DBG 0");
		if (config.scanRate_)
			setScanRate(*config.scanRate_);
		if (config.scansPerDirectReadPacket_)
			setScansNoPerDirectReadPacket(*config.scansPerDirectReadPacket_);
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
		if (config.timestamps_)
			setTimeStamps(*config.timestamps_);
		if (auto fecType = readFecType(FecIdType::_1); fecType && *fecType == FecType::_6132)
			for (auto i = 1; i < 33; ++i)
				enableChannal(i);
		else if (auto fecType = readFecType(FecIdType::_2); fecType && *fecType == FecType::_6132)
			for (auto i = 1; i < 33; ++i)
				enableChannal(i);
		if (auto fecType = readFecType(FecIdType::_1); fecType && *fecType == FecType::_6111)
			for (auto i = 1; i < 256; ++i)
				enableChannal(i);
		else if (auto fecType = readFecType(FecIdType::_2); fecType && *fecType == FecType::_6111)
			for (auto i = 1; i < 256; ++i)
				enableChannal(i);
		
		
	}

	void handleStartAcqReq() {
		showWarningAboutCommunicationWithDeviceDuringAcq();
		if (/*auto mode = startMode(); mode && *mode == AcquisitionStartModeEnum::IMMEDIATE && */invokeCmd("MEASure:ASYNc")) {
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

			if (request.model.at(TestTypeEnum::FIFO)) {
///// temporary solution TODO remove this and create only valid stream
				if (dataStream6111_.isConnected()) {
					portToReconnectAfterFifoTest = dataStream6111_.peerPort();
					addressToRecnnectAfterFifoTest = dataStream6111_.peerAddress();
					withStreamShouldReconnectAfterFifoTest = 6111;
					dataStream6111_.disconnect();
					dataStreamFifo_.connect(addressToRecnnectAfterFifoTest.toString(), portToReconnectAfterFifoTest);
					qDebug() << "6111 was connected!";
				}
				else if (dataStream6132_.isConnected()) {
					portToReconnectAfterFifoTest = dataStream6132_.peerPort();
					addressToRecnnectAfterFifoTest = dataStream6132_.peerAddress();
					withStreamShouldReconnectAfterFifoTest = 6132;
					dataStream6132_.disconnect();
					dataStreamFifo_.connect(addressToRecnnectAfterFifoTest.toString(), portToReconnectAfterFifoTest);
					qDebug() << "6132 was connected!";
				}
				else
					qDebug() << "no data stream connected!";

//////////////////////////////////////////////////////////////////////
				fifoTest_.startTest(request.fifoTestConfig_);
			}

			clTest_.startTest(request.model.at(TestTypeEnum::CL0), request.model.at(TestTypeEnum::CL1));
			emit testsStarted();
			invokeCmd("SYSTem:LOCK 0");
		}
	}

	void handleStopTestsReq() noexcept {
		//stopping tests order matters, cl test must be stopped first
		if (auto id = controllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
			clTest_.stopTest();
			fifoTest_.stopTest();
			if (withStreamShouldReconnectAfterFifoTest != 0) {
				if (withStreamShouldReconnectAfterFifoTest == 6111)
					dataStream6111_.connect(addressToRecnnectAfterFifoTest.toString(), portToReconnectAfterFifoTest);
				else if (withStreamShouldReconnectAfterFifoTest == 6132) 
					dataStream6132_.connect(addressToRecnnectAfterFifoTest.toString(), portToReconnectAfterFifoTest);
				withStreamShouldReconnectAfterFifoTest = 0;
			}
			dlTest_.stopTest();		
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
