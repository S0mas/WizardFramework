#include "../include/Tests/HalfBridgeTest.h"
#include "../include/defines.h"

bool HalfBridgeTest::test() const {
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	configureVoltageReferanceSwitches(0x60);

	//Step 1)
	{
		connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_HALF_BRIDGE, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
		connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
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

	//Step 4)
	auto channelsErrorsMask = 0;
	{
		channelsErrorsMask = checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1201", 0.0, 0.050, 0.050);
		bu3100_sleep(50);
	}

	//Step 5)
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", CHANNEL_MASK, bu6716_MODE_FULL_BRIDGE);

	//6)
	bu3100_sleep(50);
	channelsErrorsMask |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1202", -2.5, 1.0, 1.0);
	//7)
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", CHANNEL_MASK, bu6716_MODE_HALF_BRIDGE);
	//8)
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 10.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	//9)
	bu3100_sleep(50);

	channelsErrorsMask |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1201", 0.0, 0.050, 0.050);
	/* removed at the moment
	//10)
	T028_CHECK_ERR(t028_setRelay, T028_CHAN_ALL, T028_RELAY_1, T028_OFF);

	//11)
	bu3100_sleep(50);
	BU3416_CHECK_ERR(readValues, env, bu3416_GAIN_1000, CHANNEL_MASK, 0.1, values);

	channelsErrorsMask |= checkValues(CHANNEL_MASK, values, "L1203", -2.5e-3, 1e-3, 1e-3);
	*/
	//12)
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 0.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	//13)
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xFFFF, T028_MODE_EXCAL);
	return channelsErrorsMask == 0;
}

HalfBridgeTest::HalfBridgeTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Half Bridge", connection, true) {}
