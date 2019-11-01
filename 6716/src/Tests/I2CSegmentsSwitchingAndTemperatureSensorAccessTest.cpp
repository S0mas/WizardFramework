#include "../include/Tests/I2CSegmentsSwitchingAndTemperatureSensorAccessTest.h"
#include "../include/defines.h"

bool I2CSegmentsSwitchingAndTemperatureSensorAccessTest::test() const {
	if (!testWriteRead(1, bu6716_FPGA_SEGSW))
		return false;
	if (!testWriteRead(0x12, bu6716_FPGA_TEST_RW/*Temp reg H) || !testWR(0x00, 0Temp reg L*/))
		return false;
	if (!testWriteRead(0, bu6716_FPGA_SEGSW))
		return false;
	if (!testWriteRead(0x34, bu6716_FPGA_TEST_RW/*Temp reg H) || !testWR(0x00, 0Temp reg L*/))
		return false;
	if (!testWriteRead(1, bu6716_FPGA_SEGSW))
		return false;
	if (!testWriteRead(0x12, bu6716_FPGA_TEST_RW/*Temp reg H) || !testWR(0x00, 0Temp reg L*/))
		return false;
	if (!testWriteRead(0, bu6716_FPGA_SEGSW))
		return false;
	return testWriteRead(0x34, bu6716_FPGA_TEST_RW/*Temp reg H) && testWR(0x00, 0Temp reg L*/);
}

I2CSegmentsSwitchingAndTemperatureSensorAccessTest::I2CSegmentsSwitchingAndTemperatureSensorAccessTest(const std::shared_ptr<Communication_6716>& connection)
	: Abstract6716Test("I2C Segments switching and temperature sensor access", connection) {}
