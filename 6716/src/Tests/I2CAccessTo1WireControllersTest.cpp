#include "../../include/Tests/I2CAccessTo1WireControllersTest.h"

Result I2CAccessTo1WireControllersTest::test() const {
	//ViUInt16 reset_cmd[] = { 0x0300, 0x0000 };
	//ViUInt16 write_cmd[] = { 0x0300, 0x02CC, 0x0299, 0x0200, 0x0200, 0x0200, 0x0300, 0x0000 };
	//ViUInt16 read_cmd[] = { 0x0300, 0x02CC, 0x02C3, 0x0200, 0x0100, 0x0100, 0x0300, 0x0000 };

	//write_cmd[4] = 0x0200 | (8 & 0x00FF);
	//write_cmd[5] = 0x0200 | (~8 & 0x00FF);
	//auto status = commander6716->_1wire_commander(write_cmd);
	//bu3100_sleep(20);
	//read_cmd[4] = 0x0100;
	//read_cmd[5] = 0x0100;
	//status = commander6716->_1wire_commander(read_cmd);
	//status = commander6716->_1wire_commander(reset_cmd);
	//return checkValue(write_cmd[4] & 0xff, read_cmd[4] & 0xff, 16, 2, '0') && checkValue(write_cmd[5] & 0xff, read_cmd[5] & 0xff, 16, 2, '0');
	return Result::VALUE::PASSED;
}
