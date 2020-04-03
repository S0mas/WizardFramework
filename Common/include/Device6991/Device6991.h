#pragma once
#include "../ScpiDevice.h"
#include "../ChannelsIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "../DeviceIdentityResourcesIF.h"
#include "Defines6991.h"
#include <QTcpSocket>

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
	QString readError() const noexcept {
		scpiCmd("SYSTem:ERR?");
		return readResponse();
	}

	bool succeeded() const noexcept {
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
		return status;
	}

	std::optional<DeviceState> status() const noexcept {
		return invokeCmd(QString("SYSTem::STATe?")) ? std::optional{extractStatus(readResponse())} : std::nullopt;
	}

	std::optional<ScanRateModel> scanRate() const noexcept {
		if (invokeCmd(QString("CONFigure:SCAN:RATe?"))) {
			auto list = readResponse().split(',');
			return std::optional(ScanRateModel{ list[0].toInt(), ScanRateUnitsEnum::fromString(list[1]) });
		}
		return std::nullopt;
	}

	void setScanRate(ScanRateModel const scanRate) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:RATe %1,%2").arg(scanRate.value_).arg(ScanRateUnitsEnum::toString(scanRate.units_)));
	}

	std::optional<AcquisitionStartModeEnum::Type> startMode() const noexcept {
		return invokeCmd(QString("CONFigure:TRIGger:STARt?")) ? std::optional(AcquisitionStartModeEnum::fromString(readResponse())) : std::nullopt;
	}

	void setStartMode(AcquisitionStartModeEnum::Type const mode) const noexcept {
		invokeCmd(QString("CONFigure:TRIGger:STARt %1").arg(AcquisitionStartModeEnum::toString(mode)));
	}

	std::optional<PtpTime> ptpTime() const noexcept {
		if (invokeCmd(QString("SYSTem:PTPTime?"))) {
			auto list = readResponse().split(',');
			return std::optional(PtpTime{ list[0].toInt(), list[1].toInt() });
		}
		return std::nullopt;
	}

	void setPtpTime(PtpTime const time) const noexcept {
		invokeCmd(QString("TRIGger:PTPTime %1,%2").arg(time.seconds_).arg(time.nanoseconds_));
	}

	std::optional<bool> stopOnError() const noexcept {
		return invokeCmd(QString("CONFigure:STOPonerr?")) ? std::optional{readResponse().toInt() == 1} : std::nullopt;
	}

	void setStopOnError(bool const stopOnError) const noexcept {
		invokeCmd(QString("CONFigure:STOPonerr %1").arg(stopOnError ? "ON" : "OFF"));
	}

	std::optional<uint32_t> scansTreshold() const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:POST?")) ? std::optional{readResponse().toUInt()} : std::nullopt;
	}

	void setScansTreshold(int const scansTreshold) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:POST %1").arg(scansTreshold));
	}

	std::optional<bool> timestamps() const noexcept {
		return invokeCmd(QString("CONFigure:SCAN:TIMestamp?")) ? std::optional{readResponse().toInt() == 1} : std::nullopt;
	}

	void setTimeStamps(bool const mode) const noexcept {
		invokeCmd(QString("CONFigure:SCAN:TIMestamp %1").arg(mode ? "ON" : "OFF"));
	}

	std::optional<DeviceState> channelState() const noexcept { // TODO
		return std::nullopt;
	}

	std::optional<Temperature> temperature(TemperaturesEnum::Type const source) const noexcept {
		return invokeCmd(QString("SYSTem:TEMPerature? %1").arg(TemperaturesEnum::toString(source))) ? std::optional{Temperature{readResponse().toInt(), source}} : std::nullopt;
	}

	std::optional<std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()>> temperatures() const noexcept {
		if (invokeCmd(QString("SYSTem:TEMPerature?"))) {
			std::array<std::optional<Temperature>, TemperaturesEnum::TYPES.size()> temps;
			for (auto source : TemperaturesEnum::TYPES)
				temps[source] = temperature(source);	
		}
		return std::nullopt;
	}

	std::optional<FansModeEnum::Type> fansMode() const noexcept {
		return invokeCmd(QString("SYSTem:FAN?")) ? std::optional{FansModeEnum::fromString(readResponse())} : std::nullopt;
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
			auto list = readResponse().split(',');
			return std::optional(PtpTime{ list[0].toInt(), list[1].toInt() });
		}
		return std::nullopt;
	}

	void setPtpAlarm(PtpTime const alarmTime) const noexcept {
		invokeCmd(QString("TRIGger:PTP:ALARm %1,%2").arg(alarmTime.seconds_).arg(alarmTime.nanoseconds_));
	}

	std::optional<int> scansNoPerDirectReadPacket() const noexcept { 
		return invokeCmd(QString("CONFigure:DIRectread?")) ? std::optional{ readResponse().toInt() } : std::nullopt;
	}

	void setScansNoPerDirectReadPacket(int const scansNo) const noexcept {
		invokeCmd(QString("CONFigure:DIRectread %1").arg(scansNo));
	}

	bool openSocketConnection(QString const& addresss, int const port) noexcept {
		tcpSocket_.abort();
		qDebug() << "Connecting: " << addresss << ":" << port;
		tcpSocket_.connectToHost(addresss, port);
		QObject::connect(&tcpSocket_, &QIODevice::readyRead, this, &Device6991::readData);
		QObject::connect(&tcpSocket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Device6991::displayError);
		return tcpSocket_.isOpen();
	}

	bool closeSocketConnection() noexcept {
		tcpSocket_.abort();
		return !tcpSocket_.isOpen();
	}
private slots:
	void displayError(QAbstractSocket::SocketError socketError) const noexcept {
		qDebug() << "Error: " << socketError;
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
			return std::optional<bool>{opt->state() == DeviceStateEnum::ACQUISITION};
		return std::nullopt;
	}
public slots:
	void connectDataStreamRequest(int const port) {
		auto ipResource = inputResources().back();
		bool status = openSocketConnection({ ipResource->load() }, port);
		if (status)
			emit connectedDataStream();
	}

	void disconnectDataStreamRequest(int const port) {
		bool status = closeSocketConnection();
		if (status)
			emit disconnectedDataStream();
	}

	void deviceStateRequest() const noexcept {
		if (invokeCmd(QString("SYSTem::STATe?")))
			emit state(extractStatus(readResponse()));
	}

	void releaseControlRequest() const noexcept {
		if (invokeCmd(QString("SYSTem::LOCK 0")))
			emit controlReleased();
	}

	void startAcquisitionRequest() const noexcept {
		if(invokeCmd(QString("MEASure::ASYNc")))
			emit acquisitionStarted();
	}

	void stopAcquisitionRequest() const noexcept {
		if (invokeCmd(QString("MEASure::ABORt")))
			emit acquisitionStopped();
	}

	void takeControlRequest(int const id) const noexcept {
		if (invokeCmd(QString("SYSTem::LOCK %1").arg(id)))
			emit controlGranted();
	}

	void controllerKeyRequest() const noexcept {
		if(invokeCmd(QString("SYSTem::LOCK?")))
			emit actualControllerKey(readResponse().toInt());
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
		if (config.ptpTime_)
			setPtpTime(*config.ptpTime_);
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
		//configurationDataRequest();
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
};
