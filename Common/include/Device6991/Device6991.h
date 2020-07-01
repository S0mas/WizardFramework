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
	QString toChannelList(std::vector<uint32_t> const& channelIds) const;
	bool isAnyTestRunning();
	void showWarningAboutCommunicationWithDeviceDuringAcq() const;
	DeviceState extractStatus(QString const& stateData);
	std::optional<DeviceState> status();
	bool enableChannal(uint32_t const channelId, bool const state = true);
	bool enableChannals(std::vector<uint32_t> const& channelIds, bool const state = true);
	bool enableChannals(std::vector<bool> const& states);
	std::optional<bool> isChannelEnabled(uint32_t const channelId);
	std::optional<std::vector<bool>> areChannelsEnabled(std::vector<uint32_t> const& channelIds);
	std::optional<std::vector<bool>> areChannelsEnabled();
	std::optional<ScanRateModel> scanRate() const;
	bool setScanRate(ScanRateModel const scanRate) const;
	std::optional<AcquisitionStartModeEnum::Type> startMode() const;
	bool setStartMode(AcquisitionStartModeEnum::Type const mode) const;
	bool setFilter(FilterType6132::Type const filter, std::vector<uint32_t> const& channelsIds) const;
	bool setFilters(std::vector<FilterType6132::Type> const& gains) const;
	bool setFilter(FilterType6132::Type const gain, uint32_t const& channelId) const;
	std::optional<std::vector<FilterType6132::Type>> filter(std::vector<uint32_t> const& channelsIds) const;
	std::optional<FilterType6132::Type> filter(uint32_t const& channelsId) const;
	std::optional<std::vector<FilterType6132::Type>> filter() const;
	bool setGain(GainType6132::Type const gain, std::vector<uint32_t> const& channelsIds) const;
	bool setGains(std::vector<GainType6132::Type> const& gains) const;
	bool setGain(GainType6132::Type const gain, uint32_t const& channelId) const;
	std::optional<std::vector<GainType6132::Type>> gain(std::vector<uint32_t> const& channelsIds) const;
	std::optional<GainType6132::Type> gain(uint32_t const& channelsId) const;
	std::optional<std::vector<GainType6132::Type>> gain() const;
	std::optional<PtpTime> ptpTime() const;
	std::optional<bool> stopOnError() const;
	bool setStopOnError(bool const stopOnError) const;
	std::optional<uint32_t> scansTreshold() const;
	bool setScansTreshold(uint32_t const scansTreshold) const;
	std::optional<bool> timestamps() const;
	bool setTimeStamps(bool const mode) const;
	std::optional<DeviceState> channelState() const;
	std::optional<Temperature> temperature(TemperaturesEnum::Type const source) const;
	std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temperatures() const;
	std::optional<FansModeEnum::Type> fansMode() const;
	bool setFansMode(FansModeEnum::Type const mode) const;
	std::optional<ClockSourceEnum::Type> clockSource() const;
	void setClockSource(ClockSourceEnum::Type const source) const;
	std::optional<PtpTime> ptpAlarm() const;
	bool setPtpAlarm(PtpTime const alarmTime) const;
	std::optional<uint32_t> scansNoPerDirectReadPacket() const;
	bool setScansNoPerDirectReadPacket(uint32_t const scansNo) const;
	bool setUserEvent() const;
	QString prepareDataFileHeader();
	QFile* createFileForDataCollection(QString const& name);
	void replaceAcqDataFile();
	bool is6111();
	bool is6132();
public:
	Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF, QObject* parent = nullptr);
	~Device6991() override = default;

	void saveSubtype(const QString& str) const override {}
	void saveSerialnumber(const QString& str) const override {}
	void saveFirmwareRevision(const QString& str) const override {}
	void saveDriverRevision(const QString& str) const override {}
	QString loadSubtype() const override { return ""; }
	QString loadSerialnumber() const override { return ""; }
	QString loadFirmwareRevision() const override { return ""; }
	QString loadDriverRevision() const override { return ""; }
	bool isAcquisitionActive();
	void checkIfAcqWasStartedFromOtherClient();
	void checkIfAcqWasStoppedFromOtherClient();
	bool clearUserEvent() const; //it can be invoked by multiple clients (when it is already cleared) the problem is - if it is possible that one client will clear this event before another will handle it..
	std::optional<bool> isThisTheMasterController() const;
	std::optional<uint32_t> masterControllerId() const;
	uint32_t controllerId() const { return controllerId_; }
public slots:
	void deviceStateRequest();
	void controllerKeyRequest() const;
	void testCountersRequest();
	bool writeFpgaRegister(uint32_t const address, uint32_t const data) const;
	bool writeFecRegister(uint32_t const fcId, uint32_t const address, uint32_t const data) const;
	bool writeCpuRegister(uint32_t const address, uint32_t const data) const;
	std::optional<uint32_t> readFpgaRegister(uint32_t const address) const;
	std::optional<uint32_t> readFecRegister(uint32_t const fcId, uint32_t const address) const;
	std::optional<std::pair<uint32_t, uint32_t>> readBothFecRegisters(uint32_t const address) const;
	bool writeBothFecRegisters(uint32_t const address, uint32_t const data1, uint32_t const data2) const;
	std::optional<uint32_t> readCpuRegister(uint32_t const address) const;
	bool testWithTimeout(std::function<bool()> const& functionCondition, uint32_t const timeoutInMilliseconds = 0x7FFFFFFF, uint32_t const breakTimeInMilliseconds = 0);
	std::optional<FecType::Type> readFecType(FecIdType::Type const fecId);
	bool isFecIdle(FecIdType::Type fecId, FecType::Type const type);
	bool isFecIdle(FecIdType::Type fecId);
	bool setProperDlFrame(FecIdType::Type fecId, FecType::Type const type);
	bool setProperDlFrame(FecIdType::Type fecId);
public slots:
	void handleSetStoreDataReq(bool const state);
	void handleSetForwardDataReq(bool const state);
	void handleFpgaRegisterReadReq(uint32_t const address) const;
	void handleFecRegisterReadReq(FecIdType::Type const fecId, uint32_t const address) const;
	void handleFpgaRegisterWriteReq(uint32_t const address, uint32_t const data) const;
	void handleFecRegisterWriteReq(FecIdType::Type const fecId, uint32_t const address, uint32_t const data) const;		
	//TODO uncomment when implementation will be ready on the device
	void handleFecStateReq(FecIdType::Type const fecId) const;
	void handleTakeControlReq(int const id) const;
	void handleReleaseControlReq() const;
	//temporary solution TODO create and mantain only one stream
	void handleConnectDataStreamReq(int const dataStreamId);

	//temporary solution TODO create and mantain only one stream
	void handleDisconnectDataStreamReq();
	void handleConfigureDeviceReq(Configuration6991 const& config);
	void handleStartAcqReq();
	void handleStopAcqReq();
	void handleStartTestsReq(StartTestsRequest const& request);
	void handleStopTestsReq();
	void handleSetFiltersReq(FilterType6132::Type const filter, std::vector<uint32_t> const& channelIds);
	void handleSetGainsReq(GainType6132::Type const gain, std::vector<uint32_t> const& channelIds);
	void handleSetChannelsEnabledReq(bool const state, std::vector<uint32_t> const& channelIds);
	void handleChannelsConfigurationReq();
	void handleDeviceConfigurationReq();
	void handleIdChanged(uint32_t const id);
	void handleSaveChannelConfigurationToFileReq(QString const& fileName);
	void handleLoadChannelConfigurationFromFileReq(QString const& fileName);
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
