#include "../../include/Device6991/Device6991.h"
#include "../../include/ScpiDevice.h"
#include "../../include/ChannelsIF.h"
#include "../../include/DeviceIdentityResourcesIF.h"
#include "../../include/Device6991/Defines6991.h"
#include "../../include/Device6991/FifoTest.h"
#include "../../include/Device6991/ClTest.h"
#include "../../include/Device6991/DlTest.h"
#include "../../include/Device6991/Registers6991.h"
#include "../../include/DataStream.h"

#include <QFile>
#include <future>

#include <array>
#include <atomic>
#include <optional>

void Device6991::checkIfAcqWasStartedFromOtherClient() noexcept {
	if (!isAcqActive_) {
		replaceAcqDataFile(); // This solution is not perfect, possible problems : 1. The old data is still queued in the stream while signal about new acq arrived.(it will appear in new file) 2. Some new data already arrived before file opening procedure was triggered. (it wont be stored at all).
		isAcqActive_ = true;
		emit acquisitionStarted();
	}
}

void Device6991::checkIfAcqWasStoppedFromOtherClient() noexcept {
	if (isAcqActive_) {
		isAcqActive_ = false;
		emit acquisitionStopped();
	}
}

bool Device6991::clearUserEvent() const noexcept {
	return invokeCmd("SYST:USER 0");
}

std::optional<bool> Device6991::isThisTheMasterController() const noexcept {
	if (auto id = masterControllerId(); id)
		return *id == controllerId_;
	return std::nullopt;
}

QString Device6991::toChannelList(std::vector<uint32_t> const& channelIds) const noexcept {
	auto sortedIds = channelIds;
	std::sort(sortedIds.begin(), sortedIds.end());
	QString channels;
	for (int i = 0; i < channelIds.size(); ++i) {
		uint32_t startChannel = channelIds[i];
		uint32_t nextChannel = i + 1 == channelIds.size() ? 0 : channelIds[i + 1];
		uint32_t endChannel = startChannel;
		while (i < (channelIds.size() - 1) && ((endChannel + 1) == nextChannel)) {
			endChannel = nextChannel;
			if (++i < (channelIds.size() - 1))
				nextChannel = channelIds[i + 1];
		}
		channels += QString::number(startChannel);
		if (startChannel != endChannel && startChannel != channelIds.back())
			channels += QString(":%1").arg(endChannel);
		if (i < (channelIds.size() - 1))
			channels += ", ";
	}
	return QString("(@%1)").arg(channels);
}

bool Device6991::isAnyTestRunning() noexcept {
	return clTest_.isRunning() || dlTest_.isRunning() || fifoTest_.isRunning();
}

void Device6991::showWarningAboutCommunicationWithDeviceDuringAcq() const noexcept {
	sender_->waitForConfirmation("Please, keep in mind that any type of communication with the device during acquisition will have impact on the performance of the whole process.");
}

DeviceState Device6991::extractStatus(QString const& stateData) noexcept {
	auto list = stateData.split(',');
	DeviceState state;
	state.setState(DeviceStateEnum::fromString(list[0]));
	state.set(list[1]);
	return state;
}

std::optional<DeviceState> Device6991::status() noexcept {
	auto resp = invokeQuery(QString("SYSTem:STATe?"));
	return resp ? std::optional{ extractStatus(*resp) } : std::nullopt;
}

bool Device6991::enableChannal(uint32_t const channelId, bool const state) noexcept {
	clearUserEvent();
	setUserEvent();
	return invokeCmd(QString("CONFigure%1:ENABled %2").arg(channelId).arg(state ? 1 : 0));
}

bool Device6991::enableChannals(std::vector<uint32_t> const& channelIds, bool const state) noexcept {
	clearUserEvent();
	setUserEvent();
	return invokeCmd(QString("CONFigure:ENABled %1, %2").arg(state ? 1 : 0).arg(toChannelList(channelIds)));
}

std::optional<bool> Device6991::isChannelEnabled(uint32_t const channelId) noexcept {
	return invokeQuery(QString("CONFigure%1:ENABled?").arg(channelId))->toUInt() == 1;
}

std::optional<std::vector<bool>> Device6991::areChannelsEnabled(std::vector<uint32_t> const& channelIds) noexcept {
	if (auto resp = invokeQuery(QString("CONFigure:ENABled? %1").arg(toChannelList(channelIds))); resp) {
		auto list = resp->split(',');
		std::vector<bool> states;
		for (auto const& str : list)
			states.push_back(str.toUInt() == 1);
		return states;
	}
	return std::nullopt;
}

std::optional<std::vector<bool>> Device6991::areChannelsEnabled() noexcept {
	if (auto resp = invokeQuery(QString("CONFigure0:ENABled?")); resp) {
		auto list = resp->split(',');
		std::vector<bool> states;
		for (auto const& str : list)
			states.push_back(str.toUInt() == 1);
		return states;
	}
	return std::nullopt;
}

std::optional<ScanRateModel> Device6991::scanRate() const noexcept {
	if (auto rate = invokeQuery(QString("CONFigure:SCAN:RATe?")); rate)
		return ScanRateModel::fromString(*rate);
	return std::nullopt;
}

bool Device6991::setScanRate(ScanRateModel const scanRate) const noexcept {
	return invokeCmd(QString("CONFigure:SCAN:RATe %1,%2").arg(scanRate.value_).arg(ScanRateUnitsEnum::toString(scanRate.units_)));
}

std::optional<AcquisitionStartModeEnum::Type> Device6991::startMode() const noexcept {
	if (auto mode = invokeQuery(QString("CONFigure:TRIGger:STARt?")); mode)
		return std::optional(AcquisitionStartModeEnum::fromString(*mode));
	return std::nullopt;
}

bool Device6991::setStartMode(AcquisitionStartModeEnum::Type const mode) const noexcept {
	return invokeCmd(QString("CONFigure:TRIGger:STARt '%1'").arg(AcquisitionStartModeEnum::toString(mode)));
}

bool Device6991::setFilter(FilterType6132::Type const filter, std::vector<uint32_t> const& channelsIds) const noexcept {
	return invokeCmd(QString("CONFigure:FILTer %1, %2").arg(FilterType6132::toString(filter)).arg(toChannelList(channelsIds)));
}

bool Device6991::setFilter(FilterType6132::Type const gain, uint32_t const& channelId) const noexcept {
	return invokeCmd(QString("CONFigure%1:FILTer %2").arg(channelId).arg(FilterType6132::toString(gain)));
}

std::optional<std::vector<FilterType6132::Type>> Device6991::filter(std::vector<uint32_t> const& channelsIds) const noexcept {
	if (auto resp = invokeQuery(QString("CONFigure:FILTer? %1").arg(toChannelList(channelsIds))); resp) {
		auto list = resp->split(',');
		std::vector<FilterType6132::Type> filters;
		for (auto const& str : list)
			filters.push_back(FilterType6132::fromString(str));
		return filters;
	}
	return std::nullopt;
}

std::optional<FilterType6132::Type> Device6991::filter(uint32_t const& channelsId) const noexcept {
	if (auto resp = invokeQuery(QString("CONFigure:FILTer? %1").arg(channelsId)); resp)
		return FilterType6132::fromString(*resp);
	return std::nullopt;
}

std::optional<std::vector<FilterType6132::Type>> Device6991::filter() const noexcept {
	if (auto resp = invokeQuery(QString("CONFigure0:FILTer?")); resp) {
		auto list = resp->split(',');
		std::vector<FilterType6132::Type> filters;
		for (auto const& str : list)
			filters.push_back(FilterType6132::fromString(str));
		return filters;
	}
	return std::nullopt;
}

bool Device6991::setGain(GainType6132::Type const gain, std::vector<uint32_t> const& channelsIds) const noexcept {
	return invokeCmd(QString("CONFigure:GAIN %1, %2").arg(GainType6132::toString(gain)).arg(toChannelList(channelsIds)));
}

bool Device6991::setGain(GainType6132::Type const gain, uint32_t const& channelId) const noexcept {
	return invokeCmd(QString("CONFigure%1:GAIN %2").arg(channelId).arg(GainType6132::toString(gain)));
}

std::optional<std::vector<GainType6132::Type>> Device6991::gain(std::vector<uint32_t> const& channelsIds) const noexcept {
	if (auto resp = invokeQuery(QString("CONFigure:GAIN? %1").arg(toChannelList(channelsIds))); resp) {
		auto list = resp->split(',');
		std::vector<GainType6132::Type> gains;
		for (auto const& str : list)
			gains.push_back(GainType6132::fromString(str));
		return gains;
	}
	return std::nullopt;
}

std::optional<GainType6132::Type> Device6991::gain(uint32_t const& channelsId) const noexcept {
	if (auto resp = invokeQuery(QString("CONFigure:GAIN? %1").arg(channelsId)); resp)
		return GainType6132::fromString(*resp);
	return std::nullopt;
}

std::optional<std::vector<GainType6132::Type>> Device6991::gain() const noexcept {
	if (auto resp = invokeQuery(QString("CONFigure0:GAIN?")); resp) {
		auto list = resp->split(',');
		std::vector<GainType6132::Type> gains;
		for (auto const& str : list)
			gains.push_back(GainType6132::fromString(str));
		return gains;
	}
	return std::nullopt;
}

std::optional<PtpTime> Device6991::ptpTime() const noexcept {
	if (auto ptpTime = invokeQuery(QString("SYSTem:PTPTime?")); ptpTime)
		return PtpTime::fromString(*ptpTime);
	return std::nullopt;
}

std::optional<bool> Device6991::stopOnError() const noexcept {
	if (auto stopOnError = invokeQuery(QString("CONFigure:STOPonerr?")); stopOnError)
		return (*stopOnError).toUInt() == 1;
	return std::nullopt;
}

bool Device6991::setStopOnError(bool const stopOnError) const noexcept {
	return invokeCmd(QString("CONFigure:STOPonerr %1").arg(stopOnError ? 1 : 0));
}

std::optional<uint32_t> Device6991::scansTreshold() const noexcept {
	if (auto scanTreshold = invokeQuery(QString("CONFigure:SCAN:POST?")); scanTreshold)
		return (*scanTreshold).toUInt();
	return std::nullopt;
}

bool Device6991::setScansTreshold(uint32_t const scansTreshold) const noexcept {
	return invokeCmd(QString("CONFigure:SCAN:POST %1").arg(scansTreshold));
}

std::optional<bool> Device6991::timestamps() const noexcept {
	if (auto timestamp = invokeQuery(QString("CONFigure:SCAN:TIMestamp?")); timestamp)
		return (*timestamp).toUInt() == 1;
	return std::nullopt;
}

bool Device6991::setTimeStamps(bool const mode) const noexcept {
	return invokeCmd(QString("CONFigure:SCAN:TIMestamp %1").arg(mode ? 1 : 0));
}

std::optional<DeviceState> Device6991::channelState() const noexcept { // TODO
	return std::nullopt;
}

std::optional<Temperature> Device6991::temperature(TemperaturesEnum::Type const source) const noexcept {
	if (auto temp = invokeQuery(QString("SYSTem:TEMPerature? '%1'").arg(TemperaturesEnum::toString(source))); temp)
		return Temperature{ (*temp).toUInt(), source };//TODO check temp response format
	return std::nullopt;
}

std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> Device6991::temperatures() const noexcept {
	std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temps;
	for (auto source : TemperaturesEnum::TYPES)
		temps[source] = temperature(source);
	return temps;
}

std::optional<FansModeEnum::Type> Device6991::fansMode() const noexcept {
	if (auto fansMode = invokeQuery(QString("SYSTem:FAN?")); fansMode)
		return FansModeEnum::fromString(*fansMode);
	return std::nullopt;
}

bool Device6991::setFansMode(FansModeEnum::Type const mode) const noexcept {
	return invokeCmd(QString("SYSTem:FAN %1").arg(mode));
}

std::optional<ClockSourceEnum::Type> Device6991::clockSource() const noexcept { // TODO
	return std::nullopt;
}

void Device6991::setClockSource(ClockSourceEnum::Type const source) const noexcept { // TODO

}

std::optional<PtpTime> Device6991::ptpAlarm() const noexcept {
	if (auto ptpAlarm = invokeQuery(QString("CONFigure:TRIGger:PTP:ALARm?")); ptpAlarm) {
		auto list = (*ptpAlarm).split(',');
		return PtpTime{ list[0].toUInt(), list[1].toUInt() };
	}
	return std::nullopt;
}

bool Device6991::setPtpAlarm(PtpTime const alarmTime) const noexcept {
	return invokeCmd(QString("CONFigure:TRIGger:PTP:ALARm %1,%2").arg(alarmTime.seconds_).arg(alarmTime.nanoseconds_));
}

std::optional<uint32_t> Device6991::scansNoPerDirectReadPacket() const noexcept {
	if (auto scansPerPacket = invokeQuery(QString("CONFigure:DIRectread?")); scansPerPacket)
		return (*scansPerPacket).toUInt();
	return std::nullopt;
}

bool Device6991::setScansNoPerDirectReadPacket(uint32_t const scansNo) const noexcept {
	return invokeCmd(QString("CONFigure:DIRectread %1").arg(scansNo));
}

bool Device6991::setUserEvent() const noexcept {
	auto result = invokeCmd("SYST:USER 1");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return result;
}

QString Device6991::prepareDataFileHeader() noexcept {
	QString header;
	auto type = readFecType(FecIdType::_1);
	header += QString("Device Type: %1\n").arg(type ? FecType::toString(*type) : "read error");
	QString serialnumber = "read error";
	if (auto devInfo = invokeQuery("*IDN?"); devInfo) {
		auto list = devInfo->split(',');
		if (list.size() == 4)
			serialnumber = list[2];
	}
	header += QString("Serial Number: %1\n").arg(serialnumber);
	header += QString("IP Address: %1\n").arg(QString::fromStdString(connector_->resources().back()->value().toStdString()));
	header += QString("Scan Rate: %1\n").arg(lastConfig_.scanRate_ ? lastConfig_.scanRate_->toString() : "read error");

	if (auto channelStates = areChannelsEnabled(); channelStates) {
		header += "          TIMESTAMPS[s]         |";
		for (int i = 0; i < channelStates->size(); ++i) {
			if (channelStates->at(i)) {
				int channelId = i + 1;
				double x = 12 - QString::number(channelId).size();
				int additional = (QString::number(channelId).size() % 2 == 0) ? 0 : 1;
				header += QString("%1%2%3|").arg(QString(x / 2, ' ')).arg(channelId).arg(QString(x / 2 + additional, ' '));
			}
		}
		header += '\n';
	}
	return header;
}

QFile* Device6991::createFileForDataCollection(QString const& name) noexcept {
	auto dateTime = QDateTime::currentDateTimeUtc().toString("dd.MM.yyyy_hh-mm-ss");
	return new QFile(QString("%1_%2.csv").arg(name).arg(dateTime), this);
}

void Device6991::replaceAcqDataFile() noexcept {
	auto file = createFileForDataCollection("ACQ");
	auto header = prepareDataFileHeader();
	dataStream6111_.setHeader(header);
	dataStream6132_.setHeader(header);
	dataStream6111_.setDataFile(file);
	dataStream6132_.setDataFile(file);
	if (auto channels = areChannelsEnabled(); channels) {
		dataStream6111_.setChannelConfiguration(*channels);
		dataStream6132_.setChannelConfiguration(*channels);
	}
}

Device6991::Device6991(const QString& nameId, AbstractHardwareConnector* connector, ScpiIF* scpiIF, QObject* parent) noexcept : ScpiDevice(nameId, connector, scpiIF, parent), DeviceIdentityResourcesIF(nameId) {
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

bool Device6991::isAcquisitionActive() noexcept {
	return isAcqActive_;
}
std::optional<uint32_t> Device6991::masterControllerId() const noexcept {
	if (auto lock = invokeQuery(QString("SYSTem:LOCK?")); lock)
		return (*lock).toUInt();
	return std::nullopt;
}

void Device6991::deviceStateRequest() noexcept {
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
			st->setControllerId(masterControllerId());
			emit state(*st);
		}
	}
}

void Device6991::controllerKeyRequest() const noexcept {
	if (auto id = masterControllerId(); id)
		emit actualControllerKey(*id);
}

void Device6991::handleDeviceConfigurationReq() noexcept {
	Configuration6991 config;
	config.scanRate_ = scanRate();
	config.startMode_.mode_ = startMode();
	if (config.startMode_.mode_ == AcquisitionStartModeEnum::PTP_ALARM)
		config.startMode_.ptpAlarm_ = ptpAlarm();
	config.stopMode_.scansThreshold_ = scansTreshold();
	config.stopMode_.stopOnError_ = stopOnError();

	config.scansPerDirectReadPacket_ = scansNoPerDirectReadPacket();
	config.temperatures_ = temperatures();
	config.timestamps_ = timestamps();
	config.fansMode_ = fansMode();
	config.ptpTime_ = ptpTime();
	lastConfig_ = config;
	emit deviceConfiguration(config);
}

void Device6991::testCountersRequest() noexcept {
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
		if (cl0Running)
			result_CL0.count_ = count;
		if (cl1Running)
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

bool Device6991::writeFpgaRegister(uint32_t const address, uint32_t const data) const noexcept {
	return invokeCmd(QString("*HW:FPGA #h%1, #h%2").arg(toHex(address, 8)).arg(toHex(data, 8)));
}

bool Device6991::writeFecRegister(uint32_t const fcId, uint32_t const address, uint32_t const data) const noexcept {
	return invokeCmd(QString("*HW:FEC %1, #h%2, #h%3").arg(fcId).arg(toHex(address, 8)).arg(toHex(data, 8)));
}

bool Device6991::writeCpuRegister(uint32_t const address, uint32_t const data) const noexcept {
	return invokeCmd(QString("*HW:REG #h%1, #h%2").arg(toHex(address, 8)).arg(toHex(data, 8)));
}

std::optional<uint32_t> Device6991::readFpgaRegister(uint32_t const address) const noexcept {
	if (auto reg = invokeQuery(QString("*HW:FPGA? #h%1").arg(toHex(address, 8))); reg)
		return (*reg).toUInt(nullptr, 16);
	return std::nullopt;
}

std::optional<uint32_t> Device6991::readFecRegister(uint32_t const fcId, uint32_t const address) const noexcept {
	if (auto reg = invokeQuery(QString("*HW:FEC? %1, #h%2").arg(fcId).arg(toHex(address, 8))); reg)
		return (*reg).toUInt(nullptr, 16);
	return std::nullopt;
}

std::optional<std::pair<uint32_t, uint32_t>> Device6991::readBothFecRegisters(uint32_t const address) const noexcept {
	if (auto regs = invokeQuery(QString("*HW:FEC? 0, #h%1").arg(toHex(address, 8))); regs) {
		auto list = (*regs).split(';');
		return std::pair{ list[0].toUInt(nullptr, 16), list[1].toUInt(nullptr, 16) };
	}
	return std::nullopt;
}

bool Device6991::writeBothFecRegisters(uint32_t const address, uint32_t const data1, uint32_t const data2) const noexcept {
	return invokeCmd(QString("*HW:FEC 0, #h%1, #h%2, #h%3").arg(toHex(address, 8), toHex(data1, 8), toHex(data2, 8)));
}

std::optional<uint32_t> Device6991::readCpuRegister(uint32_t const address) const noexcept {
	if (auto reg = invokeQuery(QString("*HW:REG? #h%1").arg(toHex(address, 8))); reg)
		return (*reg).toUInt(nullptr, 16);
	return std::nullopt;
}

bool Device6991::testWithTimeout(std::function<bool()> const& functionCondition, uint32_t const timeoutInMilliseconds, uint32_t const breakTimeInMilliseconds) noexcept {
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

std::optional<FecType::Type> Device6991::readFecType(FecIdType::Type const fecId) noexcept {
	return fecRegs_[fecId].FE_ID_reg_.fecType();
}

bool Device6991::isFecIdle(FecIdType::Type fecId, FecType::Type const type) noexcept {
	return fecRegs_[fecId].BOARD_CSR_reg_.isIdle(type);
}

bool Device6991::isFecIdle(FecIdType::Type fecId) noexcept {
	auto fecType = readFecType(fecId);
	return fecType && fecRegs_[fecId].BOARD_CSR_reg_.isIdle(*fecType);
}

bool Device6991::setProperDlFrame(FecIdType::Type fecId, FecType::Type const type) noexcept {
	return DL_SPI_CSR2_reg_.setDlFrameLength(type, fecId);
}

bool Device6991::setProperDlFrame(FecIdType::Type fecId) noexcept {
	auto fecType = readFecType(fecId);
	return fecType && DL_SPI_CSR2_reg_.setDlFrameLength(*fecType, fecId);
}

void Device6991::handleSetStoreDataReq(bool const state) noexcept {
	dataStreamFifo_.setStoreData(state);
	dataStream6111_.setStoreData(state);
	dataStream6132_.setStoreData(state);
}

void Device6991::handleSetForwardDataReq(bool const state) noexcept {
	dataStream6111_.setForwardData(state);
	dataStream6132_.setForwardData(state);
}

void Device6991::handleFpgaRegisterReadReq(uint32_t const address) const noexcept {
	if (auto reg = readFpgaRegister(address); reg) {
		emit fpgaRegisterReadResp(*reg);
		emit logMsg(QString("Fpga register read successful, address:%1, data:%2").arg(toHex(address, 8), toHex(*reg, 8)));
	}
	else
		emit reportError(QString("Failed to read fpga register, address:%1").arg(toHex(address, 8)));
}

void Device6991::handleFecRegisterReadReq(FecIdType::Type const fecId, uint32_t const address) const noexcept {
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

void Device6991::handleFpgaRegisterWriteReq(uint32_t const address, uint32_t const data) const noexcept {
	if (writeFpgaRegister(address, data))
		emit logMsg(QString("Fpga register write successful, address:%1, data:%2").arg(toHex(address, 8), toHex(data, 8)));
	else
		emit reportError(QString("Failed to write fpga register, address:%1, data:%2").arg(toHex(address, 8), toHex(data, 8)));
}

void Device6991::handleFecRegisterWriteReq(FecIdType::Type const fecId, uint32_t const address, uint32_t const data) const noexcept {
	if (writeFecRegister(fecId, address, data))
		emit logMsg(QString("Fec register write successful, fecId:%1, address:%2, data:%3").arg(fecId).arg(toHex(address, 8), toHex(data, 8)));
	else
		emit reportError(QString("Failed to write fec register, fecId:%1, address:%2, data:%3").arg(fecId).arg(toHex(address, 8), toHex(data, 8)));
}

//TODO uncomment when implementation will be ready on the device
void Device6991::handleFecStateReq(FecIdType::Type const fecId) const noexcept {
	//if (auto present = BOARD_CSR1_reg_.isFecPresent(fecId); present && *present)
	emit fecEnabledResp(fecId);
	//else
	//	emit fcCardDisabled(fecId);
}

void Device6991::handleTakeControlReq(int const id) const noexcept {
	if (invokeCmd(QString("SYSTem:LOCK %1").arg(id)))
		emit controlGranted();
}

void Device6991::handleReleaseControlReq() const noexcept {
	if (invokeCmd("SYSTem:LOCK 0"))
		emit controlReleased();
}

//temporary solution TODO create and mantain only one stream
void Device6991::handleConnectDataStreamReq(int const dataStreamId) noexcept {
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
void Device6991::handleDisconnectDataStreamReq() noexcept {
	if (dataStream6111_.isConnected())
		dataStream6111_.disconnect();
	else if (dataStream6132_.isConnected())
		dataStream6132_.disconnect();
	else if (dataStreamFifo_.isConnected())
		dataStreamFifo_.disconnect();
	return;
}

void Device6991::handleConfigureDeviceReq(Configuration6991 const& config) noexcept {
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
	setUserEvent();
	handleDeviceConfigurationReq();
}

void Device6991::handleStartAcqReq() noexcept {
	showWarningAboutCommunicationWithDeviceDuringAcq();
	replaceAcqDataFile();
	if (/*auto mode = startMode(); mode && *mode == AcquisitionStartModeEnum::IMMEDIATE && */invokeCmd("MEASure:ASYNc")) {
		isAcqActive_ = true;
		emit acquisitionStarted();
	}
}

void Device6991::handleStopAcqReq() noexcept {
	if (invokeCmd("MEASure:ABORt")) {
		isAcqActive_ = false;
		emit acquisitionStopped();
	}
}

void Device6991::handleStartTestsReq(StartTestsRequest const& request) noexcept {
	handleStopTestsReq();
	//starting tests order matters, fifo test cant be run with dl test and dl test must be started before cl
	if (auto id = masterControllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
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
			}
			else if (dataStream6132_.isConnected()) {
				portToReconnectAfterFifoTest = dataStream6132_.peerPort();
				addressToRecnnectAfterFifoTest = dataStream6132_.peerAddress();
				withStreamShouldReconnectAfterFifoTest = 6132;
				dataStream6132_.disconnect();
				dataStreamFifo_.connect(addressToRecnnectAfterFifoTest.toString(), portToReconnectAfterFifoTest);
			}
			//////////////////////////////////////////////////////////////////////
			dataStreamFifo_.setDataFile(createFileForDataCollection("FIFO_TEST"));
			fifoTest_.startTest(request.fifoTestConfig_);
		}

		clTest_.startTest(request.model.at(TestTypeEnum::CL0), request.model.at(TestTypeEnum::CL1));
		emit testsStarted();
		invokeCmd("SYSTem:LOCK 0");
	}
}

void Device6991::handleStopTestsReq() noexcept {
	//stopping tests order matters, cl test must be stopped first
	if (auto id = masterControllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
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

void Device6991::handleSetFiltersReq(FilterType6132::Type const filter, std::vector<uint32_t> const& channelIds) noexcept {
	setFilter(filter, channelIds);
}

void Device6991::handleSetGainsReq(GainType6132::Type const gain, std::vector<uint32_t> const& channelIds) noexcept {
	setGain(gain, channelIds);
}

void Device6991::handleSetChannelsEnabledReq(bool const state, std::vector<uint32_t> const& channelIds) noexcept {
	enableChannals(channelIds, state);
}

void Device6991::handleChannelsConfigurationReq() noexcept {
	ChannelsConfiguration configuration;
	if (auto const& [fc1, fc2] = std::pair{ readFecType(FecIdType::_1), readFecType(FecIdType::_2) }; (fc1 && *fc1 == FecType::_6132) || (fc2 && *fc2 == FecType::_6132)) {
		configuration.gains_ = gain();
		configuration.filters_ = filter();
	}
	configuration.states_ = areChannelsEnabled();
	emit channelConfiguration(configuration);
}

void Device6991::handleIdChanged(uint32_t const id) noexcept {
	controllerId_ = id;
}
