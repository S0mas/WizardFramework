#pragma once
#include "../ScpiDevice.h"
#include "../ChannelsIF.h"
#include "../DeviceIdentityResourcesIF.h"

#include "FifoTest.h"
#include "ClTest.h"
#include "DlTest.h"
#include "Registers6991.h"
#include "../DataStream.h"
#include <QFile>
#include <future>

#include <array>
#include <atomic>
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
	Configuration6991 lastConfig_;

	uint32_t controllerId_ = 0;

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
	QString toChannelList(std::vector<uint32_t> const& channelIds) const noexcept;
	bool isAnyTestRunning() noexcept;
	void showWarningAboutCommunicationWithDeviceDuringAcq() const noexcept;
	DeviceState extractStatus(QString const& stateData) noexcept;
	std::optional<DeviceState> status() noexcept;
	bool enableChannal(uint32_t const channelId, bool const state = true) noexcept;
	bool enableChannals(std::vector<uint32_t> const& channelIds, bool const state = true) noexcept;
	bool enableChannals(std::vector<bool> const& states) noexcept;
	std::optional<bool> isChannelEnabled(uint32_t const channelId) noexcept;
	std::optional<std::vector<bool>> areChannelsEnabled(std::vector<uint32_t> const& channelIds) noexcept;
	std::optional<std::vector<bool>> areChannelsEnabled() noexcept;
	std::optional<ScanRateModel> scanRate() const noexcept;
	bool setScanRate(ScanRateModel const scanRate) const noexcept;
	std::optional<AcquisitionStartModeEnum::Type> startMode() const noexcept;
	bool setStartMode(AcquisitionStartModeEnum::Type const mode) const noexcept;
	bool setFilter(FilterType6132::Type const filter, std::vector<uint32_t> const& channelsIds) const noexcept;
	bool setFilters(std::vector<FilterType6132::Type> const& gains) const noexcept;
	bool setFilter(FilterType6132::Type const gain, uint32_t const& channelId) const noexcept;
	std::optional<std::vector<FilterType6132::Type>> filter(std::vector<uint32_t> const& channelsIds) const noexcept;
	std::optional<FilterType6132::Type> filter(uint32_t const& channelsId) const noexcept;
	std::optional<std::vector<FilterType6132::Type>> filter() const noexcept;
	bool setGain(GainType6132::Type const gain, std::vector<uint32_t> const& channelsIds) const noexcept;
	bool setGains(std::vector<GainType6132::Type> const& gains) const noexcept;
	bool setGain(GainType6132::Type const gain, uint32_t const& channelId) const noexcept;
	std::optional<std::vector<GainType6132::Type>> gain(std::vector<uint32_t> const& channelsIds) const noexcept;
	std::optional<GainType6132::Type> gain(uint32_t const& channelsId) const noexcept;
	std::optional<std::vector<GainType6132::Type>> gain() const noexcept;
	std::optional<PtpTime> ptpTime() const noexcept;
	std::optional<bool> stopOnError() const noexcept;
	bool setStopOnError(bool const stopOnError) const noexcept;
	std::optional<uint32_t> scansTreshold() const noexcept;
	bool setScansTreshold(uint32_t const scansTreshold) const noexcept;
	std::optional<bool> timestamps() const noexcept;
	bool setTimeStamps(bool const mode) const noexcept;
	std::optional<DeviceState> channelState() const noexcept;
	std::optional<Temperature> temperature(TemperaturesEnum::Type const source) const noexcept;
	std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temperatures() const noexcept;
	std::optional<FansModeEnum::Type> fansMode() const noexcept;
	bool setFansMode(FansModeEnum::Type const mode) const noexcept;
	std::optional<ClockSourceEnum::Type> clockSource() const noexcept;
	void setClockSource(ClockSourceEnum::Type const source) const noexcept;
	std::optional<PtpTime> ptpAlarm() const noexcept;
	bool setPtpAlarm(PtpTime const alarmTime) const noexcept;
	std::optional<uint32_t> scansNoPerDirectReadPacket() const noexcept;
	bool setScansNoPerDirectReadPacket(uint32_t const scansNo) const noexcept;
	bool setUserEvent() const noexcept;
	QString prepareDataFileHeader() noexcept;
	QFile* createFileForDataCollection(QString const& name) noexcept;
	void replaceAcqDataFile() noexcept;
	bool is6111() noexcept;
	bool is6132() noexcept;
public:
	Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF, QObject* parent = nullptr) noexcept;
	~Device6991() override = default;

	void saveSubtype(const QString& str) const override {}
	void saveSerialnumber(const QString& str) const override {}
	void saveFirmwareRevision(const QString& str) const override {}
	void saveDriverRevision(const QString& str) const override {}
	QString loadSubtype() const override { return ""; }
	QString loadSerialnumber() const override { return ""; }
	QString loadFirmwareRevision() const override { return ""; }
	QString loadDriverRevision() const override { return ""; }
	bool isAcquisitionActive() noexcept;
	void checkIfAcqWasStartedFromOtherClient() noexcept;
	void checkIfAcqWasStoppedFromOtherClient() noexcept;
	bool clearUserEvent() const noexcept; //it can be invoked by multiple clients (when it is already cleared) the problem is - if it is possible that one client will clear this event before another will handle it..
	std::optional<bool> isThisTheMasterController() const noexcept;
	std::optional<uint32_t> masterControllerId() const noexcept;
	uint32_t controllerId() const noexcept { return controllerId_; }
public slots:
	void deviceStateRequest() noexcept;
	void controllerKeyRequest() const noexcept;
	void testCountersRequest() noexcept;
	bool writeFpgaRegister(uint32_t const address, uint32_t const data) const noexcept;
	bool writeFecRegister(uint32_t const fcId, uint32_t const address, uint32_t const data) const noexcept;
	bool writeCpuRegister(uint32_t const address, uint32_t const data) const noexcept;
	std::optional<uint32_t> readFpgaRegister(uint32_t const address) const noexcept;
	std::optional<uint32_t> readFecRegister(uint32_t const fcId, uint32_t const address) const noexcept;
	std::optional<std::pair<uint32_t, uint32_t>> readBothFecRegisters(uint32_t const address) const noexcept;
	bool writeBothFecRegisters(uint32_t const address, uint32_t const data1, uint32_t const data2) const noexcept;
	std::optional<uint32_t> readCpuRegister(uint32_t const address) const noexcept;
	bool testWithTimeout(std::function<bool()> const& functionCondition, uint32_t const timeoutInMilliseconds = 0x7FFFFFFF, uint32_t const breakTimeInMilliseconds = 0) noexcept;
	std::optional<FecType::Type> readFecType(FecIdType::Type const fecId) noexcept;
	bool isFecIdle(FecIdType::Type fecId, FecType::Type const type) noexcept;
	bool isFecIdle(FecIdType::Type fecId) noexcept;
	bool setProperDlFrame(FecIdType::Type fecId, FecType::Type const type) noexcept;
	bool setProperDlFrame(FecIdType::Type fecId) noexcept;
public slots:
	void handleSetStoreDataReq(bool const state) noexcept;
	void handleSetForwardDataReq(bool const state) noexcept;
	void handleFpgaRegisterReadReq(uint32_t const address) const noexcept;
	void handleFecRegisterReadReq(FecIdType::Type const fecId, uint32_t const address) const noexcept;
	void handleFpgaRegisterWriteReq(uint32_t const address, uint32_t const data) const noexcept;
	void handleFecRegisterWriteReq(FecIdType::Type const fecId, uint32_t const address, uint32_t const data) const noexcept;		
	//TODO uncomment when implementation will be ready on the device
	void handleFecStateReq(FecIdType::Type const fecId) const noexcept;
	void handleTakeControlReq(int const id) const noexcept;
	void handleReleaseControlReq() const noexcept;
	//temporary solution TODO create and mantain only one stream
	void handleConnectDataStreamReq(int const dataStreamId) noexcept;

	//temporary solution TODO create and mantain only one stream
	void handleDisconnectDataStreamReq() noexcept;
	void handleConfigureDeviceReq(Configuration6991 const& config) noexcept;
	void handleStartAcqReq() noexcept;
	void handleStopAcqReq() noexcept;
	void handleStartTestsReq(StartTestsRequest const& request) noexcept;
	void handleStopTestsReq() noexcept;
	void handleSetFiltersReq(FilterType6132::Type const filter, std::vector<uint32_t> const& channelIds) noexcept;
	void handleSetGainsReq(GainType6132::Type const gain, std::vector<uint32_t> const& channelIds) noexcept;
	void handleSetChannelsEnabledReq(bool const state, std::vector<uint32_t> const& channelIds) noexcept;
	void handleChannelsConfigurationReq() noexcept;
	void handleDeviceConfigurationReq() noexcept;
	void handleIdChanged(uint32_t const id) noexcept;
	void handleSaveChannelConfigurationToFileReq(QString const& fileName) noexcept;
	void handleLoadChannelConfigurationFromFileReq(QString const& fileName) noexcept;
signals:
	void actualControllerKey(int const id) const;
	void acquisitionStarted() const;
	void acquisitionStopped() const;
	void connectedDataStream() const;
	void controlGranted() const;
	void controlReleased() const;
	void disconnectedDataStream() const;
	void state(DeviceState const state) const;
	void deviceConfiguration(Configuration6991 const configuration) const;
	void testsStarted() const;
	void testsStopped() const;
	void testCounters(TestsStatus const& status) const;
	void fecEnabledResp(int const fcId) const;
	void fecDisabledResp(int const fcId) const;
	void fpgaRegisterReadResp(unsigned int const data) const;
	void fecRegisterReadResp(FecIdType::Type const fecId, unsigned int const data) const;
	void channelConfiguration(ChannelsConfiguration const& configuration) const;
};
