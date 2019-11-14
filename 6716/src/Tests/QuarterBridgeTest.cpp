#include "../include/Tests/QuarterBridgeTest.h"
#include "../include/defines.h"

bool QuarterBridgeTest::test() const {
	connection->callAndThrowOnError6716(bu6716_reloadConfig, "bu6716_reloadConfig", true);
	//1)
	connection->callAndThrowOnError6716(bu6716_setChannelConf, "bu6716_setChannelConf", CHANNEL_MASK, bu6716_MODE_QBRIDGE_120, bu6716_GAIN_1, bu6716_COUPLING_DC, bu6716_INP_SRC_FP);
	configureVoltageReferanceSwitches(0x60);

	connection->callAndThrowOnError6716(bu6716_setExcitationMonitor, "bu6716_setExcitationMonitor", bu6716_EXCMON_OFF);
	//2)
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", CHANNEL_MASK, T028_MODE_QBRIDGE);
	//3)
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 5.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	//4)
	bu3100_sleep(50);
	auto channelsErrorsMask = checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1301", 0.0, 0.050, 0.050);
	//5)
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", CHANNEL_MASK, bu6716_MODE_DI);
	bu3100_sleep(100);
	connection->callAndThrowOnError6716(bu6716_setMode, "bu6716_setMode", CHANNEL_MASK, bu6716_MODE_QBRIDGE_350);
	//6)
	bu3100_sleep(50);
	channelsErrorsMask |= checkValues(CHANNEL_MASK, readValues(bu3416_GAIN_1, CHANNEL_MASK, 0.1), "L1302", -1.225, 0.075, 0.075);
	//7)
	connection->callAndThrowOnError6716(bu6716_setPosExcitation, "bu6716_setPosExcitation", CHANNEL_MASK, 0.0);
	connection->callAndThrowOnError6716(bu6716_setNegExcitation, "bu6716_setNegExcitation", CHANNEL_MASK, 0.0);
	//8)
	connection->callAndThrowOnErrorT028(t028_setChannelsConfig, "t028_setChannelsConfig", 0xFFFF, T028_MODE_EXCAL);
	return channelsErrorsMask == 0;
}

QuarterBridgeTest::QuarterBridgeTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Quarter Bridge", connection, true) {}
