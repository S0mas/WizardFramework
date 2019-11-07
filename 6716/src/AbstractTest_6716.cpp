#include "../include/AbstractTest_6716.h"
#include <bu6100.h>

bool Abstract6716Test::checkValue(const unsigned short expected, const unsigned short actual, const int form, const int width, const char fill) const noexcept {
	const auto& status = expected == actual;
	auto str = QString(" Expected: %1, Actual: %2\n").arg(QString::number(expected, form), width, fill).arg(QString::number(actual, form), width, fill);
	status ? str.prepend("Success:") : str.prepend("Error:");
	log(str);
	return status;
}

bool Abstract6716Test::checkId() const {
	const auto& read1 = connection->readFPGAreg(bu6716_FPGA_ID);
	const auto& read2 = connection->readFPGAreg(bu6716_FPGA_ID + 1);
	return checkValue(0x57, read1, hexFormat) && checkValue(0x16, read2, hexFormat);
}

bool Abstract6716Test::testWriteRead(const unsigned char expected, const unsigned char address) const {
	connection->writeFPGAreg(address, expected);
	const auto& read = connection->readFPGAreg(address);
	return checkValue(expected, read, hexFormat);
}

std::vector<ViReal64> Abstract6716Test::readValues(ViInt16 gain, const ViUInt16 channelMask, const ViReal64 warnMargin, const ViReal64 scanRate, const uint64_t scanCount) const {
	uint64_t n = 0;
	std::vector<ViReal64> wave;
	std::vector<ViReal64> averageValues;
	averageValues.resize(bu3416_NUM_CHAN);
	uint64_t channels = bu3416_NUM_CHAN;
	wave.resize(scanCount * channels);
	connection->callAndThrowOnError3416(bu3416_configureChannels, "bu3416_configureChannels", 0xffff, bu3416_CH_OFF, gain, false);
	connection->callAndThrowOnError3416(bu3416_configureChannels, "bu3416_configureChannels", channelMask, bu3416_CH_FP, gain, false);
	connection->callAndThrowOnError3416(bu3416_acquireWaveforms, "bu3416_acquireWaveforms", scanRate, scanCount, bu3416_GROUP_BY_CHANNEL, wave.data(), nullptr);

	for (int j = 0; j < bu3416_NUM_CHAN; j++) {
		if (channelMask & (1 << j)) {
			ViReal64 min = wave[n * channels];
			ViReal64 max = wave[n * channels];
			for (uint64_t i = 1; i < scanCount; i++) {
				ViReal64 value = wave[i + n * scanCount];
				if (value < min)
					min = value;
				if (value > max)
					max = value;
				averageValues[j] += value;
			}
			averageValues[j] /= scanCount;
			n++;
			if (max - min > warnMargin)
				log(QString("WARNING: channel_%1: Difference between min and max exceeds margin\n\t(min=%2, max=%3, average=%4, max-min=%5, margin=%6)\n").arg(j + 1).arg(min).arg(max).arg(averageValues[j]).arg(max - min).arg(warnMargin));
		}
	}
	return averageValues;
}

ViReal64 Abstract6716Test::readValue_oneChannel(const ViInt16 gain, const ViUInt16 channel, const ViReal64 warnMargin, const ViReal64 scanRate, const uint64_t scanCount) const {
	return readValues(gain, 1 << (channel - 1), warnMargin, scanRate, scanCount)[channel-1];
}

unsigned char Abstract6716Test::readDSR() const {
	connection->sendCmd("*DSR?\n");
	return connection->readResponse().toUInt();
}

std::vector<ViReal64> Abstract6716Test::getAutoDAC(ViUInt16 channelMask, ViInt16 select) const {
	std::vector<ViReal64> voltages;
	for (int i = 0; i < bu6716_NUM_CHAN; i++) {
		if (channelMask & (1 << i)) {
			select == AUTO_DAC_POSITIVE ?	connection->sendCmd("input%d:autodac:positive?\n", i+1) : connection->sendCmd("input%d:autodac:negative?\n", i + 1);
			voltages.push_back(connection->readResponse().toUInt());
		}
	}
	return voltages;
}

void Abstract6716Test::preTestSetUp() const {
	connection->callAndThrowOnError6716(bu6716_reset, "bu6716_reset");
}

void Abstract6716Test::postTestCleanUp() const {
	//connection->callAndThrowOnError6716(bu6716_reset, "bu6716_reset");
}

std::vector<ViReal64> Abstract6716Test::getAutoDACPositive(const ViUInt16 channelMask) const {
	return getAutoDAC(channelMask, AUTO_DAC_POSITIVE);
}

std::vector<ViReal64> Abstract6716Test::getAutoDACNegative(const ViUInt16 channelMask) const {
	return getAutoDAC(channelMask, AUTO_DAC_NEGATIVE);
}

void Abstract6716Test::setAutoDAC(const ViUInt16 channelMask, const ViReal64 voltage, const  ViInt16 select) const {
	for (int i = 0; i < bu6716_NUM_CHAN; i++)
		if (channelMask & (1 << i))
			select == AUTO_DAC_POSITIVE ? connection->sendCmd("input%d:autodac:positive %lf", i + 1, voltage) : connection->sendCmd("input%d:autodac:negative %lf", i + 1, voltage);
}

void Abstract6716Test::setAutoDACPositive(const ViUInt16 channelMask, const ViReal64 voltage) const {
	setAutoDAC(channelMask, voltage, AUTO_DAC_POSITIVE);
}

void Abstract6716Test::setAutoDACNegative(const ViUInt16 channelMask, const ViReal64 voltage) const {
	setAutoDAC(channelMask, voltage, AUTO_DAC_NEGATIVE);
}

ViUInt16 Abstract6716Test::checkValues(const ViUInt16 channelMask, const std::vector<ViReal64>& values, const std::string& limitName, const ViReal64 expected, const ViReal64 marginNeg, const ViReal64 marginPos, ViBoolean verbose, ViBoolean checkGains) const noexcept {
	ViUInt16 errorMask = 0;
	if (verbose) {
		log(QString("Checking value. Limit: %1, expecting %2 (range: %3 .. %4): ").arg(limitName.c_str()).arg(expected).arg(expected - marginNeg).arg(expected + marginPos));
	}
	for (int i = 0; i < bu3416_NUM_CHAN; i++) {
		if (!(channelMask & (1 << i)))
			continue;
		if (values[i] - expected > marginPos) {
			if (errorMask == 0)
				log("\n");
			errorMask |= (1 << i);
			log(QString("ERROR (%1): channel_%2: Value exceeds positive margin (expected=%3, measured=%4, marginPos=%5)\n").arg(limitName.c_str()).arg(i + 1).arg(expected).arg(values[i]).arg(marginPos));
		}
		if (expected - values[i] > marginNeg) {
			if (errorMask == 0)
				log("\n");
			errorMask |= (1 << i);
			log(QString("ERROR (%1): channel_%2: Value exceeds negative margin (expected=%3, measured=%4, marginNeg=%5)\n").arg(limitName.c_str()).arg(i + 1, expected).arg(values[i]).arg(marginNeg));
		}
	}
	if (verbose) {
		if (errorMask == 0)
			log("OK\n");
		for (int i = 0; i < bu3416_NUM_CHAN; i++) {
			if (!(channelMask & (1 << i)))
				continue;
			log(QString("Channel_%1 =\t%2  %3\n").arg(i + 1).arg(values[i]).arg(errorMask & (1 << i) ? "[ERROR]" : ""));
		}
		log("\n");
	}
	return errorMask;
}

ViUInt16 Abstract6716Test::checkValue_oneChannel(const ViUInt16 channel, const ViReal64 value, const std::string& limitName, const ViReal64 expected, const ViReal64 marginNeg, const ViReal64 marginPos, ViBoolean verbose, ViBoolean checkGains) const noexcept {
	std::vector<ViReal64> values;
	values.resize(bu3416_NUM_CHAN);
	values[channel - 1] = value;
	return checkValues(1 << (channel - 1), values, limitName, expected, marginNeg, marginPos, verbose, checkGains);
}

bool Abstract6716Test::excitCalibration(ViInt16 channel, ViReal64* offsetPos, ViReal64* offsetNeg, ViReal64* gainPos, ViReal64* gainNeg, ViBoolean store) const {
	const double CALIB_EXC_PV = 9.0;
	const double CALIB_EXC_NV = -9.0;
	int errorDetected = 0;
	ViReal64 excitationVoltagePos = 0;
	ViReal64 excitationVoltageNeg = 0;
	ViInt16 excMonitorMode = 0;
	ViInt16 mode = 0;
	if (channel < 1 || channel > bu6716_NUM_CHAN)
		return false;
	ViUInt16 channelMask = (1 << (channel - 1));  // One channel at a time!
												  // Store current settings for the channel
	connection->callAndThrowOnError6716(bu6716_getMode, "bu6716_getMode", channelMask, &mode);
	connection->callAndThrowOnError6716(bu6716_getExcitationMonitor, "bu6716_getExcitationMonitor", &excMonitorMode);
	connection->callAndThrowOnError6716(bu6716_getPosExcitation, "bu6716_getPosExcitation", channelMask, &excitationVoltagePos);
	connection->callAndThrowOnError6716(bu6716_getNegExcitation, "bu6716_getNegExcitation", channelMask, &excitationVoltageNeg);
	// Set default calib (offsets = 0, gains = 1)
	connection->callAndThrowOnError6716(bu6716_resetExcCalibCoeff, "bu6716_resetExcCalibCoeff", channelMask, false);
	// Do the calibration
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, bu6716_MODE_DI);
	bu3100_sleep(100);
	// - offset.
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 0.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, 0.0);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_POSITIVE);

	bu3100_sleep(100);
	auto readValOffPos = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
	errorDetected |= checkValue_oneChannel(channel, readValOffPos, "L1101", 0.0, 0.050, 0.050, false);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_NEGATIVE);
	bu3100_sleep(100);
	auto readValOffNeg = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
	errorDetected |= checkValue_oneChannel(channel, readValOffNeg, "L1101", 0.0, 0.050, 0.050, false);
	// - gain
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, CALIB_EXC_PV);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, CALIB_EXC_NV);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_POSITIVE);
	bu3100_sleep(100);
	auto readValPos = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
	readValPos -= readValOffPos;
	errorDetected |= checkValue_oneChannel(channel, readValPos, "L1102", 9.0, 0.2, 0.2, false);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_NEGATIVE);
	bu3100_sleep(100);
	auto readValNeg = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
	readValNeg -= readValOffNeg;
	errorDetected |= checkValue_oneChannel(channel, readValNeg, "L1103", -9.0, 0.2, 0.2, false);
	*offsetPos = -readValOffPos;
	*offsetNeg = -readValOffNeg;
	*gainPos = CALIB_EXC_PV / readValPos;
	*gainNeg = CALIB_EXC_NV / readValNeg;
	// Set calculated coefficients
	connection->callAndThrowOnError6716(bu6716_setExcCalibCoeff, "bu6716_setExcCalibCoeff", channelMask, *offsetPos, *offsetNeg, *gainPos, *gainNeg);
	if (store && errorDetected == 0) {
		connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 0.0);
		connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, 0.0);
		connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_POSITIVE);
		bu3100_sleep(100);
		auto value = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
		errorDetected |= checkValue_oneChannel(channel, value, "L2201", 0.0, 0.001, 0.001, false);
		connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 9.99);
		connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_POSITIVE);
		bu3100_sleep(100);
		value = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
		errorDetected |= checkValue_oneChannel(channel, value, "L2202", 9.99, 0.003, 0.003, false);
		connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, -9.99);
		connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_NEGATIVE);
		bu3100_sleep(100);
		value = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
		errorDetected |= checkValue_oneChannel(channel, value, "L2203", -9.99, 0.003, 0.003, false);
		if (errorDetected == 0) {
			connection->callAndThrowOnError6716(bu6716_storeExcCalibCoeff, "bu6716_storeExcCalibCoeff", channelMask);
			connection->callAndThrowOnError6716(bu6716_resetExcCalibCoeff, "bu6716_resetExcCalibCoeff", channelMask, true);
		}
	}
	// Restore settings
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, excitationVoltagePos);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, excitationVoltageNeg);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", excMonitorMode);
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, mode);
	return errorDetected == 0;
}

bool Abstract6716Test::excitationTest(const bool positivePolarization) const {
	ViReal64 positiveExcitationMultipli;
	ViReal64 negativeExcitationMultipli;
	ViInt16 monitorMode;
	if (positivePolarization) {
		positiveExcitationMultipli = 1;
		negativeExcitationMultipli = 0;
		monitorMode = bu6716_EXCMON_POSITIVE;
	}
	else {
		positiveExcitationMultipli = 0;
		negativeExcitationMultipli = -1;
		monitorMode = bu6716_EXCMON_NEGATIVE;
	}
		connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
		// 1
		connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
		connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", monitorMode);
		// 2
		connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_CLIMIT);
		// 3
		ViReal64 excitationPos = 6.0 * positiveExcitationMultipli;
		ViReal64 excitationNeg = 6.0 * negativeExcitationMultipli;
		connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, excitationPos);
		connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, excitationNeg);
		bu3100_sleep(50);
		// 4
		std::string limit;
		double margin = 0.01;
		if (positivePolarization)
			limit = "L1401";
		else
			limit = "L1501";
		auto errorDetected = checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), limit, positivePolarization ? excitationPos : excitationNeg, margin, margin);
		// 5
		ViInt16 excl_status[bu3416_NUM_CHAN];
		connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", CHANNEL_MASK, nullptr, excl_status);
		for (int i = 0, n = 0; i < bu6716_NUM_CHAN; i++) {
			if (CHANNEL_MASK & (1 << i)) {
				if (excl_status[n] == bu6716_EXC_CURRENT_LIMIT_DETECTED) {
					log(QString("CH_%1: Excitation current limit condition detected\n").arg(i + 1));
					errorDetected |= 1 << i;
				}
				n++;
			}
		}
		// 6
		excitationPos = 8.0 * positiveExcitationMultipli;
		excitationNeg = 8.0 * negativeExcitationMultipli;
		connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, excitationPos);
		connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, excitationNeg);
		bu3100_sleep(50);
		// 7
		double marinP = 0;
		double marinN = 0;
		double expected = 0;
		if (positivePolarization) {
			limit = "L1402";
			marinP = 1.2;
			expected = 7.2;
		}
		else {
			limit = "L1502";
			marinN = 1.2;
			expected = -7.2;
		}
		errorDetected |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), limit, expected, marinP, marinN);
		// 8
		connection->callAndThrowOnError6716(bu6716_getCurrentStatus, "bu6716_getCurrentStatus", CHANNEL_MASK, nullptr, excl_status);
		for (int i = 0, n = 0; i < bu6716_NUM_CHAN; i++) {
			if (CHANNEL_MASK & (1 << i)) {
				if (excl_status[n] != bu6716_EXC_CURRENT_LIMIT_DETECTED) {
					log(QString("CH_%1: Excitation current limit condition IS NOT detected, but should be\n").arg(i + 1));
					errorDetected |= (1 << i);
				}
				n++;
			}
		}
		connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xFFFF, T028_MODE_EXCAL);
		bu3100_sleep(250);
		return errorDetected == 0;
	
}

bool Abstract6716Test::signalPathCalibration(ViInt16 channel, ViReal64 offsets[4], ViReal64 gainCoeffs[4], ViBoolean store) const {
	int errorDetected = 0;
	ViReal64 excitationVoltagePos;
	ViReal64 excitationVoltageNeg;
	ViInt16 excMonitorMode;
	ViInt16 mode;
	ViReal64 realPosVoltageValue, realNegVoltageValue;
	ViUInt16 channelMask = (1 << (channel - 1));  // One channel at a time!

	auto const& setVrefVoltage = [this](const double voltage) {
		double output = 0;
		configureVoltageReferanceSwitches(0x64);
		connection->callAndThrowOnError6100(bu6100_setVoltRefOutput, "bu6100_setVoltRefOutput", voltage);
		connection->callAndThrowOnError6100(bu6100_getVoltRefOutput, "bu6100_getVoltRefOutput", &output);
		bu3100_sleep(100);
		return output;
	};

	// Store current settings for the channel
	connection->callAndThrowOnError6716(bu6716_getMode, "bu6716_getMode", channelMask, &mode);
	connection->callAndThrowOnError6716(bu6716_getExcitationMonitor, "bu6716_getExcitationMonitor", &excMonitorMode);
	connection->callAndThrowOnError6716(bu6716_getPosExcitation, "bu6716_getPosExcitation", channelMask, &excitationVoltagePos);
	connection->callAndThrowOnError6716(bu6716_getNegExcitation, "bu6716_getNegExcitation", channelMask, &excitationVoltageNeg);
	auto autoDacNeg = getAutoDACNegative(channelMask);
	auto autoDacPos = getAutoDACPositive(channelMask);
	// Set default calib (offsets = 0, gains = 1)
	connection->callAndThrowOnError6716(bu6716_resetSignalPathCalibCoeff, "bu6716_resetSignalPathCalibCoeff", channelMask, false);
	// 1, 2 & 3
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", 0xffff, bu6716_MODE_FULL_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, 0.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, 0.0);
	setAutoDACNegative(channelMask, 0.0);
	setAutoDACPositive(channelMask, 0.0);
	for (int g = 0; g < 4; g++) {
		const ViInt16 gain[4] = { 1, 10, 100, 1000 };
		ViReal64 vrefVal = 9.0 / gain[g];
		auto const& readAndCheckValue = [this, channel, g](int& errorDetected, const double expected) {
			const ViReal64 limit[4] = { 0.01, 0.01, 0.05, 0.25 };
			auto readValOff = readValue_oneChannel(bu3416_GAIN_1, channel, 0.1);
			errorDetected |= checkValue_oneChannel(channel, readValOff, "L23xx", expected, limit[g], limit[g], false);
			return readValOff;
		};

		log(QString("  GAIN = %1\n").arg(gain[g]));
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", channel, bu6716_INP_SRC_VREF);
		connection->callAndThrowOnError6716(bu6716_setGain, "bu6716_setGain", channelMask, gain[g]);

		//4
		auto vrefOutput = setVrefVoltage(0);
		log(QString("VRef ouput:%1, should be 0\n").arg(vrefOutput));

		// 5
		auto readValOff = readAndCheckValue(errorDetected, 0);
		if (errorDetected == 0)
			connection->callAndThrowOnError6716(bu6716_setSignalPathCalibCoeff, "bu6716_setSignalPathCalibCoeff", channelMask, gain[g], -readValOff, 1.0);
		// 6
		realPosVoltageValue = setVrefVoltage(vrefVal);
		log(QString("VRef ouput:%1, should be %2\n").arg(realPosVoltageValue).arg(vrefVal));

		// 7
		auto readValPos = readAndCheckValue(errorDetected, realPosVoltageValue * gain[g]);

		// 8
		realNegVoltageValue = setVrefVoltage(-vrefVal);
		log(QString("VRef ouput:%1, should be %2\n").arg(realNegVoltageValue).arg(-vrefVal));

		// 9
		auto readValNeg = readAndCheckValue(errorDetected, realNegVoltageValue * gain[g]);

		// 10
		offsets[g] = -readValOff / gain[g];
		gainCoeffs[g] = (realPosVoltageValue - realNegVoltageValue) / (readValPos / gain[g] - readValNeg / gain[g]);
		if (errorDetected == 0)
			connection->callAndThrowOnError6716(bu6716_setSignalPathCalibCoeff, "bu6716_setSignalPathCalibCoeff", channelMask, gain[g], offsets[g], gainCoeffs[g]);
		connection->callAndThrowOnError6716(bu6716_setInputSrc, "bu6716_setInputSrc", channel, bu6716_INP_SRC_FP);
	}
	if (store && errorDetected == 0) {
		connection->callAndThrowOnError6716(bu6716_storeSignalPathCalibCoeff, "bu6716_storeSignalPathCalibCoeff", channelMask);
		connection->callAndThrowOnError6716(bu6716_resetSignalPathCalibCoeff, "bu6716_resetSignalPathCalibCoeff", channelMask, true);
	}
	// Restore settings
	auto vrefOutput = setVrefVoltage(0);
	log(QString("VRef ouput:%1, should be 0\n").arg(vrefOutput));
	bu3100_sleep(100);
	connection->callAndThrowOnError6716(bu6716_setGain, "bu6716_setGain", channelMask, bu6716_GAIN_1);
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", channelMask, excitationVoltagePos);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", channelMask, excitationVoltageNeg);
	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", excMonitorMode);
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", channelMask, mode);
	setAutoDACNegative(channelMask, autoDacNeg[0]);
	setAutoDACPositive(channelMask, autoDacPos[0]);
	return errorDetected == 0;
}

bool Abstract6716Test::configureVoltageReferanceSwitches(const unsigned char newSegConfRegValue) const {
	auto result = connection->writeFPGAreg(bu6716_FPGA_SEGCONF, newSegConfRegValue);
	bu3100_sleep(50);
	return result;
}
void Abstract6716Test::closeAll()  {
	if (connection->getVi6716())
		bu6716_close(connection->getVi6716());
	if (connection->getVi3416())
		bu3416_close(connection->getVi3416());
	if (connection->getViT028())
		t028_close(connection->getViT028());
	if (connection->getViT028Master())
		bu3416_close(connection->getViT028Master());
}
Abstract6716Test::Abstract6716Test(const QString&& name, const std::shared_ptr<Communication_6716>& connection) : AbstractTest(name), connection(connection) {}