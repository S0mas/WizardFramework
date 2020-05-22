#include "../include/AbstractTest6716.h"
#include "../../../bu6100/include/bu6100.h"

inline constexpr int hexFormat = 16;
inline constexpr int decFormat = 10;
inline constexpr int binFormat = 2;

bool AbstractTest6716::checkValue(const unsigned short expected, const unsigned short actual, const int form, const int width, const char fill) const noexcept {
	const auto& status = expected == actual;
	auto str = QString("%1: Expected: %2, Actual: %3").arg(status ? "INF": "ERR").arg(QString::number(expected, form), width, fill).arg(QString::number(actual, form), width, fill);
	log(str);
	return status;
}

bool AbstractTest6716::checkId() const {
	const auto& read1 = device6716->readFPGAreg(bu6716_FPGA_ID);
	const auto& read2 = device6716->readFPGAreg(bu6716_FPGA_ID + 1);
	return checkValue(0x57, read1, hexFormat) && checkValue(0x16, read2, hexFormat);
}

bool AbstractTest6716::testWriteRead(const unsigned char expected, const unsigned char address) const {
	device6716->writeFPGAreg(address, expected);
	const auto& read = device6716->readFPGAreg(address);
	return checkValue(expected, read, hexFormat);
}

void AbstractTest6716::preTestSetUp() const {
	device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
	deviceT028->invokeFunction(t028_reset, "t028_reset");
	device6716->invokeFunction(bu6716_reset, "bu6716_reset");
	bu3100_sleep(250);
	device6716->invokeFunction(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	bu3100_sleep(250);

	if (needToReloadExcitationCalibDataFromTestApplication) {
		auto calibDataIt = tempExcitationCalibrationData_.begin();
		for (auto const& channel : device6716->channels()) {
			device6716->invokeFunction(bu6716_setExcCalibCoeff, "bu6716_setExcCalibCoeff", channel.mask(), calibDataIt->positiveOffset, calibDataIt->negativeOffset, calibDataIt->positiveGain, calibDataIt->negativeGain);
			calibDataIt = std::next(calibDataIt);
		}
	}

	if (needToReloadSignalPathCalibDataFromTestApplication) {
		auto calibDataIt = tempSignalPathCalibrationData_.begin();
		for (auto const& channel : device6716->channels()) {
			for (int i = 0; i < calibDataIt->offsets.size(); ++i)
				device6716->invokeFunction(bu6716_setSignalPathCalibCoeff, "bu6716_setSignalPathCalibCoeff", channel.mask(), std::pow(10, i), calibDataIt->offsets[i], calibDataIt->gainCoefficients[i]);
			calibDataIt = std::next(calibDataIt);
		}
	}
}

void AbstractTest6716::postTestCleanUp() const {
	device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", 0.0);
	deviceT028->invokeFunction(t028_reset, "t028_reset");
}

double AbstractTest6716::setAndGetVrefVoltage(const double voltage) const {
	double output = 0;
	Limit voltageReferenceLimit = { "Voltage reference limit", voltage * (98 / 100),  voltage * (102 / 100) };
	device6100->invokeFunction(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", voltage);
	bu3100_sleep(100);
	device6100->invokeFunction(bu6100_getVoltRefOutput, "bu6100_getVoltRefOutput", &output);
	if (voltageReferenceLimit.checkValue(output))
		log(voltageReferenceLimit.lastStatusMsg());
	return output;
}

Result AbstractTest6716::excitationTest(const Limit& limit1, const Limit& limit2, const bool positive = true) const {
	// 1
	device6716->invokeFunction(bu6716_setChannelConf, "bu6716_setChannelConf", device6716->channelsStateAsMask(), bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	device6716->invokeFunction(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", positive ? bu6716_EXCMON_POSITIVE : bu6716_EXCMON_NEGATIVE);

	// 2
	deviceT028->invokeFunction(t028_setChannelsConfig, "t028_setChannelsConfig", device6716->channelsStateAsMask(), T028_MODE_CLIMIT);

	// 3
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), positive ? 6 : 0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), positive ? 0 : -6);
	bu3100_sleep(50);

	// 4
	auto values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		channelsResults.at(channel.index()) = Result::VALUE::PASSED;
		if (!limit1.checkValue(values[channel.index() - 1ll]))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit1.lastStatusMsg());
	}

	// 5
	auto status = true;
	ViInt16 excl_status[bu6716_NUM_CHAN];
	device6716->invokeFunction(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", device6716->channelsStateAsMask(), nullptr, excl_status);
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		if (excl_status[channel.index() - 1] == bu6716_EXC_CURRENT_LIMIT_DETECTED) {
			log(QString("CH_%1: Excitation current limit condition detected").arg(channel.index()));
			status = false;
		}
	}

	// 6
	device6716->invokeFunction(bu6716_setPosExcitation, "bu6716_setPosExcitation", device6716->channelsStateAsMask(), positive ? 8 : 0);
	device6716->invokeFunction(bu6716_setNegExcitation, "bu6716_setNegExcitation", device6716->channelsStateAsMask(), positive ? 0 : -8);
	bu3100_sleep(50);

	// 7
	values = device3416_6716->measureChannels(device6716->channelsStateAsMask());
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		if (!limit2.checkValue(values[channel.index() - 1ll]))
			channelsResults.at(channel.index()) = Result::VALUE::FAILED;
		log(limit2.lastStatusMsg());
	}

	// 8
	device6716->invokeFunction(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", device6716->channelsStateAsMask(), nullptr, excl_status);
	for (auto& channel : device6716->channels()) {
		if (channel.disabled())
			continue;
		if (excl_status[channel.index() - 1] == bu6716_EXC_CURRENT_LIMIT_DETECTED) {
			log(QString("OK: Channel %1 Excitation current limit condition detected").arg(channel.index()));
			status = false;
		}
	}

	return channelsResult();
}

void AbstractTest6716::setChannelsStates(const std::vector<bool>& states) noexcept {
	device6716->setChannelsStates(states);
}

AbstractTest6716::AbstractTest6716(const QString&& name) : AbstractChannelsTest(name, 16) {}