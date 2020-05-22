#include "../include/Device3416.h"
#include "../../../bu3416/include/bu3416.h"
#include "../../../visa/include/visa.h"
#include "../include/HardwareConnector3416.h"

Device3416::Device3416(const QString& nameId, const QString& motherBoardNameId, QObject* parent) noexcept : AbstractDevice(nameId, new HardwareConnector3416(nameId, motherBoardNameId), parent), DeviceIdentityResourcesIF(nameId), ChannelsIF(16) {}

void Device3416::saveSubtype(const QString& str) const {
	//todo
}

void Device3416::saveSerialnumber(const QString& str) const {
	//todo
}

void Device3416::saveFirmwareRevision(const QString& str) const {
	//todo
}

void Device3416::saveDriverRevision(const QString& str) const {
	//todo
}

QString Device3416::loadSubtype() const {
	char subtype[2];
	invokeFunction(bu3416_getSubType, "bu3416_getSubType", subtype);
	return subtype;
}

QString Device3416::loadSerialnumber() const {
	ViInt32 serialnumber = 0xFFFFFFFF;
	invokeFunction(bu3416_getSerNum, "bu3416_getSerNum", &serialnumber);
	return QString::number(serialnumber, 16);
}

QString Device3416::loadFirmwareRevision() const {
	char firmwareRev[256];
	invokeFunction(bu3416_revision_query, "bu3416_revision_query", nullptr, firmwareRev);
	return firmwareRev;
}

QString Device3416::loadDriverRevision() const {
	char driverRev[256];
	invokeFunction(bu3416_revision_query, "bu3416_revision_query", driverRev, nullptr);
	return driverRev;
}

const ViSession& Device3416::refereceToViSession() const noexcept {
	return dynamic_cast<HardwareConnector3416*>(connector_)->refereceToViSession();
}

std::vector<double> Device3416::measureChannels(const unsigned channelMask, const int gain, const double scanRate, const uint64_t scanCount) const {
	invokeFunction(bu3416_configureChannels, "bu3416_configureChannels", 0xffff, bu3416_CH_OFF, gain, false);
	invokeFunction(bu3416_configureChannels, "bu3416_configureChannels", channelMask, bu3416_CH_FP, gain, false);
	auto enabledChannelsCount = 0;
	for (auto const& channel : channels())
		if (channel.enabled()) ++enabledChannelsCount;

	std::vector<ViReal64> wave;
	wave.resize(scanCount * enabledChannelsCount, 0);
	invokeFunction(bu3416_acquireWaveforms, "bu3416_acquireWaveforms", scanRate, scanCount, bu3416_GROUP_BY_CHANNEL, wave.data(), nullptr);

	std::vector<ViReal64> averageValues;
	averageValues.resize(channels().size(), 0);
	int currentScanId = 0;
	for (auto& channel : channels()) {
		if (channel.disabled())
			continue;
		ViReal64 min = wave[currentScanId];
		ViReal64 max = min;
		ViReal64 sumForOneChannel = 0;
		for (uint64_t scanNo = 0; scanNo < scanCount; scanNo++) {
			ViReal64 value = wave[currentScanId];
			if (value < min)
				min = value;
			if (value > max)
				max = value;
			sumForOneChannel += value;
			++currentScanId;
		}
		averageValues[channel.index() - 1] = sumForOneChannel / scanCount;
		if (max - min > 0.1)
			logMsg(QString("WARNING: Difference between min and max exceeds margin (0.1) : %1").arg(max - min));
	}
	return averageValues;
}

double Device3416::measureChannel(const int channelIndex, const int gain, const uint64_t scanRate, const uint64_t scanCount) const {
	return measureChannels(1 << (channelIndex - 1), gain, scanRate, scanCount)[channelIndex - 1ll];
};