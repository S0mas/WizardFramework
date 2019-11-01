#include "../include/Tests/HalfBridgeTest.h"
#include "../include/defines.h"

bool HalfBridgeTest::test() const {
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);

	//Step 1)
	{
		connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_HALF_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
		connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	}

	//Check step 1
	{
		ViInt16 inputSource, couplingPos, couplingNeg, mode, gain;
		connection->callAndThrowOnError6716(bu6716_getInputSrc, "bu6716_getInputSrc", CHANNEL_MASK, &inputSource);
		connection->callAndThrowOnError6716(bu6716_getCoupling, "bu6716_getCoupling", CHANNEL_MASK, &couplingNeg, &couplingPos);
		connection->callAndThrowOnError6716(bu6716_getMode, "bu6716_getMode", CHANNEL_MASK, &mode);
		connection->callAndThrowOnError6716(bu6716_getGain, "bu6716_getGain", CHANNEL_MASK, &gain);
		if (inputSource != bu6716_INP_SRC_FP || couplingNeg != bu6716_COUPLING_DC || couplingPos != bu6716_COUPLING_DC || mode != bu6716_MODE_HALF_BRIDGE || gain != bu6716_GAIN_1) {
			printf("Device configuration error!");
			return false;
		}
		else
			printf("Device configuration succeeds!");
	}

	//Step 2)
	{
		connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_HBRIDGE);
	}

	//Step 3)
	{
		connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 5.0);
		connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	}

	//Check step 3
	{
		double neg, pos = 9;
		connection->callAndThrowOnError6716(bu6716_getNegExcitation, "bu6716_getNegExcitation", CHANNEL_MASK, &neg);
		connection->callAndThrowOnError6716(bu6716_getPosExcitation, "bu6716_getPosExcitation", CHANNEL_MASK, &pos);
		if (neg != 0 || pos != 5) {
			printf(QString("Excitation setting error: positive:%1, negative:%2\n").arg(pos).arg(neg));
			return false;
		}
	}

	//Step 4)
	auto errorDetected = 0;
	{
		errorDetected = checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1201", 0.0, 0.050, 0.050);
		bu3100_sleep(50);
	}

	//Step 5)
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", CHANNEL_MASK, bu6716_MODE_FULL_BRIDGE);

	//Check step 5
	{
		ViInt16 mode;
		connection->callAndThrowOnError6716(bu6716_getMode, "bu6716_getMode", CHANNEL_MASK, &mode);
		if (mode != bu6716_MODE_FULL_BRIDGE) {
			printf(QString("Error! Fullbridge mode didnt set\n"));
			return false;
		}
		else
			printf("Mode changed to fullbridge\n");
	}

	//6)
	bu3100_sleep(50);
	errorDetected |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1202", -2.5, 1.0, 1.0);
	//7)
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", CHANNEL_MASK, bu6716_MODE_HALF_BRIDGE);
	//8)
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 10.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	//9)
	bu3100_sleep(50);

	errorDetected |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1201", 0.0, 0.050, 0.050);
	/*
	//10)
	T028_CHECK_ERR(t028_setRelay, T028_CHAN_ALL, T028_RELAY_1, T028_OFF);

	//11)
	bu3100_sleep(50);
	BU3416_CHECK_ERR(readValues, env, bu3416_GAIN_1000, CHANNEL_MASK, 0.1, values);

	errorDetected |= checkValues(CHANNEL_MASK, values, "L1203", -2.5e-3, 1e-3, 1e-3);
	*/
	//12)
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 0.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	//13)
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xFFFF, T028_MODE_EXCAL);
	return errorDetected == 0;
}

HalfBridgeTest::HalfBridgeTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Half Bridge", connection) {}
