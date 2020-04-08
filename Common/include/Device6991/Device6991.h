#pragma once
#include "../ScpiDevice.h"
#include "../ChannelsIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "Defines6991.h"
#include "Registers6991.h"
#include <QTcpSocket>
#include <QDataStream>

#include <array>
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
	CL_SPI_CSR_reg clSpiCsrReg_{ this };
	DL_SPI_CSR1_reg dlSpiCsrReg_{ this };
	DFIFO_CSR_reg dFifoCsrReg_{ this };
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
			qDebug() << error;
			emit reportError(error);
			return false;
		}
		return true;
	}

	bool invokeCmd(const QString& cmd) const noexcept {
		scpiCmd(cmd);
		qDebug() << cmd;
		return succeeded();
	}

	DeviceState extractStatus(QString const& statusData) const noexcept {
		DeviceState status;
		//TODO EXTRACT STATUS
		return status;
	}

	std::optional<DeviceState> status() const noexcept {
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
		if (invokeCmd(QString("TRIGger:PTP:ALARm?"))) {
			auto list = response_.split(',');
			return std::optional(PtpTime{ list[0].toInt(), list[1].toInt() });
		}
		return std::nullopt;
	}

	void setPtpAlarm(PtpTime const alarmTime) const noexcept {
		invokeCmd(QString("TRIGger:PTP:ALARm %1,%2").arg(alarmTime.seconds_).arg(alarmTime.nanoseconds_));
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
private slots:
	void displayError(QAbstractSocket::SocketError socketError) noexcept {
		qDebug() << "Error: " << socketError;
		closeSocketConnection();
	}
	void readData() noexcept {
		in_.startTransaction();
		QString data;
		in_ >> data;
		if (!in_.commitTransaction())
			return;
		qDebug() << "I read: " << data;
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
	~Device6991() override = default;

	void saveSubtype(const QString& str) const override {}
	void saveSerialnumber(const QString& str) const override {}
	void saveFirmwareRevision(const QString& str) const override {}
	void saveDriverRevision(const QString& str) const override {}
	QString loadSubtype() const override { return ""; }
	QString loadSerialnumber() const override { return ""; }
	QString loadFirmwareRevision() const override { return ""; }
	QString loadDriverRevision() const override { return ""; }

	std::optional<bool> isAcquisitionActive() const noexcept {
		if (auto opt = status(); opt)
			return std::optional{opt->state() == DeviceStateEnum::ACQUISITION};
		return std::nullopt;
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

	void deviceStateRequest() const noexcept {
		if (invokeCmd("SYSTem:STATe?"))
			emit state(extractStatus(response_));
	}

	void releaseControlRequest() const noexcept {
		if (invokeCmd("SYSTem:LOCK 0"))
			emit controlReleased();
	}

	void startAcquisitionRequest() const noexcept {
		if(invokeCmd("MEASure:ASYNc"))
			emit acquisitionStarted();
	}

	void stopAcquisitionRequest() const noexcept {
		if (invokeCmd("MEASure:ABORt"))
			emit acquisitionStopped();
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
			setStartMode(*config.startMode_.mode_);
			if(*config.startMode_.mode_ == AcquisitionStartModeEnum::PTP_ALARM && config.startMode_.ptpAlarm_)
				setPtpAlarm(*config.startMode_.ptpAlarm_);
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
	// TODO START ONLY SELECTED TESTS, ADD STARTING DL AND FIFO TESTS
	void startTestsRequest(TestsSelectionModel const& selectioModel) noexcept {
		if (auto id = controllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
			if (invokeCmd("*DBG 1")) {
				if (auto isTestRunning = clSpiCsrReg_.isTestRunning(); isTestRunning) {
					if (!*isTestRunning && clSpiCsrReg_.startTests(CL_SPI_CSR_reg::BOTH))
						emit testsStarted();
				}
				invokeCmd("*DBG 0");
			}
			else {
				invokeCmd("SYSTem:LOCK 0");
			}
		}
	}

	// TODO ADD STOPPING DL AND FIFO TESTS
	void stopTestsRequest() noexcept {
		if (auto id = controllerId(); id && *id == 80 || invokeCmd(QString("SYSTem:LOCK %1").arg(80))) {
			if (invokeCmd("*DBG 1")) {
				if (auto isTestRunning = clSpiCsrReg_.isTestRunning(); isTestRunning) {
					if (*isTestRunning && clSpiCsrReg_.stopTests())
						emit testsStopped();
				}		
				invokeCmd("*DBG 0");
			}
		}
		else
			invokeCmd("SYSTem:LOCK 0");
	}

	void testCountersRequest() const noexcept {
		TestsResultModel result;
		//todo read from count registers
		//CL0_SPI_TLCNT_reg - executed tests for CL0
		//CL1_SPI_TLCNT_reg - executed tests for CL1
		//CL0_SPI_TLERR_reg - errors for CL0
		//CL1_SPI_TLERR_reg - errors for CL1
		//DL0_SPI_TLCNT_reg - executed tests for DL0
		//DL1_SPI_TLCNT_reg - executed tests for DL1
		//DL0_SPI_TLERR_reg - errors for DL0
		//DL1_SPI_TLERR_reg - errors for DL1
		//DFIFO DFIFO_DATA  - executed tests for FIFO
		// ?                - errors for FIFO

		emit testCounters(result);
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
	void testCounters(TestsResultModel const& resultModel) const;
	void fcCardEnabled(int const fcId) const;
	void fcCardDisabled(int const fcId) const;
};
