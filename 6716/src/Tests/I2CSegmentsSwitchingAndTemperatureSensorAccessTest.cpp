#include "../../include/Tests/I2CSegmentsSwitchingAndTemperatureSensorAccessTest.h"

Result I2CSegmentsSwitchingAndTemperatureSensorAccessTest::test() const {
	if (!testWriteRead(1, bu6716_FPGA_SEGSW))
		return Result::VALUE::FAILED;
	if (!testWriteRead(0x12, bu6716_FPGA_TEST_RW/*Temp reg H) || !testWR(0x00, 0Temp reg L*/))
		return Result::VALUE::FAILED;
	if (!testWriteRead(0, bu6716_FPGA_SEGSW))
		return Result::VALUE::FAILED;
	if (!testWriteRead(0x34, bu6716_FPGA_TEST_RW/*Temp reg H) || !testWR(0x00, 0Temp reg L*/))
		return Result::VALUE::FAILED;
	if (!testWriteRead(1, bu6716_FPGA_SEGSW))
		return Result::VALUE::FAILED;
	if (!testWriteRead(0x12, bu6716_FPGA_TEST_RW/*Temp reg H) || !testWR(0x00, 0Temp reg L*/))
		return Result::VALUE::FAILED;
	if (!testWriteRead(0, bu6716_FPGA_SEGSW))
		return Result::VALUE::FAILED;
	return testWriteRead(0x34, bu6716_FPGA_TEST_RW/*Temp reg H) && testWR(0x00, 0Temp reg L*/) ? Result::VALUE::PASSED : Result::VALUE::FAILED;
}

I2CSegmentsSwitchingAndTemperatureSensorAccessTest::I2CSegmentsSwitchingAndTemperatureSensorAccessTest()
	: AbstractTest6716("I2C Segments switching and temperature sensor access") {}
